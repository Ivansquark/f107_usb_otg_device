#ifndef USB_DEVICE_HPP_
#define USB_DEVICE_HPP_

#include "main.h"

#define RX_FIFO_SIZE         128 //размер очередей
#define TX_EP0_FIFO_SIZE     128
#define TX_EP1_FIFO_SIZE     128
#define TX_EP2_FIFO_SIZE     128
#define TX_EP3_FIFO_SIZE     128
//адреса разных FIFO
#define USB_OTG_DFIFO(i) *(__IO uint32_t *)((uint32_t)USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE) 



class USB_DEVICE
{
public: 
    USB_DEVICE();    
    static USB_DEVICE* pThis;
	void ReadSetupFIFO(void);
    void Enumerate_Setup();
	uint32_t counter{0};
	uint32_t resetFlag{0};
	uint16_t ADDRESS=0;
    uint16_t CurrentConfiguration=0;

    uint8_t bmRequestType{0};
	uint8_t bRequest{0};
	uint16_t wValue{0};
	uint16_t wIndex{0};
	uint16_t wLength{0};
	#pragma pack(push, 1)
	typedef struct setup_request
    {
    	uint8_t bmRequestType=0; // D7 направление передачи фазы данных 0 = хост передает в устройство 1 = устройство передает на хост...
    	uint8_t bRequest=0;	   // Запрос (2-OUT…6-SETUP)
    	uint16_t wValue=0;	   // (Коды Запросов дескрипторов)
    	uint16_t wIndex=0;	   //
    	uint16_t wLength=0;	   //
    }USB_SETUP_req;
	#pragma pack(pop)
	//#pragma pop
    USB_SETUP_req uSetReq; //выделяем память под структуру

    const uint8_t Device_Descriptor[18] =
    {
        /* Standard USB device descriptor for the CDC serial driver */
        18, // size
        1, // USBGenericDescriptor_DEVICE
        0x00,0x02, // USBDeviceDescriptor_USB2_00
        2, // CDCDeviceDescriptor_CLASS
        0, // CDCDeviceDescriptor_SUBCLASS
        0, // CDCDeviceDescriptor_PROTOCOL
        64, // BOARD_USB_ENDPOINTS_MAXPACKETSIZE
        0xEB,0x03, // CDCDSerialDriverDescriptors_VENDORID
        0x24,0x61, // CDCDSerialDriverDescriptors_PRODUCTID
        0x10,0x01, // CDCDSerialDriverDescriptors_RELEASE
        1, // Index of manufacturer description //0
        2, // Index of product description //0
        3, // Index of serial number description //0
        1 // One possible configuration
    };
    const uint8_t Config_Descriptor[9]
    {
        /*Configuration Descriptor*/
        0x09, /* bLength: Configuration Descriptor size */
        0x02, /* bDescriptorType: Configuration */
        67,   /* wTotalLength:no of retuint16_t ADDRESS=0;
    uint16_t CurrentConfiguration=0;urned bytes */
        0x00,
        0x02, /* bNumInterfaces: 2 interface */
        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor describing the configuration */
        0x80, /* bmAttributes - Bus powered */
        0x32 /* MaxPower 100 mA */
    };
    const uint8_t Interface_Descriptor[9]
    {
        /*Interface Descriptor */
        0x09, /* bLength: Interface Descriptor size */
        0x04, /* bDescriptorType: Interface */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x01, /* bNumEndpoints: One endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */
    };
    const uint8_t EP1_In_Descriptor[7]
    {
        /*Endpoint 2 Descriptor*/
        0x07, /* bLength: Endpoint Descriptor size */
        0x05, /* bDescriptorType: Endpoint */
        0x81, /* bEndpointAddress IN1 */
        0x03, /* bmAttributes: Interrupt */
        0x08, /* wMaxPacketSize LO: */
        0x00, /* wMaxPacketSize HI: */
        0x10, /* bInterval: */
    };
    const uint8_t EP1_OUT_Descriptor[7]
    {
        0x07,   /*Endpoint descriptor length = 7 */
        0x05,   /*Endpoint descriptor type */
        0x01,   /*Endpoint address (OUT, address 1) */
        0x03,   /*Interrupt endpoint type */
        0x8,
        0x00,
        0x00     /*Polling interval in milliseconds*/
    };    
private:
	//#pragma pack (push,1)
    
