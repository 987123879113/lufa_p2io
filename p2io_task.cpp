// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.cpp
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.h

#include "p2io_task.h"

#include "acio.h"
#include "common.h"
#include "config.h"
#include "p2io.h"

struct P2IO_PACKET_HEADER {
    uint8_t magic;
    uint8_t len;
    uint8_t seqNo;
    uint8_t cmd;
};

const uint8_t P2IO_HEADER_MAGIC = 0xaa;

enum {
    // cmd_... names are from internal namings from symbols
    P2IO_CMD_GET_VERSION = 0x01,       // cmd_getver
    P2IO_CMD_RESEND = 0x02,            // resend_cmd
    P2IO_CMD_FWRITEMODE = 0x03,        // cmd_fwritemode 0x03aa
    P2IO_CMD_SET_WATCHDOG = 0x05,      // cmd_watchdog
    P2IO_CMD_SET_AV_MASK = 0x22,       // cmd_avmask
    P2IO_CMD_GET_AV_REPORT = 0x23,     // cmd_avreport
    P2IO_CMD_LAMP_OUT = 0x24,          // cmd_lampout = 0x24, cmd_out_all = 0x24ff
    P2IO_CMD_DALLAS = 0x25,            // cmd_dallas
    P2IO_CMD_SEND_IR = 0x26,           // cmd_irsend
    P2IO_CMD_READ_DIPSWITCH = 0x27,    // cmd_dipsw
    P2IO_CMD_GET_JAMMA_POR = 0x28,     // cmd_jamma_por
    P2IO_CMD_PORT_READ = 0x29,         // cmd_portread
    P2IO_CMD_PORT_READ_POR = 0x2a,     // cmd_portread_por
    P2IO_CMD_JAMMA_START = 0x2f,       // cmd_jammastart
    P2IO_CMD_COIN_STOCK = 0x31,        // cmd_coinstock
    P2IO_CMD_COIN_COUNTER = 0x32,      // cmd_coincounter
    P2IO_CMD_COIN_BLOCKER = 0x33,      // cmd_coinblocker
    P2IO_CMD_COIN_COUNTER_OUT = 0x34,  // cmd_coincounterout
    P2IO_CMD_SCI_SETUP = 0x38,         // cmd_scisetup
    P2IO_CMD_SCI_WRITE = 0x3a,         // cmd_sciwrite
    P2IO_CMD_SCI_READ = 0x3b           // cmd_sciread
};

enum {
    P2IO_AVREPORT_MODE_15KHZ = 0x80,  // DDR says it's 15kHz, bootloader says 16kHz
    P2IO_AVREPORT_MODE_31KHZ = 0,
};

bool FORCE_31KHZ = false;
uint8_t dipSwitch[4] = {DIPSW1, DIPSW2, DIPSW3, DIPSW4};
uint16_t coinsInserted[2] = {0, 0};
uint32_t jammaIoStatus = 0xfffffffe;
uint32_t otherIoStatus = 0xffffffff;
uint16_t analogIoStatus[3] = {0, 0, 0};
bool dongleIsLoaded[2] = {false, false};
input_device *serialDevices[2] = {NULL, NULL};

int requestedDongle = -1;

