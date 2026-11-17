/* X32 ADSP-21371 SPORT DMA transport. Processing lives in src/mixer. */
#include "audio.h"
#include "system.h"
#include "spi.h"

#pragma optimize_for_speed

bool bufferSwitch = false;
#pragma align 8
int audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0};
#pragma align 8
int audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0};

/* SPORT chain pointers address the last word of each four-word TCB. */
int audioRx_tcb[8][BUFFER_COUNT][4];
int audioTx_tcb[8][BUFFER_COUNT][4];

void audioIoInit(void) {
	for (int i_buf = 0; i_buf < BUFFER_COUNT; i_buf++) {
		int nextBuffer = (i_buf + 1) % BUFFER_COUNT;
		for (int i_tcb = 0; i_tcb < TDM_INPUTS; i_tcb++) {
			audioRx_tcb[i_tcb][i_buf][0] = ((int)&audioRx_tcb[i_tcb][nextBuffer][0] + 3);
			audioRx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			audioRx_tcb[i_tcb][i_buf][2] = 1;
			audioRx_tcb[i_tcb][i_buf][3] = (int)&audioRxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];
			audioTx_tcb[i_tcb][i_buf][0] = ((int)&audioTx_tcb[i_tcb][nextBuffer][0] + 3);
			audioTx_tcb[i_tcb][i_buf][1] = BUFFER_SIZE;
			audioTx_tcb[i_tcb][i_buf][2] = 1;
			audioTx_tcb[i_tcb][i_buf][3] = (int)&audioTxBuf[(BUFFER_COUNT * BUFFER_SIZE * i_tcb) + (BUFFER_SIZE * i_buf)];
		}
	}
}

void audioRxISR(uint32_t iid, void *handlerarg) {
	bufferSwitch = !bufferSwitch;
	audioReady = true;
	audioGlitchDetected = audioProcess;
}

void audioIoPublishMeters(void) {
	#pragma loop_count(92)
	for (int i_ch = 0; i_ch < 92; i_ch++)
	{
		int tap = (i_ch < 40 ) ? TAP_PRE_EQ : TAP_POST_FADER; // TODO: needs to be refined
		memcpy(&spiCommData[SPI_DATA_VU_DATA_STARTINDEX + i_ch], &audioBuffer[tap][DSP_BUF_IDX_DSPCHANNEL + i_ch][0], sizeof(float));
	}
}
