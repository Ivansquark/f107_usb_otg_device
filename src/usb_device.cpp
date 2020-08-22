#include "usb_device.hpp"
#include "usb_descriptors.hpp"

USB_DEVICE::USB_DEVICE()
{pThis=this; usb_init();} //fifo_init();

void USB_DEVICE::usb_init()
    {
        //! -------- инициализация переферии PA11-DM PA12-DP PA9-VBusSens --------------------
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        GPIOA->CRH &=~ (GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_0 );
        GPIOA->CRH |= (GPIO_CRH_CNF11_1 | GPIO_CRH_CNF12_1 )|(GPIO_CRH_MODE11 | GPIO_CRH_MODE12 ); // 1:0 - alternative function push-pull 1:1 full speed
        GPIOA->CRH &=~ (GPIO_CRH_CNF9_1);
        GPIOA->CRH |= (GPIO_CRH_CNF9_0 ); // 0:1 input mode (reset state)
        GPIOA->CRH &=~ (GPIO_CRH_MODE9);
        //! --------- тактирование USB --------------------------------------------------------------------
        RCC->CFGR &=~ RCC_CFGR_OTGFSPRE; //! 0 - psk=3; (72*2/3 = 48 MHz)
        RCC->AHBENR|=RCC_AHBENR_OTGFSEN; //USB OTG clock enable
        // core        
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_GINT; // globalk interrupt mask 1: отмена маскирования прерываний для приложения.
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_TXFELVL; //1: прерывание бита TXFE (находится в регистре OTG_FS_DIEPINTx) показывает, что IN Endpoint TxFIFO полностью пуст.
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_PTXFELVL; //1: прерывание когда, непериодический TxFIFO полностью пуст.
        //USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_SRPCAP; // SRP Бит разрешения управления питанием порта USB (SRP capable bit).
        // FS timeout calibration Приложение должно запрограммировать это поле на основе скорости энумерации.
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_TOCAL_2;
        USB_OTG_FS->GUSBCFG &=~ USB_OTG_GUSBCFG_TOCAL_1|USB_OTG_GUSBCFG_TOCAL_0; //1:0:0 - 4 /0.25 = 16 интервалов бит
        // USB turnaround time Диапазон частот AHB	TRDT 32	-	0x6
        USB_OTG_FS->GUSBCFG |= (USB_OTG_GUSBCFG_TRDT_2|USB_OTG_GUSBCFG_TRDT_1);
        USB_OTG_FS->GUSBCFG &=~ (USB_OTG_GUSBCFG_TRDT_3|USB_OTG_GUSBCFG_TRDT_0); //0:1:1:0 = 6
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_OTGINT; // unmask OTG interrupt OTG INTerrupt mask.         
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_MMISM; //прерывания ошибочного доступа 
        // device
        USB_OTG_DEVICE->DCFG |= USB_OTG_DCFG_DSPD; //1:1 device speed Скорость устройства 48 MHz
        USB_OTG_DEVICE->DCFG &=~ USB_OTG_DCFG_NZLSOHSK; //0: отправляется приложению принятый пакет OUT (нулевой или ненулевой длины) и отправляется handshake на основе бит NAK и STALL 
        // ~ non-zero-length status OUT handshake
        USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM; // unmusk USB reset interrupt Сброс по шине // unmask enumeration done interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ESUSPM; // unmask early suspend interrupt //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM; // unmask USB suspend interrupt         //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_SOFM; // unmask SOF interrupt
        //---------------------------------------------------------------------------
		USB_OTG_DEVICE->DCTL = USB_OTG_DCTL_SDIS;  //Отключиться
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_FDMOD;//force device mode
        for(uint32_t i=0; i<1800000;i++){}//wait 25 ms
		//включаем подтягивающий резистор DP вернее сенсор VbusB по которому включится резистор при наличии 5 В на Vbus
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_PWRDWN; 
        USB_OTG_FS->GINTSTS=0xFFFFFFFF; //rc_w1 read_and_clear_write_1 очистить регистр статуса		
        NVIC_SetPriority(OTG_FS_IRQn,1); //приоритет 1
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
	  //USB_OTG_FS->DIEPTXF[2] = (TX_EP3_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE+TX_EP1_FIFO_SIZE+TX_EP2_FIFO_SIZE); //! размер и адрес Tx_FIFO  для EP3
	  USB_OTG_FS->DIEPTXF[2] = 0;
	  // 1 пакета SETUP, CNT=1, endpoint 0 OUT transfer size register
	  USB_OTG_OUT(0)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT_0 | USB_OTG_DOEPTSIZ_PKTCNT) ; //STUPCNT 0:1 = 1
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
        //USART_debug::usart2_sendSTR("DEVICE DESCRIPTER\n");
        //counter++;
          len = sizeof(Device_Descriptor);
          pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.
          break;
        case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
        //USART_debug::usart2_sendSTR("CONFIGURATION DESCRIPTER\n");
          len = sizeof(confDescr);
          pbuf = (uint8_t *)&confDescr;
          break;   
		           
        case USBD_IDX_LANGID_STR: //Запрос строкового дескриптора
        //USART_debug::usart2_sendSTR("USBD_IDX_LANGID_STR\n");
          len = sizeof(LANG_ID_Descriptor);
          pbuf = (uint8_t *)LANG_ID_Descriptor;                   
          break;
        case USBD_strManufacturer: //Запрос строкового дескриптора
        //USART_debug::usart2_sendSTR("USBD_strManufacturer\n");
          len = sizeof(Man_String);
          pbuf = (uint8_t *)Man_String;                             
          break;
        case USBD_strProduct: //Запрос строкового дескриптора
         //USART_debug::usart2_sendSTR("USBD_strProduct\n");
          len = sizeof(Prod_String);
          pbuf = (uint8_t *)Prod_String;         
          break;                     
        case USBD_IDX_SERIAL_STR: //Запрос строкового дескриптора
        //USART_debug::usart2_sendSTR("USBD_IDX_SERIAL_STR\n");
          len = sizeof(SN_String);
          pbuf = (uint8_t *)SN_String;    
          break;        
      }
    break;
  case SET_ADDRESS:  // Установка адреса устройства
    addressFlag = true;    
    /*!< записываем подтверждающий пакет статуса на адрес 0 нулевой длины и затем меняем адрес>*/
    //WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    break;
	case GET_CONFIGURATION:
		/*Устройство передает один байт, содержащий код конфигурации устройства*/
		pbuf=(uint8_t*)&confDescr+5; //номер конфигурации (единственной)
		len=1;//WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    //USART_debug::usart2_sendSTR("GET_CONFIGURATION\n");
	break;
    case SET_CONFIGURATION: // Установка конфигурации устройства
	/*<здесь производится конфигурация конечных точек в соответствии с принятой конфигурацией (она одна)>*/
      //Set_CurrentConfiguration((setupPack.setup.wValue>>4)); //если несколько конфигураций необходима доп функция
	  ep_1_2_init(); //инициализируем конечные точки 1-прием, передача и 2-настройка    
      USART_debug::usart2_sendSTR("SET_CONF\n");
      break;       // len-0 -> ZLP
	case SET_INTERFACE: // Установка конфигурации устройства
	/*<здесь выбирается интерфейс (в данном случае не должен выбираться, т.к. разные конечные точки)>*/
    USART_debug::usart2_sendSTR("SET_INTERFACE\n");
    break;	  	  
	/* CDC Specific requests */
    case SET_LINE_CODING: //устанавливает параметры линии передач
    USART_debug::usart2_sendSTR(" SLC \n");
	  setLineCodingFlag=true;	
      //cdc_set_line_coding();           
      break;
    case GET_LINE_CODING:
    USART_debug::usart2_sendSTR(" GLC \n");
      cdc_get_line_coding();           
      break;
    case SET_CONTROL_LINE_STATE:
    USART_debug::usart2_sendSTR(" SCLS \n");
      cdc_set_control_line_state();    
      break;
    case SEND_BREAK:
    USART_debug::usart2_sendSTR(" S_B \n");
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
	case CLEAR_FEATURE_ENDP:
			USART_debug::usart2_sendSTR(" CLEAR_FEATURE_ENDP \n");
			USART_debug::usart2_send(setupPack.b[2]);
		  USART_debug::usart2_send(setupPack.b[3]);
		//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH); //очищаем Tx_FIFO, которое почему то переполняется
		break;
	
	default: 
  //stall();
  USART_debug::usart2_sendSTR(" STALL \n");break;
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
    //USART_debug::usart2_sendSTR("ADDRESS\n");
	  // необходимо выставить подтверждение принятия пакета выставления адреса 
	  
}
//-----------------------------------------------------------------------------------------
void USB_DEVICE::WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen)
{
  USB_OTG_IN(EPnum)->DIEPTSIZ =0;
  /*!<записать количество пакетов и размер посылки>*/
  uint8_t Pcnt = minLen/64 + 1;  
  USB_OTG_IN(EPnum)->DIEPTSIZ |= (Pcnt<<19)|(minLen);
   /*!<количество передаваемых пакетов (по прерыванию USB_OTG_DIEPINT_XFRC передается один пакет)>*/
  USB_OTG_IN(EPnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); //выставляем перед записью
  if(minLen) WriteFIFO(EPnum, buf, minLen); //если нет байтов передаем пустой пакет    
}
//---------------------------------------------------------------------------------------------------
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
}
void USB_DEVICE::ep_1_2_init()
{  
  /*!<EP1_IN, EP3_OUT - BULK, EP2_IN - INTERRUPT>*/
  USB_OTG_IN(1)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(1)->DIEPCTL&=~USB_OTG_DIEPCTL_EPTYP_0; //1:0 - BULK
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_1 0:0:0:1
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
  
  USB_OTG_OUT(3)->DOEPCTL|=64;// 64 байта в пакете
  USB_OTG_OUT(3)->DOEPCTL|=USB_OTG_DOEPCTL_EPTYP_1;
  USB_OTG_OUT(3)->DOEPCTL&=~USB_OTG_DOEPCTL_EPTYP_0; //1:0 - BULK 
  USB_OTG_OUT(3)->DOEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_OUT(3)->DOEPCTL|=USB_OTG_DOEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
  //------------------------------------------------------------------
  USB_OTG_IN(2)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_0; //1:1 - INTERRUPT
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_1;
  USB_OTG_IN(2)->DIEPCTL&=~USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_2 0:0:1:0
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
	  
  //!Демаскировать прерывание для каждой активной конечной точки, и замаскировать прерывания для всех не активных конечных точек в регистре OTG_FS_DAINTMSK.
  USB_OTG_DEVICE->DAINTMSK|=(1<<19)|(3<<1);//включаем прерывания на конечных точках 1-IN 2-IN 3-OUT 
  
  /*!<задаем максимальный размер пакета 
	  и количество пакетов конечной точки BULK_OUT
	  (непонятно как может быть больше одного пакета), 
	  которое может принять Rx_FIFO>*/
  USB_OTG_OUT(3)->DOEPTSIZ = 0;
  USB_OTG_OUT(3)->DOEPTSIZ |= (1<<19)|(64<<0) ; //PKNT = 1 (DATA), макс размер пакета 64 байта	
  //USB_OTG_OUT(3)->DOEPTSIZ |= (1<<19)|(1<<0) ; //PKNT = 1 (DATA), 1 ,байт - прерывание по приему одного байта	
  // разрешаем прием пакета OUT на BULK точку 
  USB_OTG_OUT(3)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK|USB_OTG_DOEPCTL_EPENA; //разрешаем конечную точку OUT
//-------------------------------------------------------
/*< Заполняем массив line_code>*/	
	for(uint8_t i=0;i<7;i++){line_code[i] = line_coding[i];}
}

