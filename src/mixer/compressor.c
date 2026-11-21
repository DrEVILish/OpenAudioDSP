/* SPDX-License-Identifier: GPL-3.0-only
 * Compressor arithmetic extracted from OpenX32, Copyright 2025 OpenMixerProject.
 */
#include "openaudiodsp/compressor.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

static inline float linearToDb_fast(float linear) {
    if (linear < 0.0000001f) return -140.0f;

    union { float f; uint32_t i; } vx = { linear };
    float log2_approx = (float)vx.i * 1.1920928955078125e-7f - 126.94269504f;
    return 6.0206f * log2_approx;
}

static inline float dbToLinear_fast(float dB) {
    float x = dB * (1.0f / 6.0206f);
    union { float f; int32_t i; } vx;
    vx.i = (int32_t)(x * 8388608.0f) + 1065353216;
    return vx.f;
}

void compressorProcessBatch(
    float input[][OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES],
    float output[][OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES],
    const sCompressor* restrict settings, sCompressorState* restrict states,
    const bool enabled[], int channelCount, float inputScale) {
    for (int channel = 0; channel < channelCount; channel++) {
        float* src = input[channel];
        float* dst = output[channel];
        if (enabled && !enabled[channel]) {
            if (src != dst) memcpy(dst, src, sizeof(input[channel]));
            continue;
        }
        float makeUp = settings[channel].value_makeup;
        float coeff = 0.0f;
        float refValue = 0.0f;

        #pragma loop_count(OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES, OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES, OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES)
        #pragma vector_for
        for (int sample = 0; sample < OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES; sample++) {
            refValue += src[sample] * src[sample];
        }
        refValue = sqrtf(refValue * 0.0625f);

        refValue = linearToDb_fast(refValue * inputScale);
        float targetGainDb = 0.0f;
        if (refValue > settings[channel].value_thresholdDb) {
            targetGainDb = (settings[channel].value_thresholdDb - refValue) *
                settings[channel].value_1_minus_1_by_ratio;
        }
        float targetGainLinear = dbToLinear_fast(targetGainDb);

        if (targetGainLinear < states[channel].envelope) {
            coeff = settings[channel].value_coeff_attack;
            states[channel].holdTimer = settings[channel].value_hold_ticks;
        } else {
            if (states[channel].holdTimer > 0) {
                states[channel].holdTimer--;
            } else {
                coeff = settings[channel].value_coeff_release;
            }
        }

        #pragma loop_count(OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES)
        for (int sample = 0; sample < OPENAUDIODSP_COMPRESSOR_BLOCK_SAMPLES; sample++) {
            states[channel].envelope += coeff * (targetGainLinear - states[channel].envelope);
            dst[sample] = src[sample] * states[channel].envelope * makeUp;
        }
    }
}
