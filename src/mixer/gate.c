/* SPDX-License-Identifier: GPL-3.0-only
 * Gate arithmetic extracted from OpenX32, Copyright 2025 OpenMixerProject.
 */
#include "openaudiodsp/gate.h"

#include <math.h>

#pragma optimize_for_speed

void gateProcessBatch(float samples[][OPENAUDIODSP_GATE_BLOCK_SAMPLES],
                      const sGate settings[], sGateState states[], int channelCount) {
    for (int channel = 0; channel < channelCount; channel++) {
        if (!settings[channel].value_enable) {
            continue;
        }

        float* restrict src_dst = samples[channel];
        float env = states[channel].envelope;
        int holdTimer = states[channel].holdTimer;
        float threshold = settings[channel].value_threshold;
        float refValue = 0.0f;

        #pragma no_alias
        #pragma vector_for
        #pragma loop_count(OPENAUDIODSP_GATE_BLOCK_SAMPLES, OPENAUDIODSP_GATE_BLOCK_SAMPLES, OPENAUDIODSP_GATE_BLOCK_SAMPLES)
        for (int sample = 0; sample < OPENAUDIODSP_GATE_BLOCK_SAMPLES; sample++) {
            refValue += src_dst[sample] * src_dst[sample];
        }
        refValue = sqrtf(refValue * 0.0625f);

        float targetGainLinear = (refValue < threshold) ? 0.0f : 1.0f;
        float coeff = 0.0f;

        if (targetGainLinear > env) {
            coeff = settings[channel].value_coeff_attack;
            holdTimer = settings[channel].value_hold_ticks;
        } else {
            if (holdTimer > 0) {
                holdTimer--;
            } else {
                coeff = settings[channel].value_coeff_release;
            }
        }

        states[channel].holdTimer = holdTimer;

        float a1 = 1.0f - coeff;
        float b0 = coeff * targetGainLinear;

        #pragma no_alias
        #pragma loop_count(OPENAUDIODSP_GATE_BLOCK_SAMPLES, OPENAUDIODSP_GATE_BLOCK_SAMPLES, OPENAUDIODSP_GATE_BLOCK_SAMPLES)
        for (int sample = 0; sample < OPENAUDIODSP_GATE_BLOCK_SAMPLES; sample++) {
            env = (env * a1) + b0;
            src_dst[sample] *= env;
        }

        states[channel].envelope = env;
    }
}
