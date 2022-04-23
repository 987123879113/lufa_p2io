#include "icca.h"

#include <avr/pgmspace.h>

#include "config.h"
#include "p2io_task.h"
#include "io_bits.h"

const uint8_t ICCA_VERSION_INFO[44] PROGMEM = {
        0x03, 0x00, 0x00, 0x00,                                                               // Device ID
        0x00,                                                                                 // Flag
        0x01,                                                                                 // Major version
        0x01,                                                                                 // Minor, Supernova checks that this is >= 0
        0x00,                                                                                 // Version
        'I', 'C', 'C', 'A',                                                                   // Product code
        'O', 'c', 't', ' ', '2', '6', ' ', '2', '0', '0', '5', '\0', '\0', '\0', '\0', '\0',  // Date
        '1', '3', ' ', ':', ' ', '5', '5', ' ', ':', ' ', '0', '3', '\0', '\0', '\0', '\0'    // Time
};


acio_icca_device::acio_icca_device(int idx) {
    cardIdx = idx;
    memcpy_P(cardId, cardIds[cardIdx], 8);
}

bool acio_icca_device::device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) {
    const auto header = (ACIO_PACKET_HEADER*)packet;
    const auto code = BigEndian16(header->code);
    size_t outputResponseAddLen = 0;

    bool isEmptyResponse = false;
    if (code == 0x0002) {
        memcpy_P(&(outputResponse + *outputResponseLen)[outputResponseAddLen], ICCA_VERSION_INFO, 44);
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

        int curkey = 0;
        bool isInsertPressed = false;
        if (header->addr == 1) {
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_1) == 0)
                curkey |= 0b00001;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_4) == 0)
                curkey |= 0b00010;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_7) == 0)
                curkey |= 0b00011;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_00) == 0)
                curkey |= 0b00100;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_0) == 0)
                curkey |= 0b10000;

            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_2) == 0)
                curkey |= 0b00101;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_5) == 0)
                curkey |= 0b00110;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_8) == 0)
                curkey |= 0b00111;

            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_3) == 0)
                curkey |= 0b01001;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_6) == 0)
                curkey |= 0b01010;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P1_KEYPAD_9) == 0)
                curkey |= 0b01011;

            isInsertPressed = (cardIoStatus & P2IO_OTHER_CARDREADER_P1_TOGGLE_CARD) == 0;
        } else if (header->addr == 2) {
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_1) == 0)
                curkey |= 0b00001;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_4) == 0)
                curkey |= 0b00010;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_7) == 0)
                curkey |= 0b00011;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_00) == 0)
                curkey |= 0b00100;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_0) == 0)
                curkey |= 0b10000;

            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_2) == 0)
                curkey |= 0b00101;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_5) == 0)
                curkey |= 0b00110;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_8) == 0)
                curkey |= 0b00111;

            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_3) == 0)
                curkey |= 0b01001;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_6) == 0)
                curkey |= 0b01010;
            if ((cardIoStatus & P2IO_OTHER_CARDREADER_P2_KEYPAD_9) == 0)
                curkey |= 0b01011;

            isInsertPressed = (cardIoStatus & P2IO_OTHER_CARDREADER_P2_TOGGLE_CARD) == 0;
        }

        if (isInsertPressed)
        {
            if (!isCardInsertPressed)
            {
                if (accept || inserted)
                    inserted = !inserted;

                isCardInsertPressed = true;
            }
        }
        else
        {
            isCardInsertPressed = false;
        }

        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = inserted ? 2 : 1; // Reader status
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = (accept && inserted) ? 0x30 : 0; // Sensor status

        if (inserted)
            memcpy(&(outputResponse + *outputResponseLen)[outputResponseAddLen], cardId, 8);
        else
            memset(&(outputResponse + *outputResponseLen)[outputResponseAddLen], 0, 8);
        outputResponseAddLen += 8;

        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 0;
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = 3; // Keypad started

        uint8_t ev = 0;
        if (curkey & (keyLastActiveState ^ curkey)) {
            if (keyLastActiveEvent[0])
                ev = (keyLastActiveEvent[0] + 0x10) & 0xf0;

            ev |= 0x80 | curkey;
            keyLastActiveEvent[1] = keyLastActiveEvent[0];
            keyLastActiveEvent[0] = ev;
        }

        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = keyLastActiveEvent[0]; // Key events new
        (outputResponse + *outputResponseLen)[outputResponseAddLen++] = keyLastActiveEvent[1]; // Key event previous
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
