/**
 * volatile + static 关键字演示
 * 在 VS Code 中打开，点 ▶️ 运行
 */

#include <stdio.h>
#include <stdint.h>

/* ============================================================
 * static 演示 1：函数内部 static 变量
 * ============================================================ */
int count_calls(void)
{
    static int call_count = 0;  // ← 只初始化一次！
    call_count++;
    return call_count;
}

void demo_static_local(void)
{
    printf("--- static 局部变量 ---\n");
    printf("  每次调用，call_count 保留上次的值：\n");
    for (int i = 0; i < 5; i++) {
        printf("  第 %d 次调用 → call_count = %d\n", i + 1, count_calls());
    }

    /* 对比：普通局部变量 */
    printf("\n  对比普通局部变量（每次重新初始化）：\n");
    for (int i = 0; i < 3; i++) {
        int normal = 0;
        normal++;
        printf("  normal = %d\n", normal);
    }
}

/* ============================================================
 * static 演示 2：文件作用域隐藏
 * 下面 static 函数和变量只在这个文件内可见
 * ============================================================ */

/* "私有"变量 — 只有这个文件能访问 */
static int internal_counter = 0;

/* "私有"函数 — 只有这个文件能调用 */
static void increment_internal(void)
{
    internal_counter++;
}

/* 公开接口 — 外部文件通过这个函数访问内部数据 */
int get_counter(void)
{
    return internal_counter;
}

void reset_counter(void)
{
    internal_counter = 0;
}

void add_to_counter(int val)
{
    for (int i = 0; i < val; i++) {
        increment_internal();  // 内部可以调用 static 函数
    }
}

void demo_static_file_scope(void)
{
    printf("\n--- static 文件作用域（模块化封装）---\n");
    printf("  初始值: %d\n", get_counter());
    add_to_counter(3);
    printf("  加了 3 次后: %d\n", get_counter());
    reset_counter();
    printf("  重置后: %d\n", get_counter());

    printf("\n  ⚠️ 注意：increment_internal() 和 internal_counter\n");
    printf("     只能在当前 .c 文件内使用\n");
    printf("     其他文件即使 #include 也访问不到\n");
}

/* ============================================================
 * volatile 演示
 * 注意：这里无法真正展示硬件寄存器的效果（因为没有硬件）
 * 但可以展示编译器优化行为
 * ============================================================ */

/* 模拟一个"会被外部改变"的变量 */
volatile uint8_t sensor_value = 0;

void simulate_interrupt(void)
{
    /* 模拟 ISR 改变了这个值 */
    sensor_value = 128;
}

void demo_volatile(void)
{
    printf("\n--- volatile — 模拟中断场景 ---\n");

    sensor_value = 0;
    printf("  sensor_value 初始值: %d\n", sensor_value);

    printf("  模拟：主循环一直等 sensor_value 改变...\n");
    printf("  如果没有 volatile，编译器会优化成只读一次\n");
    printf("  然后永远跳出不了循环\n\n");

    /* 模拟中断触发 */
    printf("  中断到了！改变 sensor_value...\n");
    simulate_interrupt();

    /* 主循环读取 */
    printf("  sensor_value 现在 = %d (中断修改后的值)\n", sensor_value);

    /* 嵌入式真实场景举例 */
    printf("\n  嵌入式 volatile 三大场景：\n");
    printf("  1️⃣ 硬件寄存器:\n");
    printf("     volatile uint32_t *GPIOA = (volatile uint32_t*)0x40020000;\n");
    printf("     while (GPIOA[0] & 0x10) { }  // 每次真正读硬件\n\n");

    printf("  2️⃣ 中断共享变量:\n");
    printf("     volatile uint8_t uart_has_data;\n");
    printf("     // ISR: uart_has_data = 1;\n");
    printf("     // main: if (uart_has_data) { ... }\n\n");

    printf("  3️⃣ RTOS 多任务:\n");
    printf("     volatile uint32_t system_tick_ms;\n");
    printf("     // 一个任务写，另一个任务读\n");
}

/* ============================================================
 * 对比：没有 volatile 的陷阱
 * ============================================================ */

/* 模拟一个延时函数 */
void delay(void)
{
    // 空转 500 次
    volatile uint32_t count = 0;
    while (count < 500) {
        count++;
    }
}

void demo_no_volatile_trap(void)
{
    printf("\n--- 没有 volatile 的典型陷阱 ---\n");

    /* 场景：用空循环做延时 */
    printf("  ? 很多新手这样写延时函数：\n");
    printf("  for (uint32_t i = 0; i < 100000; i++);  // 空循环\n");
    printf("  开启 -O2 优化后，编译器直接把这行删掉！\n");
    printf("  因为 i 没有被任何地方使用，编译器认为它没用\n\n");
    printf("  ✅ 正确写法：\n");
    printf("  for (volatile uint32_t i = 0; i < 100000; i++);\n");
    printf("  volatile 告诉编译器：每次都要真的读写 i\n\n");

    /* 用 volatile 延时能正常工作 */
    printf("  用 volatile 实现延时...\n");
    delay();
    printf("  延时完成 ✅\n");
}

int main(void)
{
    printf("===== volatile + static 关键字 =====\n\n");

    demo_static_local();
    demo_static_file_scope();
    demo_volatile();
    demo_no_volatile_trap();

    printf("\n✅ 全部演示结束！\n");
    printf("   笔记位置: 文档/学习笔记/嵌入式C/嵌入式C语言笔记：.md\n");
    return 0;
}
