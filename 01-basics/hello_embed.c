/**
 * ============================================================
 * 嵌入式 C 入门 — 基本结构 + 变量 + 类型
 * 学习目标：
 *   1. C 程序的基本结构（main、include、printf）
 *   2. 嵌入式常用数据类型（uint8_t、uint16_t、uint32_t）
 *   3. sizeof 查看各类型在内存中的大小
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>   // 嵌入式标配：uint8_t, uint16_t, uint32_t 等
#include <stdbool.h>  // bool 类型

int main(void)
{
    printf("===== 嵌入式 C 开发环境搭建成功 =====\n\n");

    /* ---------- 基本数据类型 ---------- */
    // 嵌入式开发中，明确变量宽度非常重要（寄存器、内存对齐）
    uint8_t  a = 255;       // 8位  无符号  0 ~ 255
    uint16_t b = 65535;     // 16位 无符号  0 ~ 65535
    uint32_t c = 4294967295U; // 32位 无符号
    int8_t   d = -128;       // 8位  有符号
    bool     flag = true;    // 布尔值

    printf("uint8_t  a = %u    (size: %zu 字节)\n", a, sizeof(a));  //%d是用于有符号整数，可以打印整数负数
    printf("uint16_t b = %u   (size: %zu 字节)\n", b, sizeof(b));   //%u用于无符号整数，只能打印非负数
    printf("uint32_t c = %u   (size: %zu 字节)\n", c, sizeof(c));
    printf("int8_t   d = %d   (size: %zu 字节)\n", d, sizeof(d));
    printf("bool  flag = %d   (size: %zu 字节)\n", flag, sizeof(flag));

    /* ---------- volatile — 嵌入式专属关键字 ---------- */
    // 告诉编译器不要优化这个变量（寄存器、中断共享变量）
    volatile uint32_t systick_counter = 0;
    (void)systick_counter;  // 防止 "unused" 警告

    /* ---------- static — 嵌入式常用 ---------- */
    // 函数内的 static 变量：只初始化一次，调用间保持值
    for (int i = 0; i < 3; i++) {
        static int call_count = 0;
        call_count++;
        printf("第 %d 次调用，call_count = %d\n", i + 1, call_count);
    }

    printf("\n✅ 环境就绪，可以开始学习 C 基础了！\n");
    return 0;
}