void P2IO_Task() {
    if (USB_DeviceState != DEVICE_STATE_Configured)
        return;

    constexpr size_t DATA_OUT_LEN = CDC_TXRX_EPSIZE;
    uint8_t dataIn[CDC_TXRX_EPSIZE] = {0};
    uint8_t dataOut[DATA_OUT_LEN] = {0};
    size_t responseLen = 0;

    Endpoint_SelectEndpoint(CDC_RX_EPADDR);
    if (Endpoint_IsOUTReceived()) {
        if (Endpoint_IsReadWriteAllowed()) {
            // TODO: Make this read in full packets before processing, based on the packet size header byte
            uint16_t bytesProcessed = 0;  // This allows for partial transfers
            Endpoint_Read_Stream_LE(dataIn, CDC_TXRX_EPSIZE, &bytesProcessed);

            P2IO_PACKET_HEADER *header = (P2IO_PACKET_HEADER *)&dataIn[0];

            if (header->magic == P2IO_HEADER_MAGIC) {
                dataOut[0] = P2IO_HEADER_MAGIC;
                dataOut[1] = 2;
                dataOut[2] = dataIn[2];
                dataOut[3] = 0;

                if (header->cmd == P2IO_CMD_GET_VERSION) {
                    dataOut[1] += 7;

                    memcpy(&dataOut[4], "D44\0", 4);
                    dataOut[8] = 1;
                    dataOut[9] = 6;
                    dataOut[10] = 4;
                } else if (header->cmd == P2IO_CMD_SET_AV_MASK) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_GET_AV_REPORT) {
                    dataOut[1] += 1;
                    dataOut[4] = FORCE_31KHZ ? P2IO_AVREPORT_MODE_31KHZ : P2IO_AVREPORT_MODE_15KHZ;
                }

                else if (header->cmd == P2IO_CMD_READ_DIPSWITCH) {
                    uint8_t val = 0;
                    for (size_t i = 0; i < 4; i++)
                        val |= (1 << (3 - i)) * dipSwitch[i];

                    dataOut[1] += 1;
                    dataOut[4] = val & 0x7f;
                }

                else if (header->cmd == P2IO_CMD_COIN_STOCK) {
                    const uint8_t resp[] = {
                        0,
                        (uint8_t)(coinsInserted[0] >> 8),
                        (uint8_t)coinsInserted[0],
                        (uint8_t)(coinsInserted[1] >> 8),
                        (uint8_t)coinsInserted[1],
                    };
                    dataOut[1] += sizeof(resp);
                    memcpy(&dataOut[4], resp, sizeof(resp));
                } else if (header->cmd == P2IO_CMD_COIN_COUNTER) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_COIN_BLOCKER) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_COIN_COUNTER_OUT) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                }

                else if (header->cmd == P2IO_CMD_LAMP_OUT) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_PORT_READ) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_PORT_READ_POR) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_SEND_IR) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_SET_WATCHDOG) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_JAMMA_START) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_GET_JAMMA_POR) {
                    dataOut[1] += 4;
                    memset(&dataOut[4], 0, 4);
                } else if (header->cmd == P2IO_CMD_FWRITEMODE) {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                } else if (header->cmd == P2IO_CMD_DALLAS) {
                    uint8_t subCmd = dataIn[4];

                    dataOut[1] += 1;
                    dataOut[4] = 0;

                    if (subCmd == 0 || subCmd == 1 || subCmd == 2) {
                        // Dallas Read SID/Mem
                        if (subCmd != 2)
                            requestedDongle = subCmd;

                        if (requestedDongle >= 0 && dongleIsLoaded[requestedDongle]) {
                            dataOut[1] += 8 + 32;
                            dataOut[4] = 1;
                            memcpy_P(&dataOut[5], donglePayload[requestedDongle], 40);
                        }
                    } else if (subCmd == 3) {
                        // TODO: is this ever used?
                        // Dallas Write Mem
                        dataOut[1] += 8 + 32;
                        dataOut[4] = 1;
                        memset(&dataOut[5], 0, 40);
                    }
                } else if (header->cmd == P2IO_CMD_SCI_SETUP) {
                    const auto port = dataIn[4];
                    const auto cmd = dataIn[5];
                    const auto param = dataIn[6];

                    dataOut[1] += 1;
                    dataOut[4] = 0;

                    const auto device = serialDevices[port];
                    if (device != nullptr) {
                        if (cmd == 0)
                            device->open();
                        else if (header->cmd == 0xff)
                            device->close();
                    }
                } else if (header->cmd == P2IO_CMD_SCI_WRITE) {
                    const auto port = dataIn[4];
                    const auto packetLen = dataIn[5];

                    const auto device = serialDevices[port];
                    if (device != nullptr) {
                        device->reset_buffer();
                        acio_unescape_packet(&dataIn[6], bytesProcessed);
                        device->write(&dataIn[6], packetLen);
                    }

                    dataOut[1] += 1;
                    dataOut[4] = packetLen;
                } else if (header->cmd == P2IO_CMD_SCI_READ) {
                    const auto port = dataIn[4];
                    const auto requestedLen = dataIn[5];

                    dataOut[1] += 1;
                    dataOut[4] = 0;

                    const auto device = serialDevices[port];
                    if (device != nullptr && requestedLen > 0) {
                        dataOut[4] = device->read(&dataOut[5], 0, requestedLen);
                        responseLen = acio_escape_packet(&dataOut[5], dataOut[4], DATA_OUT_LEN - 5);
                        responseLen += dataOut[1];
                        dataOut[1] += dataOut[4];
                        device->reset_buffer();
                    }
                } else {
                    dataOut[1] += 1;
                    dataOut[4] = 0;
                }

                if (header->cmd != P2IO_CMD_SCI_READ)
                    responseLen = dataOut[1];

                responseLen += 2;
            }
        }

        Endpoint_ClearOUT();
    }

    Endpoint_SelectEndpoint(CDC_TX_EPADDR);
    // Send all bytes in buffer
    while (responseLen > 0) {
        if (Endpoint_IsINReady()) {
            const auto sendLen = responseLen > CDC_TXRX_EPSIZE ? CDC_TXRX_EPSIZE : responseLen;
            Endpoint_Write_Stream_LE(dataOut, sendLen, NULL);
            Endpoint_ClearIN();
            responseLen -= sendLen;
        }

        if (responseLen > 0)
            delay(1);
    }

    Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPADDR);
    if (Endpoint_IsINReady()) {
        uint8_t resp[12] = {0};
        uint32_t *jammaIo = (uint32_t *)&resp[0];
        uint16_t *analogIo = (uint16_t *)&resp[4];

        jammaIoStatus ^= 2;  // Watchdog bit

        *jammaIo = jammaIoStatus;
        analogIo[0] = analogIoStatus[0];
        analogIo[1] = analogIoStatus[1];
        analogIo[2] = analogIoStatus[2];

        Endpoint_Write_Stream_LE(resp, 12, NULL);
        Endpoint_ClearIN();
    }
}
