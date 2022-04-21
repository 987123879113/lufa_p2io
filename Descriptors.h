#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include "LUFAConfig.h"

#include <avr/pgmspace.h>

#include <LUFA/LUFA/Drivers/USB/USB.h>

#define CDC_TX_EPADDR (ENDPOINT_DIR_IN | 1)
#define CDC_RX_EPADDR (ENDPOINT_DIR_OUT | 2)
#define CDC_NOTIFICATION_EPADDR (ENDPOINT_DIR_IN | 3)

#define CDC_NOTIFICATION_EPSIZE 16
#define CDC_TXRX_EPSIZE 64

typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;

    USB_Descriptor_Interface_t CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t CDC_DataInterruptEndpoint;
    USB_Descriptor_Endpoint_t CDC_DataInEndpoint;
    USB_Descriptor_Endpoint_t CDC_DataOutEndpoint;
} USB_Descriptor_Configuration_t;

enum StringDescriptors_t {
    STRING_ID_Language = 0,     /**< Supported Languages string descriptor ID (must be zero) */
    STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
    STRING_ID_Product = 2,      /**< Product string ID */
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void **const DescriptorAddress)
    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif
