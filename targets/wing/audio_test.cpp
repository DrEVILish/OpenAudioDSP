/* SPDX-License-Identifier: GPL-3.0-only */
#include "audio.h"

static WingRxBlock rx;
static WingTxBlock tx;
extern "C" {
volatile int wing_test_result[4]; // checks, failures, target, sample rate
void wing_test_done(void);
}

void wing_test_done(void) {
    wing_test_result[3] = WING_SAMPLE_RATE;
}

static void (*volatile finish)(void) = wing_test_done;

static void check(bool condition) {
    ++wing_test_result[0];
    if (!condition) ++wing_test_result[1];
}

static int32_t &word(int32_t block[][WING_LANE_WORDS], int slot, int frame) {
    // Use the agreed wire stride, independent of the audio implementation.
    return block[slot / 16][frame * 16 + slot % 16];
}

int main(void) {
    const int used = WING_DSP == 4 ? 16 : WING_DSP == 3 ? 32 : WING_DSP == 2 ? 50 : 64;
    for (int slot = 0; slot < WING_RX_LANES * 16; ++slot)
        for (int n = 0; n < 16; ++n)
            word(rx, slot, n) = ((slot + 3) * 175493 + n * 3571) * (n % 2 ? -1 : 1);
    word(rx, 0, 0) = INT32_MAX;
    word(rx, used - 1, 15) = INT32_MIN;
    for (int slot = 0; slot < WING_TX_LANES * 16; ++slot)
        for (int n = 0; n < 16; ++n) word(tx, slot, n) = -18;

    wingAudioProcess(rx, tx);
    for (int slot = 0; slot < WING_TX_LANES * 16; ++slot)
        for (int n = 0; n < 16; ++n)
            check(word(tx, slot, n) == (slot < used ? word(rx, slot, n) : 0));

    wing_test_result[2] = WING_DSP;
    wing_test_result[3] = WING_SAMPLE_RATE;
    finish();
    return wing_test_result[1] != 0;
}
