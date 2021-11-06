// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.cpp
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.h

#include "p2io_task.h"
#include "p2io.h"

struct P2IO_PACKET_HEADER
{
  uint8_t magic;
  uint8_t len;
  uint8_t seqNo;
  uint8_t cmd;
};

const uint32_t P2IO_JAMMA_IO_TEST = 0x10000000;
const uint32_t P2IO_JAMMA_IO_COIN1 = 0x20000000;
const uint32_t P2IO_JAMMA_IO_COIN2 = 0x80000000;
const uint32_t P2IO_JAMMA_IO_SERVICE = 0x40000000;
const uint32_t P2IO_JAMMA_IO_SERVICE2 = 0x00000080;

const uint32_t P2IO_JAMMA_DDR_P1_START = 0x00000100;
const uint32_t P2IO_JAMMA_DDR_P1_LEFT = 0x00004000;
const uint32_t P2IO_JAMMA_DDR_P1_RIGHT = 0x00008000;
const uint32_t P2IO_JAMMA_DDR_P1_FOOT_UP = 0x00000200;
const uint32_t P2IO_JAMMA_DDR_P1_FOOT_DOWN = 0x00000400;
const uint32_t P2IO_JAMMA_DDR_P1_FOOT_LEFT = 0x00000800;
const uint32_t P2IO_JAMMA_DDR_P1_FOOT_RIGHT = 0x00001000;

const uint32_t P2IO_JAMMA_DDR_P2_START = 0x00010000;
const uint32_t P2IO_JAMMA_DDR_P2_LEFT = 0x00400000;
const uint32_t P2IO_JAMMA_DDR_P2_RIGHT = 0x00800000;
const uint32_t P2IO_JAMMA_DDR_P2_FOOT_UP = 0x00020000;
const uint32_t P2IO_JAMMA_DDR_P2_FOOT_DOWN = 0x00040000;
const uint32_t P2IO_JAMMA_DDR_P2_FOOT_LEFT = 0x00080000;
const uint32_t P2IO_JAMMA_DDR_P2_FOOT_RIGHT = 0x00100000;

const uint8_t P2IO_HEADER_MAGIC = 0xaa;

enum
{
  // cmd_... names are from internal namings from symbols
  P2IO_CMD_GET_VERSION = 0x01,      // cmd_getver
  P2IO_CMD_RESEND = 0x02,           // resend_cmd
  P2IO_CMD_FWRITEMODE = 0x03,       // cmd_fwritemode 0x03aa
  P2IO_CMD_SET_WATCHDOG = 0x05,     // cmd_watchdog
  P2IO_CMD_SET_AV_MASK = 0x22,      // cmd_avmask
  P2IO_CMD_GET_AV_REPORT = 0x23,    // cmd_avreport
  P2IO_CMD_LAMP_OUT = 0x24,         // cmd_lampout = 0x24, cmd_out_all = 0x24ff
  P2IO_CMD_DALLAS = 0x25,           // cmd_dallas
  P2IO_CMD_SEND_IR = 0x26,          // cmd_irsend
  P2IO_CMD_READ_DIPSWITCH = 0x27,   // cmd_dipsw
  P2IO_CMD_GET_JAMMA_POR = 0x28,    // cmd_jamma_por
  P2IO_CMD_PORT_READ = 0x29,        // cmd_portread
  P2IO_CMD_PORT_READ_POR = 0x2a,    // cmd_portread_por
  P2IO_CMD_JAMMA_START = 0x2f,      // cmd_jammastart
  P2IO_CMD_COIN_STOCK = 0x31,       // cmd_coinstock
  P2IO_CMD_COIN_COUNTER = 0x32,     // cmd_coincounter
  P2IO_CMD_COIN_BLOCKER = 0x33,     // cmd_coinblocker
  P2IO_CMD_COIN_COUNTER_OUT = 0x34, // cmd_coincounterout
  P2IO_CMD_SCI_SETUP = 0x38,        // cmd_scisetup
  P2IO_CMD_SCI_WRITE = 0x3a,        // cmd_sciwrite
  P2IO_CMD_SCI_READ = 0x3b          // cmd_sciread
};

enum
{
  P2IO_AVREPORT_MODE_15KHZ = 0x80, // DDR says it's 15kHz, bootloader says 16kHz
  P2IO_AVREPORT_MODE_31KHZ = 0,
};

bool FORCE_31KHZ = false;
uint8_t dipSwitch[4] = {0, 1, 0, 0};
uint16_t coinsInserted[2] = {0, 0};
uint32_t jammaIoStatus = 0xfffffffe;

