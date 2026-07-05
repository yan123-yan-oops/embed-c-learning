/**
 * ============================================================
 * nmea_parser.c — NMEA 0183 协议解析器实现
 * ============================================================
 * 支持语句:
 *   $GPGGA — GPS 定位数据
 *   $GPRMC — 推荐最小定位数据
 * ============================================================
 */

#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ========== 校验和 ========== */
uint8_t nmea_calc_checksum(const char *sentence)
{
    uint8_t crc = 0;
    const char *p = sentence;

    if (*p == '$') p++;

    while (*p && *p != '*') {
        crc ^= (uint8_t)(*p);
        p++;
    }
    return crc;
}

/* ========== 提取逗号分隔字段（无动态分配版本）========== */
typedef struct {
    char   data[16][32];  // 最多 16 个字段，每字段最长 31 字符
    int    count;
} nmea_fields_t;

static int parse_fields(const char *sentence, nmea_fields_t *fields)
{
    int field_idx = 0;
    int char_idx  = 0;
    const char *p = sentence;

    if (!sentence || !fields) return 0;

    memset(fields, 0, sizeof(nmea_fields_t));

    if (*p == '$') p++;  // 跳过 $

    while (*p && field_idx < 16) {
        if (*p == ',' || *p == '*' || *p == '\r' || *p == '\n') {
            fields->data[field_idx][char_idx] = '\0';
            field_idx++;
            char_idx = 0;

            if (*p == '*') break;  // 校验和段，不继续解析
        } else {
            if (char_idx < 31) {
                fields->data[field_idx][char_idx++] = *p;
            }
        }
        p++;
    }

    fields->count = field_idx;
    return field_idx;
}

/* ========== 字符串转浮点（NMEA 特殊格式）========== */
static double nmea_lat_to_deg(const char *lat_str, char ns)
{
    /* NMEA 纬度格式: DDMM.MMMM
     * 如 5321.6802 → 53 + 21.6802 / 60 = 53.361337
     */
    if (!lat_str || strlen(lat_str) < 4) return 0.0;

    char deg_str[4] = {0};
    strncpy(deg_str, lat_str, 2);        // "53"
    int degrees = atoi(deg_str);

    double minutes = atof(lat_str + 2);  // 21.6802
    double result = degrees + minutes / 60.0;

    if (ns == 'S') result = -result;
    return result;
}

static double nmea_lon_to_deg(const char *lon_str, char ew)
{
    /* NMEA 经度格式: DDDMM.MMMM
     * 如 00630.3372 → 006 + 30.3372 / 60 = 6.50562
     */
    if (!lon_str || strlen(lon_str) < 5) return 0.0;

    char deg_str[4] = {0};
    strncpy(deg_str, lon_str, 3);        // "006"
    int degrees = atoi(deg_str);

    double minutes = atof(lon_str + 3);  // 30.3372
    double result = degrees + minutes / 60.0;

    if (ew == 'W') result = -result;
    return result;
}

/* ========== 解析 $GPGGA ========== */
static nmea_status_t parse_gpgga(const nmea_fields_t *fields, gps_data_t *gps)
{
    /* $GPGGA,time,lat,NS,lon,EW,qual,sat,hdop,alt,M,geoid,M,,*xx */
    if (fields->count < 14) return NMEA_ERROR;

    // 时间 092750.000 → 09:27:50.000
    const char *time_str = fields->data[1];
    if (strlen(time_str) >= 6) {
        char buf[4] = {0};
        strncpy(buf, time_str, 2);  gps->hour   = (uint8_t)atoi(buf);
        strncpy(buf, time_str+2, 2); gps->minute = (uint8_t)atoi(buf);
        strncpy(buf, time_str+4, 2); gps->second = (uint8_t)atoi(buf);

        if (strlen(time_str) > 7) {
            gps->millisecond = (uint16_t)(atof(time_str + 6) * 1000);
        }
    }

    // 纬度
    gps->latitude     = nmea_lat_to_deg(fields->data[2], fields->data[3][0]);
    gps->ns_indicator = fields->data[3][0];

    // 经度
    gps->longitude     = nmea_lon_to_deg(fields->data[4], fields->data[5][0]);
    gps->ew_indicator  = fields->data[5][0];

    // 定位质量
    gps->fix_quality = (uint8_t)atoi(fields->data[6]);

    // 卫星数
    gps->satellites = (uint8_t)atoi(fields->data[7]);

    // HDOP
    gps->hdop = (float)atof(fields->data[8]);

    // 海拔
    gps->altitude = (float)atof(fields->data[9]);

    gps->valid = (gps->fix_quality > 0);
    return NMEA_OK;
}

