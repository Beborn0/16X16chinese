#include "max7219.h"

/**********delay********** */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 127; j++);
}

//��MAX7219 д���ַ������
void Max7219WR(uint8_t addr, uint8_t dat)
{
    uint8_t n = 8;
    CLK = 0;
    CS = 0; //CS=0��Ч,CS=1����
    /*�������� ��λ�ȷ�*/
    while(n--)
    {
        DIN = addr & 0x80;
        CLK = 1;
        addr <<= 1;
        CLK = 0;
    }
    n = 8;
    /*�������� ��λ�ȷ�*/
    while(n--)
    {
        DIN = dat & 0x80;
        CLK = 1;
        dat <<= 1;
        CLK = 0;
    }
}

void MAX7219Init()
{
    uint8_t i, j;

    for(i = 0; i < 5; i++) //
    {
        for(j = number; j > 0; j--)
            switch(i)
            {
                case 0:
                    Max7219WR(0x09, 0x00);
                    break; //���뷽ʽ��BCD�� ������

                case 1:
                    Max7219WR(0x0A, lum);
                    break; //���� 0-15 ��16��

                case 2:
                    Max7219WR(0x0B, 0x07);
                    break; //ɨ����ޣ�8λ��ʾ

                case 3:
                    Max7219WR(0x0C, 0x01);
                    break; //����ģʽ��0����ͨģʽ��1

                case 4:
                    Max7219WR(0x0F, 0x00);
                    break; //��ʾ���ԣ�1�����Խ�����������ʾ��0
            }

        CS = 1;
        _nop_();
        CS = 0; //ÿд���Ĵθ�һ����������
    }
}

// ȫ����ʾ������
uint8_t xdata display_buffer[MAX7219_TOTAL_MODULES][MODULE_DOTS]; // ��̬���ô�С

// ��ʼ����ʾ������
void initDisplayBuffer()
{
    uint8_t i, j;
    for(i = 0; i < MAX7219_TOTAL_MODULES; i++) {
        for(j = 0; j < MODULE_DOTS; j++) {
            display_buffer[i][j] = 0x00;
        }
    }
}

// ����ģ������ĺ�
#define GET_MODULE_ID(x, y) ((y / MODULE_DOTS) * (MAX7219_HORIZONTAL_MODULES * 2) + \
                              (x / MODULE_DOTS) + ((y % MODULE_DOTS) >= MODULE_DOTS / 2 ? \
                              MAX7219_HORIZONTAL_MODULES : 0))

// �Ż���Ļ��㺯�� - ��Ӧ������������
void drawPixel(uint8_t x, uint8_t y, uint8_t state)
{
    uint8_t module_id, local_x, local_y, bit_pos;
    uint8_t horizontal_block, vertical_block;
    uint8_t local_block_x, local_block_y;

    // �������
    if(x >= MAX_X || y >= MAX_Y) return;
    
    // ����ģ��ID (�����ķ�ʽ)
    horizontal_block = x / CHAR_WIDTH;   // ˮƽ����ڼ���16x16ģ��
    vertical_block = y / CHAR_HEIGHT;    // ��ֱ����ڼ���16x16ģ��
    
    // ������16x16���ڵľֲ�����
    local_block_x = x % CHAR_WIDTH;
    local_block_y = y % CHAR_HEIGHT;
    
    // ����S�Ͳ��ּ���ģ��ID
    if(local_block_y < MODULE_DOTS) { // �ϰ벿��
        if(local_block_x < MODULE_DOTS) {
            // ���Ͻ�ģ��
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 0;
            local_x = local_block_x;
            local_y = local_block_y;
        } else {
            // ���Ͻ�ģ��
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 1;
            local_x = local_block_x - MODULE_DOTS;
            local_y = local_block_y;
        }
    } else { // �°벿��
        if(local_block_x < MODULE_DOTS) {
            // ���½�ģ��
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 2;
            local_x = local_block_x;
            local_y = local_block_y - MODULE_DOTS;
        } else {
            // ���½�ģ��
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 3;
            local_x = local_block_x - MODULE_DOTS;
            local_y = local_block_y - MODULE_DOTS;
        }
    }
    
    // ����λλ�ã�MAX7219��λ7������ߵĵ㣩
    bit_pos = 7 - local_x;
    
    // ���û������Ӧ��λ
    if(state) {
        display_buffer[module_id][local_y] |= (1 << bit_pos);  // ����
    } else {
        display_buffer[module_id][local_y] &= ~(1 << bit_pos); // Ϩ��
    }
}

