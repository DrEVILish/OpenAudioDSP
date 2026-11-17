#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "dsp1.h"
#include "sharc_board.h"
#include "audio_io.h"

extern int audioTx_tcb[8][BUFFER_COUNT][4];
extern int audioRx_tcb[8][BUFFER_COUNT][4];

// function prototypes
void systemPllInit(void);
void systemExternalMemoryInit(void);
void systemSruInit(void);
void systemSportInit(void);
void audioRxISR(uint32_t iid, void *handlerarg);

#endif
