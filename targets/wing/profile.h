/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef WING_PROFILE_H
#define WING_PROFILE_H

#define WING_SAMPLE_RATE 48000
#define WING_BLOCK 16
#define WING_LANE_SLOTS 16

/* Each port lists its SPORT number and DAI data pin(s) in A/B lane order. */
#define WING_RX_CLOCK 08
#define WING_RX_FS 14
#define WING_TX_CLOCK 04
#define WING_TX_FS 18

#if WING_DSP == 1
#define WING_RX_PORTS(dual, single) dual(0,07,13) dual(5,19,02)
#define WING_TX_PORTS(dual, single) single(1,01) dual(2,17,16) dual(3,12,15) dual(4,11,06)
#define WING_STRIPS 56
#define WING_RETURN_STRIPS 8
#elif WING_DSP == 2
#define WING_RX_PORTS(dual, single) dual(0,17,16) dual(1,12,15) dual(2,11,06) single(3,02)
#define WING_TX_PORTS(dual, single) dual(4,07,13) dual(5,19,01)
#define WING_STRIPS 50
#define WING_RETURN_STRIPS 0
#elif WING_DSP == 3 || WING_DSP == 4
#define WING_RX_PORTS(dual, single) dual(0,17,16) dual(2,10,12)
#define WING_TX_PORTS(dual, single) single(1,20) single(3,09)
#if WING_DSP == 3
#define WING_STRIPS 24
#define WING_RETURN_STRIPS 8
#else
#define WING_FX_PAIRS 8
#endif
#else
#error Define WING_DSP as 1, 2, 3 or 4
#endif

enum {
#define WING_COUNT_DUAL(s,a,b) + 2
#define WING_COUNT_SINGLE(s,a) + 1
    WING_RX_LANES = 0 WING_RX_PORTS(WING_COUNT_DUAL, WING_COUNT_SINGLE),
    WING_TX_LANES = 0 WING_TX_PORTS(WING_COUNT_DUAL, WING_COUNT_SINGLE)
#undef WING_COUNT_DUAL
#undef WING_COUNT_SINGLE
};

#if WING_DSP == 4
#define WING_USED_SLOTS (2 * WING_FX_PAIRS)
#else
#define WING_USED_SLOTS (WING_STRIPS + WING_RETURN_STRIPS)
#endif

#define WING_LANE_WORDS (WING_BLOCK * WING_LANE_SLOTS)

#endif
