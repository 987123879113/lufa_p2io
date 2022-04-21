#include <string.h>

#include "common.h"

constexpr size_t responseBufferSize = 128;

class input_device {
   public:
    virtual ~input_device() = default;

    virtual void write(uint8_t *packet, size_t packetLen) = 0;

    void open() noexcept { isOpen = true; }
    void close() noexcept { isOpen = false; }

    virtual int read(uint8_t *buf, size_t bufLen, const size_t requestedLen) {
        size_t bufAddLen = 0;

        if (!isOpen || responseBufferLen == 0)
            return bufAddLen;

        const auto copyLen = requestedLen > responseBufferLen ? responseBufferLen : requestedLen;
        for (size_t i = 0; i < copyLen; i++)
            buf[bufLen + bufAddLen++] = responseBuffer[i];

        memmove(responseBuffer, responseBuffer + copyLen, responseBufferSize - copyLen);
        responseBufferLen -= copyLen;

        return bufAddLen;
    }

    void reset_buffer() {
        memset(responseBuffer, 0, responseBufferSize);
        responseBufferLen = 0;
    }

   protected:
    bool isOpen = false;

    uint8_t responseBuffer[responseBufferSize];
    size_t responseBufferLen = 0;

    void add_packet(const uint8_t *buffer, size_t bufferLen) {
        memcpy(responseBuffer + responseBufferLen, buffer, bufferLen);
        responseBufferLen += bufferLen;
    }

    void add_packet_byte(const uint8_t byte) {
        responseBuffer[responseBufferLen++] = byte;
    }
};
#pragma once
