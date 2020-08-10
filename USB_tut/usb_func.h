extern void usb_init();
extern void usb_control_packet_processed();
extern void GetStatusDevice();
extern void GetStatusInterface();
extern void GetStatusEndpoint();
extern void GetDescriptor();
extern void GetConfiguration();
extern void SetConfiguration();
extern void SetAddress();
extern byte * usb_send_ep0_packet(byte* tbuf, byte data_length);

/* =================================================== */
/*                    Инициализация USB                */
/* =================================================== */
/* 
   -----------------------------------------------------
   Особое внимание следует обратить на инициализацию PLL
   Констарта PLL_xxHz должна соответствовать исполозуе-
   мому кварцу. Иначе устройство не будет опознаваться
   в Windows.
   -----------------------------------------------------
*/
void usb_init()
{
 /* Инициализация PLL */
 Pll_set_div(PLL_24MHz);
 Pll_enable();

 /* Включение USB */
 Usb_enable();

 /* Отключиться-подключиться для начала нумерации */
 Usb_detach();
 delay5();
 Usb_attach();
 delay5();

 /* Конфигурирование нулевой конечной точки */
 usb_configure_endpoint(0, CONTROL|MSK_EPEN);
 usb_reset_endpoint(0);
 
 /* Номер конфигурации */
 usb_configuration_nb = 0x00;

 end_point1_ready = 0;
}

/* =================================================== */
/*                  Обработка control-пакета           */
/* =================================================== */
/* 
   -----------------------------------------------------
   Состоит из трех фаз: Setup, Data, Status.
   Обработка начинается при получении setup-пакета.
   Фаза Data может отсутствовать.
   -----------------------------------------------------
*/
void usb_control_packet_processed()
{ 
  /* ------------------------------ */
  /*       Читаем setup-пакет       */
  /* Состоит из 8 байт (см. 1.2.1): */
  /*  [0] byte bmRequestType        */
  /*  [1] byte bRequest             */
  /*  [2] word wValue               */
  /*  [4] word wIndex               */
  /*  [6] word wLength              */
  /* ------------------------------ */
  for (i=0; i<8; i++)
   SetupPacket.b[i]= Usb_read_byte(); 
  
  /* ---------------------------------- */
  /* Завершение setup-фазы. Сброс FIFO. */
  /* ---------------------------------- */
  EndSetupStage();

  /* ---------------------------- DEBUG */
/*
  for (i=0; i<8; i++)
   SendRS232(SetupPacket.b[i]);
*/
  /* ------------------------------ */
  /*         Обработка запроса      */
  /* ------------------------------ */
  switch (SetupPacket.wRequest)  
  {
    case GET_STATUS_DEVICE:
     GetStatusDevice();   
     break;

    case GET_STATUS_INTERF:     
     GetStatusInterface();   
     break;

    case GET_STATUS_ENDPNT:     
     GetStatusEndpoint();   
     break;

    case GET_DESCRIPTOR_DEVICE:
    case GET_DESCRIPTOR_INTERF:
    case GET_DESCRIPTOR_ENDPNT:
     GetDescriptor(); 
     break;

    case GET_CONFIGURATION:
     GetConfiguration();         
     break;

    case SET_CONFIGURATION:     
     SetConfiguration();
     break;

    case SET_ADDRESS:
     SetAddress();   
     break;

    /* CDC Specific requests */
    case SET_LINE_CODING:
      cdc_set_line_coding();           
      break;
    case GET_LINE_CODING:
      cdc_get_line_coding();           
      break;
    case SET_CONTROL_LINE_STATE:
      cdc_set_control_line_state();    
      break;
    case SEND_BREAK:
      cdc_send_break();                
      break;
    case SEND_ENCAPSULATED_COMMAND:
      cdc_send_encapsulated_command(); 
      break;
    case GET_ENCAPSULATED_COMMAND:
      cdc_get_encapsulated_command();  
      break;

    default:
     STALL();
     break;
  }

}

