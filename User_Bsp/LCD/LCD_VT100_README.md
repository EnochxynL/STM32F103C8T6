# LCD VT100 Terminal Emulator

这是一个为STM32F103C8T6微控制器设计的VT100终端模拟器，支持在LCD显示屏上模拟终端控制码功能。

## 功能特性

### 基本功能
- 字符和字符串显示
- 自动换行
- 光标位置控制
- 屏幕清除功能
- 颜色显示控制
- 格式化输出（printf风格）
- **ST7789硬件滚动支持**

### ST7789硬件滚动
- 利用ST7789内置的垂直滚动功能
- 支持162像素高度的滚动区域
- 比软件滚动更高效，无闪烁
- 自动回退到软件清屏（如屏幕高度>162）

### 支持的VT100控制码

#### 光标控制
- `\033[H` 或 `\033[f` - 将光标移动到左上角
- `\033[{row};{col}H` - 将光标移动到指定位置
- `\033[{n}A` - 光标上移n行
- `\033[{n}B` - 光标下移n行  
- `\033[{n}C` - 光标右移n列
- `\033[{n}D` - 光标左移n列

#### 清屏控制
- `\033[2J` - 清除整个屏幕
- `\033[0J` - 清除从光标到屏幕底部
- `\033[K` 或 `\033[0K` - 清除从光标到行尾
- `\033[2K` - 清除整行

#### 颜色控制
- `\033[0m` - 重置所有属性
- `\033[30m` - 黑色前景
- `\033[31m` - 红色前景
- `\033[32m` - 绿色前景
- `\033[33m` - 黄色前景
- `\033[34m` - 蓝色前景
- `\033[35m` - 洋红前景
- `\033[36m` - 青色前景
- `\033[37m` - 白色前景
- `\033[40m-47m` - 对应的背景色

#### 特殊字符
- `\n` - 换行
- `\r` - 回车
- `\b` - 退格
- `\t` - 制表符（4个空格对齐）

## 使用方法

### 1. 初始化

```c
#include "lcd_vt100.h"
#include "lcd.h"

// 初始化LCD（根据你的LCD驱动）
LCD_Init();

// 初始化VT100终端
// 参数：屏幕宽度，屏幕高度，字符宽度，字符高度
LCD_VT100_Init(240, 320, 8, 16);
```

### 2. 基本文本输出

```c
// 输出字符串
LCD_VT100_PutString("Hello World!\n");

// 格式化输出
LCD_VT100_Printf("Value: %d, Float: %.2f\n", 123, 3.14);

// 输出单个字符
LCD_VT100_PutChar('A');
```

### 3. 使用VT100控制码

```c
// 清屏
LCD_VT100_Printf("\033[2J");

// 移动光标到第5行第10列
LCD_VT100_Printf("\033[5;10H");

// 显示红色文字
LCD_VT100_Printf("\033[31mRed Text\033[0m\n");

// 显示黄字蓝底
LCD_VT100_Printf("\033[33;44mYellow on Blue\033[0m\n");
```

### 4. 运行演示

```c
#include "lcd_vt100_demo.h"

// 运行完整演示
LCD_VT100_Demo();

// 显示系统信息
LCD_VT100_ShowSystemInfo();

// 显示进度条
for(int i = 0; i <= 100; i += 10) {
    LCD_VT100_ShowProgress(i);
    delay_ms(100);
}

// 显示菜单
LCD_VT100_ShowMenu();
```

### 5. ST7789硬件滚动使用

```c
// 对于240x162的ST7789屏幕，硬件滚动会自动启用
LCD_VT100_Init(240, 162, 8, 16);

// 对于其他尺寸屏幕，如果高度≤162，也会启用硬件滚动
LCD_VT100_Init(135, 240, 6, 12); // 会自动禁用硬件滚动（高度>162）

// 硬件滚动状态检查（可选）
// 在vt100结构中，scroll_area > 0 表示硬件滚动已启用
```

## 文件结构

- `lcd_vt100.h` - 主要API接口
- `lcd_vt100.c` - VT100终端核心实现
- `lcd_vt100_init.h` - ST7789硬件滚动接口
- `lcd_vt100_init.c` - ST7789硬件滚动实现
- `lcd_vt100_demo.h` - 演示程序接口
- `lcd_vt100_demo.c` - 演示程序实现

## API 函数说明

### 初始化和配置
- `LCD_VT100_Init()` - 初始化VT100终端
- `LCD_VT100_SetCursor()` - 设置光标位置
- `LCD_VT100_GetCursor()` - 获取光标位置

### 显示控制
- `LCD_VT100_Clear()` - 清屏
- `LCD_VT100_ClearToEOL()` - 清除到行尾
- `LCD_VT100_ClearToEOS()` - 清除到屏幕底部

### 文本输出
- `LCD_VT100_PutChar()` - 输出单个字符
- `LCD_VT100_PutString()` - 输出字符串
- `LCD_VT100_Printf()` - 格式化输出

## 硬件要求

- STM32F103C8T6微控制器
- **ST7789控制器的LCD显示屏**（推荐用于硬件滚动）
- 或其他支持的LCD显示屏（需要提供基本的LCD驱动函数）
- 足够的RAM来存储屏幕缓冲区

## 依赖的LCD驱动函数

确保你的LCD驱动提供以下函数：

```c
void LCD_Init(void);                    // LCD初始化
void LCD_Clear(uint16_t color);         // 清屏（可选，内部有实现）
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // 填充矩形
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color); // 画点
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ch, uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t mode); // 显示字符

// ST7789硬件滚动相关（在lcd_vt100_init.c中实现）
void LCD_WR_REG(uint8_t reg);          // 写寄存器
void LCD_WR_DATA(uint8_t data);        // 写数据
void LCD_WR_DATA8(uint8_t data);       // 写8位数据
```

## 颜色定义

确保在LCD驱动中定义了以下颜色常量：

```c
#define BLACK       0x0000
#define RED         0xF800
#define GREEN       0x07E0
#define YELLOW      0xFFE0
#define BLUE        0x001F
#define MAGENTA     0xF81F
#define CYAN        0x07FF
#define WHITE       0xFFFF
```

## 注意事项

1. 字符显示依赖于LCD驱动的字符显示函数
2. **硬件滚动**：ST7789支持高效的硬件滚动，适用于≤162像素高度的屏幕
3. **滚动回退**：屏幕高度>162时自动使用软件清屏方式
4. 转义序列缓冲区大小为32字节，超长的控制序列会被截断
5. 颜色映射是简化的，可能与标准VT100不完全一致
6. **硬件滚动区域**：tfa + vsa + bta 必须等于162（ST7789限制）

## 示例应用

这个VT100终端模拟器可以用于：
- 嵌入式系统的调试输出
- 用户界面显示
- 系统状态监控
- 简单的文本编辑器
- 命令行接口模拟