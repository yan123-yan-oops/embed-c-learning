/**
 * 指针全面演示 — 配合笔记使用
 * 运行前先把笔记看一遍，再看代码效果
 */

#include <stdio.h>
#include <stdint.h>

/* ============================================================
   1. 指针的本质：地址 vs 值
   ============================================================ */
void demo_essence(void)
{
    printf("━━━ 1. 指针的本质：地址 vs 值 ━━━\n\n");

    uint32_t val = 0x12345678;
    uint32_t *ptr = &val;

    printf("  val 的值     : 0x%08X\n", val);
    printf("  &val (地址)  : %p\n", (void*)&val);
    printf("  ptr (存地址) : %p\n", (void*)ptr);
    printf("  *ptr (解引用): 0x%08X\n", *ptr);
    printf("  &ptr (指针自己的地址): %p\n\n", (void*)&ptr);
}

/* ============================================================
   2. 指针的运算（重点：+1 不是加 1 字节）
   ============================================================ */
void demo_arithmetic(void)
{
    printf("━━━ 2. 指针的运算 ━━━\n\n");

    uint32_t arr[4] = {10, 20, 30, 40};
    uint32_t *p = arr;

    printf("  arr 数组地址: %p\n", (void*)arr);
    for (int i = 0; i < 4; i++) {
        printf("  &arr[%d] = %p  (偏移 %d 字节)\n", i,
               (void*)&arr[i], i * (int)sizeof(uint32_t));
    }

    printf("\n  指针加法验证：\n");
    printf("  p     → arr[0] = %d\n", *p);
    printf("  p + 1 → arr[1] = %d\n", *(p + 1));
    printf("  p + 2 → arr[2] = %d\n", *(p + 2));
    printf("  p + 3 → arr[3] = %d\n\n", *(p + 3));

    /* 不同位宽的指针 +1 效果不同 */
    uint8_t  *p8  = (uint8_t*)0x1000;
    uint32_t *p32 = (uint32_t*)0x1000;

    printf("  uint8_t*  p8  = 0x1000, p8 + 1  = 0x%04X (+1 字节)\n",
           (uint32_t)(p8 + 1));
    printf("  uint32_t* p32 = 0x1000, p32 + 1 = 0x%04X (+4 字节)\n\n",
           (uint32_t)(p32 + 1));

    /* 指针相减 = 元素个数 */
    printf("  &arr[3] - &arr[0] = %td (中间 %td 个元素)\n\n",
           &arr[3] - &arr[0], &arr[3] - &arr[0]);
}

/* ============================================================
   3. 数组名 vs 指针
   ============================================================ */
void demo_array_vs_pointer(void)
{
    printf("━━━ 3. 数组名 vs 指针 ━━━\n\n");

    uint8_t arr[100];
    uint8_t *p = arr;

    printf("  sizeof(arr) = %zu  (整个数组)\n", sizeof(arr));
    printf("  sizeof(p)   = %zu  (指针本身)\n\n", sizeof(p));

    printf("  arr 的地址 = %p\n", (void*)arr);
    printf("  &arr[0]    = %p\n", (void*)&arr[0]);
    printf("  两者完全一样\n\n");

    printf("  区别：arr++ 不合法，p++ 合法\n\n");
}

/* ============================================================
   4. 数组参数退化为指针
   ============================================================ */
void array_param_func(uint8_t arr[100])
{
    // 这里的 arr 是指针，不是数组！
    printf("  sizeof(arr) in function = %zu (指针大小)\n", sizeof(arr));
    printf("  而不是 100！因为参数退化为指针\n\n");
}

void demo_array_decay(void)
{
    printf("━━━ 4. 数组参数退化为指针 ━━━\n\n");
    uint8_t buf[100];
    printf("  在 main 中 sizeof(buf) = %zu\n", sizeof(buf));
    array_param_func(buf);
}

/* ============================================================
   5. 指针传参（模拟引用传递）
   ============================================================ */
void swap_bad(int a, int b)
{
    int tmp = a;
    a = b;
    b = tmp;
    printf("    函数内：a = %d, b = %d (改了副本)\n", a, b);
}

