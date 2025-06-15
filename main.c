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
void main()
{
    uint8_t i, j;
    MAX7219Init();
    initDisplayBuffer(); // 初始化显示缓冲区
    // while(1)
    // {
    //     clearScreen();
        
    //     #if MAX7219_HORIZONTAL_MODULES == 1
    //         // 单个16x16模块显示
    //         MAX7219_ShowSingleChinese(0, 0, "你");
    //     #elif MAX7219_HORIZONTAL_MODULES == 2
    //         // 两个16x16模块显示
    //         MAX7219_ShowChineseString(0, 0, "你好");
    //     #elif MAX7219_HORIZONTAL_MODULES == 3
    //         // 三个16x16模块显示
    //         MAX7219_ShowChineseString(0, 0, "你好世");
    //     #elif MAX7219_HORIZONTAL_MODULES == 4
    //         // 四个16x16模块显示
    //         MAX7219_ShowChineseString(0, 0, "你好世界");
    //     #endif
    //     updateDisplay();
    //     delay_ms(2000); // 延时2秒
    //     clearScreen();
    //     for(i = 0; i < MAX_X; i++) {
    //         drawPixel(i, 0, 1);              // 顶边
    //         drawPixel(i, MAX_Y - 1, 1);      // 底边
    //     }
    //     for(j = 0; j < MAX_Y; j++) {
    //         drawPixel(0, j, 1);              // 左边
    //         drawPixel(MAX_X - 1, j, 1);      // 右边
    //     }
    //     updateDisplay();
    //     delay_ms(2000); // 延时2秒

    // }
    while(1)
    {
        // 使用模块化滚动系统
        ScrollParams_t params;
        params.text = "你好世界";
        params.direction = 0;         // 从右到左
        params.speed = 80;            // 滚动速度
        params.pause_start = 1000;    // 开始暂停1秒
        params.pause_end = 1000;      // 结束暂停1秒
        params.repeat = 2;            // 重复2次
        params.font_spacing = 1;      // 字符间距
    
        scrollDisplay(&params);
    
        // 无限循环滚动
        params.repeat = 0;            // 无限循环
        params.pause_start = 0;       // 不暂停
        params.pause_end = 0;         // 不暂停
    
        scrollDisplay(&params);
    }
}