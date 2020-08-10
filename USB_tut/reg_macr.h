#define Enable_interrupt()      (EA = 1)
#define Disable_interrupt()     (EA = 0)

/* ����祭��/�몫�祭�� ����� USB  */
#define Usb_enable()               (USBCON |= MSK_USBE)
#define Usb_disable()              (USBCON &= ~MSK_USBE)
/* ������祭��/�⪫�祭�� �� �����    */
#define Usb_detach()               (USBCON |= MSK_DETACH)
#define Usb_attach()               (USBCON &= ~MSK_DETACH)
/* ���䨣��樮��� ���               */
#define Usb_set_CONFG()            (USBCON |= MSK_CONFG)
#define Usb_clear_CONFG()          (USBCON &= ~MSK_CONFG)
/* ��� ࠧ�襭�� �㭪樨 ����樨   */
#define Usb_set_FADDEN()           (USBCON |= MSK_FADDEN)
#define Usb_clear_FADDEN()         (USBCON &= ~MSK_FADDEN)
/* ��� �ਮ�⠭���� ᨭ�஭���樨 USB */
#define Usb_set_suspend_clock()    (USBCON |= MSK_SUSPCLK)
#define Usb_clear_suspend_clock()  (USBCON &= ~MSK_SUSPCLK)


/* ���� ���뢠��� ����砭�� ��� */
#define Usb_clear_reset()             (USBINT &= ~MSK_EORINT)
#define Usb_reset()                   (USBINT & MSK_EORINT)
/* ���� ���뢠��� �஡㦤���� ��   */
#define Usb_clear_resume()            (USBINT &= ~MSK_WUPCPU)
#define Usb_resume()                  (USBINT & MSK_WUPCPU)
/* ���� ���뢠��� �� �����㦥��� ��砫� ���� */
#define Usb_clear_sof()               (USBINT &= ~MSK_SOFINT)
#define Usb_sof()                     (USBINT & MSK_SOFINT)
/* ���� ���뢠��� �� �ਮ�⠭����             */ 
#define Usb_clear_suspend()           (USBINT &= ~MSK_SPINT)
#define Usb_suspend()                 (USBINT & MSK_SPINT)

/* ����襭��/����饭�� ���뢠��� �� ������� �祪 */
#define Usb_enable_ep_int(e)          (UEPIEN |= (0x01 << e))
#define Usb_disable_ep_int(e)         (UEPIEN &= ~(0x01 << e))

/* UEPINT - (r/o) ॣ���� ���뢠��� �������� USB �祪 */
#define Usb_endpoint_interrupt()      (UEPINT != 0 )


/* ��� ����� ��⠭���� ��⠭������� �裡  */
#define Usb_set_stall_request()       (UEPSTAX |= MSK_STALLRQ)
#define Usb_clear_stall_request()     (UEPSTAX &= ~MSK_STALLRQ)
#define Usb_clear_stalled()           (UEPSTAX &= ~MSK_STALLED)
#define Usb_stall_sent()              (UEPSTAX & MSK_STALLED)
/* ���� ���뢠��� �� �ਮ�⠭���� ���뫪�   */
/* ���� ���뢠��� �� �����㦥��� CRC �訡�� */
#define Usb_stall_requested()         (UEPSTAX & MSK_STALLRQ)
/* ���� ���뢠��� �� ����祭�� ����� SETUP */
#define Usb_clear_rx_setup()          (UEPSTAX &= ~MSK_RXSETUP)
#define Usb_setup_received()          (UEPSTAX & MSK_RXSETUP)
/* ��� �ࠢ����� ���ࠢ������ �����筮� �窨 */
#define Usb_clear_DIR()               (UEPSTAX &= ~MSK_DIR)
#define Usb_set_DIR()                 (UEPSTAX |= MSK_DIR)


#define Usb_set_tx_ready()            (UEPSTAX |= MSK_TXRDY)
#define Usb_clear_tx_ready()          (UEPSTAX &= ~MSK_TXRDY)
#define Usb_clear_tx_complete()       (UEPSTAX &= ~MSK_TXCMPL)
#define Usb_tx_complete()             (UEPSTAX & MSK_TXCMPL)
#define Usb_tx_ready()                (UEPSTAX & MSK_TXRDY)
#define Usb_clear_rx()                (UEPSTAX &= ~MSK_RXOUT)
#define Usb_clear_rx_bank0()          (UEPSTAX &= ~MSK_RXOUTB0)
#define Usb_clear_rx_bank1()          (UEPSTAX &= ~MSK_RXOUTB1)
#define Usb_rx_complete()             (UEPSTAX & MSK_RXOUTB0B1)

#define Usb_configure_address(x)      (USBADDR = (0x80 | x))
#define Usb_address()                 (USBADDR & 0x7F)


#define Usb_select_ep(e)              (UEPNUM = e)
#define Usb_configure_ep_type(x)      (UEPCONX = x)

#define Usb_read_byte()               (UEPDATX)
#define Usb_write_byte(x)             (UEPDATX = x)



#define Set_x2_mode()           (CKCON0 |= MSK_X2)

#define Pll_set_div(n)          (PLLDIV = n)
#define Pll_enable()            (PLLCON |= MSK_PLLEN) 


#define wSWAP(x) ((((x)>>8)&0x00FF)|(((x)<<8)&0xFF00))


