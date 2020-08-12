#ifndef USART1_H_
#define USART1_H_

#include "main.h"
//--------------------------------------------------------------------------------------------------

class USART_debug
{
public:
    USART_debug(uint8_t num)
    {
        usart1_ini(num);
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
    inline static void usart2_send(char c)
    {
        while(!(USART2->SR&USART_SR_TC)) {}
        USART2->DR=c;
        
        //USART2->SR&=~USART_SR_TC;
    }
    inline static void usart2_sendSTR(const char* str)
    {
    	uint8_t i=0;
    	while(str[i])
    	{
    		usart2_send(str[i++]);		
    	}
    }

private:    
    void usart1_ini(uint8_t num)
    {
        switch(num)
        {
            case 1:
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
            break;
            case 2:
            {
                // настраиваем пины USART 2
                RCC->APB2ENR|=RCC_APB2ENR_IOPDEN; // тактирование на порт B
                //Tx portD-5
                GPIOD->CRL|=GPIO_CRL_MODE5; //11: Output mode, max speed 50 MHz.
                GPIOD->CRL|=GPIO_CRL_CNF5_1;
                GPIOD->CRL&=~GPIO_CRL_CNF5_0; //10: Alternate function output Push-pull
                //Rx portD-6
                GPIOD->CRL&=~GPIO_CRL_MODE6; //00: Input mode (reset state).
                GPIOD->CRL|=GPIO_CRL_CNF6_0;
                GPIOD->CRL&=~GPIO_CRL_CNF6_1; //01: Floating input (reset state)


                //настраиваем альтернативные пины usart1
                RCC->APB1ENR|=RCC_APB1ENR_USART2EN; //подаем тактирование
                AFIO->MAPR|=AFIO_MAPR_USART2_REMAP; // 1: No remap (TX/PD5, RX/PD6)
                //AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI10_PA; //0000: PA[x] pin
                //AFIO->EXTICR[2]|=AFIO_EXTICR3_EXTI9_PA;  //0000: PA[x] pin

                
                USART2->CR1&=~USART_CR1_M; //0 - 8 Data bits
                USART2->CR1|=USART_CR1_TE; // Transmission enabled
                USART2->CR1|=USART_CR1_RE; // Recieving enabled
                USART2->CR1|=USART_CR1_RXNEIE; //enable interrupt on Rx from usart1
                //USART2->BRR=0x445C; //2400 (APB1CLK - baudrate/2)/baudrate
                //USART2->BRR=0xEA5; //9600    (36000000-4800)/9600
                //USART2->BRR=0x1D4C; //9600    (72000-4800)/9600
	            USART2->BRR=0x138; //115200         
                USART2->CR1|=USART_CR1_UE; //USART EN       
                //NVIC_EnableIRQ(USART2_IRQn);
            }
            break;
        }        
    }
};
#endif //USART1_H





