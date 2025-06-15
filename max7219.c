#include "max7219.h"

/**********delay********** */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 127; j++);
}

//向MAX7219 写入地址和数据
void Max7219WR(uint8_t addr, uint8_t dat)
{
    uint8_t n = 8;
    CLK = 0;
    CS = 0; //CS=0有效,CS=1锁存
    /*发送数据 高位先发*/
    while(n--)
    {
        DIN = addr & 0x80;
        CLK = 1;
        addr <<= 1;
        CLK = 0;
    }
    n = 8;
    /*发送数据 高位先发*/
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
                    break; //译码方式：BCD码 不译码

                case 1:
                    Max7219WR(0x0A, lum);
                    break; //亮度 0-15 共16级

                case 2:
                    Max7219WR(0x0B, 0x07);
                    break; //扫描界限；8位显示

                case 3:
                    Max7219WR(0x0C, 0x01);
                    break; //掉电模式：0，普通模式：1

                case 4:
                    Max7219WR(0x0F, 0x00);
                    break; //显示测试：1；测试结束，正常显示：0
            }

        CS = 1;
        _nop_();
        CS = 0; //每写入四次给一个锁存脉冲
    }
}

// 全局显示缓冲区
uint8_t xdata display_buffer[MAX7219_TOTAL_MODULES][MODULE_DOTS]; // 动态配置大小

// 初始化显示缓冲区
void initDisplayBuffer()
{
    uint8_t i, j;
    for(i = 0; i < MAX7219_TOTAL_MODULES; i++) {
        for(j = 0; j < MODULE_DOTS; j++) {
            display_buffer[i][j] = 0x00;
        }
    }
}

// 计算模块坐标的宏
#define GET_MODULE_ID(x, y) ((y / MODULE_DOTS) * (MAX7219_HORIZONTAL_MODULES * 2) + \
                              (x / MODULE_DOTS) + ((y % MODULE_DOTS) >= MODULE_DOTS / 2 ? \
                              MAX7219_HORIZONTAL_MODULES : 0))

// 优化后的画点函数 - 适应任意数量级联
void drawPixel(uint8_t x, uint8_t y, uint8_t state)
{
    uint8_t module_id, local_x, local_y, bit_pos;
    uint8_t horizontal_block, vertical_block;
    uint8_t local_block_x, local_block_y;

    // 参数检查
    if(x >= MAX_X || y >= MAX_Y) return;
    
    // 计算模块ID (更灵活的方式)
    horizontal_block = x / CHAR_WIDTH;   // 水平方向第几个16x16模块
    vertical_block = y / CHAR_HEIGHT;    // 垂直方向第几个16x16模块
    
    // 计算在16x16块内的局部坐标
    local_block_x = x % CHAR_WIDTH;
    local_block_y = y % CHAR_HEIGHT;
    
    // 根据S型布局计算模块ID
    if(local_block_y < MODULE_DOTS) { // 上半部分
        if(local_block_x < MODULE_DOTS) {
            // 左上角模块
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 0;
            local_x = local_block_x;
            local_y = local_block_y;
        } else {
            // 右上角模块
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 1;
            local_x = local_block_x - MODULE_DOTS;
            local_y = local_block_y;
        }
    } else { // 下半部分
        if(local_block_x < MODULE_DOTS) {
            // 左下角模块
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 2;
            local_x = local_block_x;
            local_y = local_block_y - MODULE_DOTS;
        } else {
            // 右下角模块
            module_id = (vertical_block * MAX7219_HORIZONTAL_MODULES + horizontal_block) * 4 + 3;
            local_x = local_block_x - MODULE_DOTS;
            local_y = local_block_y - MODULE_DOTS;
        }
    }
    
    // 计算位位置（MAX7219的位7是最左边的点）
    bit_pos = 7 - local_x;
    
    // 设置或清除对应的位
    if(state) {
        display_buffer[module_id][local_y] |= (1 << bit_pos);  // 点亮
    } else {
        display_buffer[module_id][local_y] &= ~(1 << bit_pos); // 熄灭
    }
}

