#include "icca.h"

#include <avr/pgmspace.h>

#include "config.h"
#include "io_bits.h"
#include "p2io_task.h"

const uint8_t ICCA_VERSION_INFO[44] = {
    0x03, 0x00, 0x00, 0x00,                                                               // Device ID
    0x00,                                                                                 // Flag
    0x01,                                                                                 // Major version
    0x01,                                                                                 // Minor, Supernova checks that this is >= 0
    0x00,                                                                                 // Version
    'I', 'C', 'C', 'A',                                                                   // Product code
    'O', 'c', 't', ' ', '2', '6', ' ', '2', '0', '0', '5', '\0', '\0', '\0', '\0', '\0',  // Date
    '1', '3', ' ', ':', ' ', '5', '5', ' ', ':', ' ', '0', '3', '\0', '\0', '\0', '\0'    // Time
};

acio_icca_device::acio_icca_device(const CardInfo *playerCard) {
    cardInfo = playerCard;
}

bool acio_icca_device::device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) {
    const auto header = (ACIO_PACKET_HEADER *)packet;
    const auto code = BigEndian16(header->code);
    size_t outputResponseAddLen = 0;

    bool isEmptyResponse = false;
    if (code == 0x0002) {
        memcpy(&(outputResponse + *outputResponseLen)[outputResponseAddLen], ICCA_VERSION_INFO, 44);
        outputResponseAddLen += 44;
    } else if (code == 0x0080) {
        isEmptyResponse = true;
    } else if (code == 0x0131 || code == 0x0134 || code == 0x0135) {
        if (code == 0x0135) {
            switch (packet[7]) {
                case 0x00:
                    accept = false;
                    break;
                case 0x11:
                    accept = true;
                    break;
                case 0x12:
                    accept = false;
                    inserted = false;
                    isCardInsertPressed = false;
                    break;
                default:
                    break;
            }
        }

        uint8_t cardIoStatusVal = (cardIoStatus >> (6 * (header->addr - 1)));
        uint8_t curkey = cardIoStatusVal & 0b11111;
        uint8_t isInsertPressed = (cardIoStatusVal >> 5) & 1;

        if (isInsertPressed) {
            if (!isCardInsertPressed) {
                if (accept || inserted)
                    inserted = !inserted;

                isCardInsertPressed = true;
            }
        } else {
            isCardInsertPressed = false;
        }

        // Shitty code but shaves some bytes of progmem
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = inserted + 1;              // Reader status
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0x30 * accept * inserted;  // Sensor status

        if (cardInfo != nullptr)
            memcpy(&(outputResponse + *outputResponseLen)[outputResponseAddLen], cardInfo->cardId, 8);
        outputResponseAddLen += 8;

        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 3;  // Keypad started

        uint8_t ev = 0;
        if (curkey & (keyLastActiveState ^ curkey)) {
            if (keyLastActiveEvent[0])
                ev = (keyLastActiveEvent[0] + 0x10) & 0xf0;

            ev |= 0x80 | curkey;
            keyLastActiveEvent[1] = keyLastActiveEvent[0];
            keyLastActiveEvent[0] = ev;
        }

        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = keyLastActiveEvent[0];  // Key events new
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = keyLastActiveEvent[1];  // Key event previous
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;

        keyLastActiveState = curkey;
    } else {
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;

        if (code == 0x0003) {
            // Startup
            // Reset device state
            accept = false;
            inserted = false;
            isCardInsertPressed = false;
            keyLastActiveState = 0;
            keyLastActiveEvent[0] = keyLastActiveEvent[1] = 0;
        }
    }

    *outputResponseLen += outputResponseAddLen;
    return outputResponseAddLen > 0 || isEmptyResponse;
}
