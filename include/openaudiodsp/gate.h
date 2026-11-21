/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef OPENAUDIODSP_GATE_H
#define OPENAUDIODSP_GATE_H

#include <stdbool.h>

#define OPENAUDIODSP_GATE_BLOCK_SAMPLES 16

#ifdef __cplusplus
extern "C" {
#endif

/* Existing control values; threshold is linear in the input sample scale.
 * gainmin is retained for control compatibility but is not used by this gate.
 * hold_ticks is converted to int on attack and counts blocks, not samples. */
typedef struct {
    bool value_enable;
    float value_threshold;
    float value_gainmin;
    float value_coeff_attack;
    float value_hold_ticks;
    float value_coeff_release;
} sGate;

typedef struct {
    float envelope;
    int holdTimer;
} sGateState;

/* Process contiguous channels in place; channelCount must be nonnegative.
 * Caller owns independent settings/state per channel, disjoint from samples.
 * Disabled channels preserve both samples and state; settings are read-only.
 * Initialize states before use; this function neither allocates nor resets them.
 * Zero channels accesses no pointers. Coefficients must be in [0, 1], hold_ticks
 * nonnegative and representable as int, and initial envelopes in [0, 1]. */
void gateProcessBatch(float samples[][OPENAUDIODSP_GATE_BLOCK_SAMPLES],
                      const sGate settings[], sGateState states[], int channelCount);

#ifdef __cplusplus
}
#endif

#endif