// 优化后的更新显示函数 - 支持任意模块数量
void updateDisplay()
{
    uint8_t i, module;
    
    for(i = 0; i < MODULE_DOTS; i++) {
        // 从最后一个模块开始，逆序发送数据
        for(module = MAX7219_TOTAL_MODULES; module > 0; module--) {
            Max7219WR(i + 1, display_buffer[module-1][i]);
        }
        
        CS = 1; // 锁存数据
        _nop_();
        CS = 0;
    }
}
// 清屏函数
void clearScreen()
{
    uint8_t i, j;
    for(i = 0; i < MAX7219_TOTAL_MODULES; i++) {
        for(j = 0; j < MODULE_DOTS; j++) {
            display_buffer[i][j] = 0x00;
        }
    }
}

// 清除指定区域
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

// 画矩形函数
void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    uint8_t i;
    // 画四条边
    for(i = 0; i < width; i++) {
        drawPixel(x + i, y, 1);         // 上边
        drawPixel(x + i, y + height - 1, 1); // 下边
    }
    for(i = 0; i < height; i++) {
        drawPixel(x, y + i, 1);         // 左边
        drawPixel(x + width - 1, y + i, 1);  // 右边
    }
}

// 使用现有的laji数组测试S型级联
uint8_t code laji[][8] =
{
    {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}, //A
    {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C}, //B
    {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x7C}, //C
    {0x7C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x7C}, //D
};

// 修正后的S型显示测试函数
void displayS_Type()
{
    uint8_t i;
    
    for(i = 0; i < 8; i++)  // 8行
    {
        // 按正确的发送顺序：[3]→[2]→[1]→[0]
        Max7219WR(i + 1, laji[3][i]);  // 模块4（右下）
        Max7219WR(i + 1, laji[2][i]);  // 模块3（左下）
        Max7219WR(i + 1, laji[1][i]);  // 模块2（右上） 
        Max7219WR(i + 1, laji[0][i]);  // 模块1（左上）
        
        CS = 1;
        _nop_();
        CS = 0;
    }
}
// 适用于行列式低位在前取模方式的16x16汉字显示函数
void displayHanzi16x16_RowColumn_LowFirst(uint8_t x_offset, uint8_t y_offset, uint8_t *hanzi_data)
{
    uint8_t row, col, byte_index, bit_index;
    uint8_t current_byte;
    
    // 遍历16x16的每一个点
    for(row = 0; row < 16; row++) {
        for(col = 0; col < 16; col++) {
            // 行列式取模：每行为一个取模单位，每行16位分为2个字节
            byte_index = row * 2 + (col / 8);  // 每行2字节，列除以8确定字节
            bit_index = col % 8;               // 位索引（低位在前，从bit0开始）
            
            // 获取当前字节
            current_byte = hanzi_data[byte_index];
            
            // 检查当前位是否为1，如果是则画点
            if(current_byte & (1 << bit_index)) {
                drawPixel(x_offset + col, y_offset + row, 1);
            } else {
                drawPixel(x_offset + col, y_offset + row, 0);
            }
        }
    }
}
// 显示8x8小字符函数
void displayChar8x8(uint8_t x_offset, uint8_t y_offset, uint8_t *char_data)
{
    uint8_t row, col, bit_index;
    uint8_t current_byte;
    
    // 遍历8x8的每一个点
    for(row = 0; row < 8; row++) {
        current_byte = char_data[row];
        for(col = 0; col < 8; col++) {
            bit_index = 7 - col;  // 高位在前
            
            // 检查当前位是否为1，如果是则画点
            if(current_byte & (1 << bit_index)) {
                drawPixel(x_offset + col, y_offset + row, 1);
            } else {
                drawPixel(x_offset + col, y_offset + row, 0);
            }
        }
    }
}

