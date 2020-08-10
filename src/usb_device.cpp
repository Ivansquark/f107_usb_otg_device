#include "usb_device.hpp"
#include "usb_descriptors.hpp"

USB_DEVICE::USB_DEVICE()
{pThis=this;fifo_init(); usb_init();}

void USB_DEVICE::usb_init()
    {
        //! инициализация переферии PA11-DM PA12-DP
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        GPIOA->CRH &=~ (GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_0 );
        GPIOA->CRH |= (GPIO_CRH_CNF11_1 | GPIO_CRH_CNF12_1 )|(GPIO_CRH_MODE11 | GPIO_CRH_MODE12 ); //! 1:0 - alternative function push-pull 1:1 full speed
        GPIOA->CRH &=~ (GPIO_CRH_CNF9_1);
        GPIOA->CRH |= (GPIO_CRH_CNF9_0 ); // 0:1 input mode (reset state)
        GPIOA->CRH &=~ (GPIO_CRH_MODE9);
        //! тактирование USB
        RCC->CFGR &=~ RCC_CFGR_OTGFSPRE; //! 0 - psk=3; (72*2/3 = 48 MHz)
        RCC->AHBENR|=RCC_AHBENR_OTGFSEN; //USB OTG clock enable
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
        USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM; // unmusk USB reset interrupt Сброс по шине // unmask enumeration done interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ESUSPM; // unmask early suspend interrupt //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM; // unmask USB suspend interrupt         //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_SOFM; // unmask SOF interrupt
        USB_OTG_DEVICE->DCTL = USB_OTG_DCTL_SDIS;  //Отключиться
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_FDMOD;//force device mode
        for(uint32_t i=0; i<1800000;i++){}//wait 25 ms
		//включаем подтягивающий резистор DP вернее сенсор VbusB по которому включится резистор при наличии 5 В на Vbus
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_PWRDWN; 
        //USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // сбрасываем бит  
		USB_OTG_FS->GINTSTS=0xFFFFFFFF; //rc_w1 read_and_clear_write_1 очистить регистр статуса		
        NVIC_SetPriority(OTG_FS_IRQn,1);
        NVIC_EnableIRQ(OTG_FS_IRQn);
        USB_OTG_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;   //Подключить USB         
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

void USB_DEVICE::Enumerate_Setup(void)               
{   
  //USB_DEVICE::pThis->resetFlag++;  
  uint16_t len=0;
  uint8_t *pbuf; 
  switch(swap(setupPack.wRequest))
  {    
    case GET_DESCRIPTOR_DEVICE:        
      switch(setupPack.setup.wValue)
      {        
        case USB_DESC_TYPE_DEVICE:   //Запрос дескриптора устройства
        USART_debug::usart2_sendSTR("DEVICE DESCRIPTER\n");
        //counter++;
          len = sizeof(Device_Descriptor);
          pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.
          break;
        case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
        USART_debug::usart2_sendSTR("CONFIGURATION DESCRIPTER\n");
          len = sizeof(confDescr);
          pbuf = (uint8_t *)&confDescr;
          break;   
		  
        //case USB_DESC_TYPE_INTERFACE1:  //Запрос дескриптора USB_DESC_TYPE_INTERFACE
        //USART_debug::usart2_sendSTR("INTERFACE DESCRIPTER\n");
        //  len = sizeof(Interface_Descriptor1);
        //  pbuf = (uint8_t *)Interface_Descriptor1;             
        //  break;    
        //case USB_DESC_TYPE_EP_DESCRIPTOR1:  //Запрос дескриптора USB_DESC_TYPE_INTERFACE
        //  USART_debug::usart2_sendSTR("EP DESCRIPTER1_IN\n");
        //  len = sizeof(EP1_In_Descriptor);
        //  pbuf = (uint8_t *)EP1_In_Descriptor;             
        //  break;
        //case USB_DESC_TYPE_EP_DESCRIPTOR2:  //Запрос дескриптора USB_DESC_TYPE_INTERFACE
        //  USART_debug::usart2_sendSTR("EP DESCRIPTER1_OUT\n");
        //  len = sizeof(EP1_OUT_Descriptor);
        //  pbuf = (uint8_t *)EP1_OUT_Descriptor;             
        //  break;  
               
        case USBD_IDX_LANGID_STR: //Запрос строкового дескриптора
        USART_debug::usart2_sendSTR("USBD_IDX_LANGID_STR\n");
          len = sizeof(LANG_ID_Descriptor);
          pbuf = (uint8_t *)LANG_ID_Descriptor;                   
          break;
        case USBD_strManufacturer: //Запрос строкового дескриптора
        USART_debug::usart2_sendSTR("USBD_strManufacturer\n");
          len = sizeof(Man_String);
          pbuf = (uint8_t *)Man_String;                             
          break;
        case USBD_strProduct: //Запрос строкового дескриптора
         USART_debug::usart2_sendSTR("USBD_strProduct\n");
          len = sizeof(Prod_String);
          pbuf = (uint8_t *)Prod_String;         
          break;                     
        case USBD_IDX_SERIAL_STR: //Запрос строкового дескриптора
        USART_debug::usart2_sendSTR("USBD_IDX_SERIAL_STR\n");
          len = sizeof(SN_String);
          pbuf = (uint8_t *)SN_String;    
          break;
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
    case SET_ADDRESS:  // Установка адреса устройства
      //resetFlag=uSetReq.wValue;
	  addressFlag = true;
    /*!< записываем пакет статуса нулевой длины >*/
    //WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    break;
	case GET_CONFIGURATION:
		/*Устройство передает один байт, содержащий код конфигурации устройства*/
		pbuf=(uint8_t*)&confDescr+5; //номер конфигурации (единственной)
		len=1;
    USART_debug::usart2_sendSTR("GET_CONFIGURATION\n");
	break;
    case SET_CONFIGURATION: // Установка конфигурации устройства
	/*<здесь производится конфигурация конечных точек в соответствии с принятой конфигурацией (она одна)>*/
      Set_CurrentConfiguration((setupPack.setup.wValue>>4));
	    ep_1_2_init(); //инициализируем конечные точки 1-прием, передача и 2-настройка    
      USART_debug::usart2_sendSTR("SET_CONFIGURATION\n");
      break;       // len-0 -> ZLP
	  
	/* CDC Specific requests */
    case SET_LINE_CODING:
    USART_debug::usart2_sendSTR("SET_LINE_CODING\n");
      cdc_set_line_coding();           
      break;
    case GET_LINE_CODING:
    USART_debug::usart2_sendSTR("GET_LINE_CODING\n");
      cdc_get_line_coding();           
      break;
    case SET_CONTROL_LINE_STATE:
    USART_debug::usart2_sendSTR("SET_CONTROL_LINE_STATE\n");
      cdc_set_control_line_state();    
      break;
    case SEND_BREAK:
    USART_debug::usart2_sendSTR("SEND_BREAK\n");
      cdc_send_break();                
      break;
    case SEND_ENCAPSULATED_COMMAND:
    USART_debug::usart2_sendSTR("SEND_ENCAPSULATED_COMMAND\n");
      cdc_send_encapsulated_command(); 
      break;
    case GET_ENCAPSULATED_RESPONSE:
    USART_debug::usart2_sendSTR("GET_ENCAPSULATED_RESPONSE\n");
      cdc_get_encapsulated_command();  
      break;
	
	default: stall();break;

      // ... И так далее
  }   
  WriteINEP(0x00,pbuf,MIN(len, setupPack.setup.wLength));   // записываем в конечную точку адрес дескриптора и его размер (а также запрошенный размер)
}

void USB_DEVICE::SetAdr(uint16_t value)
{  
    ADDRESS=value;
	  addressFlag = false;	  
    uint32_t add = value<<4;
    USB_OTG_DEVICE->DCFG |= add; //запись адреса.    
    //USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT;
    USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
    
    USART_debug::usart2_sendSTR("ADDRESS\n");
	// необходимо выставить подтверждение принятия пакета выставления адреса 
	// IN status packet  (sendInZeroPacket)
}
void USB_DEVICE::Set_CurrentConfiguration(uint16_t value)
{
    CurrentConfiguration=value;
}
void USB_DEVICE::WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen)
{
  /*!<TODO: реализовать запись через очередь>*/
  USB_OTG_IN(EPnum)->DIEPTSIZ =0;
  /*!<записать количество пакетов и размер посылки>*/
  uint8_t Pcnt = minLen/64 +1;  
  USB_OTG_IN(EPnum)->DIEPTSIZ |= (Pcnt<<19);
  USB_OTG_IN(EPnum)->DIEPTSIZ |= minLen;
   /*!<количество передаваемых пакетов (по прерыванию USB_OTG_DIEPINT_XFRC передается один пакет)>*/
  USB_OTG_IN(EPnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); //выставляем перед записью
  if(minLen) WriteFIFO(EPnum, buf, minLen); //если нет байтов передаем пустой пакет    
}
uint16_t USB_DEVICE::MIN(uint16_t len, uint16_t wLength)
{
    uint16_t x=0;
    (len<wLength) ? x=len : x=wLength;
    return x;
}
void USB_DEVICE::WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len)
{
    uint32_t words2write = (len+3)>>2; // делим на четыре    
    for (uint32_t index = 0; index < words2write; index++, src += 4)
    {
        /*!<закидываем в fifo 32-битные слова>*/
        USB_OTG_DFIFO(fifo_num) = *((__packed uint32_t *)src);                
    }
    USART_debug::usart2_sendSTR("WRITE in EP0\n");    
}

