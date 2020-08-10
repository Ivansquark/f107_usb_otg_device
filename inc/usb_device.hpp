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

/*! <Simple Static Queue With Const Max Size Must Be Checked On Empty Condition>*/
struct QueWord
{
	QueWord()
	{pThis=this;}
    void push(uint32_t x)
    {
        if(start==nullptr){start = &arr[count]; arr[count]=x;count++;}
        else
        {
            if(count<size)
            {
                for(uint32_t i=count;i>0;i--)
                {arr[i]=arr[i-1];}
                arr[0]=x;
                count++;
            }
        }
    }
    uint32_t pop()
    {
        uint32_t temp=0;
        if(count){count--;} //!< super kostyl'
        if(count)
        {
            temp=arr[count];
            arr[count]=0;
        }
        else
        {
                if (start!=nullptr)
                {
                    temp=arr[0];arr[0]=0;
                    start=nullptr;
                }
                else{/*cout<<"opa";*/}
        }
        return temp;
    }
    bool is_not_empty() //!< this condition must be checked before pop
    {
        //if(start==nullptr)return true;
        //        else return false;
        return (start) ? true : false;
    }    
    uint32_t* start{nullptr};
    uint32_t count{0};
    static constexpr uint32_t size=10; //size of queue
    uint32_t arr[size]{0};
	static QueWord* pThis;
};
QueWord* QueWord::pThis = nullptr;


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

	bool addressFlag{false};
	void SetAdr(uint16_t value);
	void read_BULK_FIFO(uint8_t size);

    //uint8_t bmRequestType{0};
	//uint8_t bRequest{0};
	//uint16_t wValue{0};
	//uint16_t wIndex{0};
	//uint16_t wLength{0};
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
	//USB_SETUP_req set{0};	
	//USB_SETUP_req uSetReq; //выделяем память под структуру
	/*! <накладываем на структуру объединение, чтобы обращаться к различным полям> */
	#pragma pack(push, 1)
	typedef union
	{
		USB_SETUP_req setup; //!< размер структуры
		uint8_t b[8];	 	 //!< массив байтов равный размеру структуры
		uint16_t wRequest;	 //!< Слово объединяющее первые два байта структуры	
	} setupP;    
	setupP setupPack{1};
	#pragma pack(pop)
	//setupPack setPack;
        
