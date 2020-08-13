/*!	\file starup and initialization file*/
extern "C" void Reset_Handler();//declarations to see it from everywhere
extern "C" void NMI_Handler() __attribute__((weak,alias("Default_Handler"))); 
extern "C" void HardFault_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void MemManage_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void BusFault_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void UsageFault_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void SVC_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DebugMon_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void PendSV_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void SysTick_Handler() __attribute__((weak,alias("Default_Handler")));
extern "C" void WWDG_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void PVD_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TAMPER_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void RTC_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void FLASH_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void RCC_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI0_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI4_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel4_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel5_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel6_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA1_Channel7_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void ADC1_2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN1_TX_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN1_RX0_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN_RX1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN_SCE_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI9_5_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM1_BRK_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM1_UP_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM1_TRG_COM_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM1_CC_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM4_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void I2C1_EV_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void I2C1_ER_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void I2C2_EV_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void I2C2_ER_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void SPI1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void SPI2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void USART1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void USART2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void USART3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void EXTI15_10_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void RTCAlarm_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void OTG_FS_WKUP_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM5_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void SPI3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void UART4_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void UART5_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM6_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void TIM7_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA2_Channel1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA2_Channel2_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA2_Channel3_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA2_Channel4_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void DMA2_Channel5_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void ETH_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void ETH_WKUP_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN2_TX_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN2_RX0_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN2_RX1_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void CAN2_SCE_IRQHandler() __attribute__((weak,alias("Default_Handler")));
extern "C" void OTG_FS_IRQHandler() __attribute__((weak,alias("Default_Handler")));    
extern "C" void Default_Handler();
int main(); //!< declaration main function


extern void* _estack; //start definitions  (start programm)
void (*vectors[])() __attribute__((section(".vectors"))) //section .vectors
{
    

	
		
	
	(void(*)())(&_estack), ////_estack,  //take the address of stack(value) and cast it to function pointer
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,           			/* Window Watchdog interrupt                        */
	PVD_IRQHandler,            			/* PVD through EXTI line detection interrupt        */
	TAMPER_IRQHandler,         			/* Tamper interrupt                                 */
	RTC_IRQHandler,            			/* RTC global interrupt                             */
	FLASH_IRQHandler,          			/* Flash global interrupt                           */
	RCC_IRQHandler,            			/* RCC global interrupt                             */
	EXTI0_IRQHandler,          			/* EXTI Line0 interrupt                             */
	EXTI1_IRQHandler,          			/* EXTI Line1 interrupt                             */
	EXTI2_IRQHandler,          			/* EXTI Line2 interrupt                             */
	EXTI3_IRQHandler,          			/* EXTI Line3 interrupt                             */
	EXTI4_IRQHandler,          			/* EXTI Line4 interrupt                             */
	DMA1_Channel1_IRQHandler,  			/* DMA1 Channel1 global interrupt                   */
	DMA1_Channel2_IRQHandler,  			/* DMA1 Channel2 global interrupt                   */
	DMA1_Channel3_IRQHandler,  			/* DMA1 Channel3 global interrupt                   */
	DMA1_Channel4_IRQHandler,  			/* DMA1 Channel4 global interrupt                   */
	DMA1_Channel5_IRQHandler,  			/* DMA1 Channel5 global interrupt                   */
	DMA1_Channel6_IRQHandler,  			/* DMA1 Channel6 global interrupt                   */
	DMA1_Channel7_IRQHandler,  			/* DMA1 Channel7 global interrupt                   */
	ADC1_2_IRQHandler,         			/* ADC1 and ADC2 global interrupt                   */
	CAN1_TX_IRQHandler,  			/* USB High Priority or CAN TX interrupts           */
	CAN1_RX0_IRQHandler, 			/* USB Low Priority or CAN RX0 interrupts           */
	CAN_RX1_IRQHandler,        			/* CAN RX1 interrupt                                */
	CAN_SCE_IRQHandler,        			/* CAN SCE interrupt                                */
	EXTI9_5_IRQHandler,        			/* EXTI Line[9:5] interrupts                        */
	TIM1_BRK_IRQHandler,       			/* TIM1 Break interrupt                             */
	TIM1_UP_IRQHandler,        			/* TIM1 Update interrupt                            */
	TIM1_TRG_COM_IRQHandler,   			/* TIM1 Trigger and Commutation interrupts          */
	TIM1_CC_IRQHandler,        			/* TIM1 Capture Compare interrupt                   */
	TIM2_IRQHandler,           			/* TIM2 global interrupt                            */
	TIM3_IRQHandler,           			/* TIM3 global interrupt                            */
	TIM4_IRQHandler,           			/* TIM4 global interrupt                            */
	I2C1_EV_IRQHandler,        			/* I2C1 event interrupt                             */
	I2C1_ER_IRQHandler,        			/* I2C1 error interrupt                             */
	I2C2_EV_IRQHandler,        			/* I2C2 event interrupt                             */
	I2C2_ER_IRQHandler,        			/* I2C2 error interrupt                             */
	SPI1_IRQHandler,           			/* SPI1 global interrupt                            */
	SPI2_IRQHandler,           			/* SPI2 global interrupt                            */
	USART1_IRQHandler,         			/* USART1 global interrupt                          */
	USART2_IRQHandler,         			/* USART2 global interrupt                          */
	USART3_IRQHandler,         			/* USART3 global interrupt                          */
	EXTI15_10_IRQHandler,      			/* EXTI Line[15:10] interrupts                      */
	RTCAlarm_IRQHandler,       			/* RTC Alarms through EXTI line interrupt           */
	OTG_FS_WKUP_IRQHandler,             /* USB OTG FS WakeUp from suspend through EXTI Line Interrupt  */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	TIM5_IRQHandler,       			/* TIM8 Break interrupt                             */
	SPI3_IRQHandler,           			/* SPI3 global interrupt                            */
	UART4_IRQHandler,          			/* UART4 global interrupt                           */
	UART5_IRQHandler,          			/* UART5 global interrupt                           */
	TIM6_IRQHandler,           			/* TIM6 global interrupt                            */
	TIM7_IRQHandler,           			/* TIM7 global interrupt                            */
	DMA2_Channel1_IRQHandler,  			/* DMA2 Channel1 global interrupt                   */
	DMA2_Channel2_IRQHandler,  			/* DMA2 Channel2 global interrupt                   */
	DMA2_Channel3_IRQHandler,  			/* DMA2 Channel3 global interrupt                   */
	DMA2_Channel4_IRQHandler,			/* DMA2 Channel4 and DMA2 Channel5 global interrupt */
	DMA2_Channel5_IRQHandler,			/* DMA2 Channel4 and DMA2 Channel5 global interrupt */
	ETH_IRQHandler,     				/*!< Ethernet global Interrupt                            */
  	ETH_WKUP_IRQHandler,     			/*!< Ethernet Wakeup through EXTI line Interrupt          */
  	CAN2_TX_IRQHandler,     			/*!< CAN2 TX Interrupt                                    */
  	CAN2_RX0_IRQHandler,     			/*!< CAN2 RX0 Interrupt                                   */
  	CAN2_RX1_IRQHandler,     			/*!< CAN2 RX1 Interrupt                                   */
  	CAN2_SCE_IRQHandler,	     		/*!< CAN2 SCE Interrupt                                   */
  	OTG_FS_IRQHandler                   /*!< USB OTG FS global Interrupt                          */
};

