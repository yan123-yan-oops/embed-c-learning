# 09-pool-allocator 习题：静态内存池

---

### ✅ 概念题

1. 固定块池和可变块池分别适用于什么场景？
2. 为什么零 malloc 设计在嵌入式里是加分项？
3. 内存池的碎片和 malloc 的碎片有什么不同？

---

### 💻 编程题

**题 1：实现一个固定块大小的内存池。**
```c
// 预分配 N 个固定大小的块
// 分配返回块地址，释放标记块为空
// 需要处理：分配失败返回 NULL、双重释放检测
typedef struct {
    // 自行设计成员
} fixed_pool_t;

void pool_init(fixed_pool_t *pool, void *mem, uint32_t blk_size, uint32_t blk_count);
void *pool_alloc(fixed_pool_t *pool);
void  pool_free(fixed_pool_t *pool, void *ptr);
```

**题 2：实现一个可变块大小的内存池（类似 heap_4 简化版）。**
```c
// 使用空闲链表，支持合并相邻空闲块
// 不要用 malloc
void* mp_malloc(size_t size);
void  mp_free(void *ptr);
```
