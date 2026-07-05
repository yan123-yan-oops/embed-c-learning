/**
 * ============================================================
 * 位操作 — 嵌入式开发每天都要用的基本功
 * 学习目标：
 *   1. &  |  ~  ^  <<  >>
 *   2. 置位 / 清位 / 翻转 / 读取特定位
 *   3. 位域操作（连续位的提取与写入）
 *   4. 宏封装 — 嵌入式 HAL 库的标配写法
 *   5. 实际场景：GPIO / 状态寄存器
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>

/* ========== 典型嵌入式位操作宏 ========== */
#define BIT(n)              (1UL << (n))       // 第 n 位的掩码
#define SET_BIT(reg, bit)   ((reg) |= BIT(bit))   // 置位
#define CLR_BIT(reg, bit)   ((reg) &= ~BIT(bit))  // 清位
#define TGL_BIT(reg, bit)   ((reg) ^= BIT(bit))   // 翻转
#define GET_BIT(reg, bit)   (((reg) >> (bit)) & 1U)  // 读某位

/* 连续位操作（比如从寄存器中提取一段值） */
#define GET_BITS(reg, high, low) \
    (((reg) >> (low)) & ((1UL << ((high) - (low) + 1)) - 1))

#define SET_BITS(reg, high, low, val) \
    ((reg) = ((reg) & ~(((1UL << ((high)-(low)+1)) - 1) << (low))) | \
             ((val) << (low)))

void print_binary(uint32_t val, int bits)
{
    for (int i = bits - 1; i >= 0; i--) {
        putchar((val & BIT(i)) ? '1' : '0');
        if (i % 4 == 0) putchar(' ');
    }
}

int main(void)
{
    printf("===== 位操作 — 嵌入式 C 基本功 =====\n\n");

    /* ---------- 模拟一个寄存器 ---------- */
    uint32_t gpio_reg = 0x00000000;

    /* 1. 置位：点亮 LED（GPIO 第 5 脚） */
    SET_BIT(gpio_reg, 5);
    printf("置位第 5 位 (点亮LED) : 0x%08X  ", gpio_reg);
    print_binary(gpio_reg, 16);
    putchar('\n');

    /* 2. 置位多个脚 */
    SET_BIT(gpio_reg, 0);
    SET_BIT(gpio_reg, 1);
    printf("再置位第 0、1 位      : 0x%08X  ", gpio_reg);
    print_binary(gpio_reg, 16);
    putchar('\n');

    /* 3. 清位（关掉 LED） */
    CLR_BIT(gpio_reg, 5);
    printf("清零第 5 位 (关掉LED) : 0x%08X  ", gpio_reg);
    print_binary(gpio_reg, 16);
    putchar('\n');

    /* 4. 翻转 */
    TGL_BIT(gpio_reg, 0);
    printf("翻转第 0 位          : 0x%08X  ", gpio_reg);
    print_binary(gpio_reg, 16);
    putchar('\n');

    /* 5. 读某一位 */
    uint8_t bit = GET_BIT(gpio_reg, 1);
    printf("读取第 1 位的值      : %d\n", bit);

    /* ---------- 进阶：连续位提取（实战场景） ---------- */
    // 模拟一个 ADC 寄存器：bit[15:8] = 采样值, bit[7:0] = 通道号
    uint32_t adc_reg = (0x3F << 8) | 0x03;  // 采样值=0x3F, 通道=3
    printf("\nADC 寄存器: 0x%08X\n", adc_reg);

    uint32_t sample = GET_BITS(adc_reg, 15, 8);
    uint32_t channel = GET_BITS(adc_reg, 7, 0);
    printf("  采样值 (bit[15:8]) : 0x%02X (%d)\n", sample, sample);
    printf("  通道号 (bit[7:0])  : 0x%02X (%d)\n", channel, channel);

    // 修改采样值
    SET_BITS(adc_reg, 15, 8, 0x80);
    printf("  修改采样值后       : 0x%08X\n", adc_reg);

    /* ---------- 6. 位域：结构体位域 ---------- */
    struct {
        uint32_t enable : 1;   // 位 0
        uint32_t mode   : 2;   // 位 1-2
        uint32_t prescaler : 4; // 位 3-6
        uint32_t reserved : 25;
    } timer_cr1;

    timer_cr1.enable = 1;
    timer_cr1.mode = 2;
    timer_cr1.prescaler = 5;
    printf("\n位域结构体 (模拟定时器控制寄存器):\n");
    printf("  enable=%d, mode=%d, prescaler=%d\n",
           timer_cr1.enable, timer_cr1.mode, timer_cr1.prescaler);
    printf("  结构体大小: %zu 字节\n", sizeof(timer_cr1));

    printf("\n✅ 位操作基础学完，多练！\n");
    return 0;
}
