# 01-basics 习题：C 基础（数据类型、volatile、static、补码）

---

### ✅ 概念题

1. `uint8_t` 和 `int8_t` 都存了二进制 `1100 1000`，用 `%d` 打印分别输出什么？为什么？
2. `volatile` 关键字用在哪三种场景？不加 volatile 编译器可能做什么优化？
3. `static` 关键字在函数内部和文件顶部分别有啥作用？
4. 什么是整型提升？`int8_t x = 200; printf("%d", x);` 输出什么？
5. 补码是什么？正数和负数的补码分别怎么算？

---

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

---

### 💻 编程题

**题 1：写一个函数，判断当前系统是大端还是小端。**
```c
int is_little_endian(void);
// 返回 1 = 小端, 0 = 大端
// 提示：用 uint16_t 和 uint8_t* 观察内存布局
```

**题 2：写一个环形计数器，每次调用返回 0→1→2→...→9→0→1→...**
```c
// 要求用 static 局部变量实现
uint8_t next_counter(void);
```
