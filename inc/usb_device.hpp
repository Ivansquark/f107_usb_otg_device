#ifndef USB_DEVICE_HPP_
#define USB_DEVICE_HPP_

#include "main.h"

#define RX_FIFO_SIZE         128 //размер очередей в 32 битных словах
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
	QueByte qBulk_OUT;
	uint8_t BULK_OUT_buf[64]{0};

	void ReadSetupFIFO(void);
    void Enumerate_Setup();
	void fifo_init();
	uint32_t counter{0};
	uint32_t resetFlag{0};
	uint16_t ADDRESS=0;
    uint16_t CurrentConfiguration=0;

	bool addressFlag{false};
	bool setLineCodingFlag{false};
	void SetAdr(uint16_t value);
	void read_BULK_FIFO(uint8_t size);
	void WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen);
	void cdc_set_line_coding(uint8_t size);
	
	uint8_t BULK_BUF[64]{0};
    
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
	
	/*! <накладываем на структуру объединение, чтобы обращаться к различным полям> */
	#pragma pack(push, 1)
	typedef union
	{
		USB_SETUP_req setup; //!< размер структуры
		uint8_t b[8];	 	 //!< массив байтов равный размеру структуры
		uint16_t wRequest;	 //!< Слово объединяющее первые два байта структуры	
	} setupP;    	
	#pragma pack(pop)
	setupP setupPack{0};
	//setupPack setPack;
        
private:
	//#pragma pack (push,1)
    uint8_t line_code[7]{0};
	/*!<меняем местами байты в полуслове, т.к. 16 битные величины USB передает наоборот>*/
	inline uint16_t swap(uint16_t x) {return ((x>>8)&(0x00FF))|((x<<8)&(0xFF00));}
    void usb_init();      
	void ep_1_2_init();  
	inline void stall();
	void usbControlPacketProcessed();
	
	void cdc_get_line_coding();
	void cdc_set_control_line_state();
	void cdc_send_break();
	void cdc_send_encapsulated_command(); 
	void cdc_get_encapsulated_command();
	//void getConfiguration();
    //void Set_CurrentConfiguration(uint16_t value);
    
    uint16_t MIN(uint16_t len, uint16_t wLength);
    void WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len);    
};
USB_DEVICE* USB_DEVICE::pThis=nullptr;

//!< здесь осуществляется все взаимодействие с USB
extern "C" void OTG_FS_IRQHandler(void)
{		
	//Инициализация конечной точки 0 при USB reset:
    if(USB_OTG_FS->GINTSTS &  USB_OTG_GINTSTS_USBRST)
	{
		//USART_debug::usart2_send(USB_DEVICE::pThis->resetFlag++);		
		USART_debug::usart2_sendSTR("reset\n");	
		
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
		USB_OTG_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM; // TransfeR Completed interrupt Mask. (прерывание завершения транзакции) разрешаем на запись
		USB_OTG_IN(0)->DIEPINT |= USB_OTG_DIEPINT_ITTXFE;		
		/*!<обнуляем структуру>*/
		USB_DEVICE::pThis->setupPack.setup.bmRequestType=0;USB_DEVICE::pThis->setupPack.setup.bRequest=0;
		USB_DEVICE::pThis->setupPack.setup.wValue=0;USB_DEVICE::pThis->setupPack.setup.wIndex=0;
		USB_DEVICE::pThis->setupPack.setup.wLength=0;	
		
		USB_DEVICE::pThis->fifo_init(); //заново инициализируем FIFO
		
		//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		//Сбросить RXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH); // сбрасываем Tx и Rx FIFO
				
		//USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt
		//USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_USBRST;	
		/*!<Обнууляем адрес>*/
		uint8_t value=0x7F; //7 bits of address
		USB_OTG_DEVICE->DCFG &=~ value<<4; //запись адреса 0.
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_USBRST; // сбрасываем бит		
	}
     /*! <start of Enumeration done> */
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_ENUMDNE)
	{
		//USART_debug::usart2_sendSTR("ENUMDNE\n");
		//Инициализация конечной точки по завершению энумерации:
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // бит выставляется при окончании энумерации, необходимо его очистить
		USB_OTG_IN(0)->DIEPCTL &=~ USB_OTG_DIEPCTL_MPSIZ; //0:0 - 64 байта, Приложение должно запрограммировать это поле максимальным размером пакета для текущей логической конечной точки. 
		//!< разрешаем генерацию прерывания при приеме в FIFO, конечных точек IN, OUT и непустого буфера Rx
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;		
		//На этом этапе устройство готово принять пакеты SOF и оно сконфигурировано для выполнения управляющих транзакций на control endpoint 0.
	}
