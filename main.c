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
/******************** */
// �����ַ�ȡģ����ʾ����8x8��
uint8_t code num_0[8] = {0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x3C};
uint8_t code num_1[8] = {0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x7E};
void main()
{
    uint8_t i, j;
    MAX7219Init();
    initDisplayBuffer(); // ��ʼ����ʾ������
    // while(1)
    // {
    //     clearScreen();
        
    //     #if MAX7219_HORIZONTAL_MODULES == 1
    //         // ����16x16ģ����ʾ
    //         MAX7219_ShowSingleChinese(0, 0, "��");
    //     #elif MAX7219_HORIZONTAL_MODULES == 2
    //         // ����16x16ģ����ʾ
    //         MAX7219_ShowChineseString(0, 0, "���");
    //     #elif MAX7219_HORIZONTAL_MODULES == 3
    //         // ����16x16ģ����ʾ
    //         MAX7219_ShowChineseString(0, 0, "�����");
    //     #elif MAX7219_HORIZONTAL_MODULES == 4
    //         // �ĸ�16x16ģ����ʾ
    //         MAX7219_ShowChineseString(0, 0, "�������");
    //     #endif
    //     updateDisplay();
    //     delay_ms(2000); // ��ʱ2��
    //     clearScreen();
    //     for(i = 0; i < MAX_X; i++) {
    //         drawPixel(i, 0, 1);              // ����
    //         drawPixel(i, MAX_Y - 1, 1);      // �ױ�
    //     }
    //     for(j = 0; j < MAX_Y; j++) {
    //         drawPixel(0, j, 1);              // ���
    //         drawPixel(MAX_X - 1, j, 1);      // �ұ�
    //     }
    //     updateDisplay();
    //     delay_ms(2000); // ��ʱ2��

    // }
    while(1)
    {
        // ʹ��ģ�黯����ϵͳ
        ScrollParams_t params;
        params.text = "�������";
        params.direction = 0;         // ���ҵ���
        params.speed = 80;            // �����ٶ�
        params.pause_start = 1000;    // ��ʼ��ͣ1��
        params.pause_end = 1000;      // ������ͣ1��
        params.repeat = 2;            // �ظ�2��
        params.font_spacing = 1;      // �ַ����
    
        scrollDisplay(&params);
    
        // ����ѭ������
        params.repeat = 0;            // ����ѭ��
        params.pause_start = 0;       // ����ͣ
        params.pause_end = 0;         // ����ͣ
    
        scrollDisplay(&params);
    }
}