#include "lcd_vt100.h"
#include "lcd_vt100_init.h"
#include "lcd.h"
#include "lcd_init.h"
#include <string.h>
#include <stdlib.h>

// VT100终端状态结构
typedef struct {
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t char_width;
    uint16_t char_height;
    uint16_t max_cols;
    uint16_t max_rows;

    uint16_t cursor_x;
    uint16_t cursor_y;
    uint16_t fg_color;
    uint16_t bg_color;

    uint8_t escape_state;
    char escape_buffer[32];
    uint8_t escape_index;
    
    // 硬件滚动相关
    uint16_t scroll_area;       // 可滚动区域大小
    uint16_t scroll_start;      // 滚动起始地址
} vt100_state_t;

static vt100_state_t vt100;

static void LCD_Clear(uint16_t color) {
    LCD_Fill(0, 0, vt100.screen_width - 1, vt100.screen_height - 1, color);
}

// 初始化VT100终端
void LCD_VT100_Init(uint16_t screen_width, uint16_t screen_height, 
                   uint16_t char_width, uint16_t char_height)
{
    vt100.screen_width = screen_width;
    vt100.screen_height = screen_height;
    vt100.char_width = char_width;
    vt100.char_height = char_height;
    vt100.max_cols = screen_width / char_width;
    vt100.max_rows = screen_height / char_height;
    vt100.cursor_x = 0;
    vt100.cursor_y = 0;
    vt100.fg_color = WHITE;
    vt100.bg_color = BLACK;
    vt100.escape_state = 0;
    vt100.escape_index = 0;
    
    // 初始化硬件滚动区域
    vt100.scroll_area = screen_height;
    vt100.scroll_start = 0;
    
    // 设置硬件滚动区域 
    if (LCD_Set_Scroll_Area(0, vt100.scroll_area, 0) == 0) {
        LCD_Set_Scroll_Start_Address(vt100.scroll_start);
    } else {
        // 如果屏幕高度超过162，禁用硬件滚动
        vt100.scroll_area = 0;
    }
}

// 设置光标位置
void LCD_VT100_SetCursor(uint16_t x, uint16_t y)
{
    if (x < vt100.max_cols && y < vt100.max_rows) {
        vt100.cursor_x = x;
        vt100.cursor_y = y;
    }
}

// 获取光标位置
void LCD_VT100_GetCursor(uint16_t *x, uint16_t *y)
{
    *x = vt100.cursor_x;
    *y = vt100.cursor_y;
}

// 清屏
void LCD_VT100_Clear(void)
{
    LCD_Fill(0, 0, vt100.screen_width - 1, vt100.screen_height - 1, vt100.bg_color);
    vt100.cursor_x = 0;
    vt100.cursor_y = 0;
}

// 清除从光标到行尾
void LCD_VT100_ClearToEOL(void)
{
    uint16_t start_x = vt100.cursor_x * vt100.char_width;
    uint16_t start_y = vt100.cursor_y * vt100.char_height;
    uint16_t width = vt100.screen_width - start_x;
    
    LCD_Fill(start_x, start_y, start_x + width - 1, 
             start_y + vt100.char_height - 1, vt100.bg_color);
}

// 清除从光标到屏幕底部
void LCD_VT100_ClearToEOS(void)
{
    // 清除当前行剩余部分
    LCD_VT100_ClearToEOL();
    
    // 清除下面的行
    if (vt100.cursor_y + 1 < vt100.max_rows) {
        uint16_t start_y = (vt100.cursor_y + 1) * vt100.char_height;
        LCD_Fill(0, start_y, vt100.screen_width - 1, 
                 vt100.screen_height - 1, vt100.bg_color);
    }
}

// 向上滚动一行
void LCD_VT100_ScrollUp(void)
{
    if (vt100.scroll_area > 0) {
        // 使用硬件滚动
        vt100.scroll_start += vt100.char_height;
        
        // 如果滚动超出范围，回到起始位置
        if (vt100.scroll_start >= vt100.scroll_area) {
            vt100.scroll_start = 0;
        }
        
        // 设置新的滚动起始地址
        LCD_Set_Scroll_Start_Address(vt100.scroll_start);
        
        // 清除即将显示的新行区域
        uint16_t clear_y_start = (vt100.max_rows - 1) * vt100.char_height;
        LCD_Fill(0, clear_y_start, vt100.screen_width - 1, 
                 clear_y_start + vt100.char_height - 1, vt100.bg_color);
    } else {
        // 硬件滚动不可用，使用软件清屏方式
        LCD_VT100_Clear();
    }
}

