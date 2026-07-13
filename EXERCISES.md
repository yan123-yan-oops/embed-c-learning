# 嵌入式 C 学习 — 章节习题集

> 每章分三部分：
> **✅ 概念题**（检验理解）
> **🔍 代码阅读题**（猜输出）
> **💻 编程题**（动手写代码）

---

## 01-basics：C 基础（数据类型、volatile、static、补码）

### ✅ 概念题

1. `uint8_t` 和 `int8_t` 都存了二进制 `1100 1000`，用 `%d` 打印分别输出什么？为什么？
2. `volatile` 关键字用在哪三种场景？不加 volatile 编译器可能做什么优化？
3. `static` 关键字在函数内部和文件顶部分别有啥作用？
4. 什么是整型提升？`int8_t x = 200; printf("%d", x);` 输出什么？
5. 补码是什么？正数和负数的补码分别怎么算？

### 🔍 代码阅读题

**题 A：**
```c
uint8_t a = 200;
int8_t  b = 200;
printf("%u %d %u", a, b, b);
// 输出？
```

**题 B：**
```c
int count(void) {
    static int n = 0;
    return n++;
}
int main() {
    printf("%d ", count());
    printf("%d ", count());
    printf("%d",  count());
}
// 输出？
```

### 💻 编程题

**题 1：写一个函数，判断当前系统是大端还是小端。**
```c
// 函数原型
int is_little_endian(void);
// 返回 1 = 小端, 0 = 大端
// 提示：用 uint16_t 和 uint8_t* 观察内存布局
```

**题 2：写一个环形计数器，每次调用返回 0→1→2→...→9→0→1→...**
```c
// 要求用 static 局部变量实现
uint8_t next_counter(void);
```

---

## 02-pointers：指针

### ✅ 概念题

1. `int arr[5]` 中 `arr` 和 `&arr[0]` 是什么关系？`arr+1` 和 `&arr[1]` 呢？
2. 数组作为函数参数传递时发生了什么？`sizeof(arr)` 在函数内和函数外一样吗？
3. 指针 `+1` 实际加了多少字节，由什么决定？
4. `const int *p` 和 `int *const p` 有什么区别？
5. 野指针是什么？怎么避免？
6. 函数指针在嵌入式里用来干什么？

### 🔍 代码阅读题

**题 A：**
```c
uint32_t arr[4] = {10, 20, 30, 40};
uint32_t *p = arr + 2;
printf("%d %d", arr[1], *p);
// 输出？
```

**题 B：**
```c
void func(uint8_t a[100]) {
    printf("%zu", sizeof(a));
}
uint8_t buf[100];
printf("%zu ", sizeof(buf));
func(buf);
// 输出？
```

### 💻 编程题

**题 1：实现一个函数，用指针反转数组。**
```c
// 不可以用数组下标 []
void reverse_array(int *arr, int len);
// 输入: [1,2,3,4,5] → 输出: [5,4,3,2,1]
```

**题 2：实现一个通用的"查找最大值"函数。**
```c
// 用函数指针 + void* 实现通用性
int find_max(void *arr, int len, size_t elem_size,
             int (*cmp)(void *a, void *b));
// 提示：用 (char*) 逐字节偏移
```

**题 3：手写一个 `memcpy` 函数（考虑内存重叠）。**
```c
void* my_memcpy(void *dest, const void *src, size_t n);
// 要求：正确处理 dest 和 src 重叠的情况
```

---

## 03-bitwise：位操作

### ✅ 概念题

1. 嵌入式里 `SET_BIT(reg, 5)` 和 `CLR_BIT(reg, 5)` 的宏怎么写？
2. `x & (x-1)` 这个表达式有什么用？
3. 位域结构体和手动位操作比有什么优缺点？
4. `GET_BITS(reg, 15, 8)` 的实现原理是什么？

### 🔍 代码阅读题

**题 A：**
```c
uint8_t x = 0b10101010;
uint8_t y = x & 0x0F;
uint8_t z = x | 0x0F;
// y 和 z 的值（十六进制）？
```

**题 B：**
```c
uint32_t reg = 0x12345678;
uint32_t val = ((reg >> 8) & 0xFF);
// val 等于？
```

