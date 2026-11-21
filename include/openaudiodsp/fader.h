/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef OPENAUDIODSP_FADER_H
#define OPENAUDIODSP_FADER_H

#define OPENAUDIODSP_FADER_BLOCK_SAMPLES 16

#ifdef __cplusplus
extern "C" {
#endif

/* Caller-owned contiguous channel ranges; channelCount must be nonnegative.
 * Smooth once per block, independently of how many processing batches use gains.
 * targets are not modified; gains retain the smoothing state between blocks. */
void faderSmoothGains(float gains[], const float targets[], int channelCount,
                      float coefficient);

/* Multiply each channel's block by its current gain without smoothing again.
 * input/output may be identical (in-place) or disjoint, not partially overlapping.
 * Input and gains are otherwise unchanged. Zero channels accesses no pointers.
 * Input rows are unqualified for C99 callers; their samples are read-only. */
void faderProcessBatch(
    float input[][OPENAUDIODSP_FADER_BLOCK_SAMPLES],
    float output[][OPENAUDIODSP_FADER_BLOCK_SAMPLES],
    const float gains[], int channelCount);

#ifdef __cplusplus
}
#endif

#endif
