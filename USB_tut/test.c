/* ���ᠭ�� ॣ���஢ AT89C5131 */
#include "INC\i5131.h"
/* ���ᠭ�� ����⠭� ��� ����㯠 � ॣ���ࠬ */
#include "INC\ext_5131.h"

#include "types.h"
#include "const.h"
#include "reg_macr.h"
#include "descript.h"
#include "test.h"

/* �㭪樨 usb */
#include "usb_util.h"
/* ���ᠭ�� USB ���ਯ�஢ */
#include "usb_enum.h"
/* �㭪樨 cdc */
#include "usb_cdc.h"

#include "usb_func.h"

/* ================================= */
/* MAIN - �᭮���� �㭪�� �ணࠬ�� */
/* ================================= */
void main()
{
 /* �믮����� ���樠������ USB */
 usb_init();
 /* �믮����� ���樠������ CDC */
 cdc_init();
 /* ���ன�⢮ ������祭� � 設� */
 usb_connected = FALSE;

 /* �᭮���� 横� �ணࠬ�� */
 for (;;) 
 {
  main_task();

  /* �᫨ ���ன�⢮ �⪫�祭� �� 設� */
  if (!usb_connected)
  {
    /* �᫨ ����祭 ᨣ��� ���㤪� */
    if (Usb_resume())
    {
      /* ��⠭����� 䫠� ��⨢���� */
      usb_connected = TRUE;
      /* ��� ०��� SUSPEND */
      Usb_clear_suspend_clock();
      Usb_clear_suspend();
      Usb_clear_resume();
      Usb_clear_sof();
    }
  /* �᫨ ���ன�⢮ ������祭� � 設� */
  } else {
   /* �᫨ ����祭 ᨣ��� "���믠���" */
   if (Usb_suspend())
   {
     usb_connected = FALSE;
     Usb_clear_suspend();
     Usb_set_suspend_clock();
   }

   /* �᫨ ����祭 ᨣ��� ��� */
   if (Usb_reset())
   { 
     Usb_clear_reset();
   }

   /* ᨣ��� SOF */
   if (Usb_sof())
   {
    Usb_clear_sof();
   }


   /* �����㦥�� ���뢠��� �� ����筮� �窨 */
   if (Usb_endpoint_interrupt())
   {
    /* ��४������� �� 0 ������� ��� */
    Usb_select_ep(0);

    /* �᫨ ����祭 ����� SETUP */
    if (Usb_setup_received()) 
    { 
     /* ��ࠡ�⪠ ����� setup */
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
   
    /* ����祭�� ������ � 2 ����筮� �窨 */
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
 /* ���ன�⢮ �� ��⮢� � ��।�� ������ */
 if ((usb_configuration_nb == 0) || (Usb_suspend()))
  return;

 if (end_point1_ready == 0) /* �।��騩 ����� ��ࠢ��� */
 {
   Usb_select_ep(1);
   for (i=0; i<32; i++) Usb_write_byte(i);
   Usb_set_tx_ready();
   if (i==32) end_point1_ready= 2; else end_point1_ready= 1;
 }
}
