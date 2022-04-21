#include "thrilldrive_handle.h"

#include <avr/pgmspace.h>

const uint8_t TD_HANDLE_VERSION_INFO[44] PROGMEM = {
    0x03, 0x00, 0x00, 0x00,                                                               // Device ID
    0x00,                                                                                 // Flag
    0x01,                                                                                 // Major version
    0x01,                                                                                 // Minor
    0x00,                                                                                 // Version
    'H', 'N', 'D', 'L',                                                                   // Product code
    'O', 'c', 't', ' ', '2', '6', ' ', '2', '0', '0', '5', '\0', '\0', '\0', '\0', '\0',  // Date
    '1', '3', ' ', ':', ' ', '5', '5', ' ', ':', ' ', '0', '3', '\0', '\0', '\0', '\0'    // Time
};

bool thrilldrive_handle_device::device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) {
    const auto header = (ACIO_PACKET_HEADER*)packet;
    const auto code = BigEndian16(header->code);
    size_t outputResponseAddLen = 0;

    if (code == 0x0002) {
        // Not the real information for this device
        memcpy_P(outputResponse + outputResponseAddLen + *outputResponseLen, TD_HANDLE_VERSION_INFO, 44);
        outputResponseAddLen += 44;
    } else {
        memset(outputResponse + outputResponseAddLen + *outputResponseLen, 0, 4);
        outputResponseAddLen++;

        if (code == 0x0100) {
            // Init
            wheelForceFeedback = 0;
            wheelCalibrationHack = false;
        } else if (code == 0x0120) {
            // Force feedback
            const int8_t ffb1 = packet[6];  // Controls X?
            const int8_t ffb2 = packet[7];
            const int8_t ffb3 = packet[8];
            const int8_t ffb4 = packet[9];

            int16_t* val = (int16_t*)(outputResponse + *outputResponseLen)[outputResponseAddLen + 1];
            *val = BigEndian16(0);  // What is this value?
            outputResponseAddLen += 3;

            wheelForceFeedback = ffb1;
            wheelCalibrationHack = ffb4 == -2;
        }
    }

    auto v = *outputResponseLen;
    *outputResponseLen = v + outputResponseAddLen;
    return outputResponseAddLen > 0;
}
