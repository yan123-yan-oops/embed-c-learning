/**
 * ============================================================
 * 环形缓冲区 — 嵌入式串口接收的标配数据结构
 * 学习目标：
 *   1. 什么是环形缓冲区（FIFO，无锁设计）
 *   2. 生产者-消费者模型（中断写入、主循环读取）
 *   3. ISR 安全（单生产者单消费者无锁）
 *   4. 与 FreeRTOS 队列的关系
 *   5. 实战：模拟 UART 中断接收 → NMEA 数据
 * ============================================================
 *
 * FreeRTOS 映射：
 *   本文件 = xQueueGenericCreate / xQueueSendFromISR / xQueueReceive
 *   Ring Buffer 是队列的底层实现，理解它 = 理解 RTOS 队列本质
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* ============================================================
 * 环形缓冲区实现 (单生产者-单消费者，无锁安全)
 * ============================================================ */

typedef struct {
    uint8_t *buffer;        // 数据缓冲区
    volatile uint32_t head; // 写指针（生产者）
    volatile uint32_t tail; // 读指针（消费者）
    uint32_t size;          // 缓冲区大小（必须为 2^n）
    uint32_t mask;          // size - 1（位运算取模）
} ring_buffer_t;

/**
 * 初始化环形缓冲区
 * size 必须是 2 的幂（如 64, 128, 256, 512）
 */
void rb_init(ring_buffer_t *rb, uint8_t *buf, uint32_t size)
{
    rb->buffer = buf;
    rb->size   = size;
    rb->mask   = size - 1;
    rb->head   = 0;
    rb->tail   = 0;
}

/**
 * 写入一个字节（ISR 安全：仅修改 head）
 * 返回 true 成功，false 缓冲区满
 */
bool rb_put(ring_buffer_t *rb, uint8_t data)
{
    uint32_t next_head = (rb->head + 1) & rb->mask;

    // 缓冲区满：head 追上 tail
    if (next_head == rb->tail) {
        return false;  // 溢出
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;
    return true;
}

/**
 * 批量写入（模拟中断批量接收）
 */
uint32_t rb_put_bulk(ring_buffer_t *rb, const uint8_t *data, uint32_t len)
{
    uint32_t written = 0;
    for (uint32_t i = 0; i < len; i++) {
        if (!rb_put(rb, data[i])) break;
        written++;
    }
    return written;
}

/**
 * 读取一个字节（主循环调用：仅修改 tail）
 * 返回 true 成功，false 缓冲区空
 */
bool rb_get(ring_buffer_t *rb, uint8_t *data)
{
    if (rb->head == rb->tail) {
        return false;  // 空
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) & rb->mask;
    return true;
}

/**
 * 当前数据量
 */
uint32_t rb_available(const ring_buffer_t *rb)
{
    return (rb->head - rb->tail) & rb->mask;
}

/**
 * 是否为空
 */
bool rb_empty(const ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

/**
 * 清空缓冲区（仅重置指针，不清数据）
 */
void rb_clear(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

/* ============================================================
 * Demo: 模拟 UART 中断接收 → NMEA 句子
 * ============================================================ */

#define RB_SIZE  256  // 必须是 2 的幂

/* 模拟的 NMEA 数据（UART 会一个字节一个字节到达） */
static const char *mock_uart_data =
    "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76\r\n";

int main(void)
{
    printf("===== 环形缓冲区 — 嵌入式串口接收 =====\n\n");

    /* ---------- 初始化 ---------- */
    static uint8_t rb_pool[RB_SIZE];
    ring_buffer_t uart_rx;

    rb_init(&uart_rx, rb_pool, RB_SIZE);

    /* ---------- 模拟：UART 中断逐字节写入 ---------- */
    printf("--- 模拟 UART 中断接收 ---\n");
    size_t data_len = strlen(mock_uart_data);
    printf("  发送 %zu 字节...\n", data_len);

    for (size_t i = 0; i < data_len; i++) {
        if (!rb_put(&uart_rx, mock_uart_data[i])) {
            printf("  ❌ 缓冲区溢出！丢失数据\n");
            break;
        }
    }
    printf("  缓冲区现有 %u 字节\n", rb_available(&uart_rx));

    /* ---------- 模拟：主循环逐字节读取并拼接 ---------- */
    printf("\n--- 主循环读取 - 拼接完整 NMEA 句子 ---\n");

    char line[128];
    uint32_t idx = 0;
    uint8_t ch;

    printf("  输出: ");

    while (rb_get(&uart_rx, &ch) && idx < sizeof(line) - 1) {
        line[idx++] = (char)ch;
    }
    line[idx] = '\0';

    printf("%s", line);

    /* ---------- 提取字段 ---------- */
    printf("\n--- 提取 NMEA 字段 ---\n");
    char *fields[20];
    int count = 0;
    char *p = line;
    char *start = line;

    // 简单分割（用 strtok 示范）
    char line_copy[128];
    strncpy(line_copy, line, sizeof(line_copy));

    char *token = strtok(line_copy, ",*");
    while (token && count < 20) {
        printf("  字段[%d] = %s\n", count++, token);
        token = strtok(NULL, ",*");
    }

    /* ---------- 与 FreeRTOS 队列的关系 ---------- */
    printf("\n--- 🔗 与 FreeRTOS 的映射 ---\n");
    printf("  Ring Buffer (本文件)        → FreeRTOS xQueue\n");
    printf("  rb_put (中断中写入)         → xQueueSendFromISR\n");
    printf("  rb_get (任务中读取)         → xQueueReceive\n");
    printf("  rb_available                → uxQueueMessagesWaiting\n\n");
    printf("  区别：FreeRTOS 队列支持多任务互斥、阻塞等待\n");
    printf("  Ring Buffer 是它的底层实现，理解它就能理解队列\n");

    printf("\n✅ 环形缓冲区学完！下一步：串口 NMEA 解析器\n");
    return 0;
}
