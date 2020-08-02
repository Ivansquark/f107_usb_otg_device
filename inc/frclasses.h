#ifndef FRCLASSES_H_
#define FRCLASSES_H_

#include "frwrapper.h"
#include "timer3.h"

/*!
    \brief classes thats inherits from frWrappers classes
    these classes realizes 
*/
//-----------------------------------------------------------------------------
class TimerSingle3s:public OS_timer
{
public:
    TimerSingle3s(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID=nullptr):OS_timer(timName,period,AutoReload,this)
    {}
    void run()override
    {
        singleShot=true;  //!< software timer counts to end     
        //start(1);
    }    
};
//bool TimerSingle1s::singleShot = false;
//-----------------------------------------------------------------------------
//!< debug class that blinking diod on PC13
class BlinkFR:public iTaskFR
{
public:
    BlinkFR(){leds_ini();}
    ~BlinkFR()override{}
    void run() override
    {
        blink13();
    }
private:
    void blink13()
    {
        while(1)
        {
            GPIOC->ODR^=GPIO_ODR_ODR13;
            OS::sleep(200);
        }        
    }
    void leds_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	    GPIOC->CRH&=~GPIO_CRH_CNF13;
	    GPIOC->CRH|=GPIO_CRH_MODE13;//50MHz
    }
};

/*! \brief calibration task class*/
class Calibration: public iTaskFR
{
public:
    Calibration(OS_timer* Ntim=nullptr){tim = Ntim; reed_Cal_ini(); L_cal=6.8;}
    void run() override
    {
        constexpr float pipi4 = 1.0/(4*3.14*3.14);
        while(1)
        {
            if(calStarts==true)
            {
                reedSwithCal();
                tim->start(10); //!< start timer first time to get calEnds     
                calStarts = false;                          
                calStartEnds=true;
            }
            else if(calStartEnds==true && tim->singleShot==true)
            {
                calStartEnds=false; tim->singleShot=false; 
                calEnds=true;
                calStarts=false;
                //tim->start(10); //!< start timer second time to get calEnds     
                {
                    if (freq!=0)
                    {
                        // TODO: rfequency allways different
                        // end need to match frequency to C_cal that is equal to 940 pF 
                        C_cal = pipi4*1E12*(1.0/(freq*freq*L_cal));
                    }
                    else C_cal=0;                    
                } //!< TODO here frequency must accounting                
            }
            OS::sleep(100);
        }
    }
    static bool calStarts;
    static bool calEnds;
    static float C_cal;
    static float L_cal; //!< 6.8 mH
private:
    bool calStartEnds{false};
    OS_timer* tim{nullptr};
    void reed_Cal_ini()
    {                   //!< 1-pb3 2-pb4 3-pb5 
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        GPIOA->CRH|=GPIO_CRH_MODE8; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF8; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE9; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF9; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE10; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF10; //!< 0:0 push-pull
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10);//!< sets to 1 thats block relay coil
    }
    void reedSwithCal() //!< swithes reed relay in calibration mode (1:0:1)
    {        
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BR9|GPIO_BSRR_BS10);//!< to zero that turn on reed
    }
};
bool Calibration::calStarts = false;
bool Calibration::calEnds = false;
float Calibration::C_cal = 0;
float Calibration::L_cal = 0;

//-------------------------------------------------------------------------------------------------
/*! \brief measuring L task class*/
class MeasureL: public iTaskFR
{
public:
    MeasureL(QueueOS<float,1>* NqueueFloat = nullptr, OS_timer* Ntim = nullptr)
    {queueFloat = NqueueFloat,tim = Ntim; reed_ports_ini();}
    void run() override
    {
        float L=0;
        constexpr float pipi4 = 1.0/(4*3.14*3.14);
        while(1)
        {
            if(Calibration::calEnds && Lflag) 
            {reedSwitchL(); tim->start(1); Lstart=true; Calibration::calEnds = false;} //!< starts singleshot timer again
            if(tim->singleShot && Lflag && Lstart)
            {   
                tim->singleShot=false;         
                Lflag = false; Lstart =false;                
                { //!< TODO here L must accounting
                    if(freq!=0)
                    {
                        float L1 = pipi4*1E12*(1.0/(freq*freq*Calibration::C_cal));
                        L = L1 - Calibration::L_cal;
                        L=L*1000;
                    }
                    else L=0;                    
                    queueFloat->queueFrom(L,10);
                    Lends = true; // flag to ends measure
                    Lqueue = true;  
                    tim->start(1);          
                }                     
            }
            if(Lends==true && tim->singleShot) //!< stopping measuring
            {
                tim->singleShot = false;
                GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10); //!< sets to 1 thats block relay coil
                Lends = false;
            }    
            OS::sleep(100);        
        }        
    }
    static bool Lflag;
    static bool Lqueue;