// 添加字符串比较函数（如果标准库不可用）
uint8_t strcmp_custom(char *str1, char *str2)
{
    while(*str1 && *str2) {
        if(*str1 != *str2) return 1;  // 不相等
        str1++;
        str2++;
    }
    return (*str1 != *str2);  // 0表示相等，1表示不相等
}
// 获取字库大小
// MAX7219显示汉字字符串函数
void MAX7219_ShowString(uint8_t X, uint8_t Y, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];  // GB2312最多2字节+结束符
    uint8_t CharLength = 0;
    uint8_t XOffset = 0;
    uint8_t YOffset = 0;
    uint8_t pIndex;
    
    while (String[i] != '\0')  // 遍历字符串
    {
        // GB2312字符集处理
        // 提取GB2312字符串中的一个字符，转存到SingleChar子字符串中
        // 判断GB2312字节的最高位标志位
        if ((String[i] & 0x80) == 0x00)  // 最高位为0，ASCII字符
        {
            CharLength = 1;                    // 字符为1字节
            SingleChar[0] = String[i++];       // 将第一个字节写入SingleChar第0个位置
            SingleChar[1] = '\0';              // 添加字符串结束标志位
        }
        else  // 最高位为1，汉字
        {
            CharLength = 2;                    // 字符为2字节
            SingleChar[0] = String[i++];       // 将第一个字节写入SingleChar第0个位置
            if (String[i] == '\0') {break;}    // 意外情况，跳出循环
            SingleChar[1] = String[i++];       // 将第二个字节写入SingleChar第1个位置
            SingleChar[2] = '\0';              // 添加字符串结束标志位
        }
        
        // 显示上述代码提取到的SingleChar
        if (CharLength == 1)  // 如果是单字节字符（ASCII）
        {
            // 显示ASCII字符（8x8）
            if (SingleChar[0] >= '0' && SingleChar[0] <= '9') {
                // 这里可以添加数字字符显示
                // displayChar8x8(X + XOffset, Y + YOffset, num_data[SingleChar[0] - '0']);
            }
            XOffset += 8;  // ASCII字符宽度为8
        }
        else  // 否则，即多字节字符（汉字）
        {
            // 遍历整个字模库，从字模库中寻找此字符的数据
            // 如果找到最后一个字符（定义为空字符串），则表示字符未在字模库定义
            for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
            {
                // 找到匹配的字符
                if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0)
                {
                    break;  // 跳出循环，此时pIndex的值为指定字符的索引
                }
            }
            // 将字模库LED_CF16x16的指定数据以16x16的图像格式显示
            displayHanzi16x16_RowColumn_LowFirst(X + XOffset, Y + YOffset, 
                                               (uint8_t*)LED_CF16x16[pIndex].Data);
            XOffset += 16;  // 汉字宽度为16
        }
        // 检查是否需要换行（16x16显示区域只能显示一个汉字）
        if (XOffset >= 16) {
            XOffset = 0;
            YOffset += 16;
            // 如果超出显示范围，停止显示
            if (YOffset >= 16) {
                break;
            }
        }
    }
}
// 简化版：显示单行汉字字符串（只显示第一个字符）
void MAX7219_ShowSingleChinese(uint8_t X, uint8_t Y, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];
    uint8_t CharLength = 0;
    uint8_t pIndex;
    
    if (String[i] != '\0')  // 如果字符串不为空
    {
        // 提取第一个字符
        if ((String[i] & 0x80) == 0x00)  // ASCII字符
        {
            CharLength = 1;
            SingleChar[0] = String[i];
            SingleChar[1] = '\0';
            
            // 显示ASCII字符的处理
            // 这里可以添加ASCII字符显示代码
        }
        else  // 汉字
        {
            CharLength = 2;
            SingleChar[0] = String[i];
            SingleChar[1] = String[i + 1];
            SingleChar[2] = '\0';
            
            // 在字库中查找汉字
            for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
            {
                if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0)
                {
                    break;
                }
            }
            
            // 显示找到的汉字
            displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[pIndex].Data);
        }
    }
}


// // 检查坐标是否在显示范围内的宏
// #define IS_COORD_VALID(x, y) ((x) < MAX_X && (y) < MAX_Y)

// 显示多个汉字的函数
void MAX7219_ShowChineseString(uint8_t x_offset, uint8_t y_offset, char *String)
{
    uint8_t i = 0;
    char SingleChar[3];  // GB2312最多2字节+结束符
    uint8_t CharLength = 0;
    uint8_t XOffset = x_offset;
    uint8_t YOffset = y_offset;
    uint8_t maxChars = MAX_X / CHAR_WIDTH;  // 可显示的最大汉字数量
    uint8_t charCount = 0;
    uint8_t pIndex;
    
    while (String[i] != '\0' && charCount < maxChars && IS_COORD_VALID(XOffset, YOffset))
    {
        // GB2312字符集处理
        if ((String[i] & 0x80) == 0x00)  // ASCII字符
        {
            CharLength = 1;
            SingleChar[0] = String[i++];
            SingleChar[1] = '\0';
            
            // ASCII字符显示处理
            if (XOffset + ASCII_WIDTH <= MAX_X) {
                // 可以添加ASCII字符显示代码
                XOffset += ASCII_WIDTH;
            } else {
                // 超出显示范围
                break;
            }
        }
        else  // 汉字
        {
            CharLength = 2;
            SingleChar[0] = String[i++];
            if (String[i] == '\0') break;
            SingleChar[1] = String[i++];
            SingleChar[2] = '\0';
            
            // 检查显示范围
            if (XOffset + CHAR_WIDTH <= MAX_X) {
                // 在字库中查找汉字
                for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
                {
                    if (strcmp_custom(LED_CF16x16[pIndex].Index, SingleChar) == 0) {
                        break;
                    }
                }
                
                // 显示找到的汉字
                displayHanzi16x16_RowColumn_LowFirst(XOffset, YOffset, (uint8_t*)LED_CF16x16[pIndex].Data);
                XOffset += CHAR_WIDTH;
                charCount++;
            } else {
                // 超出显示范围
                break;
            }
        }
    }
}