void P2IO_Task()
{
  if (USB_DeviceState != DEVICE_STATE_Configured)
    return;

  uint8_t dataIn[CDC_TXRX_EPSIZE] = {0};
  uint8_t dataOut[CDC_TXRX_EPSIZE] = {0};
  size_t responseLen = 0;

  Endpoint_SelectEndpoint(CDC_RX_EPADDR);
  if (Endpoint_IsOUTReceived())
  {
    if (Endpoint_IsReadWriteAllowed())
    {
      Endpoint_Read_Stream_LE(dataIn, CDC_TXRX_EPSIZE, NULL);

      P2IO_PACKET_HEADER *header = (P2IO_PACKET_HEADER *)&dataIn[0];

      if (header->magic == P2IO_HEADER_MAGIC)
      {
        dataOut[0] = P2IO_HEADER_MAGIC;
        dataOut[1] = 2;
        dataOut[2] = dataIn[2];
        dataOut[3] = dataIn[3];

        if (header->cmd == P2IO_CMD_GET_VERSION)
        {
          dataOut[1] += 7;

          memcpy(&dataOut[4], "D44\0", 4);
          dataOut[8] = 1;
          dataOut[9] = 6;
          dataOut[10] = 4;
        }
        else if (header->cmd == P2IO_CMD_SET_AV_MASK)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_GET_AV_REPORT)
        {
          dataOut[1] += 1;
          dataOut[4] = FORCE_31KHZ ? P2IO_AVREPORT_MODE_31KHZ : P2IO_AVREPORT_MODE_15KHZ;
        }

        else if (header->cmd == P2IO_CMD_READ_DIPSWITCH)
        {
          uint8_t val = 0;
          for (size_t i = 0; i < 4; i++)
            val |= (1 << (3 - i)) * dipSwitch[i];

          dataOut[1] += 1;
          dataOut[4] = val & 0x7f;
        }

        else if (header->cmd == P2IO_CMD_COIN_STOCK)
        {
          const uint8_t resp[] = {
              0,
              (uint8_t)(coinsInserted[0] >> 8),
              (uint8_t)coinsInserted[0],
              (uint8_t)(coinsInserted[1] >> 8),
              (uint8_t)coinsInserted[1],
          };
          dataOut[1] += sizeof(resp);
          memcpy(&dataOut[4], resp, sizeof(resp));
        }
        else if (header->cmd == P2IO_CMD_COIN_COUNTER)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_COIN_BLOCKER)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_COIN_COUNTER_OUT)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }

        else if (header->cmd == P2IO_CMD_LAMP_OUT)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_PORT_READ)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_PORT_READ_POR)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_SEND_IR)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_SET_WATCHDOG)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_JAMMA_START)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_GET_JAMMA_POR)
        {
          dataOut[1] += 4;
          memset(&dataOut[4], 0, 4);
        }
        else if (header->cmd == P2IO_CMD_FWRITEMODE)
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }
        else if (header->cmd == P2IO_CMD_DALLAS)
        {
          dataOut[1] += 8 + 32;
          dataOut[4] = 1;
        }
        else
        {
          dataOut[1] += 1;
          dataOut[4] = 0;
        }

        responseLen = dataOut[1] + 2;
      }
    }

    Endpoint_ClearOUT();
  }

  Endpoint_SelectEndpoint(CDC_TX_EPADDR);
  if (Endpoint_IsINReady())
  {
    if (responseLen > 0)
    {
      Endpoint_Write_Stream_LE(dataOut, responseLen, NULL);
      Endpoint_ClearIN();
    }
  }

  Endpoint_SelectEndpoint(CDC_NOTIFICATION_EPADDR);
  if (Endpoint_IsINReady())
  {
    uint8_t resp[12] = {0};
    uint32_t *jammaIo = (uint32_t *)&resp[0];
    uint16_t *analogIo = (uint16_t *)&resp[4];

    jammaIoStatus ^= 2; // Watchdog bit

    /*
    if (k == '2') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_UP;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_UP;
    }
    if (k == '4') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_LEFT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_LEFT;
    }
    if (k == '6') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
    }
    if (k == '8') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_DOWN;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_DOWN;
    }


    if (k == '1') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_LEFT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_LEFT;
    }

    if (k == '3') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_RIGHT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_RIGHT;
    }


    if (k == '*') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_LEFT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P2_LEFT;
    }

    if (k == '#') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_RIGHT;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P2_RIGHT;
    }



    if (k == 'A') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_START;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P1_START;
    }

    if (k == 'B') {
      jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_START;
    } else {
      jammaIoStatus |= P2IO_JAMMA_DDR_P2_START;
    }

    if (k == 'C') {
      jammaIoStatus &= ~P2IO_JAMMA_IO_SERVICE;
    } else {
      jammaIoStatus |= P2IO_JAMMA_IO_SERVICE;
    }

    if (k == 'D') {
      jammaIoStatus &= ~P2IO_JAMMA_IO_TEST;
    } else {
      jammaIoStatus |= P2IO_JAMMA_IO_TEST;
    }
*/

    *jammaIo = jammaIoStatus;

    Endpoint_Write_Stream_LE(resp, 12, NULL);
    Endpoint_ClearIN();
  }
}
