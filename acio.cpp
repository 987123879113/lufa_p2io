#include "acio.h"

size_t acio_unescape_packet(uint8_t *buffer, size_t bufferLen) {
    uint8_t invert = 0;
    size_t outputIdx = 0;

    for (size_t i = 0; i < bufferLen; i++) {
        if (buffer[i] == 0xff) {
            invert = 0xff;
        } else {
            buffer[outputIdx++] = buffer[i] ^ invert;
            invert = 0;
        }
    }

    return outputIdx;
}

size_t acio_escape_packet(uint8_t *buffer, size_t bufferLen, size_t maxBufferLen) {
    size_t inputIdx = 0, outputIdx = 0;
    uint8_t invert = 0;

    // To make simplifying code simpler for Arduino, the assumption is that the max buffer
    // length will always be twice the buffer length for a worst case scenario.
    // So if the real packet size can be a max of 128 bytes, then the max buffer length will
    // be 256 bytes. This should really be dynamic and memory resized but I want to avoid
    // reallocs.
    // The worst case scenario where every byte needs to be escaped is a very unlikely
    // scenario so it should never be a problem.
    for (size_t i = 0; i < bufferLen && outputIdx < maxBufferLen; i++) {
        if (buffer[inputIdx] == 0xaa || buffer[inputIdx] == 0xff) {
            memmove(buffer + inputIdx + 1, buffer + inputIdx, bufferLen - i);
            inputIdx++;
            buffer[outputIdx++] = 0xff;
            invert = 0xff;
        }
        buffer[outputIdx++] = buffer[inputIdx++] ^ invert;
        invert = 0;
    }

    return outputIdx;
}

void acio_device::add_acio_device(int index, acio_device_base *device) noexcept {
    devices[index] = device;
    deviceCount++;
}

void acio_device::write(uint8_t *packet, size_t packetLen) {
    const size_t responseSize = 128;
    uint8_t response[responseSize] = {};
    size_t responseLen = 0;

    if (!isOpen || packetLen < sizeof(ACIO_PACKET_HEADER))
        return;

    size_t syncByteCount = 0;
    for (size_t i = 0; i < packetLen; i++) {
        if (packet[i] == ACIO_SYNC_BYTE)
            syncByteCount++;
        else
            break;
    }

    if (syncByteCount == packetLen) {
        add_packet(packet, packetLen);
        return;
    }

    const auto header = (ACIO_PACKET_HEADER *)packet;
    if (header->magic != ACIO_HEADER_MAGIC || packetLen < header->len + sizeof(ACIO_PACKET_HEADER) + 1)
        return;

    // Verify checksum
    const auto expectedChecksum = packet[header->len + sizeof(ACIO_PACKET_HEADER)];
    const auto calculatedChecksum = accumulate(packet + 1, header->len + sizeof(ACIO_PACKET_HEADER) - 1);

    if (expectedChecksum != calculatedChecksum)
        return;

    // Copy header from packet to response
    memcpy(&response[0], &packet[1], 4);
    responseLen += 5;

    const auto code = BigEndian16(header->code);
    bool isResponseAccepted = false;
    if (header->addr == 0 && code == 0x0001) {
        response[responseLen++] = deviceCount;
        isResponseAccepted = true;
    } else if (header->addr > 0 && header->addr <= 2) {
        isResponseAccepted = devices[header->addr - 1]->device_write(packet, response, &responseLen);
    }
    response[4] = responseLen - 5;

    if (isResponseAccepted) {
        response[0] |= 0x80; // Set response bit
        response[responseLen++] = accumulate(response, responseLen);
        responseLen = acio_escape_packet(response, responseLen, responseSize);
        add_packet_byte(0xaa);
        add_packet(response, responseLen);
    }
}
