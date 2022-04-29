#include "acio.h"
#include "config.h"

class acio_icca_device : public acio_device_base {
   protected:
    uint8_t keyLastActiveState = 0;
    uint8_t keyLastActiveEvent[2] = {0, 0};
    uint8_t accept = false;
    uint8_t inserted = false;
    uint8_t isCardInsertPressed = false;
    uint8_t isKeypadSwapped = false;
    uint8_t isKeypadSwapPressed = false;

    uint8_t cardLoaded = false;
    const CardInfo *cardInfo = nullptr;

    void write(uint8_t *packet, size_t packetLen) {}

   public:
    acio_icca_device(const CardInfo *playerCard);

    bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen);
};
#pragma once
