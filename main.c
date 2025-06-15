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

ScrollParams_t scroll_struct;
void main()
{
    uint8_t i, j;
    MAX7219Init();
    initDisplayBuffer(); // ��ʼ����ʾ������
    // ʹ��ģ�黯����ϵͳ
    scroll_struct.text = "�����������˭"; // ��ʾ�ı�
    scroll_struct.direction = 0;         // ���ҵ���
    scroll_struct.speed = 30;            // �����ٶ�
    scroll_struct.pause_start = 1000;    // ��ʼ��ͣ1��
    scroll_struct.pause_end = 1000;      // ������ͣ1��
    scroll_struct.repeat = 1;            // �ظ�2��
    scroll_struct.font_spacing = 1;      // �ַ����
    while(1)
    {
        // // ˮƽ���������ҵ���
        // scroll_struct.direction = 0;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // // ˮƽ�����������ң�
        // scroll_struct.direction = 1;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // ��ֱ���������ϵ��£�
        // scroll_struct.direction = 2;
        // scrollDisplay(&scroll_struct);
        // delay_ms(500);
        
        // ��ֱ���������µ��ϣ�
        scroll_struct.direction = 3;
        scrollDisplay(&scroll_struct);
        delay_ms(500);
        scrollDisplay(&scroll_struct);
        MAX7219_ShowChineseString(0,0, "����˭");
        updateDisplay();
    }
}