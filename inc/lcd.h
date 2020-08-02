#ifndef LED_H_
#define LED_H

#include "main.h"
#include <cmath>

class Pixel
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
        setXY(X,Y);
        sendWord(color);
    }
    
private:
    void sendCommand(uint8_t byte)
    {
        while((SPI1->SR&SPI_SR_BSY)){}  //пока не появится флаг о конце передачи
        //dc_command();
        sendByte(byte);
    }
    void sendData(uint8_t data)
    {
        while((SPI1->SR&SPI_SR_BSY)){}  //пока не появится флаг о конце передачи
        //dc_data();
        sendByte(data);
    }
    void sendByte(uint8_t byte)
    {
        while(!(SPI1->SR&SPI_SR_TXE)){} //пока не скинется ножка о передаче в регистре статуса 
        SPI1->DR = byte;
    }
    void sendWord(uint16_t data)
    {
        while((SPI1->SR&SPI_SR_BSY)){}  //пока не появится флаг о конце передачи
        sendByte(data>>8);
        sendByte(data&0x00FF);        
    }
    void setColumn(uint16_t startColumn, uint16_t endColumn)
    {
        sendCommand(0x2A);
        sendWord(startColumn);
        sendWord(endColumn);
    }
    void setPage(uint16_t startPage,uint16_t endPage)
    {
        sendCommand(0x2B);
        sendWord(startPage);
        sendWord(endPage);
    }
    void setXY(uint16_t X, uint16_t Y)
    {
        setColumn(X,X);     //координата X
        setPage(Y,Y);       //координата Y
        sendCommand(0x2C);
    }
};
class Rect:public Pixel
{
public: 
    Rect(uint16_t x1=0,uint16_t x2=0,uint16_t y1=0,uint16_t y2=0,uint16_t col=BLACK)
    {
        for(uint32_t i=x1;i<x2; i++)
        {
            for(uint32_t j=y1;i<y2; j++)
            {
                setPixel(i,j,col);
            }
        }
    }
};
class Line:public Pixel
{
public:
    Line(uint16_t x1=0,uint16_t x2=0,uint16_t y1=0,uint16_t y2=0,uint16_t col=BLACK)
    {
        if((y2-y1)!=0)
        {
            angle = -acos((x2-x1)/(y2-y1));
        }
        else if((y2-y1)==0&&x2<x1)
        {
            angle=180*3.14159265/180 ;
        }
        else angle=0;
        
        length = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
        for(uint16_t i=0;i<(uint16_t)length;i++)
        {            
            setPixel(x1+i*cos(angle),y1+i*sin(angle),col);            
        }
    }
    float angle{0};
    float length{0}; //число точек - line length
};
#endif //LED_H_