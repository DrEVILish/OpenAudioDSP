/* SPDX-License-Identifier: GPL-3.0-only
 * Fader arithmetic extracted from OpenX32, Copyright 2025 OpenMixerProject.
 */
#include "openaudiodsp/fader.h"

/* Keep block-rate smoothing at project defaults; prioritize speed per sample. */
void faderSmoothGains(float gains[], const float targets[], int channelCount,
                      float coefficient) {
    for (int channel = 0; channel < channelCount; channel++) {
        gains[channel] = gains[channel] +
            ((targets[channel] - gains[channel]) * coefficient);
    }
}

#pragma optimize_for_speed

void faderProcessBatch(
    float input[][OPENAUDIODSP_FADER_BLOCK_SAMPLES],
    float output[][OPENAUDIODSP_FADER_BLOCK_SAMPLES],
    const float gains[], int channelCount) {
    for (int channel = 0; channel < channelCount; channel++) {
        const float* src = input[channel];
        float* dst = output[channel];

        #pragma loop_count(OPENAUDIODSP_FADER_BLOCK_SAMPLES)
        #pragma vector_for
        for (int sample = 0; sample < OPENAUDIODSP_FADER_BLOCK_SAMPLES; sample++) {
            dst[sample] = gains[channel] * src[sample];
        }
    }
}
