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
/*                    ���樠������ USB                */
/* =================================================== */
/* 
   -----------------------------------------------------
   �ᮡ�� �������� ᫥��� ������ �� ���樠������ PLL
   ������� PLL_xxHz ������ ᮮ⢥��⢮���� �ᯮ����-
   ���� ������. ���� ���ன�⢮ �� �㤥� ������������
   � Windows.
   -----------------------------------------------------
*/
void usb_init()
{
 /* ���樠������ PLL */
 Pll_set_div(PLL_24MHz);
 Pll_enable();

 /* ����祭�� USB */
 Usb_enable();

 /* �⪫������-����������� ��� ��砫� �㬥�樨 */
 Usb_detach();
 delay5();
 Usb_attach();
 delay5();

 /* ���䨣��஢���� �㫥��� ����筮� �窨 */
 usb_configure_endpoint(0, CONTROL|MSK_EPEN);
 usb_reset_endpoint(0);
 
 /* ����� ���䨣��樨 */
 usb_configuration_nb = 0x00;

 end_point1_ready = 0;
}

/* =================================================== */
/*                  ��ࠡ�⪠ control-�����           */
/* =================================================== */
/* 
   -----------------------------------------------------
   ���⮨� �� ��� 䠧: Setup, Data, Status.
   ��ࠡ�⪠ ��稭����� �� ����祭�� setup-�����.
   ���� Data ����� ������⢮����.
   -----------------------------------------------------
*/
void usb_control_packet_processed()
{ 
  /* ------------------------------ */
  /*       ��⠥� setup-�����       */
  /* ���⮨� �� 8 ���� (�. 1.2.1): */
  /*  [0] byte bmRequestType        */
  /*  [1] byte bRequest             */
  /*  [2] word wValue               */
  /*  [4] word wIndex               */
  /*  [6] word wLength              */
  /* ------------------------------ */
  for (i=0; i<8; i++)
   SetupPacket.b[i]= Usb_read_byte(); 
  
  /* ---------------------------------- */
  /* �����襭�� setup-䠧�. ���� FIFO. */
  /* ---------------------------------- */
  EndSetupStage();

  /* ---------------------------- DEBUG */
/*
  for (i=0; i<8; i++)
   SendRS232(SetupPacket.b[i]);
*/
  /* ------------------------------ */
  /*         ��ࠡ�⪠ �����      */
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
/*     - ᫮�� ���ﭨ� ���ன�⢠                    */
/*     - ᫮�� ���ﭨ� ����䥩�                    */
/*     - ᫮�� ���ﭨ� ����筮� �窨                */
/* (�. ࠧ��� 1.2.2)                                  */
/* =================================================== */
void GetStatusDevice()
{
  Usb_set_DIR();

  /*
    ���ன�⢮ ��।��� ��� ���� ������:
    � ���� 0 �ᯮ������ ��� ���:
     [1]= 0: ���ன�⢮ �������� ᨣ��� ���㤪�
     [0]= 0: ���ன�⢮ ����砥� ��⠭�� �� 設� USB
    ���� 1 ��१�ࢨ஢�� � �ᥣ�� ࠢ�� 0.
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
     ���ன�⢮ ��।��� ��� ���� ������,  
     (᫮�� ���ﭨ� ���ன�⢠). ��� ����
     ��१�ࢨ஢��� � ࠢ�� ���.           
  */
  Usb_write_byte(0x00);          
  Usb_write_byte(0x00);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

void GetStatusEndpoint()
{
  data byte wIndex;

  /* ����� ����筮� �窨 � ����襬 ���� wIndex */
  wIndex = SetupPacket.b[4] & MSK_EP_DIR;

  Usb_set_DIR();
  /* 
     ���ன�⢮ ��।��� ��� ���� ������.         
     �ᯮ������ ⮫쪮 ���� ��� ��ࢮ�� ����: 
       0 - ����筠� �窠 �㭪樮����� ��ଠ�쭮
       1 - ��।�� ������ �������஢���
     ��⠫�� ���� ࠧ��ࢮ�஢��� � ࠢ�� 0.
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

 /* ��� ����訢������ ���ਯ�� ��室����  */
 /* � ���襬 ���� ���� wValue, �.�. ���� 3 */
 switch (SetupPacket.b[3])
 {
    /* ���ਯ�� ���ன�⢠ */
    case DEVICE: 
    {
      data_to_transfer = sizeof(usb_device_descriptor);
      pbuffer = &(usb_device_descriptor.bLength);
      break;
    }
    /* ���ਯ�� ���䨣��樨 */
    case CONFIGURATION: 
    {
      data_to_transfer = sizeof(usb_configuration);
      pbuffer = &(usb_configuration.cfg.bLength);
      break;
    }
    /* ���ਯ�� ��ப� */
    case STRING:
    {
      /* ������ ��ப� ��室���� � ����襬 */
      /* ���� ���� wValue, �.�. ���� 2   */
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

  /* �⥭�� �ॡ㥬��� �᫠ ���� */
  /* (���� wLenght)               */
  wLength = wSWAP(SetupPacket.setup.wLength);


  /* �ॡ���� ����� 祬 ����? */
  if (wLength > data_to_transfer)
  {
    /* �᫨ �᫮ ���� ����� ��⭮ ࠧ���� �����, */
    /* � ����� �����襭�� �ନ�㥬 ᯥ樠�쭮      */
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
    /* ���뫠�� ⮫쪮 �ॡ㥬�� �᫮ ���� */
    /* ������ �� �����, 祬 ���� ॠ�쭮  */
    /* ������� �� ����� �� �ॡ����        */
    data_to_transfer = (byte)wLength;      
  }

  /* ��४��祭�� ���ࠢ����� �㫥��� �窨 */
  Usb_set_DIR();                            

  /* 諥� ���� 墠⠥� ������ �� �ନ஢���� ������� ����� */
  /* ���⮪ ����� ���뫠�� ��⮬ (�᫨ ����)               */
  while (data_to_transfer > EP_CONTROL_LENGTH)
  {
    /* ��।�� ���� ���ᨬ��쭮� �����*/
    pbuffer = usb_send_ep0_packet(pbuffer, EP_CONTROL_LENGTH);
    /* ᤢ����� 㪠��⥫� */
    data_to_transfer -= EP_CONTROL_LENGTH;

    /* ���� �����襭�� ��।�� */
    Usb_set_tx_ready();
    while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
    Usb_clear_tx_complete();
    if ((Usb_rx_complete())) /* �᫨ ��।�� ��ࢠ�� ��⮬ */
    {
      Usb_clear_tx_ready(); /* �������� ��।��� */
      Usb_clear_rx();
      return;
    }
  }

  /* ���뫠�� ������ ����� ������          */
  /* �᫨ ⠪�� ����� �������, � �� ���� */
  /* ����⮬ �����襭�� ��।��               */ 
  pbuffer = usb_send_ep0_packet(pbuffer, data_to_transfer);
  data_to_transfer = 0;
  Usb_set_tx_ready();
  while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
  Usb_clear_tx_complete();
  if ((Usb_rx_complete())) /* �᫨ ��।�� ��ࢠ�� ��⮬ */
  {
    Usb_clear_tx_ready();
    Usb_clear_rx();
    return;
  }

  /* �� ����室����� (�᫨ �� ������ �����)       */
  /* �ନ�㥬 ����� �����襭�� ᯥ樠�쭮 - ���뫪�� */
  /* ����� �㫥��� �����                             */
  if (zlp == TRUE)
  {
    usb_send_ep0_packet(pbuffer, 0);
    Usb_set_tx_ready();
    while ((!(Usb_rx_complete())) && (!(Usb_tx_complete())));
    Usb_clear_tx_complete();
    if ((Usb_rx_complete())) /* �᫨ ��।�� ��ࢠ�� ��⮬ */
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
    Usb_clear_DIR();  /* ��४��祭�� ���ࠢ����� 0 �窨 */
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
    ���ன�⢮ ��।��� ���� ���� ������,
    ᮤ�ঠ騩 ��� ���䨣��樨 ���ன�⢠.
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

  /* ���� ����� ���䨣��樨 */
  /* �� ����襣� ���� wValue  */
  configuration_number = SetupPacket.b[2];   

  /* �᫨ ��࠭� ����㯭�� ���䨣���� */
  if (configuration_number <= CONF_NB)
  {
    /* ��࠭��� ����� ���䨣��樨 */
    usb_configuration_nb = configuration_number;
  }
  else
  {
    /* �訡��� ����� - �⪫��塞 */
    STALL();
    return;
  }

  /* 䠧� status */
  SendInZeroPacket();

  /* ���䨣��஢���� ��㣨� ������� �祪 */
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
/* ��⠭���� ���� ���ன�⢠                         */
/* =================================================== */
void SetAddress()
{
  /* ���⠢��� 䫠� "���ன�⢮ ���ᮢ���" */
  Usb_set_FADDEN();        
  SendInZeroPacket();
  /* 
    ���� ��।��� ���� ���ன�⢠ � ����襩 ���
    ���� wValue.
  */
  Usb_configure_address(SetupPacket.b[2]);
}

/* =================================================== */
/*   ��।�� ����� �� �㫥��� ����筮� �窥         */
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