    void usb_init();
    void fifo_init();    
    void SetAdr(uint16_t value);
    void Set_CurrentConfiguration(uint16_t value);
    void WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen);
    uint16_t MIN(uint16_t len, uint16_t wLength);
    void WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len);
	
    /*! <bmRequestType> */    
    static constexpr uint8_t STD_GET_STATUS = 0x00;
    static constexpr uint8_t STD_CLEAR_FEATURE = 0x01;
    static constexpr uint8_t STD_SET_FEATURE = 0x03;
    static constexpr uint8_t STD_SET_ADDRESS = 0x05;
    static constexpr uint8_t STD_GET_DESCRIPTOR = 0x06;
    static constexpr uint8_t STD_SET_DESCRIPTOR = 0x07;
    static constexpr uint8_t STD_GET_CONFIGURATION = 0x08;
    static constexpr uint8_t STD_SET_CONFIGURATION = 0x09;
    static constexpr uint8_t STD_GET_INTERFACE = 0xA;
    static constexpr uint8_t STD_SET_INTERFACE = 0x11;
    static constexpr uint8_t STD_SYNCH_FRAME = 0x12;
    /*! <mValue> */
    static constexpr uint16_t USB_DESC_TYPE_DEVICE = 0x0100;
    static constexpr uint16_t USB_DESC_TYPE_CONFIGURATION = 0x0200;
    static constexpr uint16_t USB_DESC_TYPE_STRING = 0x0300;
    static constexpr uint16_t USB_DESC_TYPE_INTERFACE = 0x0400;
    static constexpr uint16_t USB_DESC_TYPE_EP_DESCRIPTOR = 0x0500;
    static constexpr uint16_t USB_DESC_TYPE_DEVICE_QUALIFIER = 0x0700;

    static constexpr uint8_t USBD_IDX_LANGID_STR = 0x03;
    static constexpr uint8_t USBD_strManufacturer = 0x0301;
    static constexpr uint8_t USBD_strProduct = 0x0302;
    static constexpr uint8_t USBD_IDX_SERIAL_STR = 0x0303;
    static constexpr uint8_t USBD_IDX_CONFIG_STR = 0x03;
};
USB_DEVICE* USB_DEVICE::pThis=nullptr;

