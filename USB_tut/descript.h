/* Структура Setup-пакета */
typedef struct{ 
  byte bmRequestType;
  byte bRequest;  
  word wValue;
  word wIndex;  
  word wLength;
} SETUP_PACKET;

typedef union
{
  SETUP_PACKET setup;
  byte         b[8];
  word         wRequest;
} UsbSetupPacket;

/* Структура дескриптора устройства */
struct usb_st_device_descriptor
{
  byte   bLength;              
  byte   bDescriptorType;      
  uint16 bscUSB;               
  byte   bDeviceClass;         
  byte   bDeviceSubClass;      
  byte   bDeviceProtocol;      
  byte   bMaxPacketSize0;      
  uint16 idVendor;             
  uint16 idProduct;            
  uint16 bcdDevice;            
  byte   iManufacturer;        
  byte   iProduct;             
  byte   iSerialNumber;        
  byte   bNumConfigurations;   
};

/* Структура дескриптора конфигурации */
struct usb_st_configuration_descriptor
{
  byte   bLength;              
  byte   bDescriptorType;      
  uint16 wTotalLength;         
  byte   bNumInterfaces;       
  byte   bConfigurationValue;  
  byte   iConfiguration;       
  byte   bmAttibutes;          
  byte   MaxPower;             
};

/* Структура дескриптора интерфейса */
struct usb_st_interface_descriptor
{
  byte bLength;                
  byte bDescriptorType;        
  byte bInterfaceNumber;       
  byte bAlternateSetting;      
  byte bNumEndpoints;          
  byte bInterfaceClass;        
  byte bInterfaceSubClass;     
  byte bInterfaceProtocol;     
  byte iInterface;             
};

/* Структура дескриптора конечной точки */
struct usb_st_endpoint_descriptor
{
  byte   bLength;              
  byte   bDescriptorType;      
  byte   bEndpointAddress;     
  byte   bmAttributes;         
  uint16 wMaxPacketSize;       
  byte   bInterval;            
};

/* Структура HID дескриптора */
struct usb_st_hid_descriptor
{ 
  byte   bLength;               
  byte   bDescriptorType;       
  uint16 bscHID;                
  byte   bCountryCode;          
  byte   bNumDescriptors;       
  byte   bRDescriptorType;      
  uint16 wDescriptorLength;     
};

/* Дескриптор строки */
#define structSTRING_DESCRIPTOR(NAME, LEN) struct NAME \
                                           { \
                                             byte   bLength; \
                                             byte   bDescriptorType; \
                                             uint16 wstring[LEN]; \
                                            }
