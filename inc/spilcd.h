#ifndef SPILCD_H_
#define SPILCD_H_

#include "main.h"
#include <cmath>
#include "string.h"
#include "stdio.h"

class Spi1_interface
{
public:
    inline void reset_active(){GPIOE->BSRR|=GPIO_BSRR_BR2;}
    inline void reset_idle(){GPIOE->BSRR|=GPIO_BSRR_BS2;}
    inline void cs_active(){GPIOE->BSRR|=GPIO_BSRR_BR4;}
    inline void cs_idle(){GPIOE->BSRR|=GPIO_BSRR_BS4;}
    inline void dc_command(){GPIOE->BSRR|=GPIO_BSRR_BR3;}
    inline void dc_data(){GPIOE->BSRR|=GPIO_BSRR_BS3;}
    
    inline void ST7789_SendCommand(uint8_t cmd)
    {
        while((SPI1->SR&SPI_SR_BSY)){}
        dc_command();
        spi_sendByte(cmd);  
        delay(40);
    }
    inline void ST7789_SendData(uint8_t data)
    {        
        dc_data();
        spi_sendByte(data);
    }    
    inline void spi_sendByte(uint8_t data)
    {
        while(!(SPI1->SR&SPI_SR_TXE)){}        
        SPI1->DR=data;
        //*(uint8_t*)SPI1->DR=data;
        while((SPI1->SR&SPI_SR_BSY)){}
    }
    inline void spi_sendWord(uint16_t data)
    {
        //ждем пока освободится буффер передатчика
        while((SPI1->SR&SPI_SR_BSY)){}
        dc_data();
        ST7789_SendData(data>>8);
        while((SPI1->SR&SPI_SR_BSY)){}
        ST7789_SendData(data&0x00FF);
        //while((SPI1->SR&SPI_SR_BSY)){}
    }
    inline void tft_setColumn(int StartCol, int EndCol)
    {
        ST7789_SendCommand(0x2A);                                                     // Column Command address
        spi_sendWord(StartCol);
        spi_sendWord(EndCol);
    }
    inline void tft_setRow(int StartRow, int EndRow)
    {
        ST7789_SendCommand(0x2B);                                                  // Column Command address
        spi_sendWord(StartRow);
        spi_sendWord(EndRow);
    }
    inline void tft_setXY(int poX, int poY)
    {
        tft_setColumn(poX, poX);
        tft_setRow(poY, poY);
        ST7789_SendCommand(0x2C);
    }
    inline void tft_setPixel(int poX, int poY, int color)
    {
        tft_setXY(poX, poY);
        spi_sendWord(color);
        spi_sendWord(color);
    }
    inline void swap(uint16_t x1, uint16_t x2)
    {
        if(x2>x1){uint16_t z=x2;x2=x1;x1=z;}
    }
    inline void checkXYswap(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
    {
        swap(x1,x2); swap(y1,y2);
    }

    uint16_t spi_read()
    {
        SPI1->DR = 0; //запускаем обмен
        while(!(SPI1->SR&SPI_SR_RXNE)) //ждем пока не появится новое значение в буффере приемника
        return SPI1->DR; //возвращаем значение из буффера приемника
    }
    inline void fillScreen(uint16_t color)
    {
        tft_setRow(0, 240);
        tft_setColumn(0,240);        
        ST7789_SendCommand(0x2C); //memory write
        //dc_data();
        //cs_active();
        for(volatile uint32_t i=0; i<57600; i++)
        {           
            //while((SPI1->SR&SPI_SR_BSY)){}
            spi_sendWord(color);           
        }
    }        
    inline void delay(volatile uint32_t x)
    {
        for(volatile uint32_t i=0;i<x;i++){;}
    }
};

class SpiLcd:public Spi1_interface
{
public:
    SpiLcd(){spi1_ini(); tft_ini(240,240);}

private:
    void spi1_ini()
    {
        //------------- настройка RESET (display reset) PE2-------------------
        RCC->APB2ENR|=RCC_APB2ENR_IOPEEN;
        GPIOE->CRL|=GPIO_CRL_MODE2; // output mode 1:1 max speed
        GPIOE->CRL&=~GPIO_CRL_CNF2_0;
        GPIOE->CRL&=~GPIO_CRL_CNF2_1; // 0:0 - output push pull
	    GPIOE->BSRR=0x0000ffff; // low level

        //------------- настройка DC (data command) PE3-------------------
        GPIOE->CRL|=GPIO_CRL_MODE3; // output mode 1:1 max speed
        GPIOE->CRL&=~GPIO_CRL_CNF3_0;
        GPIOE->CRL&=~GPIO_CRL_CNF3_1; // 0:0 - output push pull
	    
        //------------- настройка CS (chip select NSS) PA4-------------------
        GPIOE->CRL|=GPIO_CRL_MODE4; // output mode 1:1 max speed
        GPIOE->CRL&=~GPIO_CRL_CNF4_0;
        GPIOE->CRL&=~GPIO_CRL_CNF4_1; // 0:0 - output push pull

        //------------- настройка ножек SPI:  PB3-_SCK, PB4-MISO(not realized), PB5-MOSI :::AF5
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        AFIO->MAPR|=AFIO_MAPR_SPI1_REMAP;
        GPIOB->CRL|=GPIO_CRL_MODE3; // output mode 1:1 max speed
        GPIOB->CRL&=~GPIO_CRL_CNF3_0;
        GPIOB->CRL|=GPIO_CRL_CNF3_1; // 1:0 - alternate push pull
        GPIOB->CRL|=GPIO_CRL_MODE5; // output mode 1:1 max speed
        GPIOB->CRL&=~GPIO_CRL_CNF5_0;
        GPIOB->CRL|=GPIO_CRL_CNF5_1; // 1:0 - alternate push pull
        
        //RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        //GPIOA->CRL|=GPIO_CRL_MODE5; // output mode 1:1 max speed
        //GPIOA->CRL&=~GPIO_CRL_CNF5_0;
        //GPIOA->CRL|=GPIO_CRL_CNF5_1; // 1:0 - alternate push pull
        //GPIOA->CRL|=GPIO_CRL_MODE7; // output mode 1:1 max speed
        //GPIOA->CRL&=~GPIO_CRL_CNF7_0;
        //GPIOA->CRL|=GPIO_CRL_CNF7_1; // 1:0 - alternate push pull
                
        //------------- тактируем SPI-1  ---------------------------
        RCC->APB2ENR|=RCC_APB2ENR_SPI1EN; //clock on fast SPI-1
        //------------- предделитель SPI-1 -----------------------------
        SPI1->CR1&=~SPI_CR1_BR; // (000=>psc=1) 1:1:1  => 84000000/256 = 328.125 kHz - SPI-1 clk
        //------------- настройка SPI - 1 -----------------------------------------
        SPI1->CR1&=~SPI_CR1_BIDIMODE; // двухпроводной режим работы
        SPI1->CR1|=SPI_CR1_BIDIOE; //1: Output enabled (transmit-only mode)
        SPI1->CR1&=~SPI_CR1_CRCEN; // 0: CRC calculation disabled
        SPI1->CR1&=~SPI_CR1_CRCNEXT; //0: Data phase (no CRC phase)
        SPI1->CR1&=~SPI_CR1_DFF; //0: 8-bit data frame format is selected for transmission/reception
        SPI1->CR1&=~SPI_CR1_RXONLY; //0: Full duplex (Transmit and receive)
        SPI1->CR1|=SPI_CR1_SSM; // программное управление ведомым включено 
        SPI1->CR1|=SPI_CR1_SSI; // нужно чтобы эти два бита были в 1 для правильной инициализации мастера
        SPI1->CR1&=~SPI_CR1_LSBFIRST; //1: LSB first //0: MSB transmitted first
        SPI1->CR1|=SPI_CR1_MSTR; //1: Master configuration
        SPI1->CR1|=SPI_CR1_CPOL; //1: CK to 1 when idle (смотреть в datasheet slave) 
        SPI1->CR1|=SPI_CR1_CPHA; //1: The second clock transition is the first data capture edge (тоже)
        
        //NVIC_EnableIRQ(SPI1_IRQn);
        //----------- включаем SPI-1 --------------------------------------------
        SPI1->CR1|=SPI_CR1_SPE;
    }
     void tft_ini(uint16_t w_size, uint16_t h_size)
    {  
        //cs_idle();
        //dc_data();
        //cs_active();
        reset_active();   
        delay(36000);     
        reset_idle();
        ST7789_SendCommand(0x01);
        delay(36000);

        ST7789_SendCommand(0x11);   //SLPOUT sleep out
        delay(36000); // 100 ms
        //ST7789_SendCommand(0x28); //display off
        //---------------------------------------------------
        ST7789_SendCommand(0x3A);   //COLMOD RGB444(12bit) 0x03, RGB565(16bit) 0x05,
        ST7789_SendData(0x05);  //RGB666(18bit) 0x06
        //---------------------------------------------------
        //ST7789_SendCommand(0xEF); //
        //ST7789_SendData(0x03);
        //ST7789_SendData(0x80);
        //ST7789_SendData(0x02);
        //Power Control A
        //ST7789_SendCommand(0xCB);  
        //ST7789_SendData(0x39);
        //ST7789_SendData(0x2C);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x34);
        //ST7789_SendData(0x02); 
        //Power Control B
        //ST7789_SendCommand(0xCF);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0xC1);
        //ST7789_SendData(0x30);
        //Power on Sequence control
        //ST7789_SendCommand(0xED);
        //ST7789_SendData(0x64);
        //ST7789_SendData(0x03);
        //ST7789_SendData(0x12);
        //ST7789_SendData(0x81);     
        //ST7789_SendCommand(0x20); //Display inversion off
        //ST7789_SendCommand(0x21); //Display inversion on   
        //Driver timing control A
        //ST7789_SendCommand(0xE8); //power control 2
        //ST7789_SendData(0x85);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x78);        
        //Driver timing control B
        //ST7789_SendCommand(0xEA);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x00);                
        //Pump ratio control
        //ST7789_SendCommand(0xF7);
        //ST7789_SendData(0x20);
        //Power Control,VRH[5:0]
        //ST7789_SendCommand(0xC0);//LCM control
        //ST7789_SendData(0x23);  //VRH[5:0
        //Power Control,SAP[2:0];BT[3:0]
        //ST7789_SendCommand(0xC1);
        //ST7789_SendData(0x10);   //SAP[2:0];BT[3:0
        //VCOM Control 1
        //ST7789_SendCommand(0xC5);//VCM control
        //ST7789_SendData(0x3E);  //Contrast
        //ST7789_SendData(0x28);
        //VCOM Control 2
        //ST7789_SendCommand(0xC7);//VCM control2
        //ST7789_SendData(0x86);
        //ST7789_SendCommand(0x37); //Vertical Scroll Start Address of RAM ...
        //ST7789_SendData(0x00);
        //Memory Acsess Control - rotation
        ST7789_SendCommand(0x36); //Memory Data Access Control
        ST7789_SendData(0x00);  // 1-полубайт ориентация (через 2) - 2-ой цветовая схема (0 или 8)
        //Pixel Format Set
        //ST7789_SendCommand(0x3A);
        //ST7789_SendData(0x55);//16bit
        //Frame Rratio Control, Standard RGB Color
        //ST7789_SendCommand(0xB1);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x18);
        //ST7789_SendCommand(0xB6);      // Display Function Control
        //ST7789_SendData(0x08); 
        //ST7789_SendData(0x82); 
        //ST7789_SendData(0x27);
        //Enable 3G (пока не знаю что это за режим)
        //ST7789_SendCommand(0xF2);
        //ST7789_SendData(0x00);//не включаем        
        //Gamma set
        //ST7789_SendCommand(0x26); //Gamma set
        //ST7789_SendData(0x01);//Gamma Curve (G2.2) (Кривая цветовой гаммы)
        //Positive Gamma  Correction
        //ST7789_SendCommand(0xE0);
        //ST7789_SendData(0x0F);
        //ST7789_SendData(0x31);
        //ST7789_SendData(0x2B);
        //ST7789_SendData(0x0C);
        //ST7789_SendData(0x0E);
        //ST7789_SendData(0x08);
        //ST7789_SendData(0x4E);
        //ST7789_SendData(0xF1);
        //ST7789_SendData(0x37);
        //ST7789_SendData(0x07);
        //ST7789_SendData(0x10);
        //ST7789_SendData(0x03);
        //ST7789_SendData(0x0E);
        //ST7789_SendData(0x09);
        //ST7789_SendData(0x00);
        //Negative Gamma  Correction
        //ST7789_SendCommand(0xE1);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x0E);
        //ST7789_SendData(0x14);
        //ST7789_SendData(0x03);
        //ST7789_SendData(0x11);
        //ST7789_SendData(0x07);
        //ST7789_SendData(0x31);
        //ST7789_SendData(0xC1);
        //ST7789_SendData(0x48);
        //ST7789_SendData(0x08);
        //ST7789_SendData(0x0F);
        //ST7789_SendData(0x0C);
        //ST7789_SendData(0x31);
        //ST7789_SendData(0x36);
        //ST7789_SendData(0x0F);

        //ST7789_SendCommand(0x2B); //page set row
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0xEF);
        //
        //ST7789_SendCommand(0x2A); // column set
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0x00);
        //ST7789_SendData(0xEF);
        
