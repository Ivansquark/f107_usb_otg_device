#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

constexpr uint8_t Device_Descriptor[18] =
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
        0x10,0x01, // CDCDSerialDriverDescriptors_RELEASE  bcdDevice (00 01)
        1, // Index of manufacturer description //0
        2, // Index of product description //0
        3, // Index of serial number description //0
        1 // One possible configuration
    };

    constexpr struct
    {
        const uint8_t Config_Descriptor[9];
        const uint8_t Interface_Descriptor1[9];
        const uint8_t Header_Functional_Descriotor[5];
        const uint8_t Call_Management_Functional_Descriptor[5];
        const uint8_t ACM_Functional_Descriptor[4];
        const uint8_t Union_Functional_Descriptor[5];
        const uint8_t Ep2_Descriptor[7];
        const uint8_t Interface_Descriptor2[9];
        const uint8_t EP1_In_Descriptor[7];
        const uint8_t EP1_OUT_Descriptor[7];
    }  confDescr =    
    {
        /*Configuration Descriptor*/
        {
            0x09, /* bLength: Configuration Descriptor size */
            0x02, /* bDescriptorType: Configuration */
            67,   /* wTotalLength:no of retuinturned bytes */
            0x00,
            0x02, /* bNumInterfaces: 2 interface */
            0x01, /* bConfigurationValue: Configuration value */
            0x00, /* iConfiguration: Index of string descriptor describing the configuration */
            0x80, /* bmAttributes - Bus powered 00 [6] 0-bus powered 1-no bus power [5] 0-no wakeup 1- wakeup */
            0x32 /* MaxPower 100 mA */
        },
	/*!<Communication interface>*/    
        /*Interface Descriptor master*/
        {
            0x09, /* bLength: Interface Descriptor size */
            0x04, /* bDescriptorType: Interface */
            0x00, /* bInterfaceNumber: Number of Interface */
            0x00, /* bAlternateSetting: Alternate setting */
            0x01, /* bNumEndpoints: One endpoints used */
            0x02, /* bInterfaceClass: Communication Interface Class */
            0x02, /* bInterfaceSubClass: Abstract Control Model */
            0x01, /* bInterfaceProtocol: Common AT commands */
            0x00 /* iInterface: */
        },
			/*!<header descriptor>*/
			{
				0x05, /* bLength: Endpoint Descriptor size */
				0x24, /* bDescriptorType: CS_INTERFACE */
				0x00, /* bDescriptorSubtype: Header Func Desc */
				0x10, /* bcdCDC: spec release number */
				0x01	
			},
			/*Call Management Functional Descriptor*/
			{		    
				/*!<Дескриптор режима команд, описывает процесс вызовов коммуникационного интерфейса.>*/
				0x05, /* bFunctionLength */
				0x24, /* bDescriptorType: CS_INTERFACE */
				0x01, /* bDescriptorSubtype: Call Management Func Desc */
				0x00, /* bmCapabilities: D0+D1 устройство принимает и передает команды только через коммуникационный интерфейс*/
				0x01 /* bDataInterface: 1  number of interfaces (two interfaces)*/
			},
			/*ACM Functional Descriptor*/
			{		
				0x04, /* bFunctionLength */
				0x24, /* bDescriptorType: CS_INTERFACE */
				0x02, /* bDescriptorSubtype: Abstract Control Management desc */
				0x02 /* bmCapabilities , битовая маска поддерживаемых команд*/
				/*! x:x:x [2]-SendBreak [1]-Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding,
				Serial_State [0] - Set_Comm_Feature, Clear_Com_Feature, Get_Comm_Feature*/
			},
			/*Union Functional Descriptor*/
			{
				/*! Описывается отношение интерфейсов в группе. Один интерфейс Master, остальные Slave,
				Запросы и нотификации проходящие через главный интерфейс, применяются ко всей
				группе интерфейсов */		
				0x05, /* bFunctionLength */
				0x24, /* bDescriptorType: CS_INTERFACE */
				0x06, /* bDescriptorSubtype: Union func desc */
				0x00, /* bMasterInterface: Communication class interface - master*/
				0x01 /* bSlaveInterface0: Data Class Interface  - slave*/
			},
			{
				/*Endpoint 3 IN Descriptor*/
				0x07, /* bLength: Endpoint Descriptor size */
				0x05, /* bDescriptorType: Endpoint */
				0x81, /* bEndpointAddress IN  8-IN 1-endpoint1*/
				0x03, /* bmAttributes: Interrupt */
				64, /* wMaxPacketSize LO: */
				0x00, /* wMaxPacketSize HI: */
				0x10, /* bInterval: */
			},
        /*!<Data interface>*/
        {
            /*Interface Descriptor slave interface*/
            0x09, /* bLength: Interface Descriptor size */
            0x04, /* bDescriptorType: Interface */
            0x01, /* bInterfaceNumber: Number of Interface */
            0x00, /* bAlternateSetting: Alternate setting */
            0x02, /* bNumEndpoints: Two endpoints used */
            0x0A, /* bInterfaceClass: DATA Interface Class */
            0x02, /* bInterfaceSubClass: No class */
            0x00, /* bInterfaceProtocol: No special */
            0x00 /* iInterface: */
        },
			{
				/*Endpoint 1 Descriptor*/
				0x07, /* bLength: Endpoint Descriptor size */
				0x05, /* bDescriptorType: Endpoint */
				0x83, /* bEndpointAddress IN1  8-IN 3-endpoint1*/
				0x02, /* bmAttributes: BULK */
				64, /* wMaxPacketSize LO: */
				0x00, /* wMaxPacketSize HI: */
				0x01 /* bInterval: */
			},
			/*EP1_OUT_Descriptor[7]*/
			{
				0x07,   /*Endpoint descriptor length = 7 */
				0x05,   /*Endpoint descriptor type */
				0x03,   /*Endpoint address (0-OUT 3-endpoint1) */
				0x02,   /*Interrupt endpoint type Interrupt 0x02 -BULK*/
				64,
				0x00,
				0x01     /*Polling interval in milliseconds*/
			}
    };		
