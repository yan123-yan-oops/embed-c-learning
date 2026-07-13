# 04-memory 习题：内存管理

---

### ✅ 概念题

1. C 程序的内存分哪几个区？局部变量、全局变量、malloc、字符串常量分别在哪个区？
2. 栈溢出是什么？嵌入式中怎么避免？
3. 为什么嵌入式系统（裸机/RTOS）中不推荐用 `malloc`？
4. 静态内存池相比 `malloc` 有什么优势？

---

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

---

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