//!< здесь осуществляется все взаимодействие с USB
extern "C" void OTG_FS_IRQHandler(void)
{	
	//Инициализация конечной точки 0 при USB reset:
    if(USB_OTG_FS->GINTSTS &  USB_OTG_GINTSTS_USBRST)
	{		
		//USB_DEVICE::pThis->resetFlag++;	
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_USBRST; // сбрасываем бит
		//1. Установка бита NAK для всех конечных точек OUT: SNAK = 1 в регистре OTG_FS_DOEPCTLx (для всех конечных точек OUT, x это номер конечной точки).
		//Используя этот бит, приложение может управлять передачей отрицательных подтверждений NAK конечной точки.
		USB_OTG_OUT(0)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(1)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(2)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(3)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		//2. Демаскирование следующих бит прерывания:
		// включаем прерывания конечной точки 0
		USB_OTG_DEVICE->DAINTMSK |= (1<<0); //control 0 IN endpoint  биты маски прерываний конечной точки IN разрешаем прерывания 
		USB_OTG_DEVICE->DAINTMSK |= (1<<16); //control 0 OUT endpoint  биты маски прерываний конечной точки OUT разрешаем прерывания
		// разрешаем прерывания завершения фазы настройки и завершения транзакции OUT
		USB_OTG_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_STUPM; //1 разрешаем прерывание SETUP Phase done .
		USB_OTG_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM; //1 TransfeR Completed interrupt Mask. разрешаем на чтение
		// разрешаем прерывания таймаута и завершения транзакции IN
		USB_OTG_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_TOM; //TimeOut condition Mask (не изохронные конечные точки). Если этот бит сброшен в 0, то прерывание таймаута маскировано (запрещено).
		USB_OTG_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM; // TransfeR Completed interrupt Mask. разрешаем на запись
		
		//FIFO, их ещё в обработчике RESET очищать надо
		//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		//Сбросить RXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH); // сбрасываем Tx и Rx FIFO
		USB_DEVICE::pThis->uSetReq.bmRequestType=0;USB_DEVICE::pThis->uSetReq.bRequest=0;
		USB_DEVICE::pThis->uSetReq.wValue=0;USB_DEVICE::pThis->uSetReq.wIndex=0;
		USB_DEVICE::pThis->uSetReq.wLength=0;	
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_USBRST;	
		
	}
     /*! <start of Enumeration done> */
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_ENUMDNE)
	{
		//Инициализация конечной точки по завершению энумерации:
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // бит выставляется при окончании энумерации, необходимо его очистить
		//uint32_t enSize = OTG->OTG_FS_DSTS;//прочитайте регистр OTG_FS_DSTS, чтобы определить скорость энумерации. (FS постоянный)
		USB_OTG_IN(0)->DIEPCTL &=~ USB_OTG_DIEPCTL_MPSIZ; //0:0 - 64 байта, Приложение должно запрограммировать это поле максимальным размером пакета для текущей логической конечной точки. 
		//USB_OTG_GLOBAL->GUSBCFG |= USB_OTG_GUSBCFG_TRDT(5);   USB TuRnaround Time. Эти биты позволяют установить время выполнения работы в тактах PHY. 
		//!< разрешаем генерацию прерывания при приеме в FIFO, конечных точек IN, OUT
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;
		
		USB_OTG_IN(0)->DIEPCTL &=~ USB_OTG_DIEPCTL_MPSIZ;
		//На этом этапе устройство готово принять пакеты SOF и оно сконфигурировано для выполнения управляющих транзакций на control endpoint 0.
	}
    /*! < прерывание конечной точки IN  (на передачу данных)> */
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTMSK_IEPINT) 
	{
		
        uint32_t epnums  = USB_OTG_DEVICE->DAINT; // номер конечной точки вызвавшей прерывание 
        uint32_t epint;
        epnums &= USB_OTG_DEVICE->DAINTMSK;   	  // определяем этот номер	с учетом разрешенных точек
        if( epnums & 0x0001) // если конечная точка 0
		{ //EP0 IEPINT     
			USB_DEVICE::pThis->counter++;
			epint = USB_OTG_IN(0)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			
			if(epint & USB_OTG_DIEPINT_XFRC) // если Transfer Completed interrupt. Показывает, что транзакция завершена как на AHB, так и на USB.
			{
				//USB_DEVICE::pThis->resetFlag=10001;
                /*!< (TODO: реализовать очередь в которую сначала закидываем побайтово буффер с дескриптором) >*/
                // а потом вычитываем из нее побайтово очищая счетчик.
				if(USB_OTG_IN(0)->DIEPTSIZ & USB_OTG_DIEPTSIZ_XFRSIZ/*usb.Get_TX_Q_cnt(0)*/)	//если счетчик не нулевой, 			
					{
						
						//usb.EP_TX_Q(0); //  записываем в Ep Tx, пока не исчерпается счетчик, после этого в else
						//writeToFIFO;
						//USB_DEVICE::pThis->resetFlag++;
					}
				else
				{
					//EndPoint ENAble. Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
				USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
				//разрешит передачу из FIFO
				}
			}
			if(epint & USB_OTG_DIEPINT_TOC) //TimeOut Condition. Показывает, что ядро определило событие таймаута на USB для последнего токена IN на этой конечной точке.
			{				
			}
			//Показывает, что токен IN был принят, когда связанный TxFIFO (периодический / не периодический) был пуст.
			if(epint & USB_OTG_DIEPINT_ITTXFE) // IN Token received when TxFIFO is Empty.
			{                
            }
			//Показывает, что данные на вершине непериодического TxFIFO принадлежат конечной точке, отличающейся от той, для которой был получен токен IN.
			if(epint & USB_OTG_DIEPINT_INEPNE) //IN token received with EP Mismatch.
			{
			}
			//  EndPoint DISableD interrupt. Этот бит показывает, что конечная точка запрещена по запросу приложения.
			if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
			{
			}
			//когда TxFIFO для этой конечной точки пуст либо наполовину, либо полностью.
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{
			}
			USB_OTG_IN(0)->DIEPINT = epint;
		}
		if( epnums & 0x0002) // если конечная точка 1
		{ //EP1 IEPINT
		    USB_OTG_IN(1)->DIEPINT = epint;
		}
		if( epnums & 0x0004) // если конечная точка 2
		{ //EP2 IEPINT
		    USB_OTG_IN(2)->DIEPINT = epint;
		}   
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN
		return;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTMSK_OEPINT) // прерывание конечной точки OUT (на прием данных) (срабатывает в первый раз при приеме Setup пакета)
    {
		
		uint32_t epnums  = USB_OTG_DEVICE->DAINT;
		uint32_t epint;				
		epnums &= USB_OTG_DEVICE->DAINTMSK;			// определяем конечную точку	
		if( epnums & 0x00010000)		// конечная точка 0
		{ //EP0 OEPINT     
		    epint = USB_OTG_OUT(0)->DOEPINT; //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK;	 // считываем разрешенные биты 
		    // Transfer Completed interrupt. Это поле показывает, что для этой конечной точки завершена запрограммированная транзакция
		    if(epint & USB_OTG_DOEPINT_XFRC)
		    {
		    }
		    //SETUP phase done. На этом прерывании приложение может декодировать принятый пакет данных SETUP.
		    if(epint & USB_OTG_DOEPINT_STUP) // Показывает, что фаза SETUP завершена (пришел пакет)
		    {						
		    	USB_DEVICE::pThis->Enumerate_Setup(); // декодировать принятый пакет данных SETUP. (прочесть из Rx_FIFO 8 байт в первый раз должен быть запрос дескриптора устройства)
		    }   
		    //OUT Token received when EndPoint DISabled. Показывает, что был принят токен OUT, когда конечная точка еще не была разрешена.
		    if(epint & USB_OTG_DOEPINT_OTEPDIS)
		    {
		    }
		    USB_OTG_OUT(0)->DOEPINT = epint; //записываем разрешенные биты
		}
		if( epnums & 0x00020000)		// конечная точка 1
		{ //EP1 OEPINT
		    USB_OTG_OUT(1)->DOEPINT = epint;
		}
		if( epnums & 0x00040000)		// конечная точка 2
		{ //EP2 OEPINT
		    USB_OTG_OUT(2)->DOEPINT = epint;
		}
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN
    return;
    }
