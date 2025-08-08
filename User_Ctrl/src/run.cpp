#include "run.h"
#include "run.hpp"

void setup() {
  // put your setup code here, to run once:

	LCD_Init();//LCDLCD初始化
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);	
//	LCD_ShowPicture(20,45,120,29,gImage_pic1);
	LCD_ShowString(50,0,(std::uint8_t*)("CIRNO!!!"),BLACK,WHITE,16,0);
	LCD_ShowChinese(50,20,(std::uint8_t*)("无名科技"),BLACK,WHITE,16,0);	

}

void loop() {
  // put your main code here, to run repeatedly:

}