private:
	//#pragma pack (push,1)
    uint8_t line_code[7]{0};
	
	inline uint16_t swap(uint16_t x) {return ((x>>8)&(0x00FF))|((x<<8)&(0xFF00));}
    void usb_init();
    void fifo_init();  
	void ep_1_2_init();  
	void stall();
	void usbControlPacketProcessed();
	void cdc_set_line_coding();
	void cdc_get_line_coding();
	void cdc_set_control_line_state();
	void cdc_send_break();
	void cdc_send_encapsulated_command(); 
	void cdc_get_encapsulated_command();
	//void getConfiguration();
    void Set_CurrentConfiguration(uint16_t value);
    void WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen);
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
		USB_OTG_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM; // TransfeR Completed interrupt Mask. (прерывание завершения транзакции) разрешаем на запись
		USB_OTG_IN(0)->DIEPINT |= USB_OTG_DIEPINT_ITTXFE;		
		/*!<обнуляем структуру>*/
		USB_DEVICE::pThis->setupPack.setup.bmRequestType=0;USB_DEVICE::pThis->setupPack.setup.bRequest=0;
		USB_DEVICE::pThis->setupPack.setup.wValue=0;USB_DEVICE::pThis->setupPack.setup.wIndex=0;
		USB_DEVICE::pThis->setupPack.setup.wLength=0;	
		//FIFO, их ещё в обработчике RESET очищать надо
		//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		//Сбросить RXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH); // сбрасываем Tx и Rx FIFO
				
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_USBRST;	
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT;			
		/*!<Обнууляем адрес>*/
		uint8_t value=0x7F; //7 bits of address
		USB_OTG_DEVICE->DCFG &=~ value<<4; //запись адреса.		
	}
     /*! <start of Enumeration done> */
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_ENUMDNE)
	{
		//USART_debug::usart2_sendSTR("ENUMDNE\n");
		//Инициализация конечной точки по завершению энумерации:
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // бит выставляется при окончании энумерации, необходимо его очистить
		//uint32_t enSize = OTG->OTG_FS_DSTS;//прочитайте регистр OTG_FS_DSTS, чтобы определить скорость энумерации. (FS постоянный)
		USB_OTG_IN(0)->DIEPCTL &=~ USB_OTG_DIEPCTL_MPSIZ; //0:0 - 64 байта, Приложение должно запрограммировать это поле максимальным размером пакета для текущей логической конечной точки. 
		//USB_OTG_GLOBAL->GUSBCFG |= USB_OTG_GUSBCFG_TRDT(5);   USB TuRnaround Time. Эти биты позволяют установить время выполнения работы в тактах PHY. 
		//!< разрешаем генерацию прерывания при приеме в FIFO, конечных точек IN, OUT и непустого буфера Rx
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;		
		//На этом этапе устройство готово принять пакеты SOF и оно сконфигурировано для выполнения управляющих транзакций на control endpoint 0.
	}
    /*! < прерывание конечной точки IN  (на передачу данных)> */
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_IEPINT) 
	{		
		uint32_t epnums  = USB_OTG_DEVICE->DAINT; // номер конечной точки вызвавшей прерывание 
        uint32_t epint;
        epnums &= USB_OTG_DEVICE->DAINTMSK;   	  // определяем этот номер	с учетом разрешенных точек
        if( epnums & 0x0001) // если конечная точка 0
		{ //EP0 IEPINT     
			epint = USB_OTG_IN(0)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			if(epint & USB_OTG_DIEPINT_XFRC) // если Transfer Completed interrupt. Показывает, что транзакция завершена как на AHB, так и на USB.
			{				
				USART_debug::usart2_sendSTR("In XFRC\n");
				
                /*!< (TODO: реализовать очередь в которую сначала закидываем побайтово буффер с дескриптором) >*/                
				if(USB_OTG_IN(0)->DIEPTSIZ & USB_OTG_DIEPTSIZ_PKTCNT)//QueWord::pThis->is_not_empty()/*usb.Get_TX_Q_cnt(0)*/)	 			
				{
					USART_debug::usart2_sendSTR("In XFRC PKTCNT \n");
					//USB_DEVICE::pThis->WriteFIFO(0, buf, minLen);
					//USB_OTG_DFIFO(0) = QueWord::pThis->pop(); //!< записываем в FIFO значения из очереди //Отправить ещё кусочек
				}
				else
				{
					//EndPoint ENAble. Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
					//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
					/*!<Разрешаем входную точку по приему пакета OUT>*/
					USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
					//разрешит передачу из FIFO
				}
			}
			if(epint & USB_OTG_DIEPINT_TOC) //TimeOut Condition. Показывает, что ядро определило событие таймаута на USB для последнего токена IN на этой конечной точке.
			{				
			}
			//Показывает, что токен IN был принят, когда связанный TxFIFO (периодический / не периодический) был пуст.
			if(epint & USB_OTG_DIEPINT_ITTXFE) // IN Token received when TxFIFO is Empty.
			{                
				USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_ITTXFE\n");
            }
			//Показывает, что данные на вершине непериодического TxFIFO принадлежат конечной точке, отличающейся от той, для которой был получен токен IN.
			if(epint & USB_OTG_DIEPINT_INEPNE) //IN token received with EP Mismatch.
			{
				USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_INEPNE\n");
			}
			//  EndPoint DISableD interrupt. Этот бит показывает, что конечная точка запрещена по запросу приложения.
			if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
			{
			}
			//когда TxFIFO для этой конечной точки пуст либо наполовину, либо полностью.
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{
				USART_debug::usart2_sendSTR("USB_OTG_DIEPINT_ITTXFE\n");
			}
			USB_OTG_IN(0)->DIEPINT = epint;
		}
		if( epnums & 0x0002) // если конечная точка 1 Bulk IN
		{ //EP1 IEPINT
		USART_debug::usart2_sendSTR("Bulk IN\n");
			epint = USB_OTG_IN(1)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем данные в BULK точку (если данные есть в данном FIFO то они передадутся и сработает это прерывание)>*/
		    USB_OTG_IN(1)->DIEPINT = epint;
		}
		if( epnums & 0x0004) // если конечная точка 2 INTERRUPT IN
		{ //EP2 IEPINT
			USART_debug::usart2_sendSTR("INTERRUPT IN\n");
			epint = USB_OTG_IN(2)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем запрошенные данные в INTERRUPT точку communication interface>*/
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
		    {// в момент SETUP приходят данные 
				//USART_debug::usart2_sendSTR("Out XFRC\n");
		    }
		    //SETUP phase done. На этом прерывании приложение может декодировать принятый пакет данных SETUP.
		    if(epint & USB_OTG_DOEPINT_STUP) // Показывает, что фаза SETUP завершена (пришел пакет)
		    {					
		    	USB_DEVICE::pThis->Enumerate_Setup(); // декодировать принятый пакет данных SETUP. (прочесть из Rx_FIFO 8 байт в первый раз должен быть запрос дескриптора устройства)
				//USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT;
				if(USB_DEVICE::pThis->addressFlag)
				{				
					USB_DEVICE::pThis->SetAdr((USB_DEVICE::pThis->setupPack.setup.wValue));
					USB_DEVICE::pThis->addressFlag=false;
				}				
		    }   
		    //OUT Token received when EndPoint DISabled. Показывает, что был принят токен OUT, когда конечная точка еще не была разрешена.
		    if(epint & USB_OTG_DOEPINT_OTEPDIS)
		    {
		    }
		    USB_OTG_OUT(0)->DOEPINT = epint; //записываем разрешенные биты
		}
		if( epnums & 0x00020000)		// конечная точка 1 BULK_OUT
		{ //EP1 OEPINT
			epint = USB_OTG_OUT(1)->DOEPINT; //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK;	 // считываем разрешенные биты 
			/*!<разгребаем принятые данные в BULK точку>*/
			//----------------------------------------------------------------------
		    USB_OTG_OUT(1)->DOEPINT = epint;
		}
		//if( epnums & 0x00040000)		// конечная точка 2 нету
		//{ //EP2 OEPINT
		//    USB_OTG_OUT(2)->DOEPINT = epint;
		//}
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN
    return;
    }
