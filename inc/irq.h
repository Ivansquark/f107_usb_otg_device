#ifndef IRQ_H_
#define IRQ_H_

#include "main.h"
/*!
    \brief class initialized static array of function pointers that wil be bined with interruptHandlers 
*/

class InterruptManager
{
public:
    static void (*IsrV[88])(); //!static array of function pointers
    static void addHandler(void (*fPtr)(),IRQn irq){IsrV[irq]=fPtr;} //set callback
    static void callIrqFunc(IRQn irq){IsrV[irq]();} // call function
};

/*!
    \brief pure abstract class 
    inherits from that class will have interrupt functionality
*/
class Interruptable
{
public:
    virtual ~Interruptable(){}
    virtual void run()=0;
}; 
/*!
    \brief template class that manage inherits of Interruptable class
*/
template<IRQn irq>
class InterruptableClass
{
public:
    static void setVector(){InterruptManager::addHandler(irqHandle,irq);} //!set method on irq (interrupt request)
    static void addClass(Interruptable* cl) //!add classes ptrs into array that will be called on irq
    {
        for(uint8_t i=0;i<NumClasses;i++)
        {
            if(arrOfInterruptableClasses[i]==nullptr)
            {
                arrOfInterruptableClasses[i]=cl;
                break;
            }
        }
    }
    static void irqHandle() //! call objects method run()
    {
        for(uint8_t i=0;i<NumClasses;i++)
        {
            if(arrOfInterruptableClasses[i]!=nullptr)
            {
                arrOfInterruptableClasses[i]->run();                
            }
        }
    }
private:
    static constexpr uint8_t NumClasses=10;
    static Interruptable* arrOfInterruptableClasses[NumClasses];
};
template<IRQn irq>
Interruptable* InterruptableClass<irq>::arrOfInterruptableClasses[NumClasses];

#endif //IRQ_H_