#include "lcd_vt100_init.h"
#include "lcd_init.h"

#define LCD_RAM_HEIGHT 162  // ST7735的RAM高度限制

/**
 * @brief Vertical Scrolling Definition.
 * @param   tfa    top fixed area
 * @param   vsa    scroll area
 * @param   bta    bottom fixed area
 * @return  errcode
 * @retval  0      success
 * @retval  -1     fail 
 * @note    https://cdn.sparkfun.com/assets/9/0/2/5/8/ST7735S_v1.1.pdf 
 */
int LCD_Set_Scroll_Area(uint16_t tfa, uint16_t vsa, uint16_t bta)
{
    if (tfa + vsa + bta != LCD_RAM_HEIGHT) {
        bta = LCD_RAM_HEIGHT - tfa - vsa;
        if (tfa + vsa + bta != LCD_RAM_HEIGHT) {
            return -1;
        }
    }
    
    LCD_WR_REG(0x33);
    LCD_WR_DATA(tfa);
    LCD_WR_DATA(vsa);
    LCD_WR_DATA(bta);
    return 0;
}

/**
 * @brief Set Vertical scroll start address of RAM.
 * @param   vsp    scroll start address of RAM
 * @return  none
 */
void LCD_Set_Scroll_Start_Address(uint16_t vsp)
{
    LCD_WR_REG(0x37);
    LCD_WR_DATA8(vsp / 256);
    LCD_WR_DATA8(vsp % 256);
}