//---------------------------------------------------------------------------------------------------------------------------------------
    /*! < прерывание конечной точки IN  (на передачу данных)> */
//-----------------------------------------------------------------------------------------------------------------------------------------
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_IEPINT) 
	{		
		uint32_t epnums  = USB_OTG_DEVICE->DAINT; // номер конечной точки вызвавшей прерывание 
        uint32_t epint;
        epnums &= USB_OTG_DEVICE->DAINTMSK;   	  // определяем этот номер	с учетом разрешенных точек
        if( epnums & 0x0001) // если конечная точка 0
		{ //EP0 IEPINT     
			epint = USB_OTG_IN(0)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению прерываниям.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем биты разрешенных прерываний 
			if(epint & USB_OTG_DIEPINT_XFRC) // если Transfer Completed interrupt. Показывает, что транзакция завершена как на AHB, так и на USB.
			{				
				//USART_debug::usart2_sendSTR("In XFRC\n");				
                /*!< (TODO: реализовать очередь в которую сначала закидываем побайтово буффер с дескриптором) >*/                
				if(USB_OTG_IN(0)->DIEPTSIZ & USB_OTG_DIEPTSIZ_PKTCNT)//QueWord::pThis->is_not_empty()/*usb.Get_TX_Q_cnt(0)*/)	 			
				{/*!<Если есть еще пакеты в Tx_FIFO, необходимо их записать, а только потом разрешать OUT>*/
					//USART_debug::usart2_sendSTR("In XFRC PKTCNT \n");
					//USB_DEVICE::pThis->WriteFIFO(0, buf, minLen);
					//USB_OTG_DFIFO(0) = QueWord::pThis->pop(); //!< записываем в FIFO значения из очереди //Отправить ещё кусочек
				}
				else
				{
					//EndPoint ENAble. Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
					//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
					/*!<Разрешаем входную точку по приему пакета OUT>*/
					USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
				}
			}
			if(epint & USB_OTG_DIEPINT_TOC) //TimeOut Condition. Показывает, что ядро определило событие таймаута на USB для последнего токена IN на этой конечной точке.
			{USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_TOC\n");}
			//Показывает, что токен IN был принят, когда связанный TxFIFO (периодический / не периодический) был пуст.
			if(epint & USB_OTG_DIEPINT_ITTXFE) // IN Token received when TxFIFO is Empty.
			{USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_ITTXFE\n");}
			//Показывает, что данные на вершине непериодического TxFIFO принадлежат конечной точке, отличающейся от той, для которой был получен токен IN.
			if(epint & USB_OTG_DIEPINT_INEPNE) //IN token received with EP Mismatch.
			{USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_INEPNE\n");}
			//  EndPoint DISableD interrupt. Этот бит показывает, что конечная точка запрещена по запросу приложения.
			if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
			{}
			//когда TxFIFO для этой конечной точки пуст либо наполовину, либо полностью.
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_TXFE\n");}
			USB_OTG_IN(0)->DIEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x0002) // если конечная точка 1 Bulk IN
		{ //EP1 IEPINT
			//USART_debug::usart2_sendSTR("Bulk IN\n");
			epint = USB_OTG_IN(1)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем данные в BULK точку (если данные есть в данном FIFO то они передадутся и сработает это прерывание)>*/
			if(epint & USB_OTG_DIEPINT_XFRC) // передача пакета окончена
			{USART_debug::usart2_sendSTR("Bulk IN XFRC\n");}
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{USART_debug::usart2_sendSTR("Bulk IN USB_OTG_DIEPINT_TXFE\n");}
			USB_OTG_IN(1)->DIEPINT = epint;//сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x0004) // если конечная точка 2 INTERRUPT IN
		{ //EP2 IEPINT
			USART_debug::usart2_sendSTR("INTERRUPT IN\n");
			epint = USB_OTG_IN(2)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем запрошенные данные в INTERRUPT точку communication interface>*/
		    USB_OTG_IN(2)->DIEPINT = epint;//сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}   
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN				
		return;
    }    
	//---------------------------------------------------------------------------------------------------------------------------------------
    /*! < прерывание конечной точки OUT  (на прием данных)> */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//На любом прерывании конечной точки OUT приложение должно прочитать регистр размера транзакции конечной точки
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
		    {// в момент SETUP приходят данные 
				//USART_debug::usart2_sendSTR("Out XFRC\n");
				//USB_DEVICE::pThis->WriteINEP(0,nullptr,0); //ZLP в пакете статуса ответа !!!ХЗ
		    }
		    //SETUP phase done. На этом прерывании приложение может декодировать принятый пакет данных SETUP.
		    if(epint & USB_OTG_DOEPINT_STUP) // Показывает, что фаза SETUP завершена (пришел пакет)
		    {					
		    	USB_DEVICE::pThis->Enumerate_Setup(); // декодировать принятый пакет данных SETUP. (прочесть из Rx_FIFO 8 байт в первый раз должен быть запрос дескриптора устройства)
				if(USB_DEVICE::pThis->addressFlag) 
				{				
					USB_DEVICE::pThis->SetAdr((USB_DEVICE::pThis->setupPack.setup.wValue));//установить адрес устройства
					USB_DEVICE::pThis->addressFlag=false; 
				}				
		    }
		    if(epint & USB_OTG_DOEPINT_OTEPDIS)//OUT Token received when EndPoint DISabled. Показывает, что был принят токен OUT, когда конечная точка еще не была разрешена.
		    {}
		    USB_OTG_OUT(0)->DOEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x00020000)		// конечная точка 1 BULK_OUT
		{ //EP1 OEPINT
			//USART_debug::usart2_sendSTR("BULK_OUT\n");	
			epint = USB_OTG_OUT(1)->DOEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK; // считываем разрешенные биты 
			/*!<Когда приложение прочитало все данные, ядро генерирует прерывание XFRC>*/ 
			if(epint & USB_OTG_DOEPINT_XFRC)
			{
				/*!<разгребаем принятые данные в BULK точку>*/
				USART_debug::usart2_sendSTR("BULK_1_OUT_XFRC \n");
				//USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK|USB_OTG_DOEPCTL_EPENA;
			}
						
			//----------------------------------------------------------------------
		    USB_OTG_OUT(1)->DOEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x00040000)		// конечная точка 2 нету
		{ //EP2 OEPINT
		    USB_OTG_OUT(2)->DOEPINT = epint;
		}
		if( epnums & 0x00080000)		// конечная точка 3 
		{ //EP3 OEPINT
		    epint = USB_OTG_OUT(3)->DOEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK; // считываем разрешенные биты 
			/*!<Когда приложение прочитало все данные, ядро генерирует прерывание XFRC>*/ 
			if(epint & USB_OTG_DOEPINT_XFRC)
			{
				/*!<разгребаем принятые данные в BULK точку>*/
				USART_debug::usart2_sendSTR("BULK_3_OUT_XFRC \n");
				//USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK|USB_OTG_DOEPCTL_EPENA;
			}						
			//----------------------------------------------------------------------
		    USB_OTG_OUT(3)->DOEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN
    return;
    }
//-----------------------------------------------------------------------------------------------	
	//!OTG_FS_GINTSTS_RXFLVL /* Receive FIFO non-empty */   буффера RX не пуст (принят пакет например: Out и Data)
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_RXFLVL)
	{		
		//USART_debug::usart2_sendSTR("RXFLVL\n");
		USB_OTG_FS-> GINTMSK &=~ USB_OTG_GINTMSK_RXFLVLM;// (запрещаем прерывания пока не прочитаем пакет, в Rx_FIFO уже лежат данные)
		/*!< !!!Чтение регистра Receive status read and pop дополнительно извлекает данные из вершины RxFIFO!!! >*/
		uint32_t reg_Rx_status = USB_OTG_FS->GRXSTSP; //считываем регистр статуса Rx_FIFO в который считывается информация из пакета
		
		uint8_t status = ((reg_Rx_status & USB_OTG_GRXSTSP_PKTSTS)>>17)&(0xF); //считываем статус пакета SETUP
		uint8_t epNum = (reg_Rx_status & USB_OTG_GRXSTSP_EPNUM); //узнаем конечную точку
		uint8_t bytesSize=((reg_Rx_status & USB_OTG_GRXSTSP_BCNT)>>4)&(0xFF);//узнаем количество пришедших байт
		if(bytesSize) 
		{//! Если количество байт принимаемого пакета не равно 0
			switch (status) 
			{
				/*! <принят пакет данных OUT. DATA stage>*/
				case 2: 
				//USART_debug::usart2_sendSTR("OUT packet\n");
				{
					//USART_debug::usart2_sendSTR("OUT packet\n");					
					if(USB_DEVICE::pThis->setLineCodingFlag)
					{/*!<Пришел control пакет для записи в конечную точку OUT>*/
						//USART_debug::usart2_sendSTR("OUT completed\n");
						//uint8_t size = 64 - (USB_OTG_OUT(0)->DOEPTSIZ & 0xFF);
						USB_DEVICE::pThis->cdc_set_line_coding(bytesSize);
						USB_DEVICE::pThis->setLineCodingFlag=false;
						// Нужно отправить пакет нулевой длины в пакете подтверждения статуса. (по прерыванию XFRC)  	
					} //читаем пакет setLineCoding (из 8 байт)
					else 
					{//!другие пакеты (BULK) принимается всегда один DATA пакет после OUT пакета					  
						if (epNum == 3)
						{/*!< считываем Rx_FIFO в очередь>*/
							/*!<необходимо записать принятые байты в память (в буфер очереди)>*/
							uint8_t size = 64 - (USB_OTG_OUT(3)->DOEPTSIZ & 0xFF);
							USB_DEVICE::pThis->resetFlag=size;
							USART_debug::usart2_send(size);
							//USB_DEVICE::pThis->resetFlag=size;
							if(size)
							{								
								USB_DEVICE::pThis->read_BULK_FIFO(size); //вычитываем из FIFO количество байт size
								//USART_debug::usart2_sendSTR("read_BULK_FIFO\n");
							}
							//uint32_t dummy = USB_OTG_DFIFO(0);
						}
					}											
				}
				break;
				/*принят пакет данных SETUP.*/
				case 6: // SETUP packet recieve Эти данные показывают, что в RxFIFO сейчас доступен для чтения пакет SETUP указанной конечной точки.
				{					
					{	
						//USART_debug::usart2_sendSTR("readFIFO\n");						
						USB_DEVICE::pThis->ReadSetupFIFO();	
						//uint32_t setupStatus = USB_OTG_DFIFO(0); // считываем Setup stage done и отбрасываем его. 					
					}				
				} break;				
			}
		}
		else
		{
			switch (status) 
			{
				case 0x03: 
				/*<После того, как эта запись была извлечена из RxFIFO, ядро выставляет прерывание XFRC на указанной конечной точке OUT>*/
						//USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT;
						if(epNum==3)
						{
							
							//USART_debug::usart2_sendSTR("BULK_OUT_COMPL \n");
							USB_OTG_OUT(3)->DOEPTSIZ = 0;
							USB_OTG_OUT(3)->DOEPTSIZ |= (1<<19)|(64<<0) ; //PKNT = 1 (DATA), макс размер пакета 64 байта
							USB_OTG_OUT(3)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
						}						
				break;
				case 0x04:  /* SETUP completed завершена транзакция SETUP (срабатывает прерывание). выставляется ACK*/
					{
						//USART_debug::usart2_sendSTR("SETUP Completed\n");
						USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT;
						USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);//запускаем конечную точку и разрешаем ее				
						//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
						// после этого необходимо заполнить Tx дескриптором устройства.
					}
				break;
			}			
		}		
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; //разрешаем генерацию прерывания наличия принятых данных в FIFO приема.		
	}
}

#endif //USB_DEVICE_HPP_