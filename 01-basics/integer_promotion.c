/**
 * 整型提升 + 符号扩展演示
 * 在 VS Code 中打开，点 ▶️ 运行
 */

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    printf("===== 整型提升与符号扩展 =====\n\n");

    uint8_t  u8 = 200;     // 二进制 1100 1000
    int8_t   s8 = 200;     // 等价于 -56（int8_t 范围 -128~127）
    uint16_t u16 = 50000;
    int16_t  s16 = -10000; //等价于 55535

    /* ===== 1. %d vs %u 输出差异 ===== */
    printf("--- 1. 同样的值，不同格式输出 ---\n");
    printf("  uint8_t a = 200,  %%u = %u\n", u8);   // 200
    printf("  uint8_t a = 200,  %%d = %d\n", u8);   // 200（因为整型提升后还是正数）
    printf("  int8_t  b = 200,  %%d = %d\n", s8);   // -56（符号扩展）
    printf("  int8_t  b = 200,  %%u = %u\n", s8);   // 大数（陷阱！）
    printf("\n");

    /* ===== 2. 查看内存中的二进制 ===== */
    printf("--- 2. 内存中的实际表示 ---\n");
    printf("  s8 = %d,  内存 8 位: ", s8);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (s8 >> i) & 1);
    }
    printf("\n");

    // 看整型提升后的 32 位 int
    int promoted = s8;  // 编译器自动完成整型提升
    printf("  提升为 int 后的 32 位: ");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (promoted >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
    printf("\n  注意高 24 位全是 1（符号位扩展）\n\n");

    /* ===== 3. 位运算中的符号陷阱 ===== */
    printf("--- 3. 右移：算术 vs 逻辑 ---\n");
    int8_t x = -128;  // 二进制 1000 0000
    printf("  int8_t x = %d, x >> 2 = %d\n", x, x >> 2);
    printf("  原因：有符号右移是算术右移（补符号位）\n\n");

    uint8_t y = 0x80; // 二进制 1000 0000
    printf("  uint8_t y = %d, y >> 2 = %d\n", y, y >> 2);
    printf("  原因：无符号右移是逻辑右移（补0）\n\n");

    /* ===== 4. 协议解析的实际场景 ===== */
    printf("--- 4. 串口数据解析 ---\n");
    printf("  收到字节 0xFA:\n");
    printf("  用 uint8_t 接收: %u   (正确)\n", (uint8_t)0xFA);
    printf("  用 int8_t  接收: %d   (-6, 后面做比较全错)\n", (int8_t)0xFA);
    printf("  结论：协议解析全用无符号类型\n\n");

    printf("✅ 演示结束，理解这些就能避免 90%% 的嵌入式 C 陷阱\n");
    return 0;
}
