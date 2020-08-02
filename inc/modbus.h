#ifndef MODBUS_H
#define MODBUS_H

//#include "stm32f10x.h"
#include "led.h"

//uart structure структура работы с модбас
class ModBus {
public: 
    ModBus() //инициализируем константы
    {                     
        //res_table.regs[Hold_Reg_Size];    // выделяем память под внутренние 16битные регистры инф с которых передается
        //res_table.bytes[Hold_Reg_Size*2]; // выделяем память под coils registers
        res_table.regs[0] = 1;
	    res_table.regs[1] = 2;
	    res_table.regs[2] = 3;
    }
public:
        
    //buffer uart    
    uint16_t Crc16(uint8_t* data,uint8_t length)
    {   
        unsigned short reg_crc=0xFFFF;//Load a 16–bit register with FFFF hex (all 1’s).
        while(length--)
        {
            unsigned char data1=*data;
            data++;
            reg_crc ^= data1;
            //Exclusive OR the first 8–bit byte of the message with the low–order byte of the 16–bit CRC register,
            //putting the result in the CRC register.
            for(int j=0;j<8;j++)
            {
                //Shift the CRC register one bit to the right (toward the LSB),
                //zero–filling the MSB. //"EXTRACT!!!- то есть сравнивать байт до XOR-а => в if-е" and examine the LSB.

                //(If the LSB was 1): Exclusive OR the CRC register with the polynomial value 0xA001
                //(1010 0000 0000 0001).
                if(reg_crc & 0x0001){ reg_crc=(reg_crc>>1) ^0xA001;} // LSB(b0)=1
                // (If the LSB was 0): Repeat Step 3 (another shift).
                else reg_crc=reg_crc>>1;
            }
        }
        //uint8_t CRChi=reg_crc>>8;
        //uint8_t CRClo=reg_crc&0x00FF;
        //uint16_t mod=(CRClo<<8|CRChi);
        //reg_crc=mod;
        return reg_crc;
    }    

    void TX_03_04() //метод обрабытывающая команды 03 и 04
    {
        //******************************************************************
        //READING input & holding registers
        //*******************************************************************
        volatile uint16_t tmp=0;
        volatile uint16_t tmp1=0; // переменные для адреса и количества регистров
        unsigned int m=0,n=0;
        int  tmp_val;     
        int  tmp_val_pos; // for minus
        //MODBUS->buffer[0] =SET_PAR[0]; // adress - stays a same as in received
        //MODBUS->buffer[1] = 3; //query type - - stay a same as in recived
        //MODBUS->buffer[2] = data byte count
        //2-3  - starting address
        tmp=((buffer[2]<<8)+buffer[3]); //стартовый адрес для чтения (два байта адреса младший справа)
                                                       // если старший равен нулю то вычисляется просто по младшему
        //4-5 - number of registers
        tmp1=((buffer[4]<<8)+buffer[5]);//количество регистров для чтения (также как и для адресов)
        //default answer length if error
        n=3; // длина пакета с ответом по умолчанию если ошибка
        //если нас устраивает длина запроса и и стартовый адрес
        if((((tmp+tmp1)<Hold_Reg_Size)               // длина запроса не более 123 
                &(tmp1<MODBUS_WRD_SZ+1)))   // количество регистров для чтения не более 125
        {
            //if(GPIOD->ODR&GPIO_ODR_ODR_13){GPIOD->ODR&=~GPIO_ODR_ODR_13;}
        	//else {GPIOD->ODR|=GPIO_ODR_ODR_13;}	

            for(m=0;m<tmp1;m++)                // пробегаем поочереди все регистры 
            {
               tmp_val=res_table.regs[m+tmp];   //читаем текущее значение 
               if(tmp_val<0)
               {
               //пакуем отрицательное
               tmp_val_pos=tmp_val;
               buffer[n]=(tmp_val_pos>>8)|0b10000000; // запись отрицательного числа добавляется в начало единичка
               buffer[n+1]=tmp_val_pos;
               }
               else
               {
               //пакуем положительное
               buffer[n]=tmp_val>>8; // начиная от n = 3 первые три зарезервированы
               buffer[n+1]=tmp_val;  // регистры 16-битные => кладем их побайтно сначала младший потом старший
               }
               n=n+2; //приплюсовываем два байта     
            }     
            //запишем длину переменных пакета в байтах и вставим в сообщение
            buffer[2]=m*2; //byte count  количество байт в ответе
            //подготовим к отправке
            txlen=m*2+5; //responce length 5 = адр, функц, колво байт, CRCh, CRCl;
        }
        else
        {        
            //exception illegal data adress 0x02
            TX_EXCEPTION(0x02);
        }         
    }