### 💻 编程题

**题 1：用一个宏实现「在指定位置插入指定位宽的值」。**
```c
#define MODIFY_BITS(reg, high, low, val) ...
// 示例: MODIFY_BITS(adc_reg, 15, 8, 0x3F)
// 将 adc_reg 的 bit[15:8] 替换为 0x3F，其他位不变
```

**题 2：不用临时变量，用异或交换两个变量的值。**
```c
void swap_xor(int *a, int *b);
```

**题 3：写一个函数统计 32 位整数中 1 的个数（popcount）。**
```c
int popcount(uint32_t x);
// 输入 0b10101010 → 返回 4
```

---

## 04-memory：内存管理

### ✅ 概念题

1. C 程序的内存分哪几个区？局部变量、全局变量、malloc、字符串常量分别在哪个区？
2. 栈溢出是什么？嵌入式中怎么避免？
3. 为什么嵌入式系统（裸机/RTOS）中不推荐用 `malloc`？
4. 静态内存池相比 `malloc` 有什么优势？

### 🔍 代码阅读题

**题 A：**
```c
int *foo(void) {
    int x = 10;
    return &x;  // 有什么问题？
}
```

**题 B：**
```c
char *p = "hello";
p[0] = 'H';  // 会不会 crash？为什么？
```

### 💻 编程题

**题 1：实现一个固定块大小的静态内存池。**
```c
// 初始化：用一个 char[POOL_SIZE][BLOCK_SIZE] 作为池
void pool_init(void);
void *pool_alloc(void);
void pool_free(void *ptr);
// 要求：分配失败返回 NULL，释放后块可重用
```

**题 2：写一个函数打印当前各段地址分布。**
```c
// 输出类似：
// 代码区: 0x0040xxxx
// 常量区: 0x0040xxxx
// 全局区: 0x0040xxxx
// 栈区:   0x0060xxxx
// 堆区:   0x0090xxxx
// 提示：用全局变量、局部变量、malloc、函数名等取地址
```

---

## 05-struct-enum：结构体与枚举

### ✅ 概念题

1. `struct` 和 `union` 的区别是什么？union 的内存怎么布局？
2. 什么是内存对齐？编译器为什么要在结构体成员之间插入 padding？
3. `__attribute__((packed))` 是干什么的？什么时候必须用？
4. 枚举和 `#define` 比有什么优势？

### 🔍 代码阅读题

**题 A：**
```c
struct {
    uint8_t a;
    uint32_t b;
    uint8_t c;
} s;
printf("%zu", sizeof(s));
// 输出？为什么不是 1+4+1=6？
```

**题 B：**
```c
union {
    uint32_t word;
    uint8_t  bytes[4];
} u;
u.word = 0x12345678;
printf("0x%02X", u.bytes[0]);
// 小端系统输出？
```

### 💻 编程题

**题 1：解析一个从串口收到的数据帧。**
```c
// 帧格式: 头(1) + 长度(1) + 数据(len) + CRC(1)
// 用 packed 结构体定义帧格式，写解析函数
```

**题 2：用结构体位域定义一个定时器控制寄存器。**
```c
// 包含: 使能(1bit)、模式(2bit)、预分频(4bit)、保留(25bit)
// 写函数设置各字段的值
```

---

## 06-strings：字符串处理

### ✅ 概念题

1. `char *s1 = "hello"` 和 `char s2[] = "hello"` 有什么区别？
2. `strtok` 函数是线程安全的吗？嵌入式中怎么替代？
3. `snprintf` 比 `sprintf` 好在哪？

### 🔍 代码阅读题

```c
char str[] = "192.168.1.1";
char *token = strtok(str, ".");
int count = 0;
while (token) {
    count++;
    token = strtok(NULL, ".");
}
printf("%d", count);
// 输出？
```

### 💻 编程题

**题 1：解析 NMEA 语句中的经纬度字段。**
```c
// 输入: "$GPGGA,092750.000,5321.6802,N,00630.3372,W,..."
// 输出: 纬度 = 53.216802°, 经度 = 6.303372°
// 提示: NMEA 纬度格式 ddmm.mmmm → 度分格式
```

