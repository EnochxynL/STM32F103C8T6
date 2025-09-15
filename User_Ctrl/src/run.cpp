#include "run.h"
#include <rtthread.h>
extern "C" {
#include "lcd_init.h"
#include "lcd.h"
#include "lcd_vt100_demo.h"
#include "lcd_vt100.h"
}

void setup() {
  // put your setup code here, to run once:
	LCD_Init();//LCDLCD初始化
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);	
//	LCD_ShowPicture(20,45,120,29,gImage_pic1);
	LCD_ShowString(50,0,(std::uint8_t*)("CIRNO!!!"),BLACK,WHITE,16,0);
	LCD_ShowChinese(50,20,(std::uint8_t*)("无名科技"),BLACK,WHITE,16,0);	
//	LCD_VT100_Init(LCD_W, LCD_H, 8, 16);
//	LCD_VT100_Printf("\033[31mRed Text\033[0m\n");        // 红色文字
//	LCD_VT100_Printf("\033[32mGreen Text\033[0m\n");      // 绿色文字
//	LCD_VT100_Printf("\033[34mBlue Text\033[0m\n");       // 蓝色文字
	LCD_VT100_ScrollTest();
}

void loop() {
  // put your main code here, to run repeatedly:
  rt_thread_mdelay(1000);
  rt_kprintf("\nHello RT-Thread \n");
}