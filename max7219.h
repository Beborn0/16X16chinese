#ifndef _Max7219_H_
#define _Max7219_H_

#include <REG52.H>
#include <intrins.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define uint8_t unsigned char
#define uint8_t unsigned char
#define uint unsigned char
#define int8_t  char
#define uint16_t  unsigned int
#define uint32_t unsigned long
#define int16_t  int
#define IS_COORD_VALID(x, y) ((x) < MAX_X && (y) < MAX_Y) // 检查坐标是否有效
#define IS_MODULE_VALID(module_id) ((module_id) < MAX7219_TOTAL_MODULES) // 检查模块ID是否有效
#define number  4 //点阵级联个数
#define lum     5 //亮度级别，0-F,共16级


// 模块配置宏定义区
#define MAX7219_HORIZONTAL_MODULES 2    // 水平方向模块数量
#define MAX7219_VERTICAL_MODULES   1    // 垂直方向模块数量
#define MAX7219_TOTAL_MODULES      (MAX7219_HORIZONTAL_MODULES * MAX7219_VERTICAL_MODULES * 4)  // 总8x8模块数量
// 虚拟缓冲区定义
#define VIRTUAL_WIDTH      MAX7219_TOTAL_MODULES * 16     // 虚拟宽度，可根据需要调整
#define VIRTUAL_HEIGHT     16      // 虚拟高度，通常与实际显示高度相同

// 显示参数常量
#define MODULE_DOTS          8     // 每个模块的点数（8x8）
#define CHAR_WIDTH          16     // 汉字宽度
#define CHAR_HEIGHT         16     // 汉字高度
#define ASCII_WIDTH          8     // ASCII字符宽度

// 计算显示范围
#define MAX_X               (MAX7219_HORIZONTAL_MODULES * CHAR_WIDTH)   // X轴最大坐标
#define MAX_Y               (MAX7219_VERTICAL_MODULES * CHAR_HEIGHT)    // Y轴最大坐标

//定义Max7219端口
sbit CLK = P2 ^ 2; //时钟 Max7219_pin
sbit CS  = P2 ^ 1; //锁存
sbit DIN = P2 ^ 0; //数据

// 汉字字库结构体
/*字模基本单元*/
typedef struct 
{
    char Index[3];          // 汉字索引，GB2312编码，2字节汉字+1字节结束符
    uint8_t Data[32];         // 16x16字模数据，行列式低位在前
} ChineseCell_t;

/*汉字字模数据声明*/
extern const ChineseCell_t  code LED_CF16x16[];


void Max7219WR(uint8_t addr, uint8_t dat);
void MAX7219Init();

void initDisplayBuffer();
void drawPixel(uint8_t x, uint8_t y, uint8_t state);
void updateDisplay();
void clearScreen();
void clearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void displayS_Type();
void displayHanzi16x16_RowColumn_LowFirst(uint8_t x_offset, uint8_t y_offset, uint8_t *hanzi_data);
void displayChar8x8(uint8_t x_offset, uint8_t y_offset, uint8_t *char_data);
void MAX7219_ShowString(uint8_t X, uint8_t Y, char *String);
void MAX7219_ShowSingleChinese(uint8_t X, uint8_t Y, char *String);
void MAX7219_ShowChineseByIndex(uint8_t X, uint8_t Y, uint8_t index);
void MAX7219_ShowChineseString(uint8_t x_offset, uint8_t y_offset, char *String);
uint8_t strcmp_custom(char *str1, char *str2);
void scrollHorizontal(char *string, uint8_t speed_delay, uint8_t cycles);

#endif