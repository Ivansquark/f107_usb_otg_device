#ifndef LED_H_
#define LED_H_

#include "stm32f10x.h"
class LED13
{
public:
	LED13()
	{
		led_ini();
	}
	static void ledOff()
	{       
        if(!(GPIOC->ODR&GPIO_ODR_ODR13))
        {
            GPIOC->ODR^=GPIO_ODR_ODR13;
        }
    }
	static void ledOn()
	{
        if(GPIOC->ODR&GPIO_ODR_ODR13)
        {
            GPIOC->ODR^=GPIO_ODR_ODR13;
        }
    }
	static void toggle()
	{
		GPIOC->ODR^=GPIO_ODR_ODR13;
	}
private:
	void led_ini()
	{
		RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
		GPIOC->CRH|=GPIO_CRH_MODE13_0;
		GPIOC->ODR^=GPIO_ODR_ODR13;	
		//GPIOC->BRR|=GPIO_BRR_BR13;
	}
};

#endif //LED_H_