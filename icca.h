#include "acio.h"

class acio_icca_device : public acio_device_base {
   protected:
    uint8_t keyLastActiveState = 0;
    uint8_t keyLastActiveEvent[2] = {0, 0};
    bool accept = false;
    bool inserted = false;
    bool isCardInsertPressed = false;
    bool isKeypadSwapped = false;
    bool isKeypadSwapPressed = false;

    bool cardLoaded = false;
    uint8_t cardId[8] = {0};
    uint8_t cardIdx = 0;

    void write(uint8_t *packet, size_t packetLen) {}

   public:
    acio_icca_device(int idx);

    bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen);
};
#pragma once
