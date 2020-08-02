#ifndef ADC_H_
#define ADC_H

#include "main.h"
#include "usart1.h"

class ADC
{
public:
    ADC()
    {
        ADC_ini();
    }
    volatile uint8_t num = 0;

    float gausRand(float MO, float sko)
    {
    	volatile float sum=0;
    	volatile float x=0;
    	for(int i=0; i<=25;i++)
    	{
    		sum+=ADC_rand()/10.0;		
    	}
    	x=(1.33*(sko)*(sum-13.93))/(1.91) + sko + MO; //sqrt(1.77)=1.33
    	return x;
    }

    

private: 
    void ADC_ini()
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ; // разрешение тактирования порта A
        //GPIOA->CRL &= ~ (GPIO_CRL_MODE0 | GPIO_CRL_CNF0); // PA0 - аналоговый вход
        //GPIOA->CRL &= ~ (GPIO_CRL_MODE1 | GPIO_CRL_CNF1); // PA1 - аналоговый вход
        //GPIOA->CRL &= ~ (GPIO_CRL_MODE2 | GPIO_CRL_CNF2); // PA2 - аналоговый вход
        //GPIOA->CRL &= ~ (GPIO_CRL_MODE3 | GPIO_CRL_CNF3); // PA3 - аналоговый вход

        RCC->APB2ENR|=RCC_APB2ENR_ADC1EN; //clock adc1
        RCC->CFGR|=RCC_CFGR_ADCPRE_DIV6; //Биты 15:14 ADCPRE [1:0]. Предделитель АЦП.
	    
	    ADC1->CR2|=ADC_CR2_ADON; //enable ADC
	    ADC1->CR2&=~ADC_CR2_CONT; //0-single conversion,                  1 - continious conversion	
	    ADC1->CR2|=ADC_CR2_SWSTART; //event start conversion from software SWSTART

        ADC1->SQR1 =0; // 1 регулярный канал
        ADC1->SQR3 =0; // 1е преобразование - канал 0
        
	    ADC1->CR2 |= ADC_CR2_EXTSEL;//111 start conversion on external signal by rising edge
        //ADC1->CR2 |= ADC_CR2_EXTSEL; // источник запуска - SWSTART
        ADC1->CR2 |= ADC_CR2_EXTTRIG; // разрешение внешнего запуска для регулярных каналов

	    //ADC1->SMPR1|=ADC_SMPR1_SMP16; // 111:480 cycles;
	    //ADC1->CR1|=ADC_CR1_AWDCH_4; //wadchdog for 10000 adc 16 channel for temp sensor
	    //ADC1->CR1&=~ADC_CR1_AWDCH_3;ADC1->CR1&=~ADC_CR1_AWDCH_2;ADC1->CR1&=~ADC_CR1_AWDCH_1;ADC1->CR1&=~ADC_CR1_AWDCH_0;
	    //ADC1->SQR1&=~ADC_SQR1_L; // 0000 - 1 conversion on a channel
	    //ADC1->SQR3|=ADC_SQR3_SQ1_4; //10000 - 16 channel on 1 conversion 
	    //ADC1->SQR3&=~ADC_SQR3_SQ1_3;ADC1->SQR3&=~ADC_SQR3_SQ1_2;ADC1->SQR3&=~ADC_SQR3_SQ1_1;ADC1->SQR3&=~ADC_SQR3_SQ1_0;	    
    }

    uint16_t ADCint()
    {
    	ADC1->CR2|=ADC_CR2_SWSTART;         // запуск АЦП
        for(int i=0;i<10;i++){}
    	while((ADC1->SR&ADC_SR_EOC)==0){;}     // ожидание завершения преобразования
        uint16_t x=ADC1->DR;
        //USART1_ini::usart1_send(x&0xFF);
        //USART1_ini::usart1_send(x>>8);
        return x;      
    }
    uint8_t ADC_rand()
    {
        uint8_t x=0;
        uint8_t y=0;
        uint8_t c=0;
        for(int i=0; i<10;i++)
        {
            c=0;
            for(int j=0;j<10;j++)
            {
                x+=0x3&ADCint();  //10 раз складываем последние 2 бита АЦП
            }
            c=x>>1; //сдвигаем на один вправо, для уменьшения числа
            y+=c;            
        }
        return y%10; //берем по модулю (число в пределах 10)
    }
    
};



#endif //ADC_H