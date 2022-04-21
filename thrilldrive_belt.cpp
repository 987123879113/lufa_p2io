#include "thrilldrive_belt.h"

#include <avr/pgmspace.h>

#include "io_bits.h"
#include "p2io_task.h"

const uint8_t TD_BELT_VERSION_INFO[44] PROGMEM = {
    0x03, 0x00, 0x00, 0x00,                                                               // Device ID
    0x00,                                                                                 // Flag
    0x01,                                                                                 // Major version
    0x01,                                                                                 // Minor
    0x00,                                                                                 // Version
    'B', 'E', 'L', 'T',                                                                   // Product code
    'O', 'c', 't', ' ', '2', '6', ' ', '2', '0', '0', '5', '\0', '\0', '\0', '\0', '\0',  // Date
    '1', '3', ' ', ':', ' ', '5', '5', ' ', ':', ' ', '0', '3', '\0', '\0', '\0', '\0'    // Time
};

bool thrilldrive_belt_device::device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) {
    const auto header = (ACIO_PACKET_HEADER *)packet;
    const auto code = BigEndian16(header->code);
    size_t outputResponseAddLen = 0;

    if ((otherIoStatus & P2IO_OTHER_THRILLDRIVE_BELT) == 0) {
        if (!seatBeltButtonPressed)
            seatBeltStatus = !seatBeltStatus;

        seatBeltButtonPressed = true;
    } else {
        seatBeltButtonPressed = false;
    }

    if (code == 0x0002) {
        // Not the real information for this device
        memcpy_P(outputResponse + outputResponseAddLen + *outputResponseLen, TD_BELT_VERSION_INFO, 44);
        outputResponseAddLen += 44;
    } else {
        memset(outputResponse + outputResponseAddLen + *outputResponseLen, 0, 4);
        outputResponseAddLen++;

        if (code == 0x0100) {
            // Init
            seatBeltStatus = false;
            seatBeltButtonPressed = false;
        } else if (code == 0x0111) {
            // ?
            outputResponseAddLen += 3;
        } else if (code == 0x0113) {
            (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;
            (outputResponse + *outputResponseLen)[outputResponseAddLen++] = seatBeltStatus ? 0 : 0xff;  // 0 = fastened
        }
    }

    *outputResponseLen += outputResponseAddLen;
    return outputResponseAddLen > 0;
}
