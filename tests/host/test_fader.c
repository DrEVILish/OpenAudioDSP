/* SPDX-License-Identifier: GPL-3.0-only */
#include "openaudiodsp/fader.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void test_smoothing(void) {
    float gains[] = {19.0f, 0.0f, 1.0f, -0.5f, 23.0f};
    const float targets[] = {1.0f, 0.0f, 0.5f};

    faderSmoothGains(&gains[1], targets, 3, 0.25f);
    assert(gains[0] == 19.0f && gains[4] == 23.0f);
    assert(gains[1] == 0.25f && gains[2] == 0.75f && gains[3] == -0.25f);

    faderSmoothGains(&gains[1], targets, 3, 0.25f);
    assert(gains[1] == 0.4375f && gains[2] == 0.5625f && gains[3] == -0.0625f);

    /* A changed target must take effect without resetting the existing state. */
    const float newTarget[] = {-0.5625f};
    faderSmoothGains(&gains[2], newTarget, 1, 0.5f);
    assert(gains[2] == 0.0f);
    assert(gains[0] == 19.0f && gains[4] == 23.0f);

    faderSmoothGains(NULL, NULL, 0, 0.01f);
}

static void test_processing(void) {
    float input[5][OPENAUDIODSP_FADER_BLOCK_SAMPLES];
    float original[5][OPENAUDIODSP_FADER_BLOCK_SAMPLES];
    float output[5][OPENAUDIODSP_FADER_BLOCK_SAMPLES];
    const float gains[] = {17.0f, 0.5f, -2.0f, 0.0f, 29.0f};

    for (int channel = 0; channel < 5; channel++) {
        for (int sample = 0; sample < OPENAUDIODSP_FADER_BLOCK_SAMPLES; sample++) {
            input[channel][sample] = (float)((channel + 1) * 32 + sample - 8);
            output[channel][sample] = -777.0f;
        }
    }
    memcpy(original, input, sizeof(input));

    /* Odd active count and nonzero offsets exercise batch/stride independence. */
    faderProcessBatch(&input[1], &output[1], &gains[1], 3);
    assert(memcmp(original, input, sizeof(input)) == 0);
    for (int sample = 0; sample < OPENAUDIODSP_FADER_BLOCK_SAMPLES; sample++) {
        assert(output[0][sample] == -777.0f && output[4][sample] == -777.0f);
        assert(output[1][sample] == 28.0f + 0.5f * sample);
        assert(output[2][sample] == -176.0f - 2.0f * sample);
        assert(output[3][sample] == 0.0f);
    }

    /* In-place batches must have the same output as disjoint buffers. */
    faderProcessBatch(&input[1], &input[1], &gains[1], 3);
    assert(memcmp(&input[1], &output[1], 3 * sizeof(input[0])) == 0);
    assert(memcmp(&input[0], &original[0], sizeof(input[0])) == 0);
    assert(memcmp(&input[4], &original[4], sizeof(input[0])) == 0);

    faderProcessBatch(NULL, NULL, NULL, 0);
}

int main(void) {
    test_smoothing();
    test_processing();
    puts("Fader batch tests passed");
    return 0;
}
