#include "p2io.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
    {
        .Config =
            {
                .ControlInterfaceNumber = 0,
                .NotificationEndpoint =
                    {
                        .Address = CDC_NOTIFICATION_EPADDR,
                        .Size = CDC_NOTIFICATION_EPSIZE,
                        .Banks = 1,
                    },
                .DataINEndpoint =
                    {
                        .Address = CDC_TX_EPADDR,
                        .Size = CDC_TXRX_EPSIZE,
                        .Banks = 1,
                    },
                .DataOUTEndpoint =
                    {
                        .Address = CDC_RX_EPADDR,
                        .Size = CDC_TXRX_EPSIZE,
                        .Banks = 1,
                    },
            },
};

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void) {
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    /* Hardware Initialization */
    USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void) {
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void) {
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void) {
    bool ConfigSuccess = true;

    ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void) {
}