// �Ż���ĸ�����ʾ���� - ֧������ģ������
void updateDisplay()
{
    uint8_t i, module;
    
    for(i = 0; i < MODULE_DOTS; i++) {
        // �����һ��ģ�鿪ʼ������������
        for(module = MAX7219_TOTAL_MODULES; module > 0; module--) {
            Max7219WR(i + 1, display_buffer[module-1][i]);
        }
        
        CS = 1; // ��������
        _nop_();
        CS = 0;
    }
}
// ��������
void clearScreen()
{
    uint8_t i, j;
    for(i = 0; i < MAX7219_TOTAL_MODULES; i++) {
        for(j = 0; j < MODULE_DOTS; j++) {
            display_buffer[i][j] = 0x00;
        }
    }
}

// ���ָ������
void clearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i, j;
    for(i = 0; i < height && y + i < MAX_Y; i++) {
        for(j = 0; j < width && x + j < MAX_X; j++) {
            drawPixel(x + j, y + i, 0);
        }
    }
}
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;
    int16_t e2;
    
    while(1) {
        drawPixel(x0, y0, 1);
        
        if(x0 == x1 && y0 == y1) break;
        
        e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// �����κ���
void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i;
    // ��������
    for(i = 0; i < width; i++) {
        drawPixel(x + i, y, 1);         // �ϱ�
        drawPixel(x + i, y + height - 1, 1); // �±�
    }
    for(i = 0; i < height; i++) {
        drawPixel(x, y + i, 1);         // ���
        drawPixel(x + width - 1, y + i, 1);  // �ұ�
    }
}

// ʹ�����е�laji�������S�ͼ���
uint8_t code laji[][8] =
{
    {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}, //A
    {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C}, //B
    {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x7C}, //C
    {0x7C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7C}, //D
};

// �������S����ʾ���Ժ���
void displayS_Type()
{
    uint8_t i;
    
    for(i = 0; i < 8; i++)  // 8��
    {
        // ����ȷ�ķ���˳��[3]��[2]��[1]��[0]
        Max7219WR(i + 1, laji[3][i]);  // ģ��4�����£�
        Max7219WR(i + 1, laji[2][i]);  // ģ��3�����£�
        Max7219WR(i + 1, laji[1][i]);  // ģ��2�����ϣ� 
        Max7219WR(i + 1, laji[0][i]);  // ģ��1�����ϣ�
        
        CS = 1;
        _nop_();
        CS = 0;
    }
}
// ����������ʽ��λ��ǰȡģ��ʽ��16x16������ʾ����
void displayHanzi16x16_RowColumn_LowFirst(uint8_t x_offset, uint8_t y_offset, uint8_t *hanzi_data)
{
    uint8_t row, col, byte_index, bit_index;
    uint8_t current_byte;
    
    // ����16x16��ÿһ����
    for(row = 0; row < 16; row++) {
        for(col = 0; col < 16; col++) {
            // ����ʽȡģ��ÿ��Ϊһ��ȡģ��λ��ÿ��16λ��Ϊ2���ֽ�
            byte_index = row * 2 + (col / 8);  // ÿ��2�ֽڣ��г���8ȷ���ֽ�
            bit_index = col % 8;               // λ��������λ��ǰ����bit0��ʼ��
            
            // ��ȡ��ǰ�ֽ�
            current_byte = hanzi_data[byte_index];
            
            // ��鵱ǰλ�Ƿ�Ϊ1��������򻭵�
            if(current_byte & (1 << bit_index)) {
                drawPixel(x_offset + col, y_offset + row, 1);
            } else {
                drawPixel(x_offset + col, y_offset + row, 0);
            }
        }
    }
}
// ��ʾ8x8С�ַ�����
void displayChar8x8(uint8_t x_offset, uint8_t y_offset, uint8_t *char_data)
{
    uint8_t row, col, bit_index;
    uint8_t current_byte;
    
    // ����8x8��ÿһ����
    for(row = 0; row < 8; row++) {
        current_byte = char_data[row];
        for(col = 0; col < 8; col++) {
            bit_index = 7 - col;  // ��λ��ǰ
            
            // ��鵱ǰλ�Ƿ�Ϊ1��������򻭵�
            if(current_byte & (1 << bit_index)) {
                drawPixel(x_offset + col, y_offset + row, 1);
            } else {
                drawPixel(x_offset + col, y_offset + row, 0);
            }
        }
    }
}