// 设置颜色（简化的颜色映射）
void LCD_VT100_SetColor(uint8_t color_code)
{
    switch (color_code) {
        case 30: vt100.fg_color = BLACK; break;
        case 31: vt100.fg_color = RED; break;
        case 32: vt100.fg_color = GREEN; break;
        case 33: vt100.fg_color = YELLOW; break;
        case 34: vt100.fg_color = BLUE; break;
        case 35: vt100.fg_color = MAGENTA; break;
        case 36: vt100.fg_color = CYAN; break;
        case 37: vt100.fg_color = WHITE; break;
        case 40: vt100.bg_color = BLACK; break;
        case 41: vt100.bg_color = RED; break;
        case 42: vt100.bg_color = GREEN; break;
        case 43: vt100.bg_color = YELLOW; break;
        case 44: vt100.bg_color = BLUE; break;
        case 45: vt100.bg_color = MAGENTA; break;
        case 46: vt100.bg_color = CYAN; break;
        case 47: vt100.bg_color = WHITE; break;
        default: break;
    }
}

// 处理转义序列
void LCD_VT100_ProcessEscape(void)
{
    char *ptr = vt100.escape_buffer;
    
    if (ptr[0] == '[') {
        ptr++; // 跳过 '['
        
        // 解析数字参数
        uint16_t params[4] = {0};
        uint8_t param_count = 0;
        
        while (*ptr && param_count < 4) {
            if (*ptr >= '0' && *ptr <= '9') {
                params[param_count] = params[param_count] * 10 + (*ptr - '0');
            } else if (*ptr == ';') {
                param_count++;
            } else {
                break;
            }
            ptr++;
        }
        
        // 执行命令
        switch (*ptr) {        
            case 'A': // 光标上移
                if (vt100.cursor_y > 0) {
                    vt100.cursor_y -= (params[0] > 0 ? params[0] : 1);
                    if (vt100.cursor_y >= vt100.max_rows) vt100.cursor_y = 0;
                }
                break;
                
            case 'B': // 光标下移
                vt100.cursor_y += (params[0] > 0 ? params[0] : 1);
                if (vt100.cursor_y >= vt100.max_rows) {
                    vt100.cursor_y = vt100.max_rows - 1;
                }
                break;
                
            case 'C': // 光标右移
                vt100.cursor_x += (params[0] > 0 ? params[0] : 1);
                if (vt100.cursor_x >= vt100.max_cols) {
                    vt100.cursor_x = vt100.max_cols - 1;
                }
                break;
                
            case 'D': // 光标左移
                if (vt100.cursor_x > 0) {
                    vt100.cursor_x -= (params[0] > 0 ? params[0] : 1);
                    if (vt100.cursor_x >= vt100.max_cols) vt100.cursor_x = 0;
                }
                break;

            case 'E': // 光标下移并回到行首
                vt100.cursor_y += (params[0] > 0 ? params[0] : 1);
                if (vt100.cursor_y >= vt100.max_rows) {
                    vt100.cursor_y = vt100.max_rows - 1;
                }
                vt100.cursor_x = 0;
                break;

            case 'F': // 光标上移并回到行首
                if (vt100.cursor_y > 0) {
                    vt100.cursor_y -= (params[0] > 0 ? params[0] : 1);
                    if (vt100.cursor_y >= vt100.max_rows) vt100.cursor_y = 0;
                }
                break;
            
            case 'G': // 光标水平定位
                if (param_count >= 0) {
                    vt100.cursor_x = (params[0] > 0 ? params[0] - 1 : 0);
                    if (vt100.cursor_x >= vt100.max_cols) {
                        vt100.cursor_x = vt100.max_cols - 1;
                    }
                }
                break;

            case 'H': // 设置光标位置
            case 'f':
                if (param_count >= 1) {
                    LCD_VT100_SetCursor(params[1] > 0 ? params[1] - 1 : 0, 
                                       params[0] > 0 ? params[0] - 1 : 0);
                } else {
                    LCD_VT100_SetCursor(0, 0);
                }
                break;

            case 'J': // 清屏
                if (params[0] == 0) {
                    LCD_VT100_ClearToEOS();
                } else if (params[0] == 2) {
                    LCD_VT100_Clear();
                }
                break;
                
            case 'K': // 清行
                if (params[0] == 0) {
                    LCD_VT100_ClearToEOL();
                } else if (params[0] == 2) {
                    // 清整行
                    uint16_t start_y = vt100.cursor_y * vt100.char_height;
                    LCD_Fill(0, start_y, vt100.screen_width - 1, 
                             start_y + vt100.char_height - 1, vt100.bg_color);
                }
                break;
                
            case 'm': // 设置显示属性
                for (uint8_t i = 0; i <= param_count; i++) {
                    if (params[i] == 0) {
                        // 重置所有属性
                        vt100.fg_color = WHITE;
                        vt100.bg_color = BLACK;
                    } else {
                        LCD_VT100_SetColor(params[i]);
                    }
                }
                break;
                
            default:
                break;
        }
    }
}