//-----------------------------------------------------------------------------------------------	
	//OTG_FS_GINTSTS_RXFLVL /* Receive FIFO non-empty */   буффера RX не пуст
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_RXFLVL)
	{		
		//USART_debug::usart2_sendSTR("RXFLVL\n");
		USB_OTG_FS-> GINTMSK &=~ USB_OTG_GINTMSK_RXFLVLM;
		//USB_DEVICE::pThis->resetFlag++;
		//uint8_t status = (USB_OTG_FS->GRXSTSR)>>17&0xF; // PacKeT STatuS приложение должно прочитать регистр выборки статуса приема (OTG_FS_GRXSTSP).
		//чтение регистра GRXSTSP извлечет данные из Rx_FIFO (в FIFO останется только DATA пакет)
		uint8_t status = ((USB_OTG_FS->GRXSTSP & USB_OTG_GRXSTSP_PKTSTS)>>17)&0xF; //то же самое
		//uint8_t bytesSize=((USB_OTG_FS->GRXSTSP & USB_OTG_GRXSTSP_BCNT)>>4)&0xFF;//узнаем количество пришедших байт
		//USART_debug::usart2_send(bytesSize);
		switch (status) 
		{
			/*принят пакет данных OUT. */
			case 2: 
			//USART_debug::usart2_sendSTR("OUT packet\n");
			{
				//USART_debug::usart2_sendSTR("OUT packet\n");
				/*!<Пришел пакет для записи в конечную точку OUT>*/
				//USB_DEVICE::pThis->read_BULK_FIFO(bytesSize); //считываем из FIFO количество принятых байт (в очередь)
			}
			break;
			/*принят пакет данных SETUP.*/
			case 6: // SETUP packet recieve Эти данные показывают, что в RxFIFO сейчас доступен для чтения пакет SETUP указанной конечной точки.
			{
				//if (count) // если количество принятых байт не равно нулю => читаем Rx
				{	
					//USART_debug::usart2_sendSTR("readFIFO\n");						
					USB_DEVICE::pThis->ReadSetupFIFO();	
					//uint32_t setupStatus = USB_OTG_DFIFO(0); // считываем Setup stage done и отбрасываем его.
					//USART_debug::usart2_sendSTR("readFIFO\n");
					//USB_DEVICE::pThis->resetFlag=setupStatus;
					//USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
				}
				//3. Приложение должно прочитать 2 слова пакета SETUP для RxFIFO.
				//4. Приложение должно прочитать из RxFIFO слово Setup stage done и отбросить его.
					//Flush_TX(ep);
                
			} break;
			case 0x03: 
			USART_debug::usart2_sendSTR("OUT completed\n"); /* OUT completed */			
			break;
            case 0x04:  /* SETUP completed завершена транзакция SETUP (срабатывает прерывание).
			выставляется ACK*/
			{
				//USART_debug::usart2_sendSTR("SETUP Completed\n");
				//EPENA Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
				//CNAK (бит 26): Clear NAK. Запись в этот бит очистит бит NAK для конечной точки. Ядро установить этот бит после того, как на конечной точке принят пакет SETUP
                USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT;
				//USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT;
				USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
				
				//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
				// после этого необходимо заполнить Tx дескриптором устройства.
			}			
		}
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; //разрешаем генерацию прерывания наличия принятых данных в FIFO приема.
		USB_OTG_FS-> GINTSTS = 0xFFFFFFFF;
	}
}

#endif //USB_DEVICE_HPP_