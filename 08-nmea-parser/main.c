/**
 * ============================================================
 * main.c — NMEA 解析器使用示例
 * ============================================================
 * 编译方法（多文件编译）:
 *   gcc -Wall -std=c11 main.c nmea_parser.c -o nmea_demo.exe
 * ============================================================
 */

#include "nmea_parser.h"
#include <stdio.h>
#include <string.h>

/* 模拟 UART 收到的 NMEA 数据流 */
static const char *mock_nmea_data[] = {
    "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76",
    "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280616,,,A*47",
    "$GPGGA,093000.000,5545.1234,N,03737.5678,E,2,12,0.85,185.3,M,48.7,M,,*66",
    "$GPGGA,093015.000,,,,,0,0,99.99,,,,,,*68",  // 无效数据（无定位）
};

int main(void)
{
    printf("===== NMEA 0183 协议解析器 Demo =====\n");
    printf("  🔧 阶段 1 交付物 — 简历可直接使用\n\n");

    int total = sizeof(mock_nmea_data) / sizeof(mock_nmea_data[0]);
    int success = 0;

    for (int i = 0; i < total; i++) {
        const char *sentence = mock_nmea_data[i];
        gps_data_t gps;

        printf("────────────────────────────────\n");
        printf("[%d] 原始: %s\n", i + 1, sentence);

        nmea_status_t status = nmea_parse(sentence, &gps);

        switch (status) {
            case NMEA_OK:
                printf("  状态: ✅ 解析成功\n");
                nmea_print_gps(&gps);
                success++;
                break;
            case NMEA_CRC_FAIL:
                printf("  状态: ❌ 校验和失败\n");
                break;
            case NMEA_INVALID:
                printf("  状态: ❌ 无效数据\n");
                break;
            default:
                printf("  状态: ❌ 解析错误\n");
                break;
        }
        printf("\n");
    }

    printf("────────────────────────────────\n");
    printf("📊 统计: 共 %d 条，成功 %d 条，失败 %d 条\n",
           total, success, total - success);

    printf("\n✅ Demo 运行完成！\n");
    printf("   这个模块可以直接移植到 STM32 + FreeRTOS 上，\n");
    printf("   替换 printf → UART 输出，malloc → 静态内存池\n");
    return 0;
}