        //ST7789_SendCommand(0x34); //tearing effect off
        //ST7789_SendCommand(0xB7); //entry mode set
        //ST7789_SendData(0x07);
        
        ST7789_SendCommand(0x21);//inv on
        ST7789_SendCommand(0x13);     //Normal display Mode on   
        //delay(3600000); //100 ms
                
        ST7789_SendCommand(0x29);//display on
        //ST7789_SendData(TFT9341_ROTATION);        
        
        delay(3600000); //100 ms       
        
    }
};
// ///////////////////////////////////////////////////////////////////////////
//*********** Класс обработки цифр и букв ********************
class Font_interface:public Spi1_interface
{
public:
    Font_interface(){}
    uint32_t char_to_int(char* str,uint8_t size)
   {
       uint32_t x;
       for(uint8_t i=0;i<size;i++)
       {
           uint8_t dec;
           if (str[i]==48){dec=0;} if (str[i]==49){dec=1;}
           if (str[i]==50){dec=2;} if (str[i]==51){dec=3;}
           if (str[i]==52){dec=4;} if (str[i]==53){dec=5;}
           if (str[i]==54){dec=6;} if (str[i]==55){dec=7;}
           if (str[i]==56){dec=8;} if (str[i]==57){dec=9;}
           x+=dec*pow(10,size-i);           
       }
       return x;
   }
    char arr[20];
    char arrFloat[20]{0};
    volatile uint8_t arrSize=0;