//---------------------------------------------------------------------------------------------------	
	/*! <Всего 67 байт>*/	
	constexpr uint8_t LANG_ID_Descriptor[4]
    {
        /* Index 0x00: LANGID Codes */
        0x04,   /*descriptor length = 7 */
        0x03,   /*STRING descriptor type */
        0x09,	/*LANG_US uint16_t*/
		0x04      /*! !!! все что больше байта необходимо переворачивать*/  
    };   
	constexpr uint8_t Man_String[13]
	{
		/* Index 0x01:  */
		0x0a,   /*descriptor length = 13 */
        0x03,   /*STRING descriptor type */
        'I',0,
		'v',0,
		'a',0,
		'n',0
	};
	constexpr uint8_t Prod_String[14] 
	{
		/* Index 0x02:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        'O',0,
		'p',0,
		'A',0		
	};
	constexpr uint8_t SN_String[14]
	{
		/* Index 0x03:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        '1',0,
		'2',0,
		'3',0		
	};
	/*! <wRequest> */
	static constexpr uint16_t GET_STATUS_DEVICE = 0x8000;
	static constexpr uint16_t GET_STATUS_INTERFACE = 0x8100;
	static constexpr uint16_t GET_STATUS_ENDP = 0x8200;
    static constexpr uint16_t CLEAR_FEATURE_DEVICE = 0x0001;
	static constexpr uint16_t CLEAR_FEATURE_INTERFACE = 0x0101;
	static constexpr uint16_t CLEAR_FEATURE_ENDP = 0x0201;
    static constexpr uint16_t SET_FEATURE_DEVICE = 0x0003;
	static constexpr uint16_t SET_FEATURE_INTERFACE = 0x0103;
	static constexpr uint16_t SET_FEATURE_ENDP = 0x0203;
    static constexpr uint16_t SET_ADDRESS = 0x0005;
    static constexpr uint16_t GET_DESCRIPTOR_DEVICE = 0x8006;
	static constexpr uint16_t GET_DESCRIPTOR_INTERFACE = 0x8106;
	static constexpr uint16_t GET_DESCRIPTOR_ENDP = 0x8206;
    static constexpr uint16_t SET_DESCRIPTOR = 0x0007;
    static constexpr uint16_t GET_CONFIGURATION = 0x8008;
    static constexpr uint16_t SET_CONFIGURATION = 0x0009;
    static constexpr uint16_t GET_INTERFACE = 0x810A;
    static constexpr uint16_t SET_INTERFACE = 0x010B;
    static constexpr uint16_t SYNCH_FRAME = 0x820C;
	static constexpr uint16_t GET_REPORT = 0xA101;
	/*! <bmRequestType> */    
    static constexpr uint8_t STD_GET_STATUS = 0x00;
    static constexpr uint8_t STD_CLEAR_FEATURE = 0x01;
    static constexpr uint8_t STD_SET_FEATURE = 0x03;
    static constexpr uint8_t STD_SET_ADDRESS = 0x05;
    static constexpr uint8_t STD_GET_DESCRIPTOR = 0x06;
    static constexpr uint8_t STD_SET_DESCRIPTOR = 0x07;
    static constexpr uint8_t STD_GET_CONFIGURATION = 0x08;
	/*!<Драйвер в опреационной системе по окончанию энумерации посылает такой запрос>*/
    static constexpr uint8_t STD_SET_CONFIGURATION = 0x09; //в Value задается конфигурация в младшем байте
    static constexpr uint8_t STD_GET_INTERFACE = 0xA;
    static constexpr uint8_t STD_SET_INTERFACE = 0x11;
    static constexpr uint8_t STD_SYNCH_FRAME = 0x12;
    /*! <mValue> */
    static constexpr uint16_t USB_DESC_TYPE_DEVICE = 0x0100;
    static constexpr uint16_t USB_DESC_TYPE_CONFIGURATION = 0x0200;
    //static constexpr uint16_t USB_DESC_TYPE_DEVICE_QUALIFIER = 0x0600;
	/*<В запросах на передачу дескриптора Value содержит в старшем байте тип дескриптора, а в младшем индекс>*/
    static constexpr uint16_t USBD_IDX_LANGID_STR = 0x0300;
    static constexpr uint16_t USBD_strManufacturer = 0x0301;
    static constexpr uint16_t USBD_strProduct = 0x0302;
    static constexpr uint16_t USBD_IDX_SERIAL_STR = 0x0303;
    //static constexpr uint16_t USBD_IDX_CONFIG_STR = 0x0304;
	
	//<(bRequest<<8)|(bmRequestType)>
	static constexpr uint16_t SEND_ENCAPSULATED_COMMAND = 0x2100; //посылка команды
	static constexpr uint16_t GET_ENCAPSULATED_RESPONSE = 0xA101; // прием команды
	static constexpr uint16_t SET_LINE_CODING = 0x2120; //хост устанавливает параметры передачи данных
	static constexpr uint16_t GET_LINE_CODING = 0xA121;	// хост узнает текущие настройки линии передачи
	static constexpr uint16_t SET_CONTROL_LINE_STATE = 0x2122; // устанавливает состояние линии передачи (RTS[1] DTR[0])
	static constexpr uint16_t SEND_BREAK = 0x2123; //замораживает передачу данных
	
	constexpr uint8_t line_coding[7]={0x00,0xC2,0x01,0x00,0x00,0x00,0x08}; //uint32_t baudRate 115200; стоп бит, четность, размер посылки
	
	
#endif //USB_DESCRIPTORS_H