/* =================================================== */
/*  GetStatusXxx                                       */
/*     - слово состояния устройства                    */
/*     - слово состояния интерфейса                    */
/*     - слово состояния конечной точки                */
/* (см. раздел 1.2.2)                                  */
/* =================================================== */
void GetStatusDevice()
{
  Usb_set_DIR();

  /*
    Устройство передает два байта данных:
    В байте 0 используется два бита:
     [1]= 0: устройство игнорирует сигнал побудки
     [0]= 0: устройство получает питание от шины USB
    Байт 1 зарезервирован и всегда равен 0.
  */
  Usb_write_byte(0x00);  
  Usb_write_byte(0x00);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

void GetStatusInterface()
{
  Usb_set_DIR();

  /* 
     Устройство передает два байта данных,  
     (слово состояния устройства). Оба байта
     зарезервированы и равны нулю.           
  */
  Usb_write_byte(0x00);          
  Usb_write_byte(0x00);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

void GetStatusEndpoint()
{
  data byte wIndex;

  /* номер конечной точки в младшем байте wIndex */
  wIndex = SetupPacket.b[4] & MSK_EP_DIR;

  Usb_set_DIR();
  /* 
     Устройство передает два байта данных.         
     Используется только первый бит первого байта: 
       0 - конечная точка функционирует нормально
       1 - передача данных заблокирована
     Остальные биты разерерворированы и равны 0.
  */
  Usb_write_byte(0x00);
  Usb_write_byte(0x00);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

/* =================================================== */
/*                    GetDescriptor                    */
/* =================================================== */
void GetDescriptor()
{
 data byte    data_to_transfer;
 data uint16  wLength;
 bit          zlp;
 data byte   *pbuffer;

 zlp = FALSE;    

 /* Тип запрашиваемого дескриптора находится  */
 /* в старшем байте поля wValue, т.е. байте 3 */
 switch (SetupPacket.b[3])
 {
    /* дескриптор устройства */
    case DEVICE: 
    {
      data_to_transfer = sizeof(usb_device_descriptor);
      pbuffer = &(usb_device_descriptor.bLength);
      break;
    }
    /* дескриптор конфигурации */
    case CONFIGURATION: 
    {
      data_to_transfer = sizeof(usb_configuration);
      pbuffer = &(usb_configuration.cfg.bLength);
      break;
    }
    /* дескриптор строки */
    case STRING:
    {
      /* Индекс строки находится в младшем */
      /* байте поля wValue, т.е. байте 2   */
      switch (SetupPacket.b[2])
      {
        case LANG_ID:
        {
          data_to_transfer = sizeof (usb_language);
          pbuffer = &(usb_language.bLength);
          break;
        }
        case MAN_INDEX:
        {
          data_to_transfer = sizeof (usb_manufacturer);
          pbuffer = &(usb_manufacturer.bLength);
          break;
        }
        case PRD_INDEX:
        {
          data_to_transfer = sizeof (usb_product);
          pbuffer = &(usb_product.bLength);
          break;
        }
        case SRN_INDEX:
        {
          data_to_transfer = sizeof (usb_serial_number);
          pbuffer = &(usb_serial_number.bLength);
          break;
        }
        default:
        {
          STALL();
          return;
        }
      }
      break;
    }

    default:
    { 
      STALL();
      return;
    }
  }

  /* Чтение требуемого числа байт */
  /* (поле wLenght)               */
  wLength = wSWAP(SetupPacket.setup.wLength);


  /* Требуется больше чем есть? */
  if (wLength > data_to_transfer)
  {
    /* Если число байт пакета кратно размеру пакета, */
    /* то пакет завершения формируем специально      */
    if ((data_to_transfer % EP_CONTROL_LENGTH) == 0) 
    { 
     zlp = TRUE; 
    }
    else 
    { 
     zlp = FALSE; 
    }
  }
  else
  {
    /* посылаем только требуемое число байт */
    /* иногда это меньше, чем есть реально  */
    /* деление на блоки не требуется        */
    data_to_transfer = (byte)wLength;      
  }

  /* переключение направления нулевой точки */
  Usb_set_DIR();                            

  /* шлем пока хватает данных на формирование полного пакета */
  /* остаток пакета досылаем потом (если надо)               */
  while (data_to_transfer > EP_CONTROL_LENGTH)
  {
    /* передача буфера максимальной длины*/
    pbuffer = usb_send_ep0_packet(pbuffer, EP_CONTROL_LENGTH);
    /* сдвигаем указатель */
    data_to_transfer -= EP_CONTROL_LENGTH;

    /* ждем завершения передачи */
    Usb_set_tx_ready();
    while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
    Usb_clear_tx_complete();
    if ((Usb_rx_complete())) /* если передача прервана хостом */
    {
      Usb_clear_tx_ready(); /* завершить передачу */
      Usb_clear_rx();
      return;
    }
  }

  /* посылаем остаточный пакет данных          */
  /* если такой пакет неполный, то он является */
  /* пакетом завершения передачи               */ 
  pbuffer = usb_send_ep0_packet(pbuffer, data_to_transfer);
  data_to_transfer = 0;
  Usb_set_tx_ready();
  while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
  Usb_clear_tx_complete();
  if ((Usb_rx_complete())) /* если передача прервана хостом */
  {
    Usb_clear_tx_ready();
    Usb_clear_rx();
    return;
  }

  /* при необходимости (если все пакеты полные)       */
  /* формируем пакет завершения специально - посылкой */
  /* пакета нулевой длины                             */
  if (zlp == TRUE)
  {
    usb_send_ep0_packet(pbuffer, 0);
    Usb_set_tx_ready();
    while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
    Usb_clear_tx_complete();
    if ((Usb_rx_complete())) /* если передача прервана хостом */
    {
      Usb_clear_tx_ready();
      Usb_clear_rx();
      Usb_clear_DIR();  
      return;
    }

  }

  while ((!(Usb_rx_complete())) && (!(Usb_setup_received())));
  if (Usb_setup_received())
  {
    return;
  }

  if (Usb_rx_complete())
  {
    Usb_clear_DIR();  /* переключение направления 0 точки */
    Usb_clear_rx();
  }

}

/* =================================================== */
/*                  GetConfiguration                   */
/* =================================================== */
void GetConfiguration()
{
  Usb_set_DIR();

  /*
    Устройство передает один байт данных,
    содержащий код конфигурации устройства.
  */
  Usb_write_byte(usb_configuration_nb);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

/* =================================================== */
/*                  SetConfiguration                   */
/* =================================================== */
void SetConfiguration()
{
  data byte configuration_number;

  /* читать номер конфигурации */
  /* из младшего байта wValue  */
  configuration_number = SetupPacket.b[2];   

  /* если выбрана доступная конфигурация */
  if (configuration_number <= CONF_NB)
  {
    /* сохранить номер конфигурации */
    usb_configuration_nb = configuration_number;
  }
  else
  {
    /* ошибочный запрос - отклоняем */
    STALL();
    return;
  }

  /* фаза status */
  SendInZeroPacket();

  /* Конфигурирование других конечных точек */
  usb_configure_endpoint(1 , BULK_IN);
  usb_reset_endpoint(1);
  Usb_enable_ep_int(1);

  usb_configure_endpoint(2 , BULK_OUT);
  usb_reset_endpoint(2);
  Usb_enable_ep_int(2);

  usb_configure_endpoint(3 , INTERRUPT_IN);
  usb_reset_endpoint(3);
  Usb_enable_ep_int(3);
}

/* =================================================== */
/* Установка адреса устройства                         */
/* =================================================== */
void SetAddress()
{
  /* выставить флаг "устройство адресовано" */
  Usb_set_FADDEN();        
  SendInZeroPacket();
  /* 
    Хост передает адрес устройства в младшей части
    байта wValue.
  */
  Usb_configure_address(SetupPacket.b[2]);
}

/* =================================================== */
/*   Передача пакета по нулевой конечной точке         */
/* =================================================== */
byte * usb_send_ep0_packet(byte* tbuf, byte data_length)
{
  data int  i;
  data byte b;

  Usb_select_ep(0);

  for (i=0; i<data_length; i++)
  { 
   b = *tbuf;
   Usb_write_byte(b); 
   tbuf++;
  }
  return tbuf;
}
