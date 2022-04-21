#include "acio.h"

class thrilldrive_handle_device : public acio_device_base {
   private:
    void write(uint8_t *packet, size_t packetLen) {}

   public:
    thrilldrive_handle_device() noexcept {}

    bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen);

    int8_t wheelForceFeedback = 0;
    bool wheelCalibrationHack = false;
};

#pragma once
