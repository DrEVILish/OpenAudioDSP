/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef OPENAUDIODSP_EQ_H
#define OPENAUDIODSP_EQ_H

#include <stdbool.h>

#define OPENAUDIODSP_EQ_BLOCK_SAMPLES 16

#ifdef __cplusplus
extern "C" {
#endif

/* SHARC/ADI biquad_trans batch; no board or channel-map dependencies.
 * Caller owns contiguous blocks and independent, initialized state per channel.
 * channelCount must be nonnegative; bandCount must be positive.
 * Coefficients use ADI's paired-section interleaving in PM (5 * bandCount floats
 * per channel); states use the matching ADI layout in DM (2 * bandCount floats
 * per channel). Coefficients and enabled are read-only, disjoint from audio/state.
 * Process samples in place. A NULL enabled mask processes every channel;
 * false preserves both samples and state. The caller selects/copies taps.
 * No allocation, coefficient conversion or state reset.
 * Zero channels accesses no pointers. Requires the SHARC compiler and ADI SDK. */
void eqProcessBatch(
    float samples[][OPENAUDIODSP_EQ_BLOCK_SAMPLES],
    const bool enabled[], const float pm coefficients[], float states[],
    int channelCount, int bandCount);

#ifdef __cplusplus
}
#endif

#endif
