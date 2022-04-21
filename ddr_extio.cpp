#include "ddr_extio.h"

extio_device::extio_device() {
}

// Reference: https://github.com/nchowning/open-io/blob/master/extio-emulator.ino
void extio_device::write(uint8_t *packet, size_t bufferLen) {
    if (!isOpen || bufferLen != 4)
        return;

    /*
     * DDR:
     * 80 00 40 40 CCFL
     * 90 00 00 10 1P FOOT LEFT
     * c0 00 00 40 1P FOOT UP
     * 88 00 00 08 1P FOOT RIGHT
     * a0 00 00 20 1P FOOT DOWN
     * 80 10 00 10 2P FOOT LEFT
     * 80 40 00 40 2P FOOT UP
     * 80 08 00 08 2P FOOT RIGHT
     * 80 20 00 20 2P FOOT DOWN
     */

    const auto expectedChecksum = packet[3];
    const uint8_t calculatedChecksum = (packet[0] + packet[1] + packet[2]) & 0x7f;

    if (calculatedChecksum != expectedChecksum) {
        return;
    }

    add_packet_byte(0x11);
}
