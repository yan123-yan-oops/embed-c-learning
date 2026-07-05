/**
 * ============================================================
 * 内存管理 — 嵌入式 C 内存模型
 * 学习目标：
 *   1. 栈（Stack）— 局部变量，自动分配释放
 *   2. 堆（Heap） — malloc/free，动态分配
 *   3. 全局区（静态区）— 全局变量、static 变量
 *   4. 常量区（只读）— 字符串常量
 *   5. 嵌入式注意：避免堆碎片、栈溢出
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ========== 全局变量（全局区/静态区）========== */
int global_val = 100;               // 已初始化全局区
static int static_val = 200;        // 已初始化静态区
static int uninit_val;              // BSS 段（未初始化，自动清零）

/* 字符串常量（常量区/只读区） */
static const char *message = "Hello Embedded World";

void print_addr(const char *label, const void *addr)
{
    printf("  %-30s : %p\n", label, addr);
}

void stack_demo(void)
{
    /* ---------- 栈上的局部变量 ---------- */
    int local_a = 10;
    int local_b = 20;
    char buffer[64];

    printf("\n--- 栈区 ---\n");
    print_addr("局部变量 local_a", &local_a);
    print_addr("局部变量 local_b", &local_b);
    print_addr("数组 buffer", buffer);
}

void heap_demo(void)
{
    /* ---------- 堆分配 ---------- */
    printf("\n--- 堆区 ---\n");

    uint8_t *p1 = (uint8_t*)malloc(128);
    uint8_t *p2 = (uint8_t*)malloc(256);

    if (p1 && p2) {
        memset(p1, 0xAA, 128);
        memset(p2, 0xBB, 256);

        print_addr("堆 malloc(128)", p1);
        print_addr("堆 malloc(256)", p2);
        printf("  p1[0]=0x%02X, p1[127]=0x%02X\n", p1[0], p1[127]);
        printf("  p2[0]=0x%02X, p2[255]=0x%02X\n", p2[0], p2[255]);

        free(p1);
        free(p2);
        printf("  ✅ 已释放堆内存\n");
    } else {
        printf("  ❌ malloc 失败（嵌入式中要始终检查返回值！）\n");
    }

    /* ⚠️ 嵌入式警告：
     * 裸机/RTOS 中慎用 malloc！
     * 1. 堆碎片 — long run 后分配失败
     * 2. 不确定执行时间 — 不适合实时任务
     * 3. 多线程不安全 — 需要加锁
     * ✅ 推荐：静态池分配（预分配固定大小缓冲区）
     */
}

/* 内存池示例 — 嵌入式推荐方案 */
#define POOL_SIZE   4
#define BLOCK_SIZE  64

static uint8_t mem_pool[POOL_SIZE][BLOCK_SIZE];   // 预分配的静态内存池
static bool pool_used[POOL_SIZE] = {false};

void *pool_alloc(void)
{
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool_used[i]) {
            pool_used[i] = true;
            memset(mem_pool[i], 0, BLOCK_SIZE);
            return mem_pool[i];
        }
    }
    return NULL;  // 池已耗尽
}

void pool_free(void *ptr)
{
    for (int i = 0; i < POOL_SIZE; i++) {
        if (mem_pool[i] == ptr) {
            pool_used[i] = false;
            return;
        }
    }
}

void pool_demo(void)
{
    printf("\n--- 静态内存池（嵌入式推荐方案）---\n");

    void *blk1 = pool_alloc();
    void *blk2 = pool_alloc();

    print_addr("内存池块1", blk1);
    print_addr("内存池块2", blk2);

    pool_free(blk1);
    pool_free(blk2);
    printf("  ✅ 已释放内存池块\n");
}

int main(void)
{
    printf("===== C 内存模型 — 嵌入式必知 =====\n\n");

    /* ---------- 各内存区域地址分布 ---------- */
    const char *literal = "常量字符串";

    printf("--- 各段地址分布（越低越像嵌入式 Flash/RAM）---\n");
    print_addr("main 函数（代码区）",  main);
    print_addr("字符串常量",          literal);
    print_addr("全局变量 global_val", &global_val);
    print_addr("静态变量 static_val", &static_val);
    print_addr("未初始化 BSS",        &uninit_val);

    stack_demo();
    heap_demo();
    pool_demo();

    printf("\n--- 嵌入式中内存布局的典型示意图 ---\n");
    printf(" 高地址\n");
    printf("  ┌─────────────┐  ← 栈顶（向下增长）\n");
    printf("  │   栈区      │  局部变量、函数调用帧\n");
    printf("  ├─────────────┤\n");
    printf("  │   堆区      │  malloc（小系统慎用）\n");
    printf("  ├─────────────┤\n");
    printf("  │   BSS       │  未初始化全局/静态变量\n");
    printf("  ├─────────────┤\n");
    printf("  │   数据区    │  已初始化全局/静态变量\n");
    printf("  ├─────────────┤\n");
    printf("  │   常量区    │  字符串字面量、const\n");
    printf("  ├─────────────┤\n");
    printf("  │   代码区    │  程序指令（Flash）\n");
    printf(" 低地址\n");

    printf("\n✅ 内存基础学完！\n");
    return 0;
}
