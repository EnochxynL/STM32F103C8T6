#ifndef __LCD_VT100_DEMO_H__
#define __LCD_VT100_DEMO_H__

#include <stdint.h>

// VT100终端演示函数
void LCD_VT100_Demo(void);

// 系统信息显示
void LCD_VT100_ShowSystemInfo(void);

// 进度条显示
void LCD_VT100_ShowProgress(uint8_t percent);

// 菜单显示
void LCD_VT100_ShowMenu(void);

// 滚动测试
void LCD_VT100_ScrollTest(void);

#endif /* __LCD_VT100_DEMO_H__ */