**题 2：实现一个安全的字符串转整数函数。**
```c
// 处理: 空指针、非数字字符、溢出（返回错误码）
int safe_atoi(const char *str, int *result);
// 返回 0 成功，-1 失败
```

---

## 07-ringbuffer：环形缓冲区

### ✅ 概念题

1. 环形缓冲区解决了什么问题？
2. 为什么环形缓冲区的大小通常是 2 的幂？`(head + 1) & mask` 比 `(head + 1) % size` 好在哪？
3. 单生产者-单消费者场景为什么不需要加锁？
4. 环形缓冲区满和空怎么判断？

### 💻 编程题

**题 1：实现一个环形缓冲区（不支持覆盖模式）。**
```c
typedef struct {
    uint8_t *buf;
    uint32_t head;  // 写位置
    uint32_t tail;  // 读位置
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

---

## 08-nmea-parser：NMEA 0183 解析器

### ✅ 概念题

1. NMEA 0183 的帧格式是什么？`$`、`,`、`*` 分别什么作用？
2. 什么是 CRC 校验？`$GPGGA` 的校验怎么算？
3. 模块化设计中 `.h` 文件里通常放什么？`.c` 文件里放什么？

### 💻 编程题

**题 1：实现 `$GPGGA` 和 `$GPRMC` 两种帧的解析器。**
```c
// 输入: 完整的 NMEA 句子
// 输出: 结构体包含 时间、纬度、经度、海拔、速度、航向
typedef struct {
    uint8_t  hour, min, sec;      // UTC 时间
    double   latitude;             // 纬度 (度)
    char     lat_dir;              // N/S
    double   longitude;            // 经度 (度)
    char     lon_dir;              // E/W
    float    altitude;             // 海拔 (米)
    float    speed;                // 速度 (节)
    float    course;               // 航向 (度)
} nmea_gpgga_t;

int parse_gpgga(const char *sentence, nmea_gpgga_t *out);
// 返回 0 成功，-1 校验失败，-2 格式错误
```

**题 2：写一个函数验证 NMEA 句子的 CRC 校验和。**
```c
// 输入: "$GPGGA,...*76"
// 计算: $ 和 * 之间所有字符的异或
// 验证: 计算结果是否等于 * 后面的十六进制值
bool nmea_checksum_verify(const char *sentence);
```

---

## 09-pool-allocator：静态内存池

### ✅ 概念题

1. 固定块池和可变块池分别适用于什么场景？
2. 为什么零 malloc 设计在嵌入式里是加分项？
3. 内存池的碎片和 malloc 的碎片有什么不同？

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

---

## 10-state-machine：状态机

### ✅ 概念题

1. 什么是有限状态机？哪些场景用状态机实现？
2. 状态机的三要素是什么？
3. 用 `switch-case` 实现和用函数指针表实现各有什么优缺点？

### 💻 编程题

**题 1：实现一个 NMEA 解析器的状态机。**
```c
// 状态: WAIT_DOLLAR, READ_FIELDS, CHECK_CRC
// 输入: 逐字节到达的 UART 数据
// 输出: 解析完成时触发回调

typedef enum {
    STATE_WAIT_DOLLAR,
    STATE_READ_FIELDS,
    STATE_CHECK_CRC,
    STATE_COMPLETE
} parser_state_t;

// 每收到一个字节调用一次
parser_state_t nmea_feed_byte(parser_state_t state, uint8_t byte);
```

**题 2：实现一个按键消抖状态机。**
```c
// 状态: IDLE, DEBOUNCE, PRESSED, RELEASE
// 输入: 当前按键电平 (0/1)
// 输出: 稳定的按键事件 (PRESS / RELEASE)
// 要求: 用状态图先画出状态转换
```

**题 3：用函数指针表实现状态机。**
```c
// 对比 switch-case 的实现方式
// 每个状态一个处理函数
// 状态切换通过函数指针跳转
```

---

> 💡 **做题建议**：
> 1. 先对着每章的 .c 文件复习一遍
> 2. 概念题→代码阅读题→编程题，按顺序做
> 3. 编程题先在 VS Code 里跑通
> 4. 卡住先看原代码找灵感，实在不行问我
