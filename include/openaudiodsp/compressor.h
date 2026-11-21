/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef OPENAUDIODSP_COMPRESSOR_H
#define OPENAUDIODSP_COMPRESSOR_H

#include <stdbool.h>

#define OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES 16

#ifdef __cplusplus
extern "C" {
#endif

/* Existing hard-knee, block-RMS detector and fast dB approximations.
 * value_1_minus_1_by_ratio is precalculated by the control owner.
 * value_hold_ticks truncates to int on attack and counts blocks, not samples. */
typedef struct {
    float value_thresholdDb;
    float value_1_minus_1_by_ratio;
    float value_makeup;
    float value_coeff_attack;
    float value_hold_ticks;
    float value_coeff_release;
} sCompressor;

typedef struct {
    float envelope;
    int holdTimer;
} sCompressorState;

/* Caller owns contiguous buffers, read-only settings and initialized histories.
 * A null enabled mask processes every channel. A disabled channel copies input
 * to output unchanged and preserves its state.
 * input/output may be identical or disjoint, not partially overlapping; settings
 * and states must be disjoint from samples and each other. Input rows are
 * unqualified for C99 callers but otherwise read-only. No allocation/reset.
 * inputScale is positive and converts detector RMS to the threshold's reference
 * scale (1 for normalized samples; X32 uses INT32_TO_FLOAT_NORM). Samples and
 * detector arithmetic must remain finite. Attack/release and ratio factor must
 * be in [0, 1], makeup nonnegative, envelope in [0, 1], hold values nonnegative
 * and representable as int. Use thresholds in [-140, 0] dB and requested gains
 * in [-700, 0] dB for the existing IEEE-754 binary32 bit-level approximations.
 * Envelope smoothing runs per sample; hold advances once per block.
 * channelCount must be nonnegative; zero channels accesses no pointers. */
void compressorProcessBatch(
    float input[][OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES],
    float output[][OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES],
    const sCompressor settings[], sCompressorState states[], const bool enabled[],
    int channelCount, float inputScale);

#ifdef __cplusplus
}
#endif

#endif