    void intToChar(uint32_t x) //int to char*
    {
        uint32_t y=x;        
        uint8_t count=0;
        while (y>0)
        {  y=y/10;  count++; }//считаем количество цифр
        y=x;
        
        arrSize=count;
        if(x==0) {arrSize=1;arr[arrSize-1]='0';arr[arrSize]='\0';return;} 
        for(uint8_t i=0;i<arrSize;i++)
        {            
            int x=y%10;
            if(x==0) {arr[arrSize-1-i]='0';} if(x==1) {arr[arrSize-1-i]='1';}
            if(x==2) {arr[arrSize-1-i]='2';} if(x==3) {arr[arrSize-1-i]='3';}
            if(x==4) {arr[arrSize-1-i]='4';} if(x==5) {arr[arrSize-1-i]='5';}
            if(x==6) {arr[arrSize-1-i]='6';} if(x==7) {arr[arrSize-1-i]='7';}
            if(x==8) {arr[arrSize-1-i]='8';} if(x==9) {arr[arrSize-1-i]='9';}
            y=y/10;        
        }
        if(arrSize+1<10)
        {
            //strcat(arr+arrSize,'\0'); 
            arr[arrSize]='\0';
        }
    }
    void floatTochar(float x)
    {        
        sprintf(arrFloat, "%.3f", x);           
    }
};
class Font_8x8: public Font_interface
{
public:
    Font_8x8()
    {}
    void symbol(uint16_t x, uint16_t y, uint16_t col,const uint8_t* data)
    {
        tft_setColumn(x,x+8);
        tft_setRow(y,y+8);
        ST7789_SendCommand(0x2C);
        for(uint16_t i=0; i <y+8-y;i++)   //заполняем 8-битную матрицу
        {
            //spi_sendWord(col);
            for(uint16_t j=0; j <x+8-x+1;j++)
            {
                if((j==0)&&(data[i]&0x80)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==1)&&(data[i]&0x40)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==2)&&(data[i]&0x20)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==3)&&(data[i]&0x10)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==4)&&(data[i]&0x08)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==5)&&(data[i]&0x04)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==6)&&(data[i]&0x02)) {spi_sendWord(col);}//else spi_sendWord(0xF800);
                else if((j==7)&&(data[i]&0x01)) {spi_sendWord(col);}
                else spi_sendWord(0x0000);
            }
        }
    }    
    void print(uint16_t x, uint16_t y,uint16_t col,char* str)
    {
        for(uint8_t i=0;i<strlen(str);i++)
        {
            if (str[i]==48){symbol(x, y, col, data0);}
            if (str[i]==49){symbol(x, y, col, data1);}
            if (str[i]==50){symbol(x, y, col, data2);}
            if (str[i]==51){symbol(x, y, col, data3);}
            if (str[i]==52){symbol(x, y, col, data4);}
            if (str[i]==53){symbol(x, y, col, data5);}
            if (str[i]==54){symbol(x, y, col, data6);}
            if (str[i]==55){symbol(x, y, col, data7);}
            if (str[i]==56){symbol(x, y, col, data8);}
            if (str[i]==57){symbol(x, y, col, data9);}
            x+=9;            
        }
    }    
    const uint8_t data0[8]={0x7C,0xFE,0xC6,0xC6,0xC6,0xC6,0xFE,0x7C}; //0         
    const uint8_t data1[8]={0x30,0x70,0xF0,0x30,0x30,0x30,0x30,0x30}; //1         
    const uint8_t data2[8]={0x78,0xFC,0xCC,0x1C,0x38,0x70,0xFC,0xFC}; //2         
    const uint8_t data3[8]={0x78,0xFC,0x0C,0x78,0xFC,0x0C,0xFC,0x78}; //3         
    const uint8_t data4[8]={0x1C,0x3C,0x6C,0xCC,0xFC,0x0C,0x0C,0x0C}; //4         
    const uint8_t data5[8]={0xFC,0xC0,0xC0,0xF8,0x0C,0xCC,0xFC,0x78}; //5         
    const uint8_t data6[8]={0x7E,0xC6,0xC0,0xFC,0xC6,0xC6,0xFE,0x7C}; //6         
    const uint8_t data7[8]={0xFE,0xFE,0x06,0x0E,0x1C,0x38,0x70,0x60}; //7         
    const uint8_t data8[8]={0x7C,0xFE,0xC6,0x7C,0x7C,0xC6,0xFE,0x7C}; //8         
    const uint8_t data9[8]={0x7C,0xFE,0xC6,0x7E,0x06,0x86,0xFE,0x7C}; //9 
};

