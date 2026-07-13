# 07-ringbuffer 习题：环形缓冲区

---

### ✅ 概念题

1. 环形缓冲区解决了什么问题？
2. 为什么环形缓冲区的大小通常是 2 的幂？`(head + 1) & mask` 比 `(head + 1) % size` 好在哪？
3. 单生产者-单消费者场景为什么不需要加锁？
4. 环形缓冲区满和空怎么判断？

---

### 💻 编程题

**题 1：实现一个环形缓冲区（不支持覆盖模式）。**
```c
typedef struct {
    uint8_t *buf;
    uint32_t head;
    uint32_t tail;
    uint32_t size;  // 必须是 2^n
    uint32_t mask;
} rb_t;

void     rb_init(rb_t *rb, uint8_t *buf, uint32_t size);
bool     rb_put(rb_t *rb, uint8_t data);       // 满返回 false
bool     rb_get(rb_t *rb, uint8_t *data);       // 空返回 false
uint32_t rb_available(rb_t *rb);                // 当前数据量
bool     rb_full(rb_t *rb);
bool     rb_empty(rb_t *rb);
```

**题 2：扩展环形缓冲区，支持覆盖模式（overwrite）。**
```c
// 当缓冲区满时，丢弃最旧的数据，写入新的
bool rb_put_overwrite(rb_t *rb, uint8_t data);
```

**题 3：用环形缓冲区模拟一个 1024 字节的 UART 接收队列。**
```c
// 中断写入（模拟随机的 1~16 字节到达）
// 主循环每秒读取并打印所有数据
// 检测溢出并打印警告
```
