/* SPDX-License-Identifier: GPL-3.0-only */
#include "openaudiodsp/compressor.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void test_rms_attack_and_ranges(void) {
    float input[5][16], original[5][16], output[5][16];
    sCompressor settings[5], originalSettings[5];
    sCompressorState states[5];
    for (int c = 0; c < 5; c++) {
        settings[c] = (sCompressor){-12.0f, 0.75f, 0.5f + 0.25f * c,
                                    0.5f, 2.75f, 0.75f};
        states[c] = (sCompressorState){1.0f, 7};
        for (int s = 0; s < 16; s++) {
            input[c][s] = c == 1 ? (s == 0 ? 4.0f : 0.0f) :
                (c == 2 && s % 2 ? -1.0f : 1.0f);
            output[c][s] = 37.0f;
        }
    }
    memcpy(original, input, sizeof(input));
    memcpy(originalSettings, settings, sizeof(settings));
    const bool enabled[] = {true, false, true};
    compressorProcessBatch(input + 1, output + 1, settings + 1, states + 1, enabled, 3, 1.0f);
    assert(memcmp(original, input, sizeof(input)) == 0);
    assert(memcmp(originalSettings, settings, sizeof(settings)) == 0);

    /* Frozen pre-extraction result for RMS=1, threshold=-12 dB, ratio factor=.75.
     * Deliberately preserves the fast approximation, not an ideal log/pow curve.
     * Impulse, bipolar and constant inputs all have RMS=1, but different peaks/means. */
    const float target = 0x1.764fbcp-2f;
    for (int c = 0; c < 5; c++) {
        for (int s = 0; s < 16; s++) {
            if (c == 0 || c == 4) {
                assert(output[c][s] == 37.0f);
            } else if (c == 2) {
                assert(output[c][s] == input[c][s]);
            } else {
                float envelope = target + ldexpf(1.0f - target, -(s + 1));
                float expected = input[c][s] * envelope * settings[c].value_makeup;
                assert(fabsf(output[c][s] - expected) < 5.0e-7f);
            }
        }
        if (c == 0 || c == 2 || c == 4) {
            assert(states[c].envelope == 1.0f && states[c].holdTimer == 7);
        } else {
            assert(fabsf(states[c].envelope - (target + ldexpf(1.0f - target, -16))) < 1.0e-7f);
            assert(states[c].holdTimer == 2);
        }
    }
    compressorProcessBatch(NULL, NULL, NULL, NULL, NULL, 0, 1.0f);
}

static void test_hold_release_and_equality(void) {
    float input[1][16], output[1][16];
    sCompressor settings = {0.0f, 0.75f, 2.0f, 0.25f, 4.75f, 0.5f};
    sCompressorState state = {0.25f, 2};
    for (int s = 0; s < 16; s++) input[0][s] = s % 2 ? -0.25f : 0.25f;

    for (int block = 0; block < 3; block++) {
        compressorProcessBatch(input, output, &settings, &state, NULL, 1, 1.0f);
        for (int s = 0; s < 16; s++) {
            float envelope = block < 2 ? 0.25f : 1.0f - ldexpf(0.75f, -(s + 1));
            assert(output[0][s] == input[0][s] * envelope * 2.0f);
        }
        assert(state.holdTimer == (block == 0 ? 1 : 0));
    }
    assert(state.envelope == 1.0f - ldexpf(0.75f, -16));

    /* Target equals envelope: do not reload hold, even with nonzero attack. */
    state = (sCompressorState){1.0f, 2};
    for (int block = 0; block < 3; block++) {
        compressorProcessBatch(input, output, &settings, &state, NULL, 1, 1.0f);
        assert(state.envelope == 1.0f);
        assert(state.holdTimer == (block == 0 ? 1 : 0));
    }

    /* Silence stays finite and permits release rather than freezing the state. */
    memset(input, 0, sizeof(input));
    state = (sCompressorState){0.5f, 0};
    compressorProcessBatch(input, output, &settings, &state, NULL, 1, 1.0f);
    assert(state.envelope == 1.0f - ldexpf(0.5f, -16));
    for (int s = 0; s < 16; s++) assert(output[0][s] == 0.0f);
}

static void test_scaling_in_place_and_controls(void) {
    float input[1][16], raw[1][16], inPlace[1][16], output[1][16], rawOutput[1][16];
    sCompressor settings = {-12.0f, 0.75f, 1.25f, 0.5f, 1.75f, 1.0f};
    sCompressorState state = {1.0f, 0}, rawState = state, aliasState = state;
    for (int s = 0; s < 16; s++) {
        input[0][s] = s % 2 ? -0.5f : 0.5f;
        raw[0][s] = ldexpf(input[0][s], 31);
    }
    memcpy(inPlace, input, sizeof(input));
    const bool disabled = false;
    compressorProcessBatch(inPlace, inPlace, &settings, &aliasState, &disabled, 1, 1.0f);
    assert(memcmp(input, inPlace, sizeof(input)) == 0);
    assert(aliasState.envelope == 1.0f && aliasState.holdTimer == 0);
    compressorProcessBatch(input, output, &settings, &state, NULL, 1, 1.0f);
    compressorProcessBatch(raw, rawOutput, &settings, &rawState, NULL, 1, 0x1p-31f);
    compressorProcessBatch(inPlace, inPlace, &settings, &aliasState, NULL, 1, 1.0f);
    assert(state.envelope == rawState.envelope && state.holdTimer == rawState.holdTimer);
    assert(state.envelope == aliasState.envelope && state.holdTimer == aliasState.holdTimer);
    assert(memcmp(output, inPlace, sizeof(output)) == 0);
    for (int s = 0; s < 16; s++) assert(rawOutput[0][s] == ldexpf(output[0][s], 31));

    settings.value_1_minus_1_by_ratio = 0.0f;
    settings.value_makeup = 0.25f;
    state.holdTimer = 0;
    compressorProcessBatch(input, output, &settings, &state, NULL, 1, 1.0f);
    assert(state.envelope == 1.0f);
    for (int s = 0; s < 16; s++) assert(output[0][s] == input[0][s] * 0.25f);
}

int main(void) {
    test_rms_attack_and_ranges();
    test_hold_release_and_equality();
    test_scaling_in_place_and_controls();
    puts("Compressor batch tests passed");
    return 0;
}
