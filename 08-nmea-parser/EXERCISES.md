# 08-nmea-parser 习题：NMEA 0183 解析器

---

### ✅ 概念题

1. NMEA 0183 的帧格式是什么？`$`、`,`、`*` 分别什么作用？
2. 什么是 CRC 校验？`$GPGGA` 的校验怎么算？
3. 模块化设计中 `.h` 文件里通常放什么？`.c` 文件里放什么？

---

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
