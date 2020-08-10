/* Специальные CDC запросы */
#define GET_LINE_CODING           0xA121
#define SET_LINE_CODING           0x2120
#define SET_CONTROL_LINE_STATE    0x2122
#define SEND_BREAK                0x2123
#define SEND_ENCAPSULATED_COMMAND 0x2100
#define GET_ENCAPSULATED_COMMAND  0xA101

void cdc_init()
{
  /* Конфигурирование RS232 */
  SCON    = MSK_UART_8BIT; 
  CKCON0 |= MSK_X2;        
  PCON   |= MSK_SMOD1;     
  BDRCON |= 2;       
  BDRCON |= 0x1C;    
  BDRCON &= ~1;      
  BRL     = 100;     
  SCON   |= MSK_UART_ENABLE_RX|MSK_UART_TX_READY|MSK_UART_ENABLE_RX; 

  line_coding[0] = 0x00; 
  line_coding[1] = 0xC2;
  line_coding[2] = 0x01;
  line_coding[3] = 00;
  line_coding[4] = 0;    /* stop bit  */
  line_coding[5] = 0;    /* parity    */
  line_coding[6] = 8;    /* data bits */

  CDCLineState = 0;
  tx_counter=0;
}

void cdc_set_line_coding()
{
  WaitForFillFIFO();

  line_coding[0] = Usb_read_byte();
  line_coding[1] = Usb_read_byte();
  line_coding[2] = Usb_read_byte();
  line_coding[3] = Usb_read_byte();
  line_coding[4] = Usb_read_byte();
  line_coding[5] = Usb_read_byte();
  line_coding[6] = Usb_read_byte();
  EndReadData();

  SendInZeroPacket();
}


void cdc_get_line_coding()
{
  Usb_set_DIR();

  Usb_write_byte(line_coding[0]);
  Usb_write_byte(line_coding[1]);
  Usb_write_byte(line_coding[2]);
  Usb_write_byte(line_coding[3]);
  Usb_write_byte(line_coding[4]);
  Usb_write_byte(line_coding[5]);
  Usb_write_byte(line_coding[6]);
  SendDataFromFIFO();

  WaitForOutZeroPacket();
}

void cdc_set_control_line_state()
{
  SendInZeroPacket();
}

void cdc_send_break()
{
  SendInZeroPacket();
}

void cdc_send_encapsulated_command (void)
{
}

void cdc_get_encapsulated_command (void)
{
}