void swap_good(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void demo_pass_by_pointer(void)
{
    printf("━━━ 5. 指针传参 ━━━\n\n");

    int x = 10, y = 20;
    printf("  初始: x = %d, y = %d\n", x, y);

    printf("  swap_bad(x, y)：\n");
    swap_bad(x, y);
    printf("  调用后: x = %d, y = %d (不变)\n\n", x, y);

    printf("  swap_good(&x, &y)：\n");
    swap_good(&x, &y);
    printf("  调用后: x = %d, y = %d (交换成功)\n\n", x, y);
}

/* ============================================================
   6. const 与指针的 4 种组合
   ============================================================ */
void demo_const_pointers(void)
{
    printf("━━━ 6. const 与指针的 4 种组合 ━━━\n\n");

    uint8_t a = 10, b = 20;

    // 情况 1：内容不可改
    const uint8_t *p1 = &a;
    printf("  ① const uint8_t *p → 内容不可改，指针可改\n");
    // *p1 = 100;  // ❌
    p1 = &b;        // ✅
    printf("     p1 改指向 b: *p1 = %u\n\n", *p1);

    // 情况 2：指针不可改
    uint8_t * const p2 = &a;
    printf("  ② uint8_t * const p → 内容可改，指针不可改\n");
    *p2 = 100;      // ✅
    // p2 = &b;    // ❌
    printf("     *p2 改为 100: a = %u\n\n", a);

    // 情况 3：两者都不可改
    const uint8_t * const p3 = &a;
    printf("  ③ const uint8_t * const p → 两者都不可改\n");
    // *p3 = 100;  // ❌
    // p3 = &b;   // ❌
    printf("     只能读: *p3 = %u\n\n", *p3);

    // 情况 4：都能改
    uint8_t *p4 = &a;
    printf("  ④ uint8_t *p → 两者都可改\n");
    *p4 = 200;      // ✅
    p4 = &b;        // ✅
    printf("     随便改: a = %u, *p4 = %u\n\n", a, *p4);
}

/* ============================================================
   7. 函数指针
   ============================================================ */
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

void demo_function_pointer(void)
{
    printf("━━━ 7. 函数指针 ━━━\n\n");

    // 声明函数指针
    int (*calc)(int, int);

    calc = add;
    printf("  calc = add  → calc(10, 5) = %d\n", calc(10, 5));

    calc = sub;
    printf("  calc = sub  → calc(10, 5) = %d\n", calc(10, 5));

    calc = mul;
    printf("  calc = mul  → calc(10, 5) = %d\n\n", calc(10, 5));

    /* 嵌入式中最常见的用法：回调 */
    printf("  函数指针在嵌入式的典型用法：\n");
    printf("    typedef void (*callback_t)(void);\n");
    printf("    定时器回调 / 中断回调 / 按键回调\n\n");
}

/* ============================================================
   8. volatile 指针（硬件寄存器）
   ============================================================ */
void demo_volatile_pointer(void)
{
    printf("━━━ 8. volatile 指针 ━━━\n\n");

    printf("  嵌入式硬件寄存器写法：\n");
    printf("  volatile uint32_t *GPIOA_ODR =\n");
    printf("      (volatile uint32_t*)0x40020014;\n\n");
    printf("  volatile 的作用：\n");
    printf("  ① 每次读写都真的访问硬件地址\n");
    printf("  ② 编译器不能优化掉连续的读写操作\n");
    printf("  ③ 硬件可能随时改变这个地址上的值\n\n");
}

/* ============================================================
   9. NULL 与野指针
   ============================================================ */
void demo_null_wild(void)
{
    printf("━━━ 9. NULL 与野指针 ━━━\n\n");

    uint8_t *p = NULL;

    printf("  uint8_t *p = NULL;\n");
    printf("  if (p != NULL) { *p = 100; }\n\n");

    printf("  野指针三大来源：\n");
    printf("  ① 未初始化 → 随机地址\n");
    printf("  ② free 后没用 NULL → 悬空指针\n");
    printf("  ③ 返回局部变量地址 → 栈已销毁\n\n");

    printf("  安全守则：\n");
    printf("  ① 定义时 = NULL\n");
    printf("  ② 使用前 if (p != NULL)\n");
    printf("  ③ free 后立即 p = NULL\n\n");
}

int main(void)
{
    printf("===== 指针全面演示 =====\n\n");

    demo_essence();
    demo_arithmetic();
    demo_array_vs_pointer();
    demo_array_decay();
    demo_pass_by_pointer();
    demo_const_pointers();
    demo_function_pointer();
    demo_volatile_pointer();
    demo_null_wild();

    printf("✅ 全部演示结束！配合笔记 C:\\Users\\Administrator\\Documents\\学习笔记\\嵌入式C\\指针.md\n");
    return 0;
}
