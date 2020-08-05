/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"
void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)

int main()
{	
	QueWord que;
    RCCini rcc;	//! 72 MHz
	//LED13 led;
	//__enable_irq();
	//RCC->APB2ENR|=RCC_APB2ENR_IOPEEN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->MAPR|=AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	SpiLcd lcd;
	lcd.fillScreen(0xff00);
	Font_16x16 font16;
	__enable_irq();
	USB_DEVICE usb;

	uint32_t count=0;
	USART_debug usart2(2);
	__enable_irq();
	while(1)
	{		
		//font16.intToChar(usb.resetFlag);
		//font16.print(10,10,0x00ff,font16.arr,2);
		//
		//font16.intToChar(usb.counter);
		//font16.print(100,10,0x00ff,font16.arr,2);
		//
		//font16.intToChar(usb.ADDRESS);
		//font16.print(100,80,0x00ff,font16.arr,2);
		//
		//
		//font16.intToChar(usb.bmRequestType);
		//font16.print(10,40,0x00ff,font16.arr,2);
		//
		//font16.intToChar(usb.bRequest);
		//font16.print(10,60,0x00ff,font16.arr,2);

		//font16.intToChar(usb.wValue);
		//font16.print(10,80,0x00ff,font16.arr,2);
		//
		//font16.intToChar(usb.wIndex);
		//font16.print(10,100,0x00ff,font16.arr,2);
		//
		//font16.intToChar(usb.wLength);
		//font16.print(10,120,0x00ff,font16.arr,2);
		//
		//font16.intToChar(count);
		//font16.print(50,220,0x00ff,font16.arr,2);
		//count++;
		//usart2.usart2_send(count);
		//usart2.usart2_sendSTR("opa");
	}
    return 0;
}
