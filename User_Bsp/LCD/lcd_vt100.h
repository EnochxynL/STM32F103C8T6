#ifndef __LCD_VT100_H__
#define __LCD_VT100_H__

#include <stdint.h>
#include <stdarg.h>

// VT100终端初始化
void LCD_VT100_Init(uint16_t screen_width, uint16_t screen_height, 
                   uint16_t char_width, uint16_t char_height);

// 光标控制
void LCD_VT100_SetCursor(uint16_t x, uint16_t y);
void LCD_VT100_GetCursor(uint16_t *x, uint16_t *y);

// 清屏功能
void LCD_VT100_Clear(void);
void LCD_VT100_ClearToEOL(void);
void LCD_VT100_ClearToEOS(void);

// 滚动和换行
void LCD_VT100_ScrollUp(void);
void LCD_VT100_NewLine(void);
void LCD_VT100_CarriageReturn(void);

// 颜色设置
void LCD_VT100_SetColor(uint8_t color_code);

// 转义序列处理
void LCD_VT100_ProcessEscape(void);

// 字符输出
void LCD_VT100_PutChar(char ch);
void LCD_VT100_PutString(const char *str);
void LCD_VT100_Printf(const char *format, ...);

#endif /* __LCD_VT100_H__ */