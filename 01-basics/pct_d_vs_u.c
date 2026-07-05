/**
 * 彻底搞懂 %d vs %u
 * 以及：补码是所有有符号数的存储方式
 */

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    printf("===== %%d 是 signed，%%u 是 unsigned =====\n\n");

    /* ===== 1. 同样一个数值，%d 和 %u 输出完全不同 ===== */
    printf("--- 同一个 32 位二进制，%%d 和 %%u 两种解读 ---\n");

    int x = -100;
    printf("  int  x = -100\n");
    printf("    %%d = %d\n", x);       // -100
    printf("    %%u = %u\n", x);       // 4294967196

    int y = 100;
    printf("  int  y = 100\n");
    printf("    %%d = %d\n", y);       // 100
    printf("    %%u = %u\n", y);       // 100（正数，两种解读一样）
    printf("  → 正数时 %%d 和 %%u 结果一样\n");
    printf("  → 负数时 %%d 输出负数，%%u 输出大正数\n\n");

    /* ===== 2. int8_t 的问题 ===== */
    printf("--- int8_t 越界与不越界 ---\n");

    int8_t a = 50;       // 在范围内：50
    int8_t b = -56;      // 在范围内：-56
    int8_t c = 200;      // 越界了！存的是 -56 的补码

    printf("  int8_t a =  50,  %%d = %d\n", a);    // 50
    printf("  int8_t b = -56,  %%d = %d\n", b);    // -56
    printf("  int8_t c = 200,  %%d = %d\n", c);    // -56 !!
    printf("  注意：c 不是 200，是 -56\n\n");

    /* ===== 3. 看它们的内存一模一样 ===== */
    printf("--- b 和 c 在内存中的二进制完全相同！---\n");
    printf("  b 的 8 位二进制: ");
    for (int i = 7; i >= 0; i--) printf("%d", (b >> i) & 1);
    printf("\n");
    printf("  c 的 8 位二进制: ");
    for (int i = 7; i >= 0; i--) printf("%d", (c >> i) & 1);
    printf("\n");
    printf("  b 和 c 在内存里都是 1100 1000\n");
    printf("  区别只在于你怎么解读它\n\n");

    /* ===== 4. 关键问题：%d 能打印负数，%u 不能 ===== */
    printf("--- 面试常考 ---\n");
    uint8_t u = 200;
    int8_t  s = 200;   // 等于 -56

    printf("  uint8_t u = 200,  %%d = %d\n", u);   // 200
    printf("  uint8_t u = 200,  %%u = %u\n", u);   // 200

    printf("  int8_t  s = 200,  %%d = %d\n", s);   // -56（有符号解读）
    printf("  int8_t  s = 200,  %%u = %u\n", s);   // 大数陷阱！

    printf("\n  ✅ 安全守则：\n");
    printf("     无符号类型  → 用 %%u 打印\n");
    printf("     有符号类型  → 用 %%d 打印\n");
    printf("     不确定时    → 用 %%d（int 默认有符号）\n");

    return 0;
}
