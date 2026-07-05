/**
 * ============================================================
 * 静态内存池分配器 — 嵌入式替代 malloc 的方案
 * 学习目标：
 *   1. 为什么嵌入式不用 malloc（碎片、不确定性、RTOS 不安全）
 *   2. FreeRTOS 的 pvPortMalloc 内部机制
 *   3. 实现一个固定块大小的内存池
 *   4. 实现一个可变块大小的内存池
 *   5. 与 FreeRTOS heap_4.c 的对比
 * ============================================================
 *
 * FreeRTOS 映射：
 *   本文件 → FreeRTOS 的 heap_1 / heap_4
 *   pool_alloc  → pvPortMalloc
 *   pool_free   → vPortFree
 *
 * 区别：FreeRTOS 更复杂（支持合并、多堆），这里展示核心原理
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================
 * 方案 1：固定块内存池（最快、最可预测、零碎片）
 * 适用：串口缓冲区、CAN 消息、任务栈等固定大小对象
 * ============================================================ */
#define FIXED_POOL_BLOCKS   8
#define FIXED_BLOCK_SIZE    64   // 每个块 64 字节

typedef struct {
    uint8_t  pool[FIXED_POOL_BLOCKS][FIXED_BLOCK_SIZE];
    bool     used[FIXED_POOL_BLOCKS];
    uint32_t alloc_count;       // 统计
    uint32_t free_count;
} fixed_pool_t;

fixed_pool_t fixed_pool;  // 全局内存池（静态分配）

void fixed_pool_init(fixed_pool_t *pool)
{
    memset(pool->used, 0, sizeof(pool->used));
    pool->alloc_count = 0;
    pool->free_count  = 0;
}

void *fixed_pool_alloc(fixed_pool_t *pool)
{
    for (int i = 0; i < FIXED_POOL_BLOCKS; i++) {
        if (!pool->used[i]) {
            pool->used[i] = true;
            pool->alloc_count++;
            memset(pool->pool[i], 0, FIXED_BLOCK_SIZE);
            return pool->pool[i];
        }
    }
    return NULL;  // 池耗尽
}

void fixed_pool_free(fixed_pool_t *pool, void *ptr)
{
    for (int i = 0; i < FIXED_POOL_BLOCKS; i++) {
        if (pool->pool[i] == ptr) {
            pool->used[i] = false;
            pool->free_count++;
            return;
        }
    }
    // 指针不属于该池（嵌入式里触发断言）
    printf("  ⚠️ 错误：释放不属于本池的指针！\n");
}

void fixed_pool_status(fixed_pool_t *pool)
{
    int used = 0;
    for (int i = 0; i < FIXED_POOL_BLOCKS; i++) {
        if (pool->used[i]) used++;
    }
    printf("  固定池: %d/%d 块已用, alloc=%lu, free=%lu\n",
           used, FIXED_POOL_BLOCKS,
           (unsigned long)pool->alloc_count,
           (unsigned long)pool->free_count);
}

/* ============================================================
 * 方案 2：可变块内存池（类似 FreeRTOS heap_4）
 * 用空闲链表管理不同大小的块
 * ============================================================ */

#define VAR_POOL_SIZE   1024  // 总池大小

/* 内存块头 */
typedef struct mem_block {
    struct mem_block *next;
    size_t size;
    bool   free;
} mem_block_t;

#define BLOCK_HEADER_SIZE  sizeof(mem_block_t)

/* 8 字节对齐 */
#define ALIGN_UP(x)  (((x) + 7) & ~7)

static uint8_t var_pool_mem[VAR_POOL_SIZE];
static bool    var_pool_inited = false;

void var_pool_init(void)
{
    /* 将整个池初始化为一个空闲大块 */
    mem_block_t *head = (mem_block_t*)var_pool_mem;
    head->next = NULL;
    head->size = VAR_POOL_SIZE - BLOCK_HEADER_SIZE;
    head->free = true;
    var_pool_inited = true;
}

