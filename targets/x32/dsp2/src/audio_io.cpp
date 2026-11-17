/* X32 ADSP-21371 SPORT DMA and S/PDIF transport. */
#include "audio.h"
#include "system.h"

#pragma file_attr("prefersMem=internal")

static volatile int spdifSamplePointer[3] = {0, 0, 0};
static volatile bool spdifLeftChannel[3] = {true, true, true};
int audioBufferOffset = 0;
int pm audioRxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0};
int pm audioTxBuf[TDM_INPUTS * BUFFER_COUNT * BUFFER_SIZE] = {0};
int pm audioRx_tcb[4][BUFFER_COUNT][4];
int pm audioTx_tcb[4][BUFFER_COUNT][4];

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

void audioIoBeginBlock(void) {
	spdifSamplePointer[0] = 0;
	spdifSamplePointer[1] = 0;
	spdifSamplePointer[2] = 0;
	spdifLeftChannel[0] = true;
	spdifLeftChannel[1] = true;
	spdifLeftChannel[2] = true;
}

void audioRxISR(uint32_t iid, void *handlerarg) {
	if (audioProcessing) {
		audioGlitchCounter++;
		systemCrash();
	}
	audioReady = 1;
}

void audioSpdifRxImxISR(uint32_t iid, void* handlerarg) {
	if (spdifLeftChannel[0]) {
		audioBuffer[TAP_OUTPUT][DSP_BUF_IDX_IMXIN_LEFT][spdifSamplePointer[0]] = (float)(*pRXSP4A & 0xFFFFFF00);
		spdifLeftChannel[0] = false;
	}else{
		audioBuffer[TAP_OUTPUT][DSP_BUF_IDX_IMXIN_RIGHT][spdifSamplePointer[0]] = (float)(*pRXSP4A & 0xFFFFFF00);
		spdifLeftChannel[0] = true;
		spdifSamplePointer[0]++;
	}
}

void audioSpdifTxImxISR(uint32_t iid, void* handlerarg) {
	if (spdifLeftChannel[1]) {
		*pTXSP5A = ((int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_IMXOUT_LEFT][spdifSamplePointer[1]]) & 0xFFFFFF00;
		spdifLeftChannel[1] = false;
	}else{
		*pTXSP5A = ((int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_IMXOUT_RIGHT][spdifSamplePointer[1]]) & 0xFFFFFF00;
		spdifLeftChannel[1] = true;
		spdifSamplePointer[1]++;
	}
}

void audioSpdifTxXlrISR(uint32_t iid, void* handlerarg) {
	if (spdifLeftChannel[2]) {
		*pTXSP6A = ((int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_SPDIF_LEFT][spdifSamplePointer[2]]) & 0xFFFFFF00;
		spdifLeftChannel[2] = false;
	}else{
		*pTXSP6A = ((int32_t)audioBuffer[TAP_INPUT][DSP_BUF_IDX_SPDIF_RIGHT][spdifSamplePointer[2]]) & 0xFFFFFF00;
		spdifLeftChannel[2] = true;
		spdifSamplePointer[2]++;
	}
}