void USB_DEVICE::stall()
{
	/*TODO: send STALL signal*/
	USB_OTG_IN(0)->DIEPCTL|=USB_OTG_DIEPCTL_STALL;
}

void USB_DEVICE::cdc_set_line_coding(uint8_t size)
{
	//необходимо вычитывать из OUT пакета
	uint8_t lineC[8];	
	USART_debug::usart2_send(size);
	*(uint32_t*)(lineC) = USB_OTG_DFIFO(0);
	*((uint32_t*)(lineC)+1) = USB_OTG_DFIFO(0); //заполнили структуру
	//uint32_t dummy = USB_OTG_DFIFO(0); //считали инфу чтобы очистить Rx_FIFO только для SETUP
	//for (uint8_t i=0;i<((size+3)>>4);i++)
	//{*((uint32_t*)(lineC)+i) = USB_OTG_DFIFO(0);} //заполняем массив
	for(uint8_t i=0;i<7;i++)
	{line_code[i] = *((uint8_t*)(&lineC)+i);} //это если из FIFO читается подряд (если нет надо по другому)		
	USART_debug::usart2_sendSTR("l_c \n");
}
void USB_DEVICE::cdc_get_line_coding()
{
	uint8_t* buf; 
	buf=(uint8_t*)&line_code;
	WriteINEP(0,buf,7);
}

