#ifndef __LCD_VT100_INIT_H__
#define __LCD_VT100_INIT_H__

#include <stdint.h>

// ST7789硬件滚动功能
int LCD_Set_Scroll_Area(uint16_t tfa, uint16_t vsa, uint16_t bta);
void LCD_Set_Scroll_Start_Address(uint16_t vsp);

#endif /* __LCD_VT100_INIT_H__ */