#include "acio.h"

class toysmarch_drumpad_device : public input_device {
    uint8_t stateIdx = 0;

    int read(uint8_t *buf, size_t bufLen, const size_t requestedLen) override;
    void write(uint8_t* packet, size_t packetLen) override;

   public:
    toysmarch_drumpad_device() {}
};

#pragma once
