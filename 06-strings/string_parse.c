/**
 * ============================================================
 * 字符串处理 — NMEA 解析的必备前置知识
 * 学习目标：
 *   1. 字符串基础：strlen, strcpy, strcmp
 *   2. strstr 找子串（找 $GPGGA / $GPRMC）
 *   3. strtok / strchr 拆分逗号字段（NMEA 解析核心）
 *   4. sscanf 格式化提取（纬度、经度、时间）
 *   5. 手动解析 vs 库函数（嵌入式里有时自己写更可靠）
 * ============================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* ========== 典型 NMEA 句子 (GPS 定位数据) ========== */
static const char *nema_example =
    "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76\r\n"
    "$GPGSA,A,3,10,07,05,02,29,04,24,31,,,,,1.23,1.03,0.67*00\r\n"
    "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280616,,,A*43\r\n";

/* ========== 手动解析 NMEA 数据 ========== */

/**
 * 拆分逗号分隔的字段（嵌入式写法，不用 strtok）
 * 这是面试高频考点：手动实现 NMEA 字段提取
 *
 * 参数:
 *   input  - 原始 NMEA 句子 (如 "$GPGGA,092750,...")
 *   fields - 输出字段数组 (最多 NMEA_MAX_FIELDS 个)
 *   max_fields - 最多解析多少个字段
 * 返回: 实际字段数
 */
#define NMEA_MAX_FIELDS  20

int split_nmea_fields(const char *input, char *fields[], int max_fields)
{
    int count = 0;
    const char *p = input;
    const char *start = input;

    if (!input || max_fields <= 0) return 0;

    while (*p && count < max_fields) {
        if (*p == ',' || *p == '\r' || *p == '\n' || *p == '*') {
            // 提取当前字段（使用 strndup）
            size_t len = p - start;
            fields[count] = (char*)malloc(len + 1);
            if (fields[count]) {
                strncpy(fields[count], start, len);
                fields[count][len] = '\0';
                count++;
            }
            start = p + 1;

            if (*p == '*') break;  // 后面是校验和，结束
        }
        p++;
    }

    /* 处理最后一个字段 */
    if (p > start && count < max_fields) {
        size_t len = p - start;
        fields[count] = (char*)malloc(len + 1);
        if (fields[count]) {
            strncpy(fields[count], start, len);
            fields[count][len] = '\0';
            count++;
        }
    }

    return count;
}

void free_fields(char *fields[], int count)
{
    for (int i = 0; i < count; i++) {
        free(fields[i]);
    }
}

/* ========== 校验和计算 ========== */
/* NMEA 校验和: $ 之后到 * 之前的字符异或 */
uint8_t nmea_checksum(const char *sentence)
{
    uint8_t crc = 0;
    const char *p = sentence;

    if (*p == '$') p++;  // 跳过 $

    while (*p && *p != '*') {
        crc ^= (uint8_t)(*p);
        p++;
    }
    return crc;
}

int main(void)
{
    printf("===== 字符串处理 — NMEA 解析前置 =====\n\n");

    /* ---------- 1. 基础操作 ---------- */
    const char *sentence = "$GPGGA,092750.000,5321.6802,N";

    printf("--- 字符串基础 ---\n");
    printf("  原始: %s\n", sentence);
    printf("  长度: %zu 字符\n", strlen(sentence));

    /* 搜索子串 */
    if (strstr(sentence, "GPGGA")) {
        printf("  包含 GPGGA 语句 ✅\n");
    }

    /* 找逗号位置 */
    const char *first_comma = strchr(sentence, ',');
    if (first_comma) {
        int pos = first_comma - sentence;
        printf("  第一个逗号位置: %d\n", pos);
    }

    /* ---------- 2. NMEA 句子完整解析 ---------- */
    printf("\n--- NMEA 句子解析 ---\n");
    printf("原始: %s\n", nema_example);

    /* 取出第一行 ($GPGGA) */
    char first_line[128];
    sscanf(nema_example, "%127[^\n]", first_line);
    printf("第一行: %s\n", first_line);

    /* ---------- 3. 拆分字段 ---------- */
    printf("\n--- 拆分字段 (手动实现) ---\n");

    char *fields[NMEA_MAX_FIELDS];
    int count = split_nmea_fields(first_line, fields, NMEA_MAX_FIELDS);

    /* 打印字段 */
    for (int i = 0; i < count; i++) {
        printf("  字段[%d] = \"%s\"\n", i, fields[i]);
    }
    free_fields(fields, count);

    /* ---------- 4. sscanf 格式化提取 ---------- */
    printf("\n--- sscanf 提取定位数据 ---\n");
    char time_str[16] = {0};
    char lat_str[16] = {0};
    char ns = 0, ew = 0;
    int fix_quality = 0;
    int sat_count = 0;

    // $GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,...
    int parsed = sscanf(first_line,
        "$GPGGA,%[^,],%[^,],%c,%[^,],%c,%d,%d",
        time_str, lat_str, &ns, lat_str, &ew, &fix_quality, &sat_count);

    printf("  sscanf 解析成功 %d 项\n", parsed);

    /* ---------- 5. 计算校验和 ---------- */
    printf("\n--- NMEA 校验和 ---\n");
    const char *check_line = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    uint8_t calc_crc = nmea_checksum(check_line);
    printf("  句子: %s\n", check_line);
    printf("  计算校验和: 0x%02X\n", calc_crc);
    printf("  句子中校验和: 0x76\n");
    printf("  匹配: %s\n", (calc_crc == 0x76) ? "✅ 通过" : "❌ 失败");

    /* ---------- 6. 注意：嵌入式里 strtok 有坑 ---------- */
    printf("\n--- ⚠️ 嵌入式注意事项 ---\n");
    printf("  strtok 会修改原字符串（不可重入）\n");
    printf("  多任务环境用 strtok_r（FreeRTOS 里必须用这个）\n");
    printf("  或者像上面自己写的手动分割（面试常考）\n");

    printf("\n✅ 字符串处理学完！下一步：串口接收 + NMEA 解析\n");
    return 0;
}