    void TX_06()    //метод обрабатывающая команды 06 - запись холдинг регистра
    {
        //*******************************************************
        //Writing
        //*******************************************************
        unsigned int tmp;

	    //MODBUS[0] =SET_PAR[0]; // adress - stays a same as in recived
	    //MODBUS[1] = 6; //query type - - stay a same as in recived

	    //2-3  - adress   , 4-5 - value

	    tmp = ((buffer[2] << 8) + buffer[3]); //adress  адрес регистра

	    //MODBUS->buffer[2]  - byte count a same as in rx query

	    if (tmp < Hold_Reg_Size) {
	    	txlen = rxcnt; //responce length длина отправляемой посылки равна длине принятой посылки
	    	res_table.regs[tmp] = (buffer[4] << 8) + buffer[5]; //записываем число

	    } else {
	    	//illegal data
	    	TX_EXCEPTION(0x02);
	    }
    }
    void TX_EXCEPTION(unsigned char error_type) // метод ошибки запрошенных данных
    {
        //********************************************************************
        //Exception if wrong query
        //*********************************************************************

        //modbus exception - illegal data=01 ,adress=02 etc 
        //illegal operation
    	buffer[2] = error_type; //exception
    	txlen = 5; //responce length
        //if(GPIOD->ODR&GPIO_ODR_ODR_14){GPIOD->ODR&=~GPIO_ODR_ODR_14;}
    	//else {GPIOD->ODR|=GPIO_ODR_ODR_14;}

    }
    //void TIM3_init(void); // таймер 3 для подсчета задержки
    void MODBUS_SLAVE() //метод обработки данных модбас, принятых по uart и формирования ответа
    {        
        //LED13::ledOn();
        unsigned int tmp;
        //recive and checking rx query проверяем адрес и счетчик rxcount>5
        if((buffer[0]!=0)&(rxcnt>5)& ((buffer[0]==ADDRESS)|(buffer[0]==255)))
        {
            tmp=Crc16((uint8_t*)buffer,rxcnt-2); // вычисляем CRC с помощью таблицы и кладем в tmp
            //LED13::ledOn();
            volatile uint16_t CRChi=tmp&0x00FF;
            volatile uint16_t CRClo=tmp>>8;

            if((buffer[rxcnt-2]==CRChi)  // если первый байт пришедшего CRC == второму байту вычесленного CRC и...
            & (buffer[rxcnt-1]==CRClo)) // если второй байт пришедшего CRC == первому байту вычесленного CRC
            {
                //если мы сюда попали значит пакет наш и crc совпало - надо проверить поддерживаем ли мы такой запрос
                //choosing function
                switch(buffer[1])  // проверяем второй байт пакета (метод)
                {
                    case 3:               // в случае  0x03 переходим в метод:
                    TX_03_04();
                    break;

                    case 4:              // в случае  0x04 переходим в метод:
                    TX_03_04();
                    break;

                    case 6:              // в случае  0x06 переходим в метод:
                    TX_06();
                    break;
                           default://если нет то выдаем ошибку
                    //illegal operation
                    TX_EXCEPTION(0x01);
                }
                // После обработки данных и заполнении буфера отправки
                //добавляем CRC и готовим к отсылке
                //adding CRC16 to reply
                tmp=Crc16((uint8_t*)buffer,txlen-2);  //вычисляем CRC буфера отправки
                buffer[txlen-2]=tmp;        // добавляем в буфер младший байт CRC
                buffer[txlen-1]=tmp>>8;     // добавляем в буфер старший байт CRC
                txcnt=0;  //обнуляем счетчик байтов на передачу        
            }
        }
        //сброс индикаторов окончания посылки
        rxgap=0;        // сбрасываем флаг о окончании пакета
        rxcnt=0;        // обнуляем счетчик приема
        rxtimer=0xFFFF; // обнуляем таймер
    }
    void net_tx3() //Включение передачи по окончанию приема и обработки данных 
    {
        if((txlen>0)&(txcnt==0)) // если буфер на передачу заполнен и передача не производилась
        {  
            USART1->CR1&=~USART_CR1_RXNEIE;//выкл прерывание на прием чтобы не мешалось
            USART1->CR1 |=USART_CR1_TCIE;//включаем перывание на окочание передачи
            //usart2_send(uart->buffer[uart->txcnt]); // посылаем первый байт по UART ошибка!!"!!" 
        }
    }

public:
    //PARAMETERRS ARRAY 0 PARAMETER = MODBUS ADDRESS
    static const unsigned char ADDRESS;//0-элемент это адрес SLAVE
    static const uint8_t BUF_SZ; //размер буфера
    static const uint8_t MODBUS_WRD_SZ;// = (BUF_SZ-5)/2; //максимальное количество регистров в ответе 125
    static const uint8_t Hold_Reg_Size;// = 123; //это максимальное количество объектов (холдинг регистров)