// 通过索引直接显示汉字（更高效）
void MAX7219_ShowChineseByIndex(uint8_t X, uint8_t Y, uint8_t index)
{
    // 检查索引范围
    if (strcmp_custom(LED_CF16x16[index].Index, "") != 0)
    {
        displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[index].Data);
    }
    else
    {
        // 显示默认图形（方框+问号）
        displayHanzi16x16_RowColumn_LowFirst(X, Y, (uint8_t*)LED_CF16x16[index].Data);
    }
}
uint8_t code hanzi_ni[32] = {
    0x10,0x01,0x10,0x01,0x10,0x01,0x88,0x7F,0x88,0x40,0x4C,0x20,0x2C,0x04,0x0A,0x04,
    0x89,0x14,0x88,0x24,0x48,0x24,0x48,0x44,0x28,0x44,0x08,0x04,0x08,0x05,0x08,0x02,/*"你",0*/
};
uint8_t code hanzi_hao[32] = {
    0x08,0x00,0x08,0x3F,0x08,0x20,0x08,0x10,0x3F,0x08,0x24,0x04,0x24,0x04,0xA4,0x7F,
    0x24,0x04,0x12,0x04,0x14,0x04,0x08,0x04,0x14,0x04,0x22,0x04,0x21,0x05,0x00,0x02,/*"好",1*/
};
uint8_t code hanzi_kang[32] = {
    0x80,0x00,0x00,0x01,0xFC,0x7F,0x04,0x01,0xF4,0x1F,0x04,0x11,0xFC,0x7F,0x04,0x11,
    0xF4,0x1F,0x14,0x01,0x24,0x23,0x44,0x15,0x22,0x09,0x12,0x11,0x49,0x61,0x80,0x00,/*"康",0*/
};
uint8_t code hanzi_ru[32] = {
    0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x3E,0x7F,0x22,0x44,0x22,0x44,0x22,0x44,0x22,
    0x42,0x22,0x24,0x22,0x28,0x22,0x10,0x22,0x28,0x22,0x44,0x3E,0x42,0x22,0x01,0x00,/*"如",1*/
};
uint8_t code hanzi_yun[32] = {
    0x00,0x00,0xFC,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x40,0x00,
    0x20,0x00,0x20,0x00,0x10,0x02,0x08,0x04,0x04,0x08,0xFE,0x1F,0x04,0x10,0x00,0x10,/*"云",2*/
};
/*宽16像素，高16像素*/
const ChineseCell_t  code LED_CF16x16[] = {
	
	"，",
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x58,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	
	"。",
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x18,0x24,0x24,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	
	"你",
	0x10,0x01,0x10,0x01,0x10,0x01,0x88,0x7F,0x88,0x40,0x4C,0x20,0x2C,0x04,0x0A,0x04,
    0x89,0x14,0x88,0x24,0x48,0x24,0x48,0x44,0x28,0x44,0x08,0x04,0x08,0x05,0x08,0x02,/*"你",0*/
	
	"好",
	0x08,0x00,0x08,0x3F,0x08,0x20,0x08,0x10,0x3F,0x08,0x24,0x04,0x24,0x04,0xA4,0x7F,
    0x24,0x04,0x12,0x04,0x14,0x04,0x08,0x04,0x14,0x04,0x22,0x04,0x21,0x05,0x00,0x02,/*"好",1*/

    "康",
    0x80,0x00,0x00,0x01,0xFC,0x7F,0x04,0x01,0xF4,0x1F,0x04,0x11,0xFC,0x7F,0x04,0x11,
    0xF4,0x1F,0x14,0x01,0x24,0x23,0x44,0x15,0x22,0x09,0x12,0x11,0x49,0x61,0x80,0x00,/*"康",0*/
    
    "如",
    0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x3E,0x7F,0x22,0x44,0x22,0x44,0x22,0x44,0x22,
    0x42,0x22,0x24,0x22,0x28,0x22,0x10,0x22,0x28,0x22,0x44,0x3E,0x42,0x22,0x01,0x00,/*"如",1*/

    "云",
    0x00,0x00,0xFC,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x7F,0x40,0x00,
    0x20,0x00,0x20,0x00,0x10,0x02,0x08,0x04,0x04,0x08,0xFE,0x1F,0x04,0x10,0x00,0x10,/*"云",2*/

	"世",
	0x40,0x04,0x48,0x04,0x48,0x04,0x48,0x04,0x48,0x04,0xFF,0x7F,0x48,0x04,0x48,0x04,
    0x48,0x04,0x48,0x04,0xC8,0x07,0x08,0x00,0x08,0x00,0x08,0x00,0xF8,0x3F,0x00,0x00,/*"世",0*/
	
	"界",
	0x00,0x00,0xF8,0x0F,0x88,0x08,0x88,0x08,0xF8,0x0F,0x88,0x08,0x88,0x08,0xF8,0x0F,
    0x40,0x01,0x30,0x06,0x2C,0x1A,0x23,0x62,0x20,0x02,0x10,0x02,0x10,0x02,0x08,0x02,/*"界",1*/
	
	/*按照上面的格式，在这个位置加入新的汉字数据*/
	//...
	/*未找到指定汉字时显示的默认图形（一个方框，内部一个问号），请确保其位于数组最末尾*/
	"",		
	0xFF,0x01,0x01,0x01,0x31,0x09,0x09,0x09,0x09,0x89,0x71,0x01,0x01,0x01,0x01,0xFF,
	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x96,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,

};