//-----------------------------------------------------------------------------------------------	
	//OTG_FS_GINTSTS_RXFLVL /* Receive FIFO non-empty */   буффера RX не пуст
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_RXFLVL)
	{		
		//USB_DEVICE::pThis->resetFlag++;
		//uint8_t status = (USB_OTG_FS->GRXSTSR)>>17&0xF; // PacKeT STatuS приложение должно прочитать регистр выборки статуса приема (OTG_FS_GRXSTSP).
		//чтение регистра GRXSTSP извлечет данные из Rx_FIFO (в FIFO останется только DATA пакет)
		uint8_t status = ((USB_OTG_FS->GRXSTSP & USB_OTG_GRXSTSP_PKTSTS)>>17)&0xF; //то же самое		
		switch (status) 
		{
			case 2: //OUT packet
			case 6: // SETUP packet recieve Эти данные показывают, что в RxFIFO сейчас доступен для чтения пакет SETUP указанной конечной точки.
			{
				//USB_DEVICE::pThis->resetFlag=status;
				//uint32_t count = ((USB_OTG_FS->GRXSTSP & USB_OTG_GRXSTSP_BCNT)>>4)&0xFF; //количество принятых байт
				//USB_DEVICE::pThis->resetFlag=count;
				//if (count) // если количество принятых байт не равно нулю => читаем Rx
				{
					USB_DEVICE::pThis->resetFlag++;		
					USB_DEVICE::pThis->ReadSetupFIFO();				
				}
				
				//	//Flush_TX(ep);
                
			} break;
			case 0x03:  /* OUT completed */
            case 0x04:  /* SETUP completed */
			{
				//EPENA Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
			//CNAK (бит 26): Clear NAK. Запись в этот бит очистит бит NAK для конечной точки. Ядро установить этот бит после того, как на конечной точке принят пакет SETUP
                USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
				USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
				// после этого необходимо заполнить Tx дескриптором устройства.
			}			
		}
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; //разрешаем генерацию прерывания наличия принятых данных в FIFO приема.
		USB_OTG_FS-> GINTSTS = 0xFFFFFFFF;
	}
}

#endif //USB_DEVICE_HPP_