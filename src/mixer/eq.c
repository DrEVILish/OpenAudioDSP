/* SPDX-License-Identifier: GPL-3.0-only
 * EQ orchestration extracted from OpenX32, Copyright 2025 OpenMixerProject.
 */
#include "openaudiodsp/eq.h"

#include <filter.h>
#include <stddef.h>

#pragma optimize_for_speed

void eqProcessBatch(
    float samples[][OPENAUDIODSP_EQ_BLOCK_SAMPLES],
    const bool enabled[], const float pm coefficients[], float states[],
    int channelCount, int bandCount) {
    for (int channel = 0; channel < channelCount; channel++) {
        if (enabled == NULL || enabled[channel]) {
            biquad_trans(samples[channel], coefficients + channel * 5 * bandCount,
                         states + channel * 2 * bandCount,
                         OPENAUDIODSP_EQ_BLOCK_SAMPLES, bandCount);
        }
    }
}
