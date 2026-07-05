/**
 * ============================================================
 * 状态机框架 — 嵌入式协议解析的标准写法
 * 学习目标：
 *   1. 什么是状态机（FSM）— 按键消抖、NMEA 解析、通信协议
 *   2. 状态 + 事件 + 动作的分离
 *   3. 表驱动状态机（可维护性最高）
 *   4. 实战：NMEA 句子逐字节解析
 * ============================================================
 *
 * FreeRTOS 映射：
 *   每个 FreeRTOS 任务本质上是一个状态机：
 *     Ready → Running → Blocked → Ready
 *   GPIO 按键检测、OneWire 协议、GPS 解析都用状态机
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================
 * 应用场景：串口接收 NMEA 句子的状态机
 * 每个字节依次进入，状态机输出完整的句子
 * ============================================================ */

/* NMEA 接收状态 */
typedef enum {
    NMEA_RX_IDLE,       // 等待 $ 起始符
    NMEA_RX_DATA,       // 接收数据（$ 之后到 * 之前）
    NMEA_RX_CHECKSUM,   // 接收校验和（* 之后）
    NMEA_RX_COMPLETE,   // 一行完成（收到 \n）
} nmea_rx_state_t;

/* NMEA 接收器上下文 */
typedef struct {
    nmea_rx_state_t state;
    char    buffer[128];     // 累积一行
    uint8_t idx;             // 当前写入位置
    uint8_t crc;             // 累计校验和
    uint8_t crc_received;    // 收到的校验和
    bool    has_checksum;    // 是否有 * 校验段
} nmea_rx_t;

void nmea_rx_init(nmea_rx_t *rx)
{
    rx->state  = NMEA_RX_IDLE;
    rx->idx    = 0;
    rx->crc    = 0;
    rx->crc_received = 0;
    rx->has_checksum  = false;
    memset(rx->buffer, 0, sizeof(rx->buffer));
}

/**
 * 核心：逐字节喂入状态机
 * 返回 true 表示一行完整句子已经接收完毕
 */
bool nmea_rx_feed(nmea_rx_t *rx, uint8_t ch)
{
    switch (rx->state) {

        case NMEA_RX_IDLE:
            if (ch == '$') {
                rx->state = NMEA_RX_DATA;
                rx->idx   = 0;
                rx->crc   = 0;
                rx->has_checksum = false;
                rx->buffer[rx->idx++] = (char)ch;
            }
            break;

        case NMEA_RX_DATA:
            if (ch == '*') {
                /* 开始校验和段 */
                rx->has_checksum = true;
                rx->state = NMEA_RX_CHECKSUM;
                rx->buffer[rx->idx++] = (char)ch;
                rx->crc_received = 0;
            } else if (ch == '\r' || ch == '\n') {
                /* 无校验和的句子，直接结束 */
                rx->state = NMEA_RX_COMPLETE;
                rx->buffer[rx->idx] = '\0';
                return true;
            } else if (rx->idx < sizeof(rx->buffer) - 4) {
                rx->crc ^= ch;  // 累加校验和
                rx->buffer[rx->idx++] = (char)ch;
            } else {
                /* 缓冲区满，重置 */
                rx->state = NMEA_RX_IDLE;
            }
            break;

        case NMEA_RX_CHECKSUM:
            if (ch == '\r' || ch == '\n') {
                rx->state = NMEA_RX_COMPLETE;
                rx->buffer[rx->idx] = '\0';
                return true;
            } else if (rx->idx < sizeof(rx->buffer) - 1) {
                rx->buffer[rx->idx++] = (char)ch;
            }
            break;

        case NMEA_RX_COMPLETE:
            /* 上一行已取走，重新开始 */
            nmea_rx_init(rx);
            if (ch == '$') {
                rx->state = NMEA_RX_DATA;
                rx->buffer[rx->idx++] = (char)ch;
            }
            break;
    }

    return false;
}

/* ============================================================
 * 通用状态机框架（表驱动）
 * 适合：按键检测、通信协议、菜单导航
 * ============================================================ */

/* 状态和事件（用 enum 定义，清晰可扩展） */
typedef enum {
    STATE_IDLE,
    STATE_ACTIVE,
    STATE_ERROR,
} app_state_t;

