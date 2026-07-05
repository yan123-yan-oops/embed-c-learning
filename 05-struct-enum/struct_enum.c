/**
 * ============================================================
 * 结构体、枚举、typedef — 嵌入式 C 工程化的基础
 * 学习目标：
 *   1. typedef 给类型起别名
 *   2. struct 组织数据（GPS 数据包、传感器数据）
 *   3. enum 定义状态/命令
 *   4. __packed / 对齐控制（嵌入式寄存器映射的关键）
 *   5. union 共用体（数据解析的利器）
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ========== 1. enum — 枚举状态 ========== */
typedef enum {
    GPS_STATUS_NO_FIX   = 0,
    GPS_STATUS_2D_FIX   = 1,
    GPS_STATUS_3D_FIX   = 2,
} gps_fix_status_t;

typedef enum {
    PARSE_OK       = 0,
    PARSE_ERROR    = -1,
    PARSE_CRC_FAIL = -2,
    PARSE_INCOMPLETE = -3,
} parse_result_t;

/* ========== 2. struct — 结构体组织数据 ========== */
/* GPS 位置数据（模拟 NMEA $GPGGA 的数据结构） */
typedef struct {
    double  latitude;       // 纬度 (度)
    double  longitude;      // 经度 (度)
    float   altitude;       // 海拔 (米)
    float   hdop;           // 水平精度因子
    uint8_t satellites;     // 卫星颗数
    gps_fix_status_t fix_status;  // 定位状态
} gps_position_t;

/* 传感器数据结构 */
typedef struct {
    float   temperature;    // 温度 (℃)
    float   pressure;       // 气压 (hPa)
    float   humidity;       // 湿度 (%)
    uint32_t timestamp_ms;  // 时间戳 (ms)
} sensor_data_t;

/* ========== 3. __packed — 嵌入式寄存器映射 ========== */
/* 模拟 STM32 定时器寄存器映射 (实际单片机上用 __attribute__((packed))) */
#pragma pack(push, 1)  // 1字节对齐（ARM 寄存器必须）
typedef struct {
    volatile uint16_t CR1;    // 控制寄存器 1   偏移 0x00
    volatile uint16_t CR2;    // 控制寄存器 2   偏移 0x02
    volatile uint16_t SMCR;   // 从模式控制     偏移 0x04
    volatile uint16_t DIER;   // 中断使能       偏移 0x06
    volatile uint16_t SR;     // 状态寄存器     偏移 0x08
    volatile uint16_t EGR;    // 事件生成       偏移 0x0A
    volatile uint16_t CCMR1;  // 捕获/比较模式 1 偏移 0x0C
    volatile uint16_t CCMR2;  // 捕获/比较模式 2 偏移 0x0E
    volatile uint16_t CCER;   // 捕获/比较使能   偏移 0x10
    volatile uint32_t CNT;    // 计数器         偏移 0x14
    volatile uint32_t PSC;    // 预分频器       偏移 0x18
    volatile uint32_t ARR;    // 自动重装载      偏移 0x1C
} tim_regs_t;
#pragma pack(pop)

/* ========== 4. union — 数据解析 ========== */
/* 联合体：同一块内存不同解读方式（串口数据拆包的常用技巧） */
typedef union {
    uint8_t  bytes[4];    // 按字节访问
    uint32_t word;        // 按 32 位字访问
    int32_t  signed_word; // 有符号 32 位
    float    float_val;   // 浮点数（常用于传感器数据）
} data_converter_t;

/* ========== Demo ========== */

/* 函数：打印 GPS 数据 */
void print_gps(const gps_position_t *gps)
{
    const char *fix_str[] = {"未定位", "2D 定位", "3D 定位"};

    printf("  📍 纬度: %.6f°\n", gps->latitude);
    printf("  📍 经度: %.6f°\n", gps->longitude);
    printf("  🏔️ 海拔: %.1f m\n", gps->altitude);
    printf("  🛰️ 卫星: %u 颗\n", gps->satellites);
    printf("  📡 定位: %s\n", fix_str[gps->fix_status]);
    printf("  📐 HDOP: %.1f\n", gps->hdop);
}

int main(void)
{
    printf("===== 结构体 / 枚举 / typedef =====\n\n");

    /* ---------- struct 使用 ---------- */
    gps_position_t gps = {
        .latitude   = 28.112233,   // 长沙
        .longitude  = 112.994444,
        .altitude   = 63.5,
        .hdop       = 0.8,
        .satellites = 12,
        .fix_status = GPS_STATUS_3D_FIX,
    };

    printf("--- GPS 位置数据 ---\n");
    print_gps(&gps);

    /* ---------- enum 状态判断 ---------- */
    parse_result_t result = PARSE_OK;
    if (result == PARSE_OK) {
        printf("\n✅ 解析成功\n");
    }

    /* ---------- 寄存器映射结构体 ---------- */
    printf("\n--- 结构体位对齐 ---\n");
    printf("tim_regs_t 结构体大小: %zu 字节\n", sizeof(tim_regs_t));
    printf("  如果不用 packed，会变成 %zu 字节（多了填充）\n",
           sizeof(uint16_t) * 9 + sizeof(uint32_t) * 3 + 6);

    /* ---------- union 数据拆包 ---------- */
    printf("\n--- union 数据转换 ---\n");
    data_converter_t dc;
    dc.word = 0x12345678;
    printf("  word  = 0x%08X\n", dc.word);
    printf("  bytes = 0x%02X 0x%02X 0x%02X 0x%02X\n",
           dc.bytes[0], dc.bytes[1], dc.bytes[2], dc.bytes[3]);

    /* union 用于传感器浮点数据传输（模拟） */
    dc.float_val = 25.6f;  // 温度
    printf("  温度浮点数 = %.2f ℃\n", dc.float_val);
    printf("  以 32 位字发送 = 0x%08X\n", dc.word);
    printf("  接收端转回浮点 = %.2f ℃\n", *(float*)&dc.word);

    printf("\n✅ struct/enum/union 基础学完！\n");
    return 0;
}
