/**
 * 补码完全解读 — 5 种情况全覆盖
 * 存储值 vs 逻辑值
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* 工具函数：打印二进制 */
void print_bin(const char *label, uint32_t val, int bits)
{
    printf("  %s: ", label);
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (val >> i) & 1);
        if (i && i % 8 == 0) printf(" ");
    }
    printf("\n");
}

int main(void)
{
    printf("===== 补码完全解读：5 种情况全覆盖 =====\n\n");
    printf("核心概念：存储值（二进制位）vs 逻辑值（怎么解读）\n\n");

    /* ============================
       情况 1：正常范围，不需要关心补码
       ============================ */
    printf("━━━ 情况 1：正常范围，不需要关心补码 ━━━\n\n");

    uint8_t ua = 100;
    int8_t  sa = 100;
    int8_t  sb = -100;

    printf("  uint8_t ua = 100:\n");
    print_bin("  存储值（8位）", ua, 8);
    printf("  逻辑值 = %u\n\n", ua);

    printf("  int8_t sa = 100:\n");
    print_bin("  存储值（8位）", (uint8_t)sa, 8);  // 当成无符号看位
    printf("  逻辑值 = %d\n\n", sa);

    printf("  int8_t sb = -100:\n");
    print_bin("  存储值（8位）", (uint8_t)sb, 8);
    printf("  逻辑值 = %d\n\n", sb);

    printf("  加减法 CPU 自动处理：sa + sb = %d + (%d) = %d\n", sa, sb, sa + sb);
    printf("  → 正常范围内，一切自动正确，不需要手动算补码\n\n");

    /* ============================
       情况 2：越界赋值
       ============================ */
    printf("━━━ 情况 2：越界赋值 → 截断 + 重新解读 ━━━\n\n");

    int8_t  over = 200;    // 越界
    uint8_t over_u = 300;  // 越界

    printf("  int8_t  over = 200:\n");
    print_bin("  200的二进制(9位)", 200, 9);
    print_bin("  截断到8位", (uint8_t)over, 8);
    printf("  作为 int8_t 的逻辑值 = %d\n", over);
    printf("  作为 uint8_t 的逻辑值 = %u\n\n", (uint8_t)over);

    printf("  uint8_t over_u = 300:\n");
    print_bin("  300的二进制(9位)", 300, 9);
    print_bin("  截断到8位", over_u, 8);
    printf("  逻辑值 = %u\n\n", over_u);

    /* 完整越界表验证 */
    printf("  越界表验证（int8_t）：\n");
    printf("  赋值 %3d → 存储位: 11001000 → int8_t读: %3d, uint8_t读: %3u\n",
           200, (int8_t)200, (uint8_t)(int8_t)200);
    printf("  赋值 %3d → 存储位: 11111111 → int8_t读: %3d, uint8_t读: %3u\n",
           255, (int8_t)255, (uint8_t)(int8_t)255);
    printf("  赋值 %3d → 存储位: 00000000 → int8_t读: %3d, uint8_t读: %3u\n",
           256, (int8_t)256, (uint8_t)(int8_t)256);
    printf("  赋值 %3d → 存储位: 10000000 → int8_t读: %3d, uint8_t读: %3u\n",
           -128, (int8_t)-128, (uint8_t)(int8_t)-128);
    printf("  赋值 %3d → 存储位: 01111111 → int8_t读: %3d, uint8_t读: %3u\n\n",
           -129, (int8_t)-129, (uint8_t)(int8_t)-129);

    /* ============================
       情况 3：printf 打印 — 符号扩展
       ============================ */
    printf("━━━ 情况 3：printf 打印 + 符号扩展 ━━━\n\n");

    int8_t  sv = -56;     // 1100 1000
    uint8_t uv = 200;     // 1100 1000

    printf("  int8_t  sv = %d  (存储: ", sv);
    print_bin("", (uint8_t)sv, 8);
    printf("  uint8_t uv = %u (存储: ", uv);
    print_bin("", uv, 8);
    printf("  两者存储值一样！但解读不同：\n\n");

    printf("  printf(\"%%d\", sv) = %d\n", sv);
    printf("  printf(\"%%u\", sv) = %u  ← 陷阱！\n", sv);
    printf("  printf(\"%%d\", uv) = %d\n", uv);
    printf("  printf(\"%%u\", uv) = %u\n\n", uv);

    printf("  看看整型提升后的完整32位：\n");
    int promoted_s = sv;
    int promoted_u = uv;
    print_bin("  sv提升为int", promoted_s, 32);
    print_bin("  uv提升为int", promoted_u, 32);

    printf("\n  → 无符号提升补0，有符号提升补符号位\n");
    printf("  → %%d 按有符号读，%%u 按无符号读\n\n");

    /* ============================
       情况 4：位运算右移
       ============================ */
    printf("━━━ 情况 4：右移 — 算术右移 vs 逻辑右移 ━━━\n\n");

    int8_t  sr = -128;    // 1000 0000
    uint8_t ur = 128;     // 1000 0000

    print_bin("  有符号 sr = -128", (uint8_t)sr, 8);
    print_bin("  无符号 ur = 128 ", ur, 8);
    printf("  存储值一样！\n\n");

    int8_t  sr_shifted = sr >> 2;
    uint8_t ur_shifted = ur >> 2;

    print_bin("  sr >> 2 结果", (uint8_t)sr_shifted, 8);
    printf("   逻辑值 = %d\n", sr_shifted);
    printf("   高位移了符号位1 → 算术右移\n\n");

    print_bin("  ur >> 2 结果", ur_shifted, 8);
    printf("   逻辑值 = %u\n", ur_shifted);
    printf("   高位移了0 → 逻辑右移\n\n");

    /* ============================
       情况 5：比较运算
       ============================ */
    printf("━━━ 情况 5：比较运算 ━━━\n\n");

    int8_t  ca = -1;      // 1111 1111
    uint8_t cb = 255;     // 1111 1111

    print_bin("  int8_t  ca = -1 ", (uint8_t)ca, 8);
    print_bin("  uint8_t cb = 255", cb, 8);
    printf("  存储值都是 1111 1111\n\n");

    printf("  if (ca == cb) → ");
    if (ca == cb) {
        printf("成立！\n");
    } else {
        printf("不成立！\n");
    }

    printf("  原因：ca 提升为 int = ");
    print_bin("", (int)ca, 32);
    printf("  cb 提升为 int = ");
    print_bin("", (int)cb, 32);
    printf("  32 位不一样，所以不相等\n\n");

    printf("  if ((uint8_t)ca == cb) → ");
    if ((uint8_t)ca == cb) {
        printf("成立！\n");
    } else {
        printf("不成立！\n");
    }
    printf("  先强转成 uint8_t = 255，再比较，就相等了\n\n");

    /* ========== 总结 ========== */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("总结：什么时候需要关心补码？\n\n");
    printf("  正常使用（值在范围内）→ ❌ 不用管\n");
    printf("  越界赋值               → ✅ 截断后重读\n");
    printf("  printf %%d/%%u          → ✅ 符号扩展\n");
    printf("  >> 右移                → ✅ 算术/逻辑\n");
    printf("  不同类型比较           → ✅ 整型提升\n\n");
    printf("  存储值 = 二进制位，逻辑值 = 你怎么看它\n");
    printf("  补码是解读规则，一直都在，正数时恰好等于原码\n");

    return 0;
}