// 换行处理
void LCD_VT100_NewLine(void)
{
    vt100.cursor_x = 0;
    if (vt100.cursor_y < vt100.max_rows - 1) {
        vt100.cursor_y++;
    } else {
        // 滚动屏幕
        LCD_VT100_ScrollUp();
    }
}

// 回车处理
void LCD_VT100_CarriageReturn(void)
{
    vt100.cursor_x = 0;
}

// 退格处理
void LCD_VT100_Backspace(void)
{   
    if (vt100.cursor_x > 0) {
        vt100.cursor_x--;
        // 清除当前位置的字符
        uint16_t x = vt100.cursor_x * vt100.char_width;
        uint16_t y = vt100.cursor_y * vt100.char_height;
        LCD_Fill(x, y, x + vt100.char_width - 1, 
                    y + vt100.char_height - 1, vt100.bg_color);
    }
}

// 制表符处理
void LCD_VT100_Tab(void)
{
    uint8_t spaces = 4 - (vt100.cursor_x % 4);
    for (uint8_t i = 0; i < spaces; i++) {
        LCD_VT100_PutChar(' ');
    }
}

// 字母显示功能
void LCD_VT100_Letter(char ch)
{
    if (ch >= 32 && ch < 127) { // 可打印字符
        uint16_t x = vt100.cursor_x * vt100.char_width;
        uint16_t y = vt100.cursor_y * vt100.char_height;
        
        // 显示字符
        LCD_ShowChar(x, y, ch, vt100.fg_color, vt100.bg_color, vt100.char_height, 0);

        // 移动光标
        vt100.cursor_x++;
        if (vt100.cursor_x >= vt100.max_cols) {
            LCD_VT100_NewLine();
        }
    }
}

// 显示单个字符
void LCD_VT100_PutChar(char ch)
{
    // 处理转义序列
    if (vt100.escape_state == 1) {
        if (ch == '[') {
            vt100.escape_buffer[vt100.escape_index++] = ch;
            vt100.escape_state = 2;
        } else {
            vt100.escape_state = 0;
            vt100.escape_index = 0;
        }
        return;
    } else if (vt100.escape_state == 2) {
        vt100.escape_buffer[vt100.escape_index++] = ch;
        
        // 检查是否为结束字符
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            vt100.escape_buffer[vt100.escape_index] = '\0';
            LCD_VT100_ProcessEscape();
            vt100.escape_state = 0;
            vt100.escape_index = 0;
        } else if (vt100.escape_index >= sizeof(vt100.escape_buffer) - 1) {
            // 缓冲区溢出，重置状态
            vt100.escape_state = 0;
            vt100.escape_index = 0;
        }
        return;
    }
    
    // 处理控制字符
    switch (ch) {
        case '\033': // ESC
            vt100.escape_state = 1;
            vt100.escape_index = 0;
            break;
            
        case '\n': // 换行
            LCD_VT100_NewLine();
            break;
            
        case '\r': // 回车
            LCD_VT100_CarriageReturn();
            break;
            
        case '\b': // 退格
            LCD_VT100_Backspace();
            break;
            
        case '\t': // 制表符
            LCD_VT100_Tab();
            break;
            
        default: // 普通字符
            LCD_VT100_Letter(ch);
            break;
    }
}

// 显示字符串（支持VT100控制码）
void LCD_VT100_PutString(const char *str)
{
    while (*str) {
        LCD_VT100_PutChar(*str++);
    }
}

// 格式化输出（类似printf）
void LCD_VT100_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    LCD_VT100_PutString(buffer);
}