class Font_16x16: public Font_interface
{
public:
    Font_16x16(){}
    const char strArr[10]="opa";
    void symbol(uint16_t x, uint16_t y, uint16_t col,const uint16_t* data)
    {        
        tft_setColumn(x,x+16);
        tft_setRow(y,y+16);
        ST7789_SendCommand(0x2C);
        for(uint16_t i=0; i <y+16-y;i++)   //заполняем 8-битную матрицу
        {
            //spi_sendWord(col);
            for(uint16_t j=0; j <x+16-x+1;j++)
            {
                if((j==0)&&(data[i]&0x8000)) {spi_sendWord(col);}
                else if((j==1)&&(data[i]&0x4000)) {spi_sendWord(col);}
                else if((j==2)&&(data[i]&0x2000)) {spi_sendWord(col);}
                else if((j==3)&&(data[i]&0x1000)) {spi_sendWord(col);}
                else if((j==4)&&(data[i]&0x0800)) {spi_sendWord(col);}
                else if((j==5)&&(data[i]&0x0400)) {spi_sendWord(col);}
                else if((j==6)&&(data[i]&0x0200)) {spi_sendWord(col);}
                else if((j==7)&&(data[i]&0x0100)) {spi_sendWord(col);}
                else if((j==8)&&(data[i]&0x0080)) {spi_sendWord(col);}
                else if((j==9)&&(data[i]&0x0040)) {spi_sendWord(col);}
                else if((j==10)&&(data[i]&0x0020)) {spi_sendWord(col);}
                else if((j==11)&&(data[i]&0x0010)) {spi_sendWord(col);}
                else if((j==12)&&(data[i]&0x0008)) {spi_sendWord(col);}
                else if((j==13)&&(data[i]&0x0004)) {spi_sendWord(col);}
                else if((j==14)&&(data[i]&0x0002)) {spi_sendWord(col);}
                else if((j==15)&&(data[i]&0x0001)) {spi_sendWord(col);}
                else spi_sendWord(0x0000);
            }
        }       
    }    
    void clearString(uint16_t x, uint16_t y,uint8_t size)
    {
        tft_setColumn(x,x+16*size);
        tft_setRow(y,y+16);
        ST7789_SendCommand(0x2C);
        for(uint16_t i=0; i <(y+16+1)*(x+16+1);i++)   //заполняем 8-битную матрицу
        {
            spi_sendWord(0x0000);
        }
    }
    void print(uint16_t x, uint16_t y,uint16_t col,const char* str,uint8_t numDigits)
    {        
        for(uint8_t i=0;i<strlen(str);i++)
        {
            if (str[i]==48){symbol(x, y, col, data0);}
            if (str[i]==49){symbol(x, y, col, data1);}
            if (str[i]==50){symbol(x, y, col, data2);}
            if (str[i]==51){symbol(x, y, col, data3);}
            if (str[i]==52){symbol(x, y, col, data4);}
            if (str[i]==53){symbol(x, y, col, data5);}
            if (str[i]==54){symbol(x, y, col, data6);}
            if (str[i]==55){symbol(x, y, col, data7);}
            if (str[i]==56){symbol(x, y, col, data8);}
            if (str[i]==57){symbol(x, y, col, data9);}
            if (str[i]==58){symbol(x, y, col, dataColon);}
            if (str[i]==46){symbol(x, y, col, dataPoint);x+=17;}
            else{x+=17;}                        
        }
        for(uint8_t i=strlen(str);i<strlen(str)+numDigits-1;i++)
        {
            {symbol(x, y, col, dataSpace);}
            x+=17;            
        }
    }    
    const uint16_t data0[16]={0x0FE0,0x1FF0,0x3FF8,0x7C7C,0x783C,0xF01E,0xF01E,0xF01E,          
                              0xF01E,0xF01E,0x783C,0x7C7C,0x3FF8,0x1FF0,0x0FE0,0x0000}; //0         
    const uint16_t data1[16]={0x07F8,0x0FF8,0x1FF8,0x3FF8,0x7FF8,0xFFF8,0xFDF8,0xF9F8,          
                              0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x01F8,0x0000}; //1         
    const uint16_t data2[16]={0x3FF0,0x7FF8,0xFFFC,0xF0FC,0xF0FC,0xE1F8,0x03F0,0x07E0,         
                              0x0FC0,0x1F80,0x3F00,0x7E00,0xFFFC,0xFFFC,0xFFFC,0x0000}; //2         
    const uint16_t data3[16]={0x3FF0,0x7FF8,0xFFFC,0xE07C,0x003C,0x007C,0x3FF8,0x7FF0,          
                              0x3FF8,0x007C,0x003C,0xE07C,0xFFFC,0x7FF8,0x3FF0,0x0000}; //3         
    const uint16_t data4[16]={0x03FC,0x07FC,0x0FFC,0x1E7C,0x3C7C,0x787C,0xF07C,0xFFFC,          
                              0xFFFC,0xFFFC,0x007C,0x007C,0x007C,0x007C,0x007C,0x0000}; //4         
    const uint16_t data5[16]={0xFFFC,0xFFFC,0xFFFC,0xE000,0xE000,0xFFE0,0xFFF0,0xFFF8,          
                              0x007C,0x003C,0xE03C,0xF07C,0xFFF8,0x7FF0,0x3FE0,0x0000}; //5         
    const uint16_t data6[16]={0x3FF8,0x7FFC,0xFFFC,0xF03C,0xE000,0xF000,0xFFF0,0xFFF8,          
                              0xFFFC,0xF03C,0xE01C,0xF03C,0xFFF8,0x7FF0,0x3FE0,0x0000}; //6         
    const uint16_t data7[16]={0x7FFC,0xFFFC,0xFFFC,0xFFFC,0x003C,0x007C,0x00FC,0x01F8,          
                              0x03F0,0x07E0,0x0FC0,0x1F80,0x3F00,0x7E00,0xFC00,0x0000}; //7         
    const uint16_t data8[16]={0x1FE0,0x3FF0,0x7FF8,0xF87C,0xF03C,0xF87C,0x7FF8,0x3FF0,          
                              0x7FF8,0xF87C,0xF03C,0xF87C,0x7FF8,0x3FF0,0x1FE0,0x0000}; //8         
    const uint16_t data9[16]={0x1FE0,0x3FF0,0x7FF8,0xF03C,0xF03C,0xF03C,0xFFFC,0x7FFC,          
                              0x3FFC,0x003C,0xE07C,0xF0F8,0xFFF0,0x7FE0,0x3FC0,0x0000}; //9
    const uint16_t dataColon[16]={0x0000,0x0000,0x0780,0x0780,0x0780,0x0780,0x0000,0x0000,          
                              0x0000,0x0780,0x0780,0x0780,0x0780,0x0000,0x0000,0x0000};  //:  
    const uint16_t dataSpace[16]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,          
                              0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};  //Space  
    const uint16_t dataPoint[16]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,          
                              0x0000,0x0000,0x0000,0x03C0,0x03C0,0x03C0,0x03C0,0x0000};  //.  
};
class Time:Font_16x16
{
public:
    Time(){}
    static volatile bool colon;
    void printTime(uint16_t x, uint16_t y,uint16_t col,uint8_t hour, uint8_t min, uint8_t sec)
    {
        if(Time::colon){Time::colon=false;}
        else {Time::colon=true;}
        if(hour<10)    // печатаем часы
        {
            intToChar(hour);
            print(x,y,col,"0",1);
            print(x+17,y,col,arr,1);
        }
        else
        {
            intToChar(hour);
            print(x,y,col,arr,2);
        }
        if(colon){print(x+17*2,y,col,":",1);}
        else{symbol(x+17*2,y,col,dataSpace);}
        

        if(min<10)      // печатаем минуты      
        {
            intToChar(min);
            print(x+17*3,y,col,"0",1);
            print(x+17*4,y,col,arr,1);
        }
        else
        {
            intToChar(min);
            print(x+17*3,y,col,arr,2);
        }
        if(colon){print(x+17*5,y,col,":",1);}
        else{symbol(x+17*5,y,col,dataSpace);}
        

        if(sec<10)       //печатаем секунды
        {
            intToChar(sec);
            print(x+17*6,y,col,"0",1);
            print(x+17*7,y,col,arr,1);
        }
        else
        {
            intToChar(sec);
            print(x+17*6,y,col,arr,2);
        }
    }
};