void *var_pool_alloc(size_t size)
{
    if (!var_pool_inited) var_pool_init();
    if (size == 0) return NULL;

    size = ALIGN_UP(size);  // 对齐

    mem_block_t *block = (mem_block_t*)var_pool_mem;
    mem_block_t *prev  = NULL;

    /* 首次适应 (First Fit)：找第一个够大的空闲块 */
    while (block) {
        if (block->free && block->size >= size) {
            /* 如果剩余空间还能再拆一个块（> 最小块大小） */
            size_t remaining = block->size - size;
            if (remaining > BLOCK_HEADER_SIZE + 16) {
                /* 拆分：前部分分配，后部分继续空闲 */
                mem_block_t *new_block = (mem_block_t*)
                    ((uint8_t*)block + BLOCK_HEADER_SIZE + size);
                new_block->next = block->next;
                new_block->size = remaining - BLOCK_HEADER_SIZE;
                new_block->free = true;
                block->next = new_block;
                block->size = size;
            }

            block->free = false;
            return (uint8_t*)block + BLOCK_HEADER_SIZE;
        }
        prev = block;
        block = block->next;
    }

    return NULL;  // 内存不足
}

void var_pool_free(void *ptr)
{
    if (!ptr) return;

    mem_block_t *block = (mem_block_t*)((uint8_t*)ptr - BLOCK_HEADER_SIZE);
    block->free = true;

    /* 合并相邻空闲块（防止碎片） */
    mem_block_t *curr = (mem_block_t*)var_pool_mem;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += BLOCK_HEADER_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void var_pool_status(void)
{
    mem_block_t *block = (mem_block_t*)var_pool_mem;
    int frag_count = 0;
    size_t free_total = 0;

    printf("  可变池(1024B): ");
    while (block) {
        printf("[%s %zuB] ",
               block->free ? "空闲" : "已用", block->size);
        if (block->free) {
            free_total += block->size;
            frag_count++;
        }
        block = block->next;
    }
    printf("\n  空闲总计: %zuB, 碎片数: %d\n", free_total, frag_count);
}

/* ============================================================
 * Demo
 * ============================================================ */
int main(void)
{
    printf("===== 静态内存池 — 嵌入式内存管理 =====\n\n");

    /* ---------- 固定块池 Demo ---------- */
    printf("--- 固定块内存池 (8块 × 64B) ---\n");
    fixed_pool_init(&fixed_pool);

    void *b1 = fixed_pool_alloc(&fixed_pool);
    void *b2 = fixed_pool_alloc(&fixed_pool);
    void *b3 = fixed_pool_alloc(&fixed_pool);
    printf("  分配 3 个块: %p, %p, %p\n", b1, b2, b3);
    fixed_pool_status(&fixed_pool);

    fixed_pool_free(&fixed_pool, b2);
    printf("  释放块2:\n");
    fixed_pool_status(&fixed_pool);

    /* 演示固定块池在串口接收中的应用 */
    printf("\n  应用场景：串口消息队列\n");
    void *msg = fixed_pool_alloc(&fixed_pool);
    if (msg) {
        snprintf((char*)msg, FIXED_BLOCK_SIZE, "$GPGGA,092750.000,5321.6802,N*76");
        printf("  存入消息: %s\n", (char*)msg);
    }
    fixed_pool_free(&fixed_pool, msg);

    /* ---------- 可变块池 Demo ---------- */
    printf("\n--- 可变块内存池 (1024B) ---\n");
    var_pool_init();
    var_pool_status();

    void *v1 = var_pool_alloc(100);
    void *v2 = var_pool_alloc(200);
    void *v3 = var_pool_alloc(50);
    printf("  分配 v1(100B), v2(200B), v3(50B)\n");
    var_pool_status();

    var_pool_free(v2);
    printf("  释放 v2:\n");
    var_pool_status();

    void *v4 = var_pool_alloc(80);
    printf("  分配 v4(80B) - 复用 v2 的空闲空间:\n");
    var_pool_status();

    var_pool_free(v1);
    var_pool_free(v3);
    var_pool_free(v4);
    printf("  全部释放后（碎片合并）:\n");
    var_pool_status();

    /* ---------- 与 FreeRTOS 对比 ---------- */
    printf("\n--- 🔗 与 FreeRTOS 的映射 ---\n");
    printf("  固定块池 (本文件)          → FreeRTOS heap_1\n");
    printf("  可变块池 (本文件)          → FreeRTOS heap_4\n");
    printf("  pool_alloc                 → pvPortMalloc\n");
    printf("  pool_free                  → vPortFree\n");
    printf("  区别：FreeRTOS 是线程安全的，支持中断态调用\n");

    printf("\n✅ 内存管理器学完！可直接嵌入到你的导航采集系统中\n");
    return 0;
}