typedef enum {
    EVENT_START,
    EVENT_STOP,
    EVENT_TIMEOUT,
    EVENT_DATA_READY,
} app_event_t;

/* 状态转移表的一行 */
typedef struct {
    app_state_t  current_state;
    app_event_t  event;
    app_state_t  next_state;
    void (*action)(void);     // 动作函数指针
} state_transition_t;

/* 动作函数 */
static void action_start(void)    { printf("  ▶️ 启动系统\n"); }
static void action_stop(void)     { printf("  ⏹️ 停止系统\n"); }
static void action_error(void)    { printf("  ⚠️ 进入错误状态！\n"); }
static void action_process(void)  { printf("  🔄 处理数据\n"); }

/* 状态转移表（可读性极高，加一行就是加一种情况） */
static const state_transition_t state_table[] = {
    { STATE_IDLE,   EVENT_START,     STATE_ACTIVE, action_start   },
    { STATE_ACTIVE, EVENT_STOP,      STATE_IDLE,   action_stop    },
    { STATE_ACTIVE, EVENT_DATA_READY, STATE_ACTIVE, action_process },
    { STATE_ACTIVE, EVENT_TIMEOUT,   STATE_ERROR,  action_error   },
    { STATE_ERROR,  EVENT_START,     STATE_IDLE,   action_start   },
    { STATE_ERROR,  EVENT_STOP,      STATE_IDLE,   action_stop    },
    /* 最后一行：状态守卫 */
    { (app_state_t)-1, (app_event_t)-1, (app_state_t)-1, NULL },
};

app_state_t state_run_event(app_state_t current, app_event_t event)
{
    for (int i = 0; state_table[i].action != NULL; i++) {
        if (state_table[i].current_state == current &&
            state_table[i].event == event) {

            printf("  状态: %d → %d，触发 ", current, state_table[i].next_state);
            if (state_table[i].action) {
                state_table[i].action();
            }
            return state_table[i].next_state;
        }
    }
    printf("  ⚠️ 未定义转移: state=%d, event=%d\n", current, event);
    return current;  // 保持当前状态
}

/* ============================================================
 * Demo
 * ============================================================ */
int main(void)
{
    printf("===== 状态机 — 嵌入式协议解析的核心 =====\n\n");

    /* ---------- Demo 1：NMEA 逐字节解析 ---------- */
    printf("--- NMEA 逐字节接收状态机 ---\n");

    const char *nema_line = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8*74\r\n";
    nmea_rx_t rx;
    int line_count = 0;

    nmea_rx_init(&rx);
    printf("  输入: %s", nema_line);

    const char *p = nema_line;
    while (*p) {
        if (nmea_rx_feed(&rx, (uint8_t)*p)) {
            printf("  ✅ 接收完整一行: %s\n", rx.buffer);
            line_count++;
        }
        p++;
    }

    printf("  共接收 %d 行\n\n", line_count);

    /* ---------- Demo 2：表驱动状态机 ---------- */
    printf("--- 表驱动状态机 ---\n");
    app_state_t state = STATE_IDLE;

    printf("  初始: IDLE\n");
    state = state_run_event(state, EVENT_START);
    state = state_run_event(state, EVENT_DATA_READY);
    state = state_run_event(state, EVENT_TIMEOUT);
    state = state_run_event(state, EVENT_START);
    state = state_run_event(state, EVENT_STOP);

    /* ---------- 状态机在 RTOS 中的使用 ---------- */
    printf("\n--- 🔗 与 FreeRTOS 的映射 ---\n");
    printf("  一个 RTOS 任务 = 一个状态机:\n");
    printf("    while(1) {\n");
    printf("      读取队列/信号量 (接收事件)\n");
    printf("      状态机处理               (状态切换)\n");
    printf("      输出到队列/GPIO        (执行动作)\n");
    printf("    }\n\n");
    printf("  应用场景:\n");
    printf("    - 北斗模块数据解析\n");
    printf("    - 按键消抖与长按检测\n");
    printf("    - UART 协议帧解析\n");
    printf("    - 电池充电管理\n");

    printf("\n✅ 状态机学完！这把嵌入式解析的终极武器\n");
    return 0;
}
