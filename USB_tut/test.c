/* описание регистров AT89C5131 */
#include "INC\i5131.h"
/* описание констант для доступа к регистрам */
#include "INC\ext_5131.h"

#include "types.h"
#include "const.h"
#include "reg_macr.h"
#include "descript.h"
#include "test.h"

/* функции usb */
#include "usb_util.h"
/* описание USB дескрипторов */
#include "usb_enum.h"
/* функции cdc */
#include "usb_cdc.h"

#include "usb_func.h"

/* ================================= */
/* MAIN - основная функция программы */
/* ================================= */
void main()
{
 /* выполнить инициализацию USB */
 usb_init();
 /* выполнить инициализацию CDC */
 cdc_init();
 /* устройство подключено к шине */
 usb_connected = FALSE;

 /* основной цикл программы */
 for (;;) 
 {
  main_task();

  /* если устройство отключено от шины */
  if (!usb_connected)
  {
    /* если получен сигнал побудки */
    if (Usb_resume())
    {
      /* установить флаг активности */
      usb_connected = TRUE;
      /* сброс режима SUSPEND */
      Usb_clear_suspend_clock();
      Usb_clear_suspend();
      Usb_clear_resume();
      Usb_clear_sof();
    }
  /* если устройство подключено к шине */
  } else {
   /* если получен сигнал "засыпания" */
   if (Usb_suspend())
   {
     usb_connected = FALSE;
     Usb_clear_suspend();
     Usb_set_suspend_clock();
   }

   /* если получен сигнал сброса */
   if (Usb_reset())
   { 
     Usb_clear_reset();
   }

   /* сигнал SOF */
   if (Usb_sof())
   {
    Usb_clear_sof();
   }


   /* обнаружено прерывание от конечной точки */
   if (Usb_endpoint_interrupt())
   {
    /* переключиться на 0 конечную точку */
    Usb_select_ep(0);

    /* если получен пакет SETUP */
    if (Usb_setup_received()) 
    { 
     /* обработка пакета setup */
     usb_control_packet_processed(); 
    }

    Usb_select_ep(1);
    switch (end_point1_ready)
    {
     case 1:
     {
       if (Usb_tx_complete()) 
       {
        Usb_clear_tx_complete();
        Usb_set_tx_ready();
        end_point1_ready = 2;
       }
       break;
     }
     case 2:
     {
       if (Usb_tx_complete()) 
       {
        Usb_clear_tx_complete();
        end_point1_ready = 0;
       }
       break;
     }
    }


    Usb_select_ep(3);
    if (Usb_tx_complete())  
    {
      Usb_clear_tx_complete(); 
    }
   
    /* получение данных со 2 конечной точки */
    /* BULK OUT */
    Usb_select_ep(2);
    bcount = UBYCTLX;
    if (bcount > 0)
    {
     for (i=0; i<bcount;i++)
     {
       Usb_read_byte();
     }
    }
    Usb_clear_rx();


   }/* if interrupt */

  } /* end if connected */

  SendRS232(end_point1_ready);
  P3.3=p_test; p_test=!p_test;

 } /* end for ;; */
}


void SendRS232(char ch)
{
 while(!TI);
 TI=0;
 SBUF = ch;
 while(TI);
}

void main_task()
{
 /* устройство не готово к передаче данных */
 if ((usb_configuration_nb == 0) || (Usb_suspend()))
  return;

 if (end_point1_ready == 0) /* предыдущий пакет отправлен */
 {
   Usb_select_ep(1);
   for (i=0; i<32; i++) Usb_write_byte(i);
   Usb_set_tx_ready();
   if (i==32) end_point1_ready= 2; else end_point1_ready= 1;
 }
}
