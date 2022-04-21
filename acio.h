#include "input_device.h"

struct ACIO_PACKET_HEADER {
    uint8_t magic;
    uint8_t addr;
    uint16_t code;
    uint8_t seqNo;
    uint8_t len;
};

constexpr uint8_t ACIO_HEADER_MAGIC = 0xaa;
constexpr uint8_t ACIO_SYNC_BYTE = 0xaa;

size_t acio_unescape_packet(uint8_t *buffer, size_t bufferLen);
size_t acio_escape_packet(uint8_t *buffer, size_t bufferLen, size_t maxBufferLen);

class acio_device_base : public input_device {
   public:
    virtual bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) = 0;

   protected:
    uint8_t calculate_checksum(uint8_t *buffer, size_t len);
};

class acio_device : public acio_device_base {
    bool device_write(uint8_t *packet, uint8_t *outputResponse, size_t *outputResponseLen) { return false; }

    void write(uint8_t *packet, size_t packetLen);

   public:
    void add_acio_device(int index, acio_device_base *device) noexcept;

    uint8_t deviceCount = 0;
    acio_device_base *devices[2];
};
#pragma once