extern void (*__preinit_array_start []) (void) __attribute__((weak)); //from linker
extern void (*__preinit_array_end []) (void) __attribute__((weak));	//from linker
extern void (*__init_array_start []) (void) __attribute__((weak)); //from linker constructors
extern void (*__init_array_end []) (void) __attribute__((weak)); //from linker	constructors
extern void (*__fini_array_start []) (void) __attribute__((weak)); //from linker destructors
extern void (*__fini_array_end []) (void) __attribute__((weak)); //from linker	destructors
void __attribute__ ((weak)) _init(void)  {} // dummy This section holds executable instructions that contribute to the process initialization code. When a program starts to run, the system arranges to execute the code in this section before calling the main program entry point (called main for C programs). 
void __attribute__ ((weak)) _fini(void)  {} // dummy заглушка This section holds executable instructions that contribute to the process termination code. That is, when a program exits normally, the system arranges to execute the code in this section

/* Iterate over all the init routines.  */
void
__libc_init_array (void) // static initialization constructors function
{
  int count;  int i;
  count = __preinit_array_end - __preinit_array_start; //counts of preinit functions DK what it is
  for (i = 0; i < count; i++)
    __preinit_array_start[i] ();
  _init ();
  count = __init_array_end - __init_array_start; // counts of init constructors
  for (i = 0; i < count; i++)
    __init_array_start[i] ();
}
/* Run all the cleanup routines.  */
void
__libc_fini_array (void) //!< destructors not usefull in microcontrollers
{
  int count;
  int i;  
  count = __fini_array_end - __fini_array_start;
  for (i = count; i > 0; i--)
    __fini_array_start[i-1] ();
  _fini ();
}

extern void* _sidata, *_sdata, *_edata, *_sbss, *_ebss;
void __attribute__((naked,noreturn)) Reset_Handler()    
{
    __asm volatile ("cpsid i"); //turn off interrupts
    void **i, **j;
    for(i=&_sidata,j=&_sdata;j!=&_edata;i++,j++)
    {
        *j=*i; // copy data from ROM (_sidata) to RAM (_sdata)
    }
    for (i=&_sbss;i!=&_ebss;i++) //!< zeroing bss section
    {*i=0;}  
	
	__libc_init_array(); 
    main();    
}
void __attribute__(()) Default_Handler() // naked its with no input and output parameters
{while(1);}
