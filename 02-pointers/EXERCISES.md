# 02-pointers 习题：指针

---

### ✅ 概念题

1. `int arr[5]` 中 `arr` 和 `&arr[0]` 是什么关系？`arr+1` 和 `&arr[1]` 呢？
2. 数组作为函数参数传递时发生了什么？`sizeof(arr)` 在函数内和函数外一样吗？
3. 指针 `+1` 实际加了多少字节，由什么决定？
4. `const int *p` 和 `int *const p` 有什么区别？
5. 野指针是什么？怎么避免？
6. 函数指针在嵌入式里用来干什么？

---

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

---

### 💻 编程题

**题 1：实现一个函数，用指针反转数组（不可以用数组下标 `[]`）。**
```c
void reverse_array(int *arr, int len);
// 输入: [1,2,3,4,5] → 输出: [5,4,3,2,1]
```

**题 2：实现一个通用的「查找最大值」函数（用函数指针 + void*）。**
```c
int find_max(void *arr, int len, size_t elem_size,
             int (*cmp)(void *a, void *b));
// 提示：用 (char*) 逐字节偏移
```

**题 3：手写一个 memcpy 函数（考虑内存重叠）。**
```c
void* my_memcpy(void *dest, const void *src, size_t n);
// 要求：正确处理 dest 和 src 重叠的情况
```