private:   
    bool Lstart = false;  //!< Flag that indicates start of inductance measurement and to switch reed relay
    bool Lends = false;   //!< Flag that indicates about ending of inductance measurement
    OS_timer* tim{nullptr};
    QueueOS<float,1>* queueFloat{nullptr};
    void reed_ports_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        GPIOA->CRH|=GPIO_CRH_MODE8; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF8; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE9; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF9; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE10; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF10; //!< 0:0 push-pull
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10);//!< sets to 1 thats block relay coil
    }
    void reedSwitchL() //!< switch reed relay on L measurement (1:1:0)
    {
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10);//!< sets to 1 thats block relay coil
        GPIOA->BSRR|=(GPIO_BSRR_BR8|GPIO_BSRR_BS9|GPIO_BSRR_BS10);
    }    
};
bool MeasureL::Lflag=false;
bool MeasureL::Lqueue=false;
//-----------------------------------------------------------------------------------------
/*! \brief measuring C task class*/
class MeasureC: public iTaskFR
{
public:
    MeasureC(QueueOS<float,1>* NqueueFloat = nullptr, OS_timer* Ntim=nullptr)
    {tim = Ntim; queueFloat = NqueueFloat, reed_ports_ini();}
    void run() override
    {
        float C=0;
        constexpr float pipi4 = 1.0/(4*3.14*3.14);
        while(1)
        {
            if(Calibration::calEnds && Cflag)
            {
                reedSwitchC(); tim->start(1); Cstart = true; 
                //Calibration::calEnds = false;
            } //!< starts singleshot timer again
            if(tim->singleShot && Cflag && Cstart)
            {                   
                Cstart = false;
                tim->singleShot=false;
                Cflag = false;        
                {
                    if (freq!=0)
                    {
                        float C1 = pipi4*(1.0/(1E-12*freq*freq*Calibration::L_cal)); //!< accounting C
                        C = C1-Calibration::C_cal;
                    }
                    else C=0;
                    queueFloat->queueFrom(C,10);
                    tim->start(1);
                    Cends = true;
                    Cqueue = true;
                } //!< TODO here C must accounting                                
            }
            if(Cends==true && tim->singleShot) //!< stopping measuring
            {
                tim->singleShot = false;
                GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10); //!< sets to 1 thats block relay coil
                Cends = false;
            }
            OS::sleep(100);
        }
    }    
    static bool Cflag;
    static bool Cqueue;
private:
    bool Cstart = false;   //!< Flag that indicates start of capatience measurements and to switch reed relay
    bool Cends = false;    //!< Flag that indicates about ending of capatience measurement
    OS_timer* tim{nullptr};
    QueueOS<float,1>* queueFloat{nullptr};
    void reed_ports_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        GPIOA->CRH|=GPIO_CRH_MODE8; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF8; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE9; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF9; //!< 0:0 push-pull
        GPIOA->CRH|=GPIO_CRH_MODE10; //!< 1:1 50 MHz
        GPIOA->CRH&=~GPIO_CRH_CNF10; //!< 0:0 push-pull
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10); //!< sets to 1 thats block relay coil
    }
    void reedSwitchC() //!< switch reed relay on L measurement (0:0:1)
    {
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BS9|GPIO_BSRR_BS10);//!< sets to 1 thats block relay coil
        GPIOA->BSRR|=(GPIO_BSRR_BS8|GPIO_BSRR_BR9|GPIO_BSRR_BR10);
    }
};
bool MeasureC::Cflag=false;
bool MeasureC::Cqueue=false;
//---------------------------------------------------------------------------------
//!< \brief LCD class that send information on screen
class LCD_FR: public iTaskFR
{
public:
    LCD_FR(QueueOS<float,1>* NqueueFloat=nullptr, OS_timer* nTim=nullptr)
    {queueFloat = NqueueFloat, tim = nTim;}
    ~LCD_FR(){}
    void run() override
    {
        SpiLcd lcd; //!< create object in freeRTOS stack (which in heap)
        lcd.fillScreen(0xff00); //!< yellow
        int x=0;
        Font_16x16 fontSec;  //!< object that draw 16x16 figures      
        tim->start(1);
        float C=0;
        float L=0;
        while(1)
        {
            if(tim->singleShot)
            {
                x++;
                fontSec.intToChar(x);
                fontSec.print(150,220,0x00ff,fontSec.arr,0);
                if(checkFlag==false)
                {
                    tim->singleShot=false;                
                }                
            }    
            if(tim->singleShot && checkFlag) //check button
            {
                fontSec.print(150,200,0xffff,fontSec.arr,0);
                tim->singleShot=false;
                checkFlag = false;                
            } 
            fontSec.intToChar(freq); //!< shows everytime
            fontSec.print(5,10,0x0fff,fontSec.arr,8);
            if(Calibration::calEnds)
            {
                C=Calibration::C_cal;
                Calibration::calEnds = false;
                x++;
                fontSec.intToChar(x);
                fontSec.print(150,220,0x00ff,fontSec.arr,0);
            }
            if(MeasureC::Cqueue)
            {
                queueFloat->queueRecieve(C,1);
                MeasureC::Cqueue = false;
            }
            fontSec.floatTochar(C); //!< shows everytime
            fontSec.print(10,100,0xf00f,fontSec.arrFloat,6);
            if(MeasureL::Lqueue)
            {
                queueFloat->queueRecieve(L,1);
                MeasureL::Lqueue = false;    
            }
            fontSec.floatTochar(L); //!< shows everytime
            fontSec.print(10,150,0x0ff0,fontSec.arrFloat,6);
			Timers::timerSecFlag=false;
            OS::sleep(500);
        }
    }    
    static bool checkFlag;
    OS_timer* tim;
    QueueOS<float,1>* queueFloat{nullptr};  
};
bool LCD_FR::checkFlag=false;
//-----------------------------------------------------------------------------------------------

#endif //FRCLASSES_H_