// ����ַ����ȽϺ����������׼�ⲻ���ã�
uint8_t strcmp_custom(char *str1, char *str2)
{
    while(*str1 && *str2) {
        if(*str1 != *str2) return 1;  // �����
        str1++;
        str2++;
    }
    return (*str1 != *str2);  // 0��ʾ��ȣ�1��ʾ�����
}
// ��ȡ�ֿ��С
// MAX7219��ʾ�����ַ�������
void MAX7219_ShowString(uint8_t X, uint8_t Y, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];  // GB2312���2�ֽ�+������
    uint8_t CharLength = 0;
    uint8_t XOffset = 0;
    uint8_t YOffset = 0;
    uint8_t pIndex;
    
    while (String[i] != '\0')  // �����ַ���
    {
        // GB2312�ַ�������
        // ��ȡGB2312�ַ����е�һ���ַ���ת�浽SingleChar���ַ�����
        // �ж�GB2312�ֽڵ����λ��־λ
        if ((String[i] & 0x80) == 0x00)  // ���λΪ0��ASCII�ַ�
        {
            CharLength = 1;                    // �ַ�Ϊ1�ֽ�
            SingleChar[0] = String[i++];       // ����һ���ֽ�д��SingleChar��0��λ��
            SingleChar[1] = '\0';              // ����ַ���������־λ
        }
        else  // ���λΪ1������
        {
            CharLength = 2;                    // �ַ�Ϊ2�ֽ�
            SingleChar[0] = String[i++];       // ����һ���ֽ�д��SingleChar��0��λ��
            if (String[i] == '\0') {break;}    // �������������ѭ��
            SingleChar[1] = String[i++];       // ���ڶ����ֽ�д��SingleChar��1��λ��
            SingleChar[2] = '\0';              // ����ַ���������־λ
        }
        
        // ��ʾ����������ȡ����SingleChar
        if (CharLength == 1)  // ����ǵ��ֽ��ַ���ASCII��
        {
            // ��ʾASCII�ַ���8x8��
            if (SingleChar[0] >= '0' && SingleChar[0] <= '9') {
                // ���������������ַ���ʾ
                // displayChar8x8(X + XOffset, Y + YOffset, num_data[SingleChar[0] - '0']);
            }
            XOffset += 8;  // ASCII�ַ����Ϊ8
        }
        else  // ���򣬼����ֽ��ַ������֣�
        {
            // ����������ģ�⣬����ģ����Ѱ�Ҵ��ַ�������
            // ����ҵ����һ���ַ�������Ϊ���ַ����������ʾ�ַ�δ����ģ�ⶨ��
            for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
            {
                // �ҵ�ƥ����ַ�
                if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0)
                {
                    break;  // ����ѭ������ʱpIndex��ֵΪָ���ַ�������
                }
            }
            // ����ģ��LED_CF16x16��ָ��������16x16��ͼ���ʽ��ʾ
            displayHanzi16x16_RowColumn_LowFirst(X + XOffset, Y + YOffset, 
                                               (uint8_t*)LED_CF16x16[pIndex].Data);
            XOffset += 16;  // ���ֿ��Ϊ16
        }
        // ����Ƿ���Ҫ���У�16x16��ʾ����ֻ����ʾһ�����֣�
        if (XOffset >= 16) {
            XOffset = 0;
            YOffset += 16;
            // ���������ʾ��Χ��ֹͣ��ʾ
            if (YOffset >= 16) {
                break;
            }
        }
    }
}
// �򻯰棺��ʾ���к����ַ�����ֻ��ʾ��һ���ַ���
void MAX7219_ShowSingleChinese(uint8_t X, uint8_t Y, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];
    uint8_t CharLength = 0;
    uint8_t pIndex;
    
    if (String[i] != '\0')  // ����ַ�����Ϊ��
    {
        // ��ȡ��һ���ַ�
        if ((String[i] & 0x80) == 0x00)  // ASCII�ַ�
        {
            CharLength = 1;
            SingleChar[0] = String[i];
            SingleChar[1] = '\0';
            
            // ��ʾASCII�ַ��Ĵ���
            // ����������ASCII�ַ���ʾ����
        }
        else  // ����
        {
            CharLength = 2;
            SingleChar[0] = String[i];
            SingleChar[1] = String[i + 1];
            SingleChar[2] = '\0';
            
            // ���ֿ��в��Һ���
            for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
            {
                if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0)
                {
                    break;
                }
            }
            
            // ��ʾ�ҵ��ĺ���
            displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[pIndex].Data);
        }
    }
}


