#define Enable_interrupt()      (EA = 1)
#define Disable_interrupt()     (EA = 0)

/* Включение/выключение модуля USB  */
#define Usb_enable()               (USBCON |= MSK_USBE)
#define Usb_disable()              (USBCON &= ~MSK_USBE)
/* Подключение/отключение от линии    */
#define Usb_detach()               (USBCON |= MSK_DETACH)
#define Usb_attach()               (USBCON &= ~MSK_DETACH)
/* Конфигурационный бит               */
#define Usb_set_CONFG()            (USBCON |= MSK_CONFG)
#define Usb_clear_CONFG()          (USBCON &= ~MSK_CONFG)
/* Бит разрешения функции адресации   */
#define Usb_set_FADDEN()           (USBCON |= MSK_FADDEN)
#define Usb_clear_FADDEN()         (USBCON &= ~MSK_FADDEN)
/* Бит приостановки синхронизации USB */
#define Usb_set_suspend_clock()    (USBCON |= MSK_SUSPCLK)
#define Usb_clear_suspend_clock()  (USBCON &= ~MSK_SUSPCLK)


/* Флаг прерывания окончания сброса */
#define Usb_clear_reset()             (USBINT &= ~MSK_EORINT)
#define Usb_reset()                   (USBINT & MSK_EORINT)
/* Флаг прерывания пробуждения ЦП   */
#define Usb_clear_resume()            (USBINT &= ~MSK_WUPCPU)
#define Usb_resume()                  (USBINT & MSK_WUPCPU)
/* Флаг прерывания при обнаружении начала кадра */
#define Usb_clear_sof()               (USBINT &= ~MSK_SOFINT)
#define Usb_sof()                     (USBINT & MSK_SOFINT)
/* Флаг прерывания при приостановке             */ 
#define Usb_clear_suspend()           (USBINT &= ~MSK_SPINT)
#define Usb_suspend()                 (USBINT & MSK_SPINT)

/* Разрешение/запрещение прерываний от конечных точек */
#define Usb_enable_ep_int(e)          (UEPIEN |= (0x01 << e))
#define Usb_disable_ep_int(e)         (UEPIEN &= ~(0x01 << e))

/* UEPINT - (r/o) регистр прерываний оконечных USB точек */
#define Usb_endpoint_interrupt()      (UEPINT != 0 )


/* Бит запроса остановки установления связи  */
#define Usb_set_stall_request()       (UEPSTAX |= MSK_STALLRQ)
#define Usb_clear_stall_request()     (UEPSTAX &= ~MSK_STALLRQ)
#define Usb_clear_stalled()           (UEPSTAX &= ~MSK_STALLED)
#define Usb_stall_sent()              (UEPSTAX & MSK_STALLED)
/* Флаг прерывания при приостановке посылки   */
/* Флаг прерывания при обнаружении CRC ошибки */
#define Usb_stall_requested()         (UEPSTAX & MSK_STALLRQ)
/* Флаг прерывания при получении пакета SETUP */
#define Usb_clear_rx_setup()          (UEPSTAX &= ~MSK_RXSETUP)
#define Usb_setup_received()          (UEPSTAX & MSK_RXSETUP)
/* Бит управления направлением оконечной точки */
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


