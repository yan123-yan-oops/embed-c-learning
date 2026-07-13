# 03-bitwise 习题：位操作

---

### ✅ 概念题

1. 嵌入式里 `SET_BIT(reg, 5)` 和 `CLR_BIT(reg, 5)` 的宏怎么写？
2. `x & (x-1)` 这个表达式有什么用？
3. 位域结构体和手动位操作比有什么优缺点？
4. `GET_BITS(reg, 15, 8)` 的实现原理是什么？

---

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

---

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
