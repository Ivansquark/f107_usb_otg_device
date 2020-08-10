bit        usb_connected;
bit        p_test;
data byte  end_point1_ready;
data byte  bmRequestType;
data byte  usb_configuration_nb;
data byte  point1_state;
data byte  endpoint_status[6];
data byte  hid_idle_duration;
data byte  rq_value;
data byte  bcount;
data byte  i;
data byte  tx_counter;

data byte  CDCLineState;

data UsbSetupPacket SetupPacket;
data byte line_coding[7];


extern void SendRS232(byte b);
extern void main_task();