void USB_DEVICE::ReadSetupFIFO(void)
{  
  //Прочитать SETUP пакет из FIFO, он всегда 8 байт
  *(uint32_t *)&setupPack = USB_OTG_DFIFO(0);  //! берем адрес структуры, приводим его к указателю на адресное поле STM32, разыменовываем и кладем туда адрес FIFO_0
  // тем самым считывается первые 4 байта из Rx_FIFO
  *(((uint32_t *)&setupPack)+1) = USB_OTG_DFIFO(0); // заполняем вторую часть структуры (очень мудрено сделано)	
  //USART_debug::usart2_send(setupPack.b[0]);USART_debug::usart2_send(setupPack.b[1]);
  //USART_debug::usart2_send(setupPack.b[2]);USART_debug::usart2_send(setupPack.b[3]);
  //USART_debug::usart2_send(setPack.b[4]);USART_debug::usart2_send(setPack.b[5]);
  //USART_debug::usart2_send(setPack.b[6]);USART_debug::usart2_send(setPack.b[7]);

}
void USB_DEVICE::ep_1_2_init()
{  
  /*!<EP1_IN, EP1_OUT - BULK, EP2_IN - INTERRUPT>*/
  USB_OTG_IN(1)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(1)->DIEPCTL&=~USB_OTG_DIEPCTL_EPTYP_0; //1:0 - BULK
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_1 0:0:0:1
  USB_OTG_OUT(1)->DOEPCTL|=64;// 64 байта в пакете
  USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_EPTYP_1;
  USB_OTG_OUT(1)->DOEPCTL&=~USB_OTG_DOEPCTL_EPTYP_0; //1:0 - BULK 
  //------------------------------------------------------------------
  USB_OTG_IN(2)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_0; //1:1 - INTERRUPT
  USB_OTG_IN(2)->DIEPCTL&=~USB_OTG_DIEPCTL_TXFNUM_1;
  USB_OTG_IN(2)->DIEPCTL&=~USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_2 0:0:1:0

  USB_OTG_OUT(1)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT | USB_OTG_DOEPTSIZ_PKTCNT) ; //STUPCNT 1:1 = 3
	  // XFRSIZE = 64 - размер транзакции в байтах
	USB_OTG_OUT(1)->DOEPTSIZ |= 64;//0x40 
