#ifndef USART1_H_
#define USART1_H_

#include "stm32f10x.h"
//--------------------------------------------------------------------------------------------------

class USART1_ini
{
public:
    USART1_ini()
    {
        usart1_ini();
    }
    inline static void usart1_send(char c)
    {
        while(!(USART1->SR&USART_SR_TC)) {}
        USART1->DR=c;
        //USART2->SR&=~USART_SR_TC;
    }
    inline static void usart1_sendSTR(const char* str)
    {
    	uint8_t i=0;
    	while(str[i])
    	{
    		usart1_send(str[i++]);		
    	}
    }

private:    
    void usart1_ini()
    {
        // настраиваем пины
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN; // тактирование на порт А
        //Tx portA-9
        GPIOA->CRH|=GPIO_CRH_MODE9; //11: Output mode, max speed 50 MHz.
        GPIOA->CRH|=GPIO_CRH_CNF9_1;
        GPIOA->CRH&=~GPIO_CRH_CNF9_0; //10: Alternate function output Push-pull
        //Rx portA-10
        GPIOA->CRH&=~GPIO_CRH_MODE10; //00: Input mode (reset state).
        GPIOA->CRH|=GPIO_CRH_CNF10_0;
        GPIOA->CRH&=~GPIO_CRH_CNF10_1; //01: Floating input (reset state)

        //настраиваем альтернативные пины usart1
        RCC->APB2ENR|=RCC_APB2ENR_USART1EN; //подаем тактирование
        AFIO->MAPR&=~AFIO_MAPR_USART1_REMAP; // 0: No remap (TX/PA9, RX/PA10)
        AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI10_PA; //0000: PA[x] pin
        AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI9_PA;  //0000: PA[x] pin

        USART1->CR1|=USART_CR1_UE; //USART EN
        USART1->CR1&=~USART_CR1_M; //0 - 8 Data bits
        USART1->CR1|=USART_CR1_TE; // Transmission enabled
        USART1->CR1|=USART_CR1_RE; // Recieving enabled
        USART1->CR1|=USART_CR1_RXNEIE; //enable interrupt on Rx from usart1
        //USART1->BRR=0x445C; //2400 (APB1CLK - baudrate/2)/baudrate
        //USART1->BRR=0xEA6; //9600    (36000-4800)/9600
        USART1->BRR=0x1D4C; //9600    (72000-4800)/9600
	    //USART1->BRR=0x16C; //115200

        NVIC_EnableIRQ(USART1_IRQn);
    }
};



#endif //USART1_H





