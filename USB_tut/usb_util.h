extern void delay5();
extern void usb_configure_endpoint(byte ep_num, byte ep_type);
extern void usb_reset_endpoint(byte ep_num);

/* =================================================== */
/* =================================================== */
void delay5()
{
 data unsigned int i;
 data unsigned int j;
 for (i=0; i<2000; i++) j++;
}

/* =================================================== */
/* =================================================== */
void usb_configure_endpoint(byte ep_num, byte ep_type)
{
  Usb_select_ep(ep_num);
  Usb_configure_ep_type(ep_type); 
}

/* =================================================== */
/* =================================================== */
void usb_reset_endpoint(byte ep_num)
{
  UEPRST = 0x01 << ep_num;
  UEPRST = 0x00;
}

/* =================================================== */
/* =================================================== */
void EndSetupStage()
{
  Usb_clear_rx_setup();
}

void SendDataFromFIFO()
{
  Usb_set_tx_ready();
  while ((!(Usb_tx_complete())) || (Usb_setup_received()));
  Usb_clear_tx_complete();
}

void STALL()
{
  Usb_set_stall_request();
  while (!Usb_stall_sent());
  Usb_clear_stall_request();
  Usb_clear_stalled();
  Usb_clear_DIR();
}

void WaitForOutZeroPacket()
{
  while ((!(Usb_rx_complete())) || (Usb_setup_received()));
  Usb_clear_rx();
  Usb_clear_DIR();
}

void SendInZeroPacket()
{
  SendDataFromFIFO();
  Usb_clear_DIR();
}

void WaitForFillFIFO()
{
  while (!(Usb_rx_complete()));
}

void EndReadData()
{
  Usb_clear_rx();
}


