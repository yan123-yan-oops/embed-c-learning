/**
 * ============================================================
 * 指针入门 — 嵌入式 C 的核心武器
 * 学习目标：
 *   1. 指针是什么（地址 vs 值）
 *   2. & 取地址、* 解引用
 *   3. 指针与数组
 *   4. 指针与函数参数（模拟引用传递）
 *   5. 函数指针（嵌入式回调的基石）
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>

/* 函数指针类型：嵌入式中断回调、定时器回调的基础 */
typedef void (*callback_t)(void);

/* 一个简单的回调函数 */
void led_toggle(void)
{
    printf("  🔦 LED Toggle!\n");
}

/* 传入指针，直接修改原变量的值（嵌入式寄存器操作的关键） */
void set_bit(volatile uint32_t *reg, uint8_t bit_pos)
{
    *reg |= (1U << bit_pos);
}

int main(void)
{
    printf("===== 指针 — 嵌入式 C 的灵魂 =====\n\n");

    /* ---------- 1. 基本指针：值 vs 地址 ---------- */
    uint32_t val = 0x12345678;
    uint32_t *ptr = &val;  // ptr 保存了 val 的地址

    printf("val 的值     : 0x%08X\n", val);
    printf("val 的地址   : %p\n", (void*)&val);
    printf("ptr 的值     : %p  (就是 val 的地址)\n", (void*)ptr);
    printf("解引用 *ptr  : 0x%08X\n", *ptr);

    /* ---------- 2. 指针修改原值 ---------- */
    *ptr = 0xAABBCCDD;
    printf("通过指针修改后 val = 0x%08X\n\n", val);

    /* ---------- 3. 数组与指针 ---------- */
    uint8_t buffer[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t *p = buffer;  // 数组名就是首元素地址

    printf("数组方式访问:\n");
    for (int i = 0; i < 4; i++) {
        printf("  buffer[%d] = 0x%02X\n", i, buffer[i]);
    }

    printf("指针方式访问:\n");
    for (int i = 0; i < 4; i++) {
        printf("  *(p+%d) = 0x%02X  (地址 %p)\n", i, *(p + i), (void*)(p + i));
    }

    /* ---------- 4. 指针传参（模拟寄存器操作） ---------- */
    volatile uint32_t fake_reg = 0;
    printf("\n操作前 fake_reg = 0x%08X\n", fake_reg);
    set_bit((uint32_t*)&fake_reg, 3);   // 设置第 3 位
    printf("设置第3位后  = 0x%08X\n", fake_reg);

    /* ---------- 5. 函数指针 ---------- */
    callback_t cb = led_toggle;
    printf("\n通过函数指针调用回调:\n");
    cb();  // 等价于 led_toggle()

    printf("\n✅ 指针基础学完，下一节：位操作！\n");
    return 0;
}
