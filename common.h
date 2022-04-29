#ifndef __COMMON_H__
#define __COMMON_H__
#include <stddef.h>
#include <stdint.h>

inline uint16_t BigEndian16(uint16_t val) {
    return ((val & 0xff) << 8) | (val >> 8);
}

inline uint8_t accumulate(uint8_t *buffer, size_t len) {
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += buffer[i];
    }
    return sum;
}

#endif