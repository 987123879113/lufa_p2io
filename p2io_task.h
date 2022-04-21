#ifndef _P2IO_TASK_H_
#define _P2IO_TASK_H_

#include <stdint.h>

/* Function Prototypes: */
#ifdef __cplusplus
extern "C"
{
#endif
  void P2IO_Task();
  
  extern uint32_t jammaIoStatus;
  extern uint16_t analogIoStatus[3];
  extern uint16_t coinsInserted[2];

#ifdef __cplusplus
}
#endif
#endif
