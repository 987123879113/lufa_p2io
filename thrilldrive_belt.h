#include "acio.h"

class thrilldrive_belt_device : public acio_device_base {
   private:
    bool seatBeltStatus = false;
    bool seatBeltButtonPressed = false;

    void write(uint8_t *packet, size_t packetLen) {}

   public:
    thrilldrive_belt_device() noexcept {}

    bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen);
};
#pragma once
