// Header:	AT89C52
// File Name: MAX7219
// Author:	Kry
// Date:	2025/6/11
// Version:	1.0
// Description:	基于MAX7219设计的级联型16X16汉字显示模块
/*
    支持三种方式显示，索引方式、字符串方式、单个汉字方式。
    1. 索引方式：通过索引直接显示汉字，效率更高。
    2. 字符串方式：显示字符串，支持汉字和ASCII字符。
    3. 单个汉字方式：显示单个汉字，适合动态显示。
    4. 支持8x8字符显示。
*/
#include <reg52.h>
#include "intrins.h"
#include "Max7219.h"
/******************** */
// 数字字符取模数据示例（8x8）
uint8_t code num_0[8] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x3C};
uint8_t code num_1[8] = {0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x7E};

ScrollParams_t scroll_struct;
void main()
{
    uint8_t i, j;
    MAX7219Init();
    initDisplayBuffer(); // 初始化显示缓冲区
    // 使用模块化滚动系统
    scroll_struct.text = "你好世界我是谁"; // 显示文本
    scroll_struct.direction = 0;         // 从右到左
    scroll_struct.speed = 30;            // 滚动速度
    scroll_struct.pause_start = 1000;    // 开始暂停1秒
    scroll_struct.pause_end = 1000;      // 结束暂停1秒
    scroll_struct.repeat = 1;            // 重复2次
    scroll_struct.font_spacing = 1;      // 字符间距
    while(1)
    {
        // // 水平滚动（从右到左）
        // scroll_struct.direction = 0;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // // 水平滚动（从左到右）
        // scroll_struct.direction = 1;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // 垂直滚动（从上到下）
        // scroll_struct.direction = 2;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // 垂直滚动（从下到上）
        scroll_struct.direction = 3;
        scrollDisplay(&scroll_struct);
        delay_ms(500);
        scrollDisplay(&scroll_struct);
        MAX7219_ShowChineseString(0,0, "我是谁");
        updateDisplay();
    }
}