// // ��������Ƿ�����ʾ��Χ�ڵĺ�
// #define IS_COORD_VALID(x, y) ((x) < MAX_X && (y) < MAX_Y)

// ��ʾ������ֵĺ���
void MAX7219_ShowChineseString(uint8_t x_offset, uint8_t y_offset, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];  // GB2312���2�ֽ�+������
    uint8_t CharLength = 0;
    uint8_t XOffset = x_offset;
    uint8_t YOffset = y_offset;
    uint8_t maxChars = MAX_X / CHAR_WIDTH;  // ����ʾ�����������
    uint8_t charCount = 0;
    uint8_t pIndex;
    
    while (String[i] != '\0' && charCount < maxChars && IS_COORD_VALID(XOffset, YOffset))
    {
        // GB2312�ַ�������
        if ((String[i] & 0x80) == 0x00)  // ASCII�ַ�
        {
            CharLength = 1;
            SingleChar[0] = String[i++];
            SingleChar[1] = '\0';
            
            // ASCII�ַ���ʾ����
            if (XOffset + ASCII_WIDTH <= MAX_X) {
                // �������ASCII�ַ���ʾ����
                XOffset += ASCII_WIDTH;
            } else {
                // ������ʾ��Χ
                break;
            }
        }
        else  // ����
        {
            CharLength = 2;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') break;
            SingleChar[1] = String[i++];
            SingleChar[2] = '\0';
            
            // �����ʾ��Χ
            if (XOffset + CHAR_WIDTH <= MAX_X) {
                // ���ֿ��в��Һ���
                for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
                {
                    if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0) {
                        break;
                    }
                }
                
                // ��ʾ�ҵ��ĺ���
                displayHanzi16x16_RowColumn_LowFirst(XOffset, YOffset, (uint8_t*)LED_CF16x16[pIndex].Data);
                XOffset += CHAR_WIDTH;
                charCount++;
            } else {
                // ������ʾ��Χ
                break;
            }
        }
    }
}