/* ========== 解析 $GPRMC ========== */
static nmea_status_t parse_gprmc(const nmea_fields_t *fields, gps_data_t *gps)
{
    /* $GPRMC,time,status,lat,NS,lon,EW,speed,course,date,,,*xx */
    if (fields->count < 12) return NMEA_ERROR;

    // 状态
    if (fields->data[2][0] != 'A') {  // A=有效, V=无效
        gps->valid = false;
        return NMEA_OK;
    }

    // 时间
    const char *time_str = fields->data[1];
    if (strlen(time_str) >= 6) {
        char buf[4] = {0};
        strncpy(buf, time_str, 2);  gps->hour   = (uint8_t)atoi(buf);
        strncpy(buf, time_str+2, 2); gps->minute = (uint8_t)atoi(buf);
        strncpy(buf, time_str+4, 2); gps->second = (uint8_t)atoi(buf);
    }

    // 纬度/经度
    gps->latitude     = nmea_lat_to_deg(fields->data[3], fields->data[4][0]);
    gps->ns_indicator = fields->data[4][0];
    gps->longitude    = nmea_lon_to_deg(fields->data[5], fields->data[6][0]);
    gps->ew_indicator = fields->data[6][0];

    // 速度（节）
    gps->speed_knot = (float)atof(fields->data[7]);

    // 航向
    gps->course_deg = (float)atof(fields->data[8]);

    gps->valid = true;
    return NMEA_OK;
}

/* ========== 主解析入口 ========== */
nmea_status_t nmea_parse(const char *sentence, gps_data_t *output)
{
    if (!sentence || !output || strlen(sentence) < 10) {
        return NMEA_INVALID;
    }

    memset(output, 0, sizeof(gps_data_t));

    /* 校验和检查 */
    const char *star = strchr(sentence, '*');
    if (star && strlen(star) >= 3) {
        uint8_t msg_crc = (uint8_t)strtol(star + 1, NULL, 16);
        uint8_t calc_crc = nmea_calc_checksum(sentence);
        if (msg_crc != calc_crc) {
            return NMEA_CRC_FAIL;
        }
    }

    /* 解析字段 */
    nmea_fields_t fields;
    parse_fields(sentence, &fields);

    if (fields.count < 2) return NMEA_INVALID;

    /* 判断语句类型 */
    const char *type = fields.data[0];

    if (strcmp(type, "GPGGA") == 0) {
        return parse_gpgga(&fields, output);
    } else if (strcmp(type, "GPRMC") == 0) {
        return parse_gprmc(&fields, output);
    }

    return NMEA_ERROR;  // 不支持的语句类型
}

/* ========== 打印结果 ========== */
void nmea_print_gps(const gps_data_t *gps)
{
    if (!gps) return;

    printf("  ⏱  时间: %02u:%02u:%02u.%03u\n",
           gps->hour, gps->minute, gps->second, gps->millisecond);
    printf("  📍 纬度: %.6f° %c\n", gps->latitude, gps->ns_indicator);
    printf("  📍 经度: %.6f° %c\n", gps->longitude, gps->ew_indicator);
    printf("  🛰️ 卫星: %u 颗\n", gps->satellites);
    printf("  📡 定位: %s\n",
           gps->fix_quality == 0 ? "无定位" :
           gps->fix_quality == 1 ? "GPS定位" : "差分定位");
    printf("  🏔️ 海拔: %.1f 米\n", gps->altitude);
    printf("  📐 HDOP: %.1f\n", gps->hdop);

    if (gps->speed_knot > 0) {
        printf("  🚀 速度: %.1f 节 (%.1f km/h)\n",
               gps->speed_knot, gps->speed_knot * 1.852);
    }
    if (gps->course_deg > 0) {
        printf("  🧭 航向: %.1f°\n", gps->course_deg);
    }
}
