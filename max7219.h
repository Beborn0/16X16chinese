#ifndef _Max7219_H_
#define _Max7219_H_

#include <REG52.H>
#include <intrins.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define uchar unsigned char
#define uint8_t unsigned char
#define uint unsigned char
#define int8_t  char
#define uint16_t  unsigned int
#define uint32_t unsigned long
#define int16_t  int
#define IS_COORD_VALID(x, y) ((x) < MAX_X && (y) < MAX_Y) // ��������Ƿ���Ч
#define IS_MODULE_VALID(module_id) ((module_id) < MAX7219_TOTAL_MODULES) // ���ģ��ID�Ƿ���Ч
#define number  4 //����������
#define lum     5 //���ȼ���0-F,��16��


// ģ�����ú궨����
#define MAX7219_HORIZONTAL_MODULES 2    // ˮƽ����ģ������
#define MAX7219_VERTICAL_MODULES   1    // ��ֱ����ģ������
#define MAX7219_TOTAL_MODULES      (MAX7219_HORIZONTAL_MODULES * MAX7219_VERTICAL_MODULES * 4)  // ��8x8ģ������
// ���⻺��������
#define VIRTUAL_WIDTH      MAX7219_TOTAL_MODULES * 16     // �����ȣ��ɸ�����Ҫ����
#define VIRTUAL_HEIGHT     16      // ����߶ȣ�ͨ����ʵ����ʾ�߶���ͬ

// ��ʾ��������
#define MODULE_DOTS          8     // ÿ��ģ��ĵ�����8x8��
#define CHAR_WIDTH          16     // ���ֿ��
#define CHAR_HEIGHT         16     // ���ָ߶�
#define ASCII_WIDTH          8     // ASCII�ַ����

// ������ʾ��Χ
#define MAX_X               (MAX7219_HORIZONTAL_MODULES * CHAR_WIDTH)   // X���������
#define MAX_Y               (MAX7219_VERTICAL_MODULES * CHAR_HEIGHT)    // Y���������

//����Max7219�˿�
sbit CLK = P2 ^ 2; //ʱ�� Max7219_pin
sbit CS  = P2 ^ 1; //����
sbit DIN = P2 ^ 0; //����

// �����ֿ�ṹ��
/*��ģ������Ԫ*/
typedef struct 
{
    char Index[3];          // ����������GB2312���룬2�ֽں���+1�ֽڽ�����
    uint8_t Data[32];         // 16x16��ģ���ݣ�����ʽ��λ��ǰ
} ChineseCell_t;

/*������ģ��������*/
extern const ChineseCell_t  code LED_CF16x16[];


void Max7219WR(uchar addr, uchar dat);
void MAX7219Init();

void initDisplayBuffer();
void drawPixel(uchar x, uchar y, uchar state);
void updateDisplay();
void clearScreen();
void clearArea(uchar x, uchar y, uchar width, uchar height);
void drawLine(uchar x0, uchar y0, uchar x1, uchar y1);
void drawRect(uchar x, uchar y, uchar width, uchar height);

void displayS_Type();
void displayHanzi16x16_RowColumn_LowFirst(uchar x_offset, uchar y_offset, uchar *hanzi_data);
void displayChar8x8(uchar x_offset, uchar y_offset, uchar *char_data);
void MAX7219_ShowString(uchar X, uchar Y, char *String);
void MAX7219_ShowSingleChinese(uchar X, uchar Y, char *String);
void MAX7219_ShowChineseByIndex(uchar X, uchar Y, uchar index);
void MAX7219_ShowChineseString(uchar x_offset, uchar y_offset, char *String);
uchar strcmp_custom(char *str1, char *str2);

#endif