class Pixel:public Spi1_interface
{
public:
    Pixel(uint16_t x=0,uint16_t y=0,uint16_t col=BLACK)
    {
        //setPixel(x,y,col);  
    }
    enum col
    {
        RED        =    0xf800,
        GREEN      =    0x07e0,
        BLUE       =    0x001f,
        BLACK      =    0x0000,
        YELLOW     =    0xffe0,
        WHITE      =    0xffff,
        CYAN       =    0x07ff,
        BRIGHT_RED =    0xf810,
        GRAY1      =    0x8410,
        GRAY2      =    0x4208
    };

    void setPixel(uint16_t X, uint16_t Y, uint16_t color)
    {
        tft_setXY(X, Y);
        spi_sendWord(color);
        //spi_sendWord(color);
    }
};
class Rect:public Spi1_interface//, public Pixel
{
public:
    enum col
    {
        RED        =    0xf800,
        GREEN      =    0x07e0,
        BLUE       =    0x001f,
        BLACK      =    0x0000,
        YELLOW     =    0xffe0,
        WHITE      =    0xffff,
        CYAN       =    0x07ff,
        BRIGHT_RED =    0xf810,
        GRAY1      =    0x8410,
        GRAY2      =    0x4208
    };
public: 
    Rect(uint16_t x1=0,uint16_t x2=0,uint16_t y1=0,uint16_t y2=0,uint16_t col=BLACK)
    {        
    }   
    void setRect(uint16_t x1, uint16_t y1, uint16_t x2,uint16_t y2,uint16_t col) 
    {
        checkXYswap(x1,y1,x2,y2);
        tft_setColumn(x1,x2);
        tft_setRow(y1,y2);
        ST7789_SendCommand(0x2C);
        for(uint32_t i=0; i<=uint32_t((x2-x1+1)*(y2-y1+1));i++)
        {
            spi_sendWord(col);
        }
    }
};
class Line:public Pixel
{
public:
    Line(uint16_t x1=0,uint16_t y1=0,uint16_t x2=0,uint16_t y2=0,uint16_t col=BLACK)
    {
        //SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */ //FPU enable
        //checkXYswap(x1,y1,x2,y2);
        //if(x2!=x1)
        //{
        //    angle = atanf((y2-y1)/(x2-x1));
        //}
        //else if((x2==x1)&&y2!=y1)
        //{
        //    angle=90.0f*3.14159265f/90.0f ;
        //}
        //else {angle=0;}        
        length = float(sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))); //single float
        
        for(uint16_t i=0;i<uint32_t(length);i++) //uint16_t(length)
        {     
            //setPixel(x1+i,y1+sqrtf(i),col);
            setPixel(x1+uint16_t(i*((x2-x1)/length)),y1+uint16_t(i*((y2-y1)/length)),col);            
        }
    }
    void setLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t col)
    {
        float length1 = sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
        for(uint16_t i=0;i<uint32_t(length1);i++) //uint16_t(length)
        {     
            //setPixel(x1+i,y1+sqrtf(i),col);       
            setPixel(x1+i*(x2-x1)/length1,y1+i*(y2-y1)/length1,col);            
        }
    }
    void setDiscretLine(uint16_t X1,uint16_t Y1,uint16_t X2,uint16_t Y2,uint16_t col)
    {
        volatile uint16_t x1=X1; 
        volatile uint16_t y1=Y1;
        volatile uint16_t x2=X2;
        volatile uint16_t y2=Y2;   
        volatile uint16_t step = std::abs(y2-y1)>std::abs(x2-x1);
        if(step){swap(&x1,&y1);swap(&x2,&y2);} //меняем систему координат и отсчитываем в реальности по y
        //volatile uint16_t dx;
        //volatile uint16_t dy; 
        //dx=x2-x1; dy=std::abs(y2-y1);
        volatile uint16_t err=0;
        err = (x2-x1)/2; //точка перегиба
        volatile uint16_t ystep=0;
        if(y1<y2) {ystep=1;}
        else {ystep=-1;}     // если линия идет вверх меняем направление y

        for(;x1<=x2; x1++) //увеличиваем координату x1
        {
            if(step) {setPixel(y1,x1,col);} //если была поменяна система коодинат
            else     {setPixel(x1,y1,col);}
            err=err-(y2-y1);
            if(err>0)  //увеличиваем координату y
            {
                y1=y1+ystep+1;
                err=err+x2-x1;
            }
        }        
    }
private:
    float angle{0.0F};
    float length{0.0F}; //число точек - line length
    void swap(volatile uint16_t* x1,volatile uint16_t* x2)
    {
        volatile uint16_t z = *x2;*x1=*x2;*x2=z;
    }
};
class Elipse:public Pixel
{
public:
    Elipse(uint16_t x=0,uint16_t y=0,float R=0,uint16_t col=BLACK)
    {
        //SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */ //FPU enable
        for (uint16_t i=0;i<=2*R;i++)        
        {
            setPixel(x+i,y+sqrtf(R*R - (i-R)*(i-R))+R,col);
            setPixel(x+i,y-sqrtf(R*R - (i-R)*(i-R))+R,col);
            setPixel(x+sqrtf(R*R - (i-R)*(i-R))+R,y+i,col);
            setPixel(x-sqrtf(R*R - (i-R)*(i-R))+R,y+i,col);
        }
    }
private:
    float R{0.f};
};

#endif //SPILCD_H_