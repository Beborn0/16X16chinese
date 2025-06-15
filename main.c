// Header:	AT89C52
// File Name: MAX7219
// Author:	Kry
// Date:	2025/6/11
// Version:	1.0
// Description:	����MAX7219��Ƶļ�����16X16������ʾģ��
/*
    ֧�����ַ�ʽ��ʾ��������ʽ���ַ�����ʽ���������ַ�ʽ��
    1. ������ʽ��ͨ������ֱ����ʾ���֣�Ч�ʸ��ߡ�
    2. �ַ�����ʽ����ʾ�ַ�����֧�ֺ��ֺ�ASCII�ַ���
    3. �������ַ�ʽ����ʾ�������֣��ʺ϶�̬��ʾ��
    4. ֧��8x8�ַ���ʾ��
*/
#include <reg52.h>
#include "intrins.h"
#include "Max7219.h"
/**********delay********** */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 127; j++);
}
/******************** */
// �����ַ�ȡģ����ʾ����8x8��
uint8_t code num_0[8] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x3C};
uint8_t code num_1[8] = {0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x7E};
void main()
{
    uint8_t i, j;
    MAX7219Init();
    initDisplayBuffer(); // ��ʼ����ʾ������
    while(1)
    {
        clearScreen();
        
        #if MAX7219_HORIZONTAL_MODULES == 1
            // ����16x16ģ����ʾ
            MAX7219_ShowSingleChinese(0, 0, "��");
        #elif MAX7219_HORIZONTAL_MODULES == 2
            // ����16x16ģ����ʾ
            MAX7219_ShowChineseString(0, 0, "���");
        #elif MAX7219_HORIZONTAL_MODULES == 3
            // ����16x16ģ����ʾ
            MAX7219_ShowChineseString(0, 0, "�����");
        #elif MAX7219_HORIZONTAL_MODULES == 4
            // �ĸ�16x16ģ����ʾ
            MAX7219_ShowChineseString(0, 0, "�������");
        #endif
        updateDisplay();
        delay_ms(2000); // ��ʱ2��
        clearScreen();
        for(i = 0; i < MAX_X; i++) {
            drawPixel(i, 0, 1);              // ����
            drawPixel(i, MAX_Y - 1, 1);      // �ױ�
        }
        for(j = 0; j < MAX_Y; j++) {
            drawPixel(0, j, 1);              // ���
            drawPixel(MAX_X - 1, j, 1);      // �ұ�
        }
        updateDisplay();
        delay_ms(2000); // ��ʱ2��
        delay_ms(2000); // ��ʱ2��
    }
}