//-------------------------------------------------------
/*< Заполняем массив line_code>*/	
	for(uint8_t i=0;i<7;i++){line_code[i] = line_coding[i];}
}

void USB_DEVICE::stall()
{
	/*TODO: send STALL signal*/
}

void USB_DEVICE::cdc_set_line_coding()
{
	uint8_t lineC[8];	
	*(uint32_t*)(lineC) = USB_OTG_DFIFO(0);
	*((uint32_t*)(lineC)+1) = USB_OTG_DFIFO(0); //заполнили структуру
	for(uint8_t i=0;i<7;i++)
	{line_code[i] = *((uint8_t*)(&lineC)+i);} //это если из FIFO читается подряд (если нет надо по другому)		
  USART_debug::usart2_sendSTR("line_code \n");
}
void USB_DEVICE::cdc_get_line_coding()
{
	uint8_t* buf; 
	buf=(uint8_t*)&line_code;
	WriteINEP(0,buf,7);
}

void USB_DEVICE::read_BULK_FIFO(uint8_t size)
{
	//uint8_t cnt = USB_OTG_OUT(1)->DOEPTSIZ & USB_OTG_DOEPTSIZ_PKTCNT; //считываем количество принятых пакетов (должен быть один)
	//uint8_t size = USB_OTG_OUT(1)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ;
	
}

void USB_DEVICE::cdc_set_control_line_state()
{}
void USB_DEVICE::cdc_send_break()
{}
void USB_DEVICE::cdc_send_encapsulated_command()
{}
void USB_DEVICE::cdc_get_encapsulated_command()
{}