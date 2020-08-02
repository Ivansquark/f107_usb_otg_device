#ifndef TIMER3_H_
#define TIMER3_H_

#include "main.h"

/*!
    \brief Timers class
    Tim1 - first timer - 100 ms for exclude button rattling
    Tim4 - 1 second timer interval
    --- Tim2+Tim3  - 32bits counter, counts on ETR input ---
    Tim2 - master ETR remap to pa-15 tolerant to 5V
    Tim3 - slave
*/

class Timers
{
public:
    Timers(uint8_t init){timer_ini(init);}
    static bool timerSecFlag;
private:
    void timer_ini(uint8_t init)
    {
        switch(init)
        {
            case 1:
            {
                /*! **************** \brief TIMER_1 - time counting to 100 milisecond  **************************************/
                RCC->APB2ENR|=RCC_APB2ENR_TIM1EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
                TIM1->PSC=64000; //0  //делить частоту шины apb1(64MHz*2 при SysClk -128MHz) на 64000 => частота 2kHz 
                TIM1->ARR=200-1; //считаем до 2000 => прерывание раз в 100 мс  period                 
                TIM1->CR1|=TIM_CR1_ARPE;  // задействуем регистр auto reload
	            TIM1->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
                //TIM1->CR1|=TIM_CR1_CEN; //включаем таймер
	            NVIC_EnableIRQ(TIM1_UP_IRQn); //включаем обработку прерывания по таймеру 1
            }break;
            case 2:
            {
                 //**************  TIM2_ETR - PA15 ***********************************
                RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
                RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
                GPIOA->CRH&=~GPIO_CRH_MODE15;
                GPIOA->CRH|=GPIO_CRH_CNF15_0; // 0:1 floating input (reset state)
                AFIO->MAPR|=AFIO_MAPR_TIM2_REMAP; //full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)
                //***************** TIMER_2 - master at external clock low two bytes ****************************

                RCC->APB1ENR|=RCC_APB1ENR_TIM2EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
                TIM2->PSC=1-1; //0  //делить частоту шины apb1(36MHz*2 при SysClk -72MHz) на 36 => частота 2kHz 
                TIM2->SMCR|=TIM_SMCR_ECE; // external clock enable
                TIM2->CR2&=~TIM_CR2_MMS_2;
                TIM2->CR2|=TIM_CR2_MMS_1;
                TIM2->CR2&=~TIM_CR2_MMS_0; // 0:1:0 update
                TIM2->EGR|=TIM_EGR_UG; //update generation
                
                //TIM3->ARR=1000-1; //update mode
                TIM2->CR1|=TIM_CR1_ARPE;  // задействуем регистр auto reload
	            //TIM3->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
                TIM2->CNT=0;
                TIM2->CR1|=TIM_CR1_CEN; //включаем таймер
	            //NVIC_EnableIRQ(TIM3_IRQn); //включаем обработку прерывания по таймеру 3

                
            }break;
            case 3:
            {
               //******************** TIMER_3 - slave start counts at TIMER_3 trigger high two bytes **************************************
                RCC->APB1ENR|=RCC_APB1ENR_TIM3EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
                TIM3->PSC=1-1; //0  //делить частоту шины apb1(36MHz*2 при SysClk -72MHz) на 36 => частота 2kHz 
                //TIM2->ARR=1000-1; //считаем до 1000 => прерывание раз в 500 мс  period                 
                //TIM2->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
                TIM3->CR1|=TIM_CR1_ARPE;  // задействуем регистр auto reload	            
                TIM3->SMCR&=~TIM_SMCR_TS_2;
                TIM3->SMCR&=~TIM_SMCR_TS_1; //0:0:1 trigger selection (TIM3 - slave from TIM2)
                TIM3->SMCR|=TIM_SMCR_TS_0;
                TIM3->SMCR|=TIM_SMCR_SMS; //1:1:1 slave mode selection : external trig clocks counter  
                TIM3->SMCR|=TIM_SMCR_MSM; // Master/slave mode (select slave mode)
                TIM3->CNT=0;
                TIM3->CR1|=TIM_CR1_CEN; //включаем таймер                
            }break;
            case 4:
            {
                //***************** TIMER_4 - time counting to 1 second  **************************************
                RCC->APB1ENR|=RCC_APB1ENR_TIM4EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
                TIM4->PSC=64000; //0  //делить частоту шины apb1(64MHz*2 при SysClk -128MHz) на 64000 => частота 2kHz 
                TIM4->ARR=2000-1; //считаем до 2000 => прерывание раз в 1 с  period                 
                TIM4->CR1|=TIM_CR1_ARPE;  // задействуем регистр auto reload
	            TIM4->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
                TIM4->CR1|=TIM_CR1_CEN; //включаем таймер
	            NVIC_EnableIRQ(TIM4_IRQn); //включаем обработку прерывания по таймеру 3
            }break;
            default: break;
        }
    }
};
bool Timers::timerSecFlag=false;

//--------------------------------------------------------------------------------------------------------
uint16_t low=0;
uint16_t high=0;
uint32_t freq=0;

extern "C" void TIM4_IRQHandler(void) //!обработчик прерывания раз в 1 с
{
	TIM4->SR &=~ TIM_SR_UIF; //!< скидываем флаг прерывания
	Timers::timerSecFlag=true;
    low=TIM2->CNT;
	high=TIM3->CNT;
	TIM3->CNT=0;
	TIM2->CNT=0;
    freq=(high<<16|low);   //!< frequency accounting by summarizing two timers count registers 
	NVIC_ClearPendingIRQ(TIM4_IRQn); //! скидываем флаг ожидания прерывания
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*! antirattle timer */
extern "C" void TIM1_UP_IRQHandler(void) //!обработчик прерывания раз в 100 ms
{
    TIM1->CR1 &=~ TIM_CR1_CEN; //!< Выключаем таймер
	TIM1->SR &=~ TIM_SR_UIF; //скидываем флаг прерывания
    NVIC_EnableIRQ(EXTI0_IRQn);     //! Разрешаем прерывания от кнопки на PA0
	NVIC_EnableIRQ(EXTI9_5_IRQn);   //! Разрешаем прерывания от кнопок на PB6 PB7
	NVIC_ClearPendingIRQ(TIM1_UP_IRQn); //! скидываем флаг ожидания прерывания, очищая бит NVIC_ICPRx (бит очереди)    
}
//--------------------------------------------------------------------------------------------------------

#endif //TIMER3_H_