void USB_DEVICE::cdc_set_control_line_state() //rts, dtr (не используем пока)
{}
void USB_DEVICE::cdc_send_break()
{}
void USB_DEVICE::cdc_send_encapsulated_command()
{}
void USB_DEVICE::cdc_get_encapsulated_command()
{}

void USB_DEVICE::read_BULK_FIFO(uint8_t size)
{
	uint8_t size_on_for = (size+3)>>2;//делим на 4
	uint32_t buf[16]; //выделяем промежуточный буфер на 64 байта
	//uint8_t ostatok = size%4;
/*!<Засовываем в очередь>*/		
	for (uint8_t i=0;i<size_on_for;i++)
	{
		buf[i]=USB_OTG_DFIFO(0); //вычитываем из Rx_FIFO
		//counter=buf[0];
		
		//if(i != size_on_for - 1) //запихиваем по 4 байта
		//{
    //  for(uint8_t j=0;j<4;j++){BULK_OUT_buf[4*i+j] = *((uint8_t*)(buf+i)+j);}
		//	//for(uint8_t j=0;j<4;j++){qBulk_OUT.push(*((uint8_t*)(buf+i)+j));}
		//}
		///*запихиваем оставшуюся часть*/
		//else
		//{
		//	for(uint8_t j=0;j<ostatok;j++)
		//	{
    //    {BULK_OUT_buf[4*i+j] = *((uint8_t*)(buf+i)+j);}
		//		//qBulk_OUT.push(*((uint8_t*)(buf+i)+j));
		//	}
		//}		
	}	
	for(uint8_t j=0;j<size;j++)
	{
		BULK_OUT_buf[j]=*((uint8_t*)(buf)+j); //записываем в буфер
		qBulk_OUT.push(BULK_OUT_buf[j]);
	}
}