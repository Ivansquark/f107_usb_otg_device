/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
#include "main.h"
void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)

int main()
{		
    RCCini rcc;	//! 72 MHz
	Timers tim4_1sec(4);
	Timers encoder(3); //!< encoder
	Timers antiRattleTimer(1);
	Button encoderBut;
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
	NormalQueue8 que;
	USB_DEVICE usb;
	USART_debug usart2(2);
	
	uint8_t arr[8]={'U','S','B','-',0x30,0x30,'\t','\n'};
	uint8_t count;
	__enable_irq();
	//for(uint32_t i=0;i<10000000;i++){}
	uint8_t i=0x30;
	bool startINflag=false;
	volatile uint8_t a=0;
	while(1)
	{
		uint8_t encoderCounter[1]={a};			
		if(Timers::timerSecFlag)
		{
			i++;
			if(i==0x39){i=0x30;}
			arr[5]=i;
			if(startINflag){usb.WriteINEP(0x03,arr,8);}			
			Timers::timerSecFlag=false;
		}
		if(!que.isEmpty())
		{
			count = que.pop(); //считываем из очереди
			if (count==255){startINflag=true;}
		}
		font16.intToChar(count);
		font16.print(10,10,0x00ff,font16.arr,2);
		//
		font16.intToChar(usb.resetFlag);
		font16.print(150,10,0x00ff,font16.arr,2);
		//
		a=TIM3->CNT;
		if(Button::Click){Button::Click=false;usb.WriteINEP(0x03,encoderCounter,1);}	
		font16.intToChar(a);
		font16.print(100,80,0x00ff,font16.arr,2);		
	}
    return 0;
}
