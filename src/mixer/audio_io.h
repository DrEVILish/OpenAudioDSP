#ifndef OPEN_AUDIO_DSP_MIXER_AUDIO_IO_H
#define OPEN_AUDIO_DSP_MIXER_AUDIO_IO_H

#include "dsp1.h"

/* TDM words: FPGA channels are signed PCM, inter-DSP channels carry float bits.
 * The target owns double buffering and signals a completed block via audioReady.
 * Keep memory placement identical in the declarations and target definitions. */
extern int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE];
extern int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE];
extern bool bufferSwitch;

void audioIoInit(void);
/* Publish the processed block using the target's control transport. */
void audioIoPublishMeters(void);

#endif
