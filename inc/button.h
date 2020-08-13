#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

class Button
{
public:
    Button()
    {
        but_ini();
    }
    static bool Click;
private:
    void but_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;  
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;      
        GPIOC->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3); // reset
        GPIOC->CRL|=GPIO_CRL_CNF3_1; // CNF0[1:0] 1:0  - input mode with pull-up / pull-down
        GPIOC->ODR |= GPIO_ODR_ODR3; //Подтяжка вверх
        AFIO->EXTICR[0] |= (AFIO_EXTICR1_EXTI3_PC); //третий канал EXTI подключен к порту PC3
        EXTI->RTSR=0;
        EXTI->RTSR &=~ EXTI_RTSR_TR3; //Прерывание по нарастанию импульса отключено
        EXTI->FTSR |= EXTI_FTSR_TR3; //Прерывание по спаду импульса
        EXTI->PR |= EXTI_PR_PR3;      //Сбрасываем флаг прерывания перед включением самого прерывания
        EXTI->IMR |= EXTI_IMR_MR3;   //Включаем прерывание 3-го канала EXTI
        NVIC_EnableIRQ(EXTI3_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};
bool Button::Click=false;

extern "C" void EXTI3_IRQHandler()
{
    NVIC_DisableIRQ(EXTI3_IRQn);//! отключаем прерывание от кнопки
    TIM1->CR1|=TIM_CR1_CEN; //! включаем таймер1 для отсчета задержки от дребезга по прерыванию которого включаем прерывание от кнопки
    EXTI->PR = EXTI_PR_PR3; //Сбрасываем флаг прерывания
    Button::Click=true;
}

#endif //BUTTON_H