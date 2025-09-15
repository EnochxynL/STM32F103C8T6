#include "lcd_vt100.h"
#include "lcd.h"
#include "lcd_init.h"

// 使用示例函数
void LCD_VT100_Demo(void)
{
    // 初始化VT100终端 (假设240x320屏幕，8x16字符)
    LCD_VT100_Init(LCD_W, LCD_H, 8, 16);
    
    // 基本文本输出
    LCD_VT100_Printf("VT100 Term!\n");
    LCD_VT100_Printf("Size:%dx%d\n\n", LCD_W/8, LCD_H/16);

    // 测试颜色控制
    LCD_VT100_Printf("\033[31mRed\033[0m\n");        // 红色文字
    LCD_VT100_Printf("\033[32mGreen\033[0m\n");      // 绿色文字
    LCD_VT100_Printf("\033[34mBlue\033[0m\n");       // 蓝色文字
    LCD_VT100_Printf("\033[33;44mY/B\033[0m\n");     // 黄字蓝底
    
    // 测试光标移动
    LCD_VT100_Printf("\033[8;2HPos(2,8)\n");     // 移动到第8行第2列
    LCD_VT100_Printf("\033[10;1H");              // 移动到第10行第1列
    LCD_VT100_Printf("Line10\n");
    
    // 测试清屏功能
    LCD_VT100_Printf("Press key...\n");
    // 这里可以等待按键
    
    LCD_VT100_Printf("\033[2J");                 // 清屏
    LCD_VT100_Printf("\033[1;1H");               // 移动到左上角
    
    // 测试字符格式化
    LCD_VT100_Printf("Output:\n");
    LCD_VT100_Printf("I: %d\n", 123);
    LCD_VT100_Printf("F: %.1f\n", 3.1);
    LCD_VT100_Printf("S: %s\n", "OK");
    
    // 测试自动换行
    LCD_VT100_Printf("Long line test autowrap here! ");
    LCD_VT100_Printf("Continue.\n");
    
    // 测试制表符和特殊字符
    LCD_VT100_Printf("Tab\tTest\n");
    LCD_VT100_Printf("BS: AB\b\bXY\n");       // 应该显示 "XY"
    
    // 测试部分清除
    LCD_VT100_Printf("This line will be partially cleared");
    LCD_VT100_Printf("\033[K");                           // 清除到行尾
    LCD_VT100_Printf(" - Cleared!\n");
    
    // 绘制简单的框架 (适配小屏幕)
    LCD_VT100_Printf("\033[15;1H");                       // 移动到第15行
    LCD_VT100_Printf("+");
    for(int i = 0; i < 3; i++) LCD_VT100_Printf("-");     // 只画3个短横线
    LCD_VT100_Printf("+\n");
    
    LCD_VT100_Printf("|VT1|\n");                          // 缩短内容
    LCD_VT100_Printf("|STM|\n");                          // 缩短内容
    
    LCD_VT100_Printf("+");
    for(int i = 0; i < 3; i++) LCD_VT100_Printf("-");
    LCD_VT100_Printf("+\n");
}

// 系统信息显示示例
void LCD_VT100_ShowSystemInfo(void)
{
    LCD_VT100_Printf("\033[2J\033[1;1H");                 // 清屏并移动到左上角
    
    // 标题
    LCD_VT100_Printf("\033[1;33m");                       // 设置为亮黄色
    LCD_VT100_Printf("=SYS=\n");
    LCD_VT100_Printf("\033[0m");                          // 重置颜色
    
    // 系统信息
    LCD_VT100_Printf("\033[32mF103\n");
    LCD_VT100_Printf("M3\n");
    LCD_VT100_Printf("64K\n");
    LCD_VT100_Printf("20K\n");
    LCD_VT100_Printf("72M\033[0m\n\n");
    
    // 外设状态
    LCD_VT100_Printf("Dev:\n");
    LCD_VT100_Printf("\033[32m[OK]\033[0m\n");
    LCD_VT100_Printf("\033[33m[RDY]\033[0m\n");
    LCD_VT100_Printf("\033[32m[OK]\033[0m\n");
    
    // 运行时信息
    LCD_VT100_Printf("Inf:\n");
    LCD_VT100_Printf("\033[36m123s\033[0m\n");
    LCD_VT100_Printf("\033[36m8K\033[0m\n");
    LCD_VT100_Printf("\033[36m3Act\033[0m\n");
}

// 进度条显示示例
void LCD_VT100_ShowProgress(uint8_t percent)
{
    static uint8_t last_percent = 255;  // 初始值，确保第一次会更新
    
    if (percent != last_percent) {
        LCD_VT100_Printf("\033[s");       // 保存光标位置
        LCD_VT100_Printf("\033[15;1H");   // 移动到第15行
        
        LCD_VT100_Printf("[");
        
        uint8_t bar_length = 3;  // 适配5列屏幕
        uint8_t filled = (percent * bar_length) / 100;
        
        for (uint8_t i = 0; i < bar_length; i++) {
            if (i < filled) {
                LCD_VT100_Printf("\033[42m \033[0m");  // 绿色填充
            } else {
                LCD_VT100_Printf(" ");
            }
        }
        
        LCD_VT100_Printf("]");
        LCD_VT100_Printf("\033[K");       // 清除到行尾
        LCD_VT100_Printf("\033[16;1H%3d%%\n", percent);  // 下一行显示百分比
        LCD_VT100_Printf("\033[u");       // 恢复光标位置
        
        last_percent = percent;
    }
}

// 菜单显示示例
void LCD_VT100_ShowMenu(void)
{
    LCD_VT100_Printf("\033[2J\033[1;1H");                 // 清屏
    
    LCD_VT100_Printf("\033[1;36m");                       // 亮青色
    LCD_VT100_Printf("=MENU\n");
    LCD_VT100_Printf("\033[0m");                          // 重置颜色
    
    LCD_VT100_Printf("1.Sys\n");
    LCD_VT100_Printf("2.HW\n");
    LCD_VT100_Printf("3.Set\n");
    LCD_VT100_Printf("4.Abt\n");
    LCD_VT100_Printf("0.Exit\n\n");
    
    LCD_VT100_Printf("Sel:");
}

// 小屏幕滚动测试示例
void LCD_VT100_ScrollTest(void)
{
    LCD_VT100_Printf("\033[2J\033[1;1H");                 // 清屏
    
    LCD_VT100_Printf("\033[1;32m");                       // 亮绿色
    LCD_VT100_Printf("SCRL\n");                           // 滚动测试标题
    LCD_VT100_Printf("\033[0m");                          // 重置颜色
    
    // 输出足够多的行来触发滚动
    for(int i = 1; i <= 25; i++) {
        LCD_VT100_Printf("L%02d\n", i);                   // 行号简化显示
        // 可以添加延时来观察滚动效果
        // delay_ms(200);
    }
    
    LCD_VT100_Printf("\033[31mEND\033[0m\n");            // 红色结束标记
}