// 从字库显示单个汉字的实现
void displayChineseFromFont(uint8_t x_offset, uint8_t y_offset, char *chinese)
{
    uint8_t pIndex;
    
    // 在字库中查找汉字
    for (pIndex = 0; strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0; pIndex++)
    {
        if (strcmp_custom(LED_CF16x16[pIndex].Index, chinese) == 0)
        {
            break;  // 找到匹配的汉字
        }
    }
    
    // 显示找到的汉字
    if (strcmp_custom(LED_CF16x16[pIndex].Index, "") != 0)
    {
        // 找到字模数据，显示汉字
        displayHanzi16x16_RowColumn_LowFirst(x_offset, y_offset, (uint8_t*)LED_CF16x16[pIndex].Data);
    }
    else
    {
        // 未找到，显示一个方框表示缺字
        drawRect(x_offset, y_offset, 16, 16);
    }
}
// 滚动显示控制函数
void scrollDisplay(ScrollParams_t *params)
{
    int16_t str_len = 0;
    int16_t total_width = 0;
    int16_t i = 0, pos, frame;
    char *p = params->text;
    uchar repeat_count = 0;
    
    // 计算字符串长度和总宽度
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
    
    // 减去最后一个字符后不需要的间距
    if(total_width > 0) {
        total_width -= params->font_spacing;
    }
    
    // 水平滚动
    if(params->direction <= 1) {
        int16_t start_pos, end_pos, step;
        int16_t char_pos;
        int16_t char_pos2;
        if(params->direction == 0) {  // 从右到左
            start_pos = MAX_X;
            end_pos = -total_width;
            step = -1;
        } else {  // 从左到右
            start_pos = -total_width;
            end_pos = MAX_X;
            step = 1;
        }
        
        do {
            // 开始暂停
            if(params->pause_start > 0) {
                clearScreen();
                p = params->text;
                char_pos = (params->direction == 0) ? 0 : (MAX_X - total_width);
                
                // 显示静止文本
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
            
            // 滚动显示
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
            
            // 结束暂停
            if(params->pause_end > 0) {
                clearScreen();
                p = params->text;
                char_pos = (params->direction == 0) ? (MAX_X - total_width) : 0;
                
                // 显示静止文本
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
    // 垂直滚动可以类似实现...
}