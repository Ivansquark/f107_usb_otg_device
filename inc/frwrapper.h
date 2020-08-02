/*!
    \file frapper on FreeRTOS for c++
    realized many useful classes: iTaskFR, OS, QueueOS, SemaphorOS, OS_timer     
*/
#ifndef FRWRAPPER_H_
#define FRWRAPPER_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "portmacro.h"
#include "main.h"

/*!
    \brief abstract class for TASKs interface 
    realizes virtual function run() which must override by inherits
*/
class iTaskFR
{
public: 
    iTaskFR(){}
    virtual ~iTaskFR(){}
    virtual void run()=0;
    TaskHandle_t handle=nullptr;   //!< pointer to task control structure of freertos
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
    \brief OS wrapper class
    realizes c++ wrappers on freeRTOS functions
*/
class OS
{
public:
/*************************- task -****************************************************************/
    /*!  \brief static function thats creates tasks 
     *  TODO change void on TaskHandle_t for setting task descriptor   
    */
    static void taskCreate(iTaskFR* obj, const char *pName,const uint16_t stackDepth,uint8_t priority)
    { obj->handle = (xTaskCreate(Run,pName,stackDepth,obj,priority,&obj->handle)==pdTRUE) ? obj->handle : nullptr;} //!< if true return pointer on a task else return nullptr                 
    //{ xTaskCreate(reinterpret_cast<TaskFunction_t>(Run),pName,stackDepth,&obj,priority,NULL);}
    
/*!************************- queue -**************************************************************/    
    /*! \brief Queue API wrapper
    */
    static QueueHandle_t queueCreate(uint8_t length, uint8_t size)
    {return xQueueCreate(length,size);}    
    static bool queueSend(QueueHandle_t &queue, const void *pItemToQueue,uint16_t timeout)
    {return (xQueueSend(queue, pItemToQueue, timeout)==pdTRUE);} //!< оператор == возвращает true, если 1 равно 2, в противном случае - false 
    static bool queueSendISR(QueueHandle_t &queue, const void *pItemToQueue)
    {return (xQueueSendFromISR(queue, pItemToQueue, nullptr)==pdTRUE);}
    static bool queueRecieve(QueueHandle_t &queue,void* pItem, uint32_t timeout)
    {return (xQueueReceive(queue,pItem,timeout)==pdTRUE);}
    static void queueDelete(QueueHandle_t &queue){vQueueDelete(queue);}
/*!*******************************- semaphore -*************************************************/
    static SemaphoreHandle_t semaphoreCreate()
    {return xSemaphoreCreateBinary();}
    static void semFromIsr(SemaphoreHandle_t handle)
    {xSemaphoreGiveFromISR(handle,nullptr);}
    static bool semRecieve(SemaphoreHandle_t handle, uint16_t timeout)
    {return xSemaphoreTake(handle, timeout);}
    static void deleteSemaphore(SemaphoreHandle_t handle)
    {vSemaphoreDelete(handle);}
/*!*******************************- critical section -********************************************/
    static void criticalSectionEnter()
    {taskENTER_CRITICAL();} // prohibition interrupts and sysTick    
    static void criticalSectionLeave()
    {taskEXIT_CRITICAL();}
    static void criticalSectionEnterISR()
    {taskENTER_CRITICAL_FROM_ISR(); vTaskSuspendAll();}
    static void criticalSectionLeaveISR(uint32_t x)
    {taskEXIT_CRITICAL_FROM_ISR(x);}
    static void stopScheduller(){vTaskSuspendAll();}
    static void newStartScheduller(){xTaskResumeAll();}
/*!*******************************- scheduller -**************************************************/
    static void startScheduler()
    {vTaskStartScheduler();}
    static void sleep(uint16_t x){vTaskDelay(x);}
private:
    //!static TaskFunction_t Run(void* x)  == void(*Run)(void*)    function pointer
    static void Run(void* x)  // set object here (callback function)    
    {static_cast<iTaskFR*>(x)->run();}    
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
    \brief template class for freeRTOS queue 
    because in queue we can send structure or object that has any size
*/
template<typename T,uint16_t length>
class QueueOS
{
public:
    QueueOS(){queueHandle = OS::queueCreate(length,sizeof(T));} //create queue with creation class object
    ~QueueOS(){OS::queueDelete(queueHandle);}
    bool queueFrom(const T &item,uint16_t timeout)
    {
        const void* x = reinterpret_cast<const void*>(&item);
        return OS::queueSend(queueHandle,x,timeout);
    }
    bool queueFromIsr(const T &item){return OS::queueSendISR(queueHandle,reinterpret_cast<void*>(item));}
    bool queueRecieve(T &item, uint32_t timeout){return OS::queueRecieve(queueHandle,&item,timeout);}
private:
    QueueHandle_t queueHandle; //указатель на структуру очереди
};
class SemaphoreOS
{
public:
    SemaphoreOS(){semHandle=OS::semaphoreCreate();}
    ~SemaphoreOS(){OS::deleteSemaphore(semHandle);}
    void giveFromIsr(){OS::semFromIsr(semHandle);}
    bool takeSem(uint16_t timeout){return OS::semRecieve(semHandle,timeout);}
private:
    SemaphoreHandle_t semHandle;
    //StaticSemaphore_t mutex;
};

/*!
    \brief abstract FreeRTOS wrapper timer class
    inherit class must override run() - callback function
*/
class OS_timer
{
public:
    OS_timer(const char* timName, const TickType_t period,const UBaseType_t AutoReload,
            void * const pvTimerID) //! period = 500/portTICK_RATE_MS 
    {
        //! AutoReload - pdTRUE - периодический, pdFalse - интервальный
        xTimer = xTimerCreate(timName, period/portTICK_RATE_MS, AutoReload, pvTimerID, timerCallBack); 
        timer = this;         
    }
    bool start(portTickType xBlockTime)
    {
        return (xTimerStart(timer->xTimer,xBlockTime)==pdTRUE); //        
    }
    static bool stop(portTickType xBlockTime)
    {
        return (xTimerStop(timer->xTimer,xBlockTime)==pdTRUE);
    }
    bool reset(portTickType xBlockTime)
    {
        return xTimerReset(timer->xTimer, xBlockTime) == pdFALSE ? false : true;
    }
    bool singleShot{false};
protected:
    virtual void run()=0; //! abstract function that realizes callback, must be initialized in inheritor
private:    
    xTimerHandle xTimer;//{nullptr}; //! pointer on a structure
    static void timerCallBack(xTimerHandle handle)
    {       
        timer->run();
        return;
    }
    static OS_timer* timer;    
};
OS_timer* OS_timer::timer=nullptr;
/*******************************************************************************************/

#endif //FRWRAPPER_H_