// ͨ������ֱ����ʾ���֣�����Ч��
void MAX7219_ShowChineseByIndex(uint8_t X, uint8_t Y, uint8_t index)
{
    // ���������Χ
    if (strcmp_custom(LED_CF16x16[index].Index, "") != 0)
    {
        displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[index].Data);
    }
    else
    {
        // ��ʾĬ��ͼ�Σ�����+�ʺţ�
        displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[index].Data);
    }
}
uint8_t code hanzi_ni[32] = {
    0x10,0x01,0x10,0x01,0x10,0x01,0x88,0x7F,0x88,0x40,0x4C,0x20,0x2C,0x04,0x0A,0x04,
    0x89,0x14,0x88,0x24,0x48,0x24,0x48,0x44,0x28,0x44,0x08,0x04,0x08,0x05,0x08,0x02,/*"��",0*/
};
uint8_t code hanzi_hao[32] = {
    0x08,0x00,0x08,0x3F,0x08,0x20,0x08,0x10,0x3F,0x08,0x24,0x04,0x24,0x04,0xA4,0x7F,
    0x24,0x04,0x12,0x04,0x14,0x04,0x08,0x04,0x14,0x04,0x22,0x04,0x21,0x05,0x00,0x02,/*"��",1*/
};
uint8_t code hanzi_kang[32] = {
    0x80,0x00,0x00,0x01,0xFC,0x7F,0x04,0x01,0xF4,0x1F,0x04,0x11,0xFC,0x7F,0x04,0x11,
    0xF4,0x1F,0x14,0x01,0x24,0x23,0x44,0x15,0x22,0x09,0x12,0x11,0x49,0x61,0x80,0x00,/*"��",0*/
};
uint8_t code hanzi_ru[32] = {
    0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x3E,0x7F,0x22,0x44,0x22,0x44,0x22,0x44,0x22,
    0x42,0x22,0x24,0x22,0x28,0x22,0x10,0x22,0x28,0x22,0x44,0x3E,0x42,0x22,0x01,0x00,/*"��",1*/
};
uint8_t code hanzi_yun[32] = {
    0x00,0x00,0xFC,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x40,0x00,
    0x20,0x00,0x20,0x00,0x10,0x02,0x08,0x04,0x04,0x08,0xFE,0x1F,0x04,0x10,0x00,0x10,/*"��",2*/
};
/*��16���أ���16����*/
const ChineseCell_t  code LED_CF16x16[] = {
	
	"��",
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x58,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	
	"��",
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x18,0x24,0x24,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	
	"��",
	0x10,0x01,0x10,0x01,0x10,0x01,0x88,0x7F,0x88,0x40,0x4C,0x20,0x2C,0x04,0x0A,0x04,
    0x89,0x14,0x88,0x24,0x48,0x24,0x48,0x44,0x28,0x44,0x08,0x04,0x08,0x05,0x08,0x02,/*"��",0*/
	
	"��",
	0x08,0x00,0x08,0x3F,0x08,0x20,0x08,0x10,0x3F,0x08,0x24,0x04,0x24,0x04,0xA4,0x7F,
    0x24,0x04,0x12,0x04,0x14,0x04,0x08,0x04,0x14,0x04,0x22,0x04,0x21,0x05,0x00,0x02,/*"��",1*/

    "��",
    0x80,0x00,0x00,0x01,0xFC,0x7F,0x04,0x01,0xF4,0x1F,0x04,0x11,0xFC,0x7F,0x04,0x11,
    0xF4,0x1F,0x14,0x01,0x24,0x23,0x44,0x15,0x22,0x09,0x12,0x11,0x49,0x61,0x80,0x00,/*"��",0*/
    
    "��",
    0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x3E,0x7F,0x22,0x44,0x22,0x44,0x22,0x44,0x22,
    0x42,0x22,0x24,0x22,0x28,0x22,0x10,0x22,0x28,0x22,0x44,0x3E,0x42,0x22,0x01,0x00,/*"��",1*/

    "��",
    0x00,0x00,0xFC,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x40,0x00,
    0x20,0x00,0x20,0x00,0x10,0x02,0x08,0x04,0x04,0x08,0xFE,0x1F,0x04,0x10,0x00,0x10,/*"��",2*/

	"��",
	0x40,0x04,0x48,0x04,0x48,0x04,0x48,0x04,0x48,0x04,0xFF,0x7F,0x48,0x04,0x48,0x04,
    0x48,0x04,0x48,0x04,0xC8,0x07,0x08,0x00,0x08,0x00,0x08,0x00,0xF8,0x3F,0x00,0x00,/*"��",0*/
	
	"��",
	0x00,0x00,0xF8,0x0F,0x88,0x08,0x88,0x08,0xF8,0x0F,0x88,0x08,0x88,0x08,0xF8,0x0F,
    0x40,0x01,0x30,0x06,0x2C,0x1A,0x23,0x62,0x20,0x02,0x10,0x02,0x10,0x02,0x08,0x02,/*"��",1*/
	
	/*��������ĸ�ʽ�������λ�ü����µĺ�������*/
	//...
	/*δ�ҵ�ָ������ʱ��ʾ��Ĭ��ͼ�Σ�һ�������ڲ�һ���ʺţ�����ȷ����λ��������ĩβ*/
	"",		
	0xFF,0x01,0x01,0x01,0x31,0x09,0x09,0x09,0x09,0x89,0x71,0x01,0x01,0x01,0x01,0xFF,
	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x96,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,

};



// ���ֿ���ʾ�������ֵ�ʵ��
void displayChineseFromFont(uint8_t x_offset, uint8_t y_offset, char *chinese)
{
    uint8_t pIndex;
    
    // ���ֿ��в��Һ���
    for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
    {
        if (strcmp_custom(LED_CF16x16[pIndex].Index, chinese) == 0)
        {
            break;  // �ҵ�ƥ��ĺ���
        }
    }
    
    // ��ʾ�ҵ��ĺ���
    if (strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0)
    {
        // �ҵ���ģ���ݣ���ʾ����
        displayHanzi16x16_RowColumn_LowFirst(x_offset, y_offset, (uint8_t*)LED_CF16x16[pIndex].Data);
    }
    else
    {
        // δ�ҵ�����ʾһ�������ʾȱ��
        drawRect(x_offset, y_offset, 16, 16);
    }
}
// ������ʾ���ƺ���
void scrollDisplay(ScrollParams_t *params)
{
    int16_t str_len = 0;
    int16_t total_width = 0;
    int16_t i = 0, pos, frame;
    char *p = params->text;
    uchar repeat_count = 0;
    
    // �����ַ������Ⱥ��ܿ��
    while(*p) {
        if((*p & 0x80) != 0) {
            str_len++;
            total_width += 16 + params->font_spacing;
            p += 2;
        } else {
            str_len++;
            total_width += 8 + params->font_spacing;
            p++;
        }
    }
    
    // ��ȥ���һ���ַ�����Ҫ�ļ��
    if(total_width > 0) {
        total_width -= params->font_spacing;
    }
    
    // ˮƽ����
    if(params->direction <= 1) {
        int16_t start_pos, end_pos, step;
        int16_t char_pos;
        int16_t char_pos2;
        if(params->direction == 0) {  // ���ҵ���
            start_pos = MAX_X;
            end_pos = -total_width;
            step = -1;
        } else {  // ������
            start_pos = -total_width;
            end_pos = MAX_X;
            step = 1;
        }
        
        do {
            // ��ʼ��ͣ
            if(params->pause_start > 0) {
                clearScreen();
                p = params->text;
                char_pos = (params->direction == 0) ? 0 : (MAX_X - total_width);
                
                // ��ʾ��ֹ�ı�
                while(*p) {
                    if((*p & 0x80) != 0) {
                        char chinese[3];
                        chinese[0] = *p;
                        chinese[1] = *(p+1);
                        chinese[2] = '\0';
                        displayChineseFromFont(char_pos, 0, chinese);
                        char_pos += 16 + params->font_spacing;
                        p += 2;
                    } else {
                        // displayChar8x8(char_pos, 0, *p);
                        char_pos += 8 + params->font_spacing;
                        p++;
                    }
                }
                
                updateDisplay();
                delay_ms(params->pause_start);
            }
            
            // ������ʾ
            for(pos = start_pos; (step > 0) ? (pos <= end_pos) : (pos >= end_pos); pos += step) {
                clearScreen();
                
                p = params->text;
                char_pos2 = pos;
                
                while(*p) {
                    if((*p & 0x80) != 0) {
                        char chinese[3];
                        chinese[0] = *p;
                        chinese[1] = *(p+1);
                        chinese[2] = '\0';
                        
                        if(char_pos2 + 16 > 0 && char_pos2 < MAX_X) {
                            displayChineseFromFont(char_pos2, 0, chinese);
                        }
                        
                        char_pos2 += 16 + params->font_spacing;
                        p += 2;
                    } else {
                        if(char_pos2 + 8 > 0 && char_pos2 < MAX_X) {
                            // displayChar8x8(char_pos, 0, *p);
                        }
                        
                        char_pos2 += 8 + params->font_spacing;
                        p++;
                    }
                }
                
                updateDisplay();
                delay_ms(params->speed);
            }
            
            // ������ͣ
            if(params->pause_end > 0) {
                clearScreen();
                p = params->text;
                char_pos = (params->direction == 0) ? (MAX_X - total_width) : 0;
                
                // ��ʾ��ֹ�ı�
                while(*p) {
                    if((*p & 0x80) != 0) {
                        char chinese[3];
                        chinese[0] = *p;
                        chinese[1] = *(p+1);
                        chinese[2] = '\0';
                        displayChineseFromFont(char_pos, 0, chinese);
                        char_pos += 16 + params->font_spacing;
                        p += 2;
                    } else {
                        // displayChar8x8(char_pos, 0, *p);
                        char_pos += 8 + params->font_spacing;
                        p++;
                    }
                }
                
                updateDisplay();
                delay_ms(params->pause_end);
            }
            
            repeat_count++;
        } while(params->repeat == 0 || repeat_count < params->repeat);
    }
    // ��ֱ������������ʵ��...
}