/**
 * ============================================================
 * nmea_parser.h — NMEA 0183 协议解析器
 * ============================================================
 * 这是你阶段 1 的交付物：
 *   ✅ 模块化 .h/.c 分离设计
 *   ✅ 支持 $GPGGA / $GPRMC 解析
 *   ✅ CRC 校验
 *   ✅ 内存管理（字段提取 + 释放）
 *   ✅ 可直接移植到 ARM + FreeRTOS
 * ============================================================
 */

#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 解析结果码 ========== */
typedef enum {
    NMEA_OK       = 0,
    NMEA_ERROR    = -1,
    NMEA_CRC_FAIL = -2,
    NMEA_INVALID  = -3,
} nmea_status_t;

/* ========== GPS 定位数据结构 ========== */
typedef struct {
    uint8_t  hour;          // UTC 时
    uint8_t  minute;        // 分
    uint8_t  second;        // 秒
    uint16_t millisecond;   // 毫秒

    double   latitude;      // 纬度 (度)
    double   longitude;     // 经度 (度)
    char     ns_indicator;  // 'N' 或 'S'
    char     ew_indicator;  // 'E' 或 'W'

    uint8_t  fix_quality;   // 定位质量 0=无 1=GPS 2=DGPS
    uint8_t  satellites;    // 跟踪到的卫星数
    float    hdop;          // 水平精度因子
    float    altitude;      // 海拔 (米)

    float    speed_knot;    // 对地速度 (节) — 来自 $GPRMC
    float    course_deg;    // 对地航向 (度) — 来自 $GPRMC

    bool     valid;         // 数据是否有效
} gps_data_t;

/* ========== API 接口 ========== */

/**
 * 解析一条 NMEA 句子
 * @param sentence  原始 NMEA 字符串 (如 "$GPGGA,...*76\r\n")
 * @param output    输出解析结果
 * @return nmea_status_t
 */
nmea_status_t nmea_parse(const char *sentence, gps_data_t *output);

/**
 * 计算 NMEA 校验和
 * @param sentence  $ 和 * 之间的内容
 * @return 校验和
 */
uint8_t nmea_calc_checksum(const char *sentence);

/**
 * GPS 数据格式化输出
 */
void nmea_print_gps(const gps_data_t *gps);

#ifdef __cplusplus
}
#endif

#endif /* NMEA_PARSER_H */
