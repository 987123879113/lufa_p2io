#include "toysmarch_drumpad.h"

#include "common.h"
#include "io_bits.h"
#include "p2io_task.h"

int toysmarch_drumpad_device::read(uint8_t *buf, size_t bufLen, const size_t requestedLen) {
    // The game always expects to be able to read the state from the device, regardless of what was written
    // If this packet can't be read then the game will throw a drum I/O error
    uint8_t state[9] = {
        (stateIdx++) & 7,
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P1_CYMBAL) != 0),
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P1_DRUML) != 0),
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P1_DRUMR) != 0),
        0,
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P2_CYMBAL) != 0),
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P2_DRUML) != 0),
        128 * ((otherIoStatus & P2IO_OTHER_TOYSMARCH_P2_DRUMR) != 0),
        0
    };

    buf[bufLen++] = 0xaa;
    memcpy(buf + bufLen, state, 9);
    bufLen += 9;
    buf[bufLen++] = accumulate(state, 9);

    return 9 + 2;
}

void toysmarch_drumpad_device::write(uint8_t *packet, size_t packetLen) {
    // Keep alive packets
    // These packets are sent every 1 sec and don't expect a direct response
    // aa 11 11 11 33
    // aa 22 22 22 66
}
