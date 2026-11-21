/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef WING_AUDIO_H
#define WING_AUDIO_H

#include <stdint.h>
#include "profile.h"

/* SPORT DMA layout: [lane][frame * 16 + slot within lane]. */
typedef int32_t WingRxBlock[WING_RX_LANES][WING_LANE_WORDS];
typedef int32_t WingTxBlock[WING_TX_LANES][WING_LANE_WORDS];

#ifdef __cplusplus
extern "C" {
#endif
void wingAudioProcess(const WingRxBlock rx, WingTxBlock tx);
#ifdef __cplusplus
}
#endif

#endif
