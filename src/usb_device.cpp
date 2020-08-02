#include "usb_device.hpp"

USB_DEVICE::USB_DEVICE()
{pThis=this;fifo_init(); usb_init();}

void USB_DEVICE::Enumerate_Setup(void)               
{   
  //USB_DEVICE::pThis->resetFlag++;  
  uint16_t len=0;
  uint8_t *pbuf; 
  switch(uSetReq.bRequest)
  {    
    case STD_GET_DESCRIPTOR:
      switch(uSetReq.wValue)
      {
        case USB_DESC_TYPE_DEVICE:   //Запрос дескриптора устройства
          len = sizeof(Device_Descriptor);
          pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.
          break;
        case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
          len = sizeof(Config_Descriptor);
          pbuf = (uint8_t *)Config_Descriptor;
          break;   
		  //Device Qualifier Descriptor (уточняющий дескриптор устройства) — содержит дополнительную информацию об устройстве, для его работы на другой скорости.
        case USB_DESC_TYPE_INTERFACE:  //Запрос дескриптора USB_DESC_TYPE_INTERFACE
          len = sizeof(Interface_Descriptor);
          pbuf = (uint8_t *)Interface_Descriptor;             
          break;    
        case USB_DESC_TYPE_EP_DESCRIPTOR:  //Запрос дескриптора USB_DESC_TYPE_INTERFACE
          len = sizeof(EP1_In_Descriptor);
          pbuf = (uint8_t *)EP1_In_Descriptor;             
          break;
               
        //case USBD_IDX_LANGID_STR: //Запрос строкового дескриптора
        //  len = sizeof(StringLangID);
        //  pbuf = (uint8_t *)StringLangID;                   
        //  break;
        //case USBD_IDX_MFC_STR: //Запрос строкового дескриптора
        //  len = sizeof(StringVendor);
        //  pbuf = (uint8_t *)StringVendor;                             
        //  break;
        //case USBD_IDX_PRODUCT_STR: //Запрос строкового дескриптора
        //  len = sizeof(StringProduct);
        //  pbuf = (uint8_t *)StringProduct;         
        //  break;                     
        //case USBD_IDX_SERIAL_STR: //Запрос строкового дескриптора
        //  len = sizeof(StringSerial);
        //  pbuf = (uint8_t *)StringSerial;                             
        //  break;
        //case USBD_IDX_CONFIG_STR:
        //  len = sizeof(StringConfig);
        //  pbuf = (uint8_t *)StringConfig;
        //  break;
        //case USBD_IDX_INTERFACE_STR:
        //  len = sizeof(StringInterface);
        //  pbuf = (uint8_t *)StringInterface;
        //  break;
//Device Qualifier Descriptor (уточняющий дескриптор устройства) — содержит дополнительную информацию об устройстве, для его работы на другой скорости.
        
         //... И так далее
      }
      break;
    case STD_SET_ADDRESS:  // Установка адреса устройства
      //resetFlag=uSetReq.wValue;
      SetAdr((uSetReq.wValue));
      break;
    case STD_SET_CONFIGURATION: // Установка конфигурации устройства
      Set_CurrentConfiguration((uSetReq.wValue>>4));
      break;       // len-0 -> ZLP

      // ... И так далее
  } 

  WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));   // записываем в конечную точку адрес дескриптора и его размер (а также запрошенный размер)
}

void USB_DEVICE::usb_init()
    {
        //! инициализация переферии PA11-DM PA12-DP
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        GPIOA->CRH &=~ (GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_0 );
        GPIOA->CRH |= (GPIO_CRH_CNF11_1 | GPIO_CRH_CNF12_1 ); //! 1:0 - alternative function push-pull
        GPIOA->CRH |= (GPIO_CRH_MODE11 | GPIO_CRH_MODE12 ); //! 1:1 full speed

        GPIOA->CRH &=~ (GPIO_CRH_CNF9_1);
        GPIOA->CRH |= (GPIO_CRH_CNF9_0 ); // 0:1 input mode (reset state)
        GPIOA->CRH &=~ (GPIO_CRH_MODE9);
        //! тактирование USB
        RCC->CFGR &=~ RCC_CFGR_OTGFSPRE; //! 0 - psk=3; (72*2/3 = 48 MHz)
        RCC->AHBENR|=RCC_AHBENR_OTGFSEN; //USB OTG clock enable
        USB_OTG_FS->GINTSTS=0xFFFFFFFF; //rc_w1 read_and_clear_write_1 очистить регистр статуса
        // core
        
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_GINT; // globalk interrupt mask 1: отмена маскирования прерываний для приложения.
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_TXFELVL; //1: прерывание бита TXFE (находится в регистре OTG_FS_DIEPINTx) показывает, что IN Endpoint TxFIFO полностью пуст.
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_PTXFELVL; //1: прерывание бита NPTXFE (находится в регистре OTG_FS_GINTSTS) показывает, что непериодический TxFIFO полностью пуст.
        //USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_HNPCAP; // HNP Бит разрешения функции смены ролей хост-устройство (HNP capable bit).
        //USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_SRPCAP; // SRP Бит разрешения управления питанием порта USB (SRP capable bit).
         // FS timeout calibration Приложение должно запрограммировать это поле 
         //на основе скорости энумерации.
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_TOCAL_2|USB_OTG_GUSBCFG_TOCAL_0;
        USB_OTG_FS->GUSBCFG &=~ USB_OTG_GUSBCFG_TOCAL_1;
        // USB turnaround time Диапазон частот AHB	TRDT 32	-	0x6
        USB_OTG_FS->GUSBCFG |= (USB_OTG_GUSBCFG_TRDT_2|USB_OTG_GUSBCFG_TRDT_1);
        USB_OTG_FS->GUSBCFG &=~ (USB_OTG_GUSBCFG_TRDT_3|USB_OTG_GUSBCFG_TRDT_0); 
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_OTGINT; // unmask OTG interrupt OTG INTerrupt mask.         
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_MMISM; //прерывания ошибочного доступа 
        // device
        USB_OTG_DEVICE->DCFG |= USB_OTG_DCFG_DSPD; //1:1 device speed Скорость устройства 48 MHz
        USB_OTG_DEVICE->DCFG &=~ USB_OTG_DCFG_NZLSOHSK; //0: отправляется приложению принятый пакет OUT (нулевой или ненулевой длины) и отправляется handshake на основе бит NAK и STALL 
        // ~ non-zero-length status OUT handshake
        USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST; // unmusk USB reset interrupt Сброс по шине 
        USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ESUSPM; // unmask early suspend interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM; // unmask USB suspend interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_SOFM; // unmask SOF interrupt
        //включаем подтягивающий резистор DP вернее сенсор VbusB
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_FDMOD;//force device mode
        for(uint32_t i=0; i<1000000;i++){}//wait 25 ms

        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_PWRDWN; 
        //USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // сбрасываем бит       
        NVIC_SetPriority(OTG_FS_IRQn,1);
        NVIC_EnableIRQ(OTG_FS_IRQn);
        USB_OTG_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;   //Подключить USB  
        USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		    //Сбросить RXFIFO
		    USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
		    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH); // сбрасываем Tx и Rx FIFO	
		    USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt        
}

