/**
 * ============================================================
 * main.c — NMEA 0183 解析器 Demo（增强版）
 *
 * 编译:
 *   gcc -Wall -std=c11 main.c nmea_parser.c -o nmea_demo.exe
 *
 * 运行模式:
 *   nmea_demo.exe              — 内置示例数据跑一遍
 *   nmea_demo.exe log.nmea     — 从文件读取真实 GPS 日志
 *   nmea_demo.exe --kml log.nmea — 生成 Google Earth 轨迹
 * ============================================================
 */

#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========== 模式 1：内置示例数据快速演示 ========== */
static void demo_builtin(void)
{
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║      NMEA 0183 协议解析器 · 功能演示         ║\n");
    printf("║      阶段 1 交付物 — 简历可直接使用           ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");

    static const char *test_data[] = {
        // 正常 GPS 定位
        "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76",
        // RMC 语句（含速度航向）
        "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280616,,,A*47",
        // 差分定位 12 颗星
        "$GPGGA,093000.000,5545.1234,N,03737.5678,E,2,12,0.85,185.3,M,48.7,M,,*66",
        // 无定位（校验和正确但数据无效）
        "$GPGGA,093015.000,,,,,0,0,99.99,,,,,,*68",
        // 校验和错误（篡改最后一位）
        "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*77",
    };

    int n = sizeof(test_data) / sizeof(test_data[0]);
    int ok = 0, crc_err = 0, invalid = 0;

    for (int i = 0; i < n; i++) {
        printf("┌─ [%d] 原始数据 ──────────────────────┐\n", i + 1);
        printf("│ %s\n", test_data[i]);

        gps_data_t gps;
        int ret = nmea_parse(test_data[i], &gps);

        switch (ret) {
            case NMEA_OK:
                printf("├─ ✅ 解析成功 ─────────────────────────┤\n");
                nmea_print_gps(&gps);
                ok++;
                break;
            case NMEA_CRC_FAIL:
                printf("├─ ❌ 校验和失败 ──────────────────────┤\n");
                printf("  → 数据可能在传输中被篡改\n");
                crc_err++;
                break;
            case NMEA_INVALID:
                printf("├─ ⚠️  数据无效 ───────────────────────┤\n");
                nmea_print_gps(&gps);
                invalid++;
                break;
            default:
                printf("├─ ❌ 解析错误 ────────────────────────┤\n");
                break;
        }
        printf("└──────────────────────────────────────────┘\n\n");
    }

    printf("════════════════════════════════════════════\n");
    printf("  📊 统计: 共 %d 条\n", n);
    printf("  ✅ 成功: %d\n", ok);
    printf("  ⚠️  无效: %d\n", invalid);
    printf("  ❌ CRC错: %d\n", crc_err);
    printf("  ✅ 通过率: %.0f%%\n", (float)ok / n * 100);
    printf("════════════════════════════════════════════\n\n");
}

/* ========== 模式 2：从文件读取真实日志 ========== */
static int demo_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("❌ 无法打开文件: %s\n", filename);
        return 1;
    }

    printf("📂 正在处理: %s\n\n", filename);

    char line[256];
    int total = 0, ok = 0, crc_err = 0, invalid = 0;
    int gpgga_cnt = 0, gprmc_cnt = 0;

    while (fgets(line, sizeof(line), fp)) {
        // 去掉换行符
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        if (len > 1 && line[len-2] == '\r') line[len-2] = '\0';

        // 只处理 NMEA 语句
        if (line[0] != '$') continue;
        total++;

        gps_data_t gps;
        int ret = nmea_parse(line, &gps);

        switch (ret) {
            case NMEA_OK:
                ok++;
                if (strstr(line, "GPGGA")) gpgga_cnt++;
                if (strstr(line, "GPRMC")) gprmc_cnt++;
                break;
            case NMEA_CRC_FAIL: crc_err++; break;
            case NMEA_INVALID:   invalid++; break;
        }
    }
    fclose(fp);

    printf("════════════════════════════════════════════\n");
    printf("  📊 文件处理统计\n");
    printf("  📄 文件: %s\n", filename);
    printf("  🔢 总语句: %d\n", total);
    printf("  ✅ 成功:   %d (GPGGA:%d, GPRMC:%d)\n", ok, gpgga_cnt, gprmc_cnt);
    printf("  ⚠️  无效:   %d\n", invalid);
    printf("  ❌ CRC错:  %d\n", crc_err);
    printf("  ✅ 通过率: %.1f%%\n", ok * 100.0 / (total ? total : 1));
    printf("════════════════════════════════════════════\n");
    return 0;
}

/* ========== 模式 3：生成 KML（Google Earth 轨迹）========== */
static int demo_kml(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("❌ 无法打开文件: %s\n", filename);
        return 1;
    }

    // 输出 KML 文件
    const char *kml_name = "gps_track.kml";
    FILE *kml = fopen(kml_name, "w");
    if (!kml) {
        printf("❌ 无法创建 KML 文件\n");
        fclose(fp);
        return 1;
    }

    fprintf(kml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(kml, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(kml, "  <Document>\n");
    fprintf(kml, "    <name>GPS Track - %s</name>\n", filename);
    fprintf(kml, "    <Placemark>\n");
    fprintf(kml, "      <name>轨迹</name>\n");
    fprintf(kml, "      <LineString>\n");
    fprintf(kml, "        <extrude>1</extrude>\n");
    fprintf(kml, "        <tessellate>1</tessellate>\n");
    fprintf(kml, "        <coordinates>\n");

    char line[256];
    int points = 0;

    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
        if (len > 1 && line[len-2] == '\r') line[len-2] = '\0';
        if (line[0] != '$') continue;

        gps_data_t gps;
        if (nmea_parse(line, &gps) == NMEA_OK && gps.valid) {
            // KML 坐标: 经度,纬度,海拔
            fprintf(kml, "          %.6f,%.6f,%.1f\n",
                    gps.longitude, gps.latitude, gps.altitude);
            points++;
        }
    }

    fprintf(kml, "        </coordinates>\n");
    fprintf(kml, "      </LineString>\n");
    fprintf(kml, "    </Placemark>\n");
    fprintf(kml, "  </Document>\n");
    fprintf(kml, "</kml>\n");

    fclose(kml);
    fclose(fp);

    printf("✅ 已生成轨迹文件: %s\n", kml_name);
    printf("   📍 轨迹点数: %d\n", points);
    printf("   🗺️  用 Google Earth 打开即可看到路线\n");
    return 0;
}

/* ========== 程序入口 ========== */
int main(int argc, char **argv)
{
    if (argc == 1) {
        // 无参数：内置演示
        demo_builtin();
        return 0;
    }

    if (argc == 2) {
        // 一个参数：从文件读取
        return demo_file(argv[1]);
    }

    if (argc == 3 && strcmp(argv[1], "--kml") == 0) {
        // --kml 模式：生成 Google Earth 轨迹
        return demo_kml(argv[2]);
    }

    printf("用法:\n");
    printf("  %s              — 内置数据演示\n", argv[0]);
    printf("  %s log.nmea     — 从文件解析 GPS 日志\n", argv[0]);
    printf("  %s --kml log.nmea — 生成 Google Earth 轨迹\n", argv[0]);
    return 1;
}
