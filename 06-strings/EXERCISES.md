# 06-strings 习题：字符串处理

---

### ✅ 概念题

1. `char *s1 = "hello"` 和 `char s2[] = "hello"` 有什么区别？
2. `strtok` 函数是线程安全的吗？嵌入式中怎么替代？
3. `snprintf` 比 `sprintf` 好在哪？

---

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

---

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
