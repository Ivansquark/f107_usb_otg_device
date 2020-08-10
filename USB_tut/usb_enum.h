/* DEVICE DESCRIPTOR */
#define USB_SPECIFICATION     0x1001
#define DEVICE_CLASS          0x02
#define DEVICE_SUB_CLASS      0
#define DEVICE_PROTOCOL       0
#define EP_CONTROL_LENGTH     32
#define VENDOR_ID             0xEB03        /* Atmel vendor ID = 03EBh */
#define PRODUCT_ID            0x0920        /* Product ID: 2009h = CDC C5131 */
#define RELEASE_NUMBER        0x0000
#define NB_CONFIGURATION      1

#define LANG_ID              0x00
#define MAN_INDEX            0x01 
#define PRD_INDEX            0x02 
#define SRN_INDEX            0x03

                                    /* CONFIGURATION DESCRIPTOR */
#define CONF_LENGTH           0x4300  
#define NB_INTERFACE          2
#define CONF_NB               1
#define CONF_INDEX            0
#define CONF_ATTRIBUTES       USB_CONFIG_BUSPOWERED
#define MAX_POWER             50          /* 100 mA */
                                    /* INTERFACE 0 DESCRIPTOR */
#define INTERFACE0_NB          0
#define ALTERNATE0             0
#define NB_ENDPOINT0           1
#define INTERFACE0_CLASS       0x02 /* CDC ACM Comm */
#define INTERFACE0_SUB_CLASS   0x02
#define INTERFACE0_PROTOCOL    0x01
#define INTERFACE0_INDEX       0

                                    /* INTERFACE 1 DESCRIPTOR */
#define INTERFACE1_NB          1
#define ALTERNATE1             0
#define NB_ENDPOINT1           2
#define INTERFACE1_CLASS       0x0A /* CDC ACM Data */
#define INTERFACE1_SUB_CLASS   0
#define INTERFACE1_PROTOCOL    0
#define INTERFACE1_INDEX       0

                                    /* ENDPOINT 3 DESCRIPTOR */
#define ENDPOINT_NB_3         0x83
#define EP_ATTRIBUTES_3       0x03  /* interrupt */
#define EP_SIZE_3             wSWAP(32)  
#define EP_INTERVAL_3         0x02

                                    /* ENDPOINT 1 DESCRIPTOR */
#define ENDPOINT_NB_1         0x81
#define EP_ATTRIBUTES_1       0x02  /* bulk */
#define EP_SIZE_1             wSWAP(32) 
#define EP_INTERVAL_1         0x00

                                    /* ENDPOINT 2 DESCRIPTOR */
#define ENDPOINT_NB_2         0x02
#define EP_ATTRIBUTES_2       0x02  /* bulk */
#define EP_SIZE_2             wSWAP(32)  
#define EP_INTERVAL_2         0x00

#define EP_CONTROL            0x00
#define EP_IN                 0x01
#define EP_KBD_IN             EP_IN
#define EP_IN_LENGTH          64
#define ENDPOINT_0            0x00
#define ENDPOINT_1            0x81

code struct usb_st_device_descriptor usb_device_descriptor =
{ 
  sizeof(usb_device_descriptor), DEVICE, USB_SPECIFICATION, DEVICE_CLASS,
  DEVICE_SUB_CLASS, DEVICE_PROTOCOL, EP_CONTROL_LENGTH, VENDOR_ID, PRODUCT_ID,
  RELEASE_NUMBER, MAN_INDEX, PRD_INDEX, SRN_INDEX, NB_CONFIGURATION
};

code struct  
{ struct usb_st_configuration_descriptor  cfg;
  struct usb_st_interface_descriptor      ifc0;
  byte                                    CDC[19];
  struct usb_st_endpoint_descriptor       ep3 ;
  struct usb_st_interface_descriptor      ifc1;
  struct usb_st_endpoint_descriptor       ep1 ;
  struct usb_st_endpoint_descriptor       ep2 ;
 }
  usb_configuration =
  {
    { 9, CONFIGURATION, CONF_LENGTH, NB_INTERFACE, CONF_NB,
      CONF_INDEX, CONF_ATTRIBUTES, MAX_POWER},
    { 9, INTERFACE, INTERFACE0_NB, ALTERNATE0, NB_ENDPOINT0, INTERFACE0_CLASS,
      INTERFACE0_SUB_CLASS, INTERFACE0_PROTOCOL, INTERFACE0_INDEX },
{ 
0x05, 0x24, 0x00, 0x10, 0x01, /* заголовочный дескриптор     */
0x05, 0x24, 0x01, 0x03, 0x01, /* дескриптор режима команд    */
0x04, 0x24, 0x02, 0x06,       /* дескриптор абстр устройства */
0x05, 0x24, 0x06, 0x00, 0x01  /* дескриптор группирования    */
},
    { 7, ENDPOINT, ENDPOINT_NB_3, EP_ATTRIBUTES_3, EP_SIZE_3, EP_INTERVAL_3 },
    { 9, INTERFACE, INTERFACE1_NB, ALTERNATE1, NB_ENDPOINT1, INTERFACE1_CLASS,
      INTERFACE1_SUB_CLASS, INTERFACE1_PROTOCOL, INTERFACE1_INDEX },
    { 7, ENDPOINT, ENDPOINT_NB_1, EP_ATTRIBUTES_1, EP_SIZE_1, EP_INTERVAL_1 },
    { 7, ENDPOINT, ENDPOINT_NB_2, EP_ATTRIBUTES_2, EP_SIZE_2, EP_INTERVAL_2 }
};


/* Дескриптор строки производителя */
#define USB_MANUFACTURER_NAME {'P'<<8, 'V'<<8, 'A'<<8, \
                               'S'<<8, 'o'<<8, 'f'<<8, 't'<<8}
#define USB_MN_LENGTH         7
code structSTRING_DESCRIPTOR(usb_st_manufacturer, USB_MN_LENGTH)
 usb_manufacturer =
  { sizeof(usb_manufacturer),  STRING, USB_MANUFACTURER_NAME };

/* Дескриптор строки продукта */

#define USB_PRODUCT_NAME      {'P'<<8, 'V'<<8, 'A'<<8, 'S'<<8, 'o'<<8, 'f'<<8, 't'<<8, \
                               ' '<<8,  \
                               't'<<8, 'e'<<8, 's'<<8, 't'<<8, \
                               ' '<<8, \
                               'b'<<8, 'o'<<8, 'a'<<8, 'r'<<8, 'd'<<8}
#define USB_PN_LENGTH         18

code structSTRING_DESCRIPTOR(usb_st_product, USB_PN_LENGTH)
 usb_product =
  { sizeof(usb_product), STRING, USB_PRODUCT_NAME };


/* Дескриптор строки серийного номера */
#define USB_SERIAL_NUMBER     {'1'<<8, '.'<<8, '0'<<8, '.'<<8, '0'<<8}
#define USB_SN_LENGTH         5
code structSTRING_DESCRIPTOR(usb_st_serial_number, USB_SN_LENGTH)
 usb_serial_number =
  { sizeof(usb_serial_number), STRING, USB_SERIAL_NUMBER };

/* Дескриптор строки идентификатора языка */
#define LANGUAGE_ID           0x0904

code structSTRING_DESCRIPTOR(usb_st_language_descriptor, 1)
 usb_language =
 { sizeof(usb_language), STRING, LANGUAGE_ID };