void USB_DEVICE::fifo_init()
{
    /*! < Rx_size 32-битных слов. Минимальное значение этого поля 16, максимальное 256 >*/
    USB_OTG_FS->GRXFSIZ = RX_FIFO_SIZE; 
    /*! < размер и адрес Tx_FIFO (конец Rx_FIFO) для EP0 >*/
	  USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (TX_EP0_FIFO_SIZE<<16) | RX_FIFO_SIZE; 
	  //! IN endpoint transmit fifo size register
	  USB_OTG_FS->DIEPTXF[0] = (TX_EP1_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE);  //!размер и адрес Tx_FIFO  для EP1
	  USB_OTG_FS->DIEPTXF[1] = (TX_EP2_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE+TX_EP1_FIFO_SIZE); //!размер и адрес Tx_FIFO  для EP2
	  USB_OTG_FS->DIEPTXF[2] = (TX_EP3_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE+TX_EP1_FIFO_SIZE+TX_EP2_FIFO_SIZE); //! размер и адрес Tx_FIFO  для EP3
	  // 3 пакета SETUP, CNT=1, endpoint 0 OUT transfer size register
	  USB_OTG_OUT(0)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT | USB_OTG_DOEPTSIZ_PKTCNT) ; //STUPCNT 1:1 = 3
	  // XFRSIZE = 64 - размер транзакции в байтах
	  USB_OTG_OUT(0)->DOEPTSIZ |= 64;//0x40
}

void USB_DEVICE::SetAdr(uint16_t value)
{
    ADDRESS=value;
    USB_OTG_DEVICE->DCFG |= value<<4; //запись адреса.    
    USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
    USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); 
}
void USB_DEVICE::Set_CurrentConfiguration(uint16_t value)
{
    CurrentConfiguration=value;
}
void USB_DEVICE::WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen)
{
    switch(EPnum)
    {
        case 00: WriteFIFO(0, buf, minLen);break;
        case 01: WriteFIFO(1, buf, minLen);break;
        case 02: WriteFIFO(2, buf, minLen);break;
        case 03: WriteFIFO(3, buf, minLen);break;
    }
}
uint16_t USB_DEVICE::MIN(uint16_t len, uint16_t wLength)
{
    uint16_t x=0;
     (len<wLength) ? x=len : x=wLength;
     return x;
}
void USB_DEVICE::WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len)
{
    //resetFlag = 1000;
    uint32_t words2write = (len+3)>>2; // делим на два
    for (uint32_t index = 0; index < words2write; index++, src += 4)
    {
        /*!<закидываем в fifo 32-битные слова>*/
        USB_OTG_DFIFO(fifo_num) = *((__packed uint32_t *)src);
        //resetFlag++; 
    }
}

void USB_DEVICE::ReadSetupFIFO(void)
{  
  //Прочитать SETUP пакет из FIFO, он всегда 8 байт
  *(uint32_t *)&uSetReq = USB_OTG_DFIFO(0);  //! берем адрес структуры, приводим его к указателю на адресное поле STM32, разыменовываем и кладем туда адрес FIFO_0
  // тем самым считывается первые 4 байта из Rx_FIFO
  *(((uint32_t *)&uSetReq)+1) = USB_OTG_DFIFO(0); // заполняем вторую часть структуры (очень мудрено сделано)
	USB_DEVICE::bmRequestType=uSetReq.bmRequestType;
  USB_DEVICE::bRequest=uSetReq.bRequest;
  USB_DEVICE::wValue = uSetReq.wValue;  
  USB_DEVICE::wIndex = uSetReq.wIndex;
  USB_DEVICE::wLength = uSetReq.wLength;  
}