    static volatile unsigned int rxtimer;//счетчик таймера - переменная в которой мы считаем таймаут (несколько раз пока меньше delay)
    static volatile unsigned char rxcnt; //количество принятых символов
    static volatile unsigned int delay;//задержка по которой вычисляется конец посылки
    static volatile unsigned char rxgap;//окончание приема флаг 1-окончание приема (rxtimer>delay)
    static volatile unsigned char buffer[]; //буфер в который считываются данные с uart и потом записываются в uart
    static volatile unsigned char txcnt;//количество переданных символов
    static volatile unsigned char txlen;//длина посылки на отправку   

private:    
    unsigned char protocol;//тип протокола - здесь не используется    
    
public:
    union 
    {
        int16_t regs[127];    // внутренние 16битные регистры инф с которых передается
        int8_t bytes[255];  // внутренние 8битные регистры флагов
    }res_table;
};

    const unsigned char ModBus::ADDRESS=1;//0-элемент это адрес SLAVE
    const uint8_t ModBus::BUF_SZ=255; //размер буфера
    const uint8_t ModBus::MODBUS_WRD_SZ = (BUF_SZ-5)/2; //максимальное количество регистров в ответе 125
    const uint8_t ModBus::Hold_Reg_Size = 123; //это максимальное количество объектов (холдинг регистров)

    volatile unsigned int ModBus::rxtimer=0;//счетчик таймера - переменная в которой мы считаем таймаут (несколько раз пока меньше delay)
    volatile unsigned char ModBus::rxcnt=0; //количество принятых символов
    volatile unsigned int ModBus::delay=420; //таймаут приема (равняется 3,5 символа для скорости 9600 бод)//задержка по которой вычисляется конец посылки
    volatile unsigned char ModBus::rxgap=0;//окончание приема флаг 1-окончание приема (rxtimer>delay)
    volatile unsigned char ModBus::buffer[BUF_SZ];//буфер в который считываются данные с uart и потом записываются в uart
    volatile unsigned char ModBus::txcnt=0;//количество переданных символов
    volatile unsigned char ModBus::txlen=0;//длина посылки на отправку   


class TIMER4
{
public:
    TIMER4()
    {
        timer_ini4();
    }    
private:
    void timer_ini4()
    {
        //настраиваем PIN B6 ДЛЯ ШИМ
        //RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        //GPIOB->CRL|=GPIO_CRL_MODE6; //11: Output mode, max speed 50 MHz. -max
        //GPIOB->CRL|=GPIO_CRL_CNF6_1;
        //GPIOB->CRL&=~GPIO_CRL_CNF6_0; //10: Alternate function output Push-pull
        
        // Таймер 3 на шине APB1 36MHz
        RCC->APB1ENR|=RCC_APB1ENR_TIM4EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
        TIM4->PSC=72; //0  //делить частоту шины apb1(36MHz*2 при SysClk -72MHz) на 36 => частота 1MHz 
        TIM4->ARR=100-1; //считаем до 100 => прерывание раз в 100 мкс  period 
	    
        TIM4->CR1|=TIM_CR1_ARPE;
	    TIM4->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
        TIM4->CR1|=TIM_CR1_CEN; //включаем таймер
	    NVIC_EnableIRQ(TIM4_IRQn); //включаем обработку прерывания по таймеру 3

                //ШИМ        
        //TIM4->CCER|=TIM_CCER_CC1E; //включаем выход capture compare
        //      set up timer
        //TIM4->ARR = 1;            // period
        //TIM4->PSC = 0;                // prescaler
        //TIM4->EGR = TIM_EGR_UG;        // generate an update event to reload the prescaler value immediately
        //TIM4->CR2 = 0;
        // set up compare (ch4)
        //TIM4->CCER &= ~(TIM_CCER_CC4E | TIM_CCER_CC4P);    // disable cc4 output, clear polarity
        //TIM4->CCR2 = 18;            // cc4 period
        //TIM4->CCMR2 &= 0x00FF;
        //TIM4->CCMR2 |=
        //    TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1         // PWM 1 (0x6000)
        //    | TIM_CCMR2_OC4PE;                            //  enable preload;
   //
        //TIM4->CCER |= TIM_CCER_CC4E;    // enable cc4 output//
        //TIM4->CR1 |= TIM_CR1_CEN;        // run timer
    }
};

#endif //MODBUS_H





