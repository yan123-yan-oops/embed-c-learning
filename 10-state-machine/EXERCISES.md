# 10-state-machine 习题：状态机

---

### ✅ 概念题

1. 什么是有限状态机？哪些场景用状态机实现？
2. 状态机的三要素是什么？
3. 用 `switch-case` 实现和用函数指针表实现各有什么优缺点？

---

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
