#include "input_device.h"

class extio_device : public input_device {
    enum {
        EXTIO_LIGHT_PANEL_UP = 0x40,
        EXTIO_LIGHT_PANEL_DOWN = 0x20,
        EXTIO_LIGHT_PANEL_LEFT = 0x10,
        EXTIO_LIGHT_PANEL_RIGHT = 0x08,

        EXTIO_LIGHT_SENSOR_UP = 0x10,
        EXTIO_LIGHT_SENSOR_DOWN = 0x18,
        EXTIO_LIGHT_SENSOR_LEFT = 0x20,
        EXTIO_LIGHT_SENSOR_RIGHT = 0x28,
        EXTIO_LIGHT_SENSOR_ALL = 0x08,

        EXTIO_LIGHT_NEON = 0x40,
    };

   public:
    extio_device();

    void write(uint8_t *packet, size_t bufferLen);
};
#pragma once
