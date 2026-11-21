/* SPDX-License-Identifier: GPL-3.0-only
 * ADSP-21371 SHARC Assembly Dynamics Kernel Interface
 * Copyright 2025-2026 OpenMixerProject
 */
#ifndef OPENAUDIODSP_DYNAMICS_ASM_H
#define OPENAUDIODSP_DYNAMICS_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Flat 32-bit word-aligned parameter blocks for direct DM(offset, I3) access in SHARC ASM.
 * Field offsets MUST stay in sync with dynamics_asm.asm! */

typedef struct {
    float thresholdDb;       /* offset 0  */
    float slope;             /* offset 1  (1.0f - 1.0f / ratio) */
    float makeup;            /* offset 2  (linear) */
    float coeffAttack;       /* offset 3  (>= 0.999f => 0ms peak-lookahead mode) */
    float holdTicks;         /* offset 4  */
    float coeffRelease;      /* offset 5  */
    float kneeHalf;          /* offset 6  (0.5f * kneeWidthDb) */
    float kneeCoeff;         /* offset 7  (-slope / (2.0f * kneeWidthDb), or 0.0f) */
    float env;               /* offset 8  (in/out state) */
    int   holdTimer;         /* offset 9  (in/out state) */
    float inputScale;        /* offset 10 (1.0f normalized; INT32_TO_FLOAT_NORM on X32) */
    float inputScaleSqDiv16; /* offset 11 (0.0625f * inputScale * inputScale) */
} sCompAsmParams;

typedef struct {
    float thresholdSq16;     /* offset 0  (16 * threshold_lin^2) */
    float gainMin;           /* offset 1  (linear min gain from Range) */
    float coeffAttack;       /* offset 2  */
    float holdTicks;         /* offset 3  */
    float coeffRelease;      /* offset 4  */
    float thresholdDb;       /* offset 5  */
    float negRangeDb;        /* offset 6  */
    float expanderSlope;     /* offset 7  (0.0f = hard Gate, 1/2/3 = Exp 2:1/3:1/4:1) */
    float env;               /* offset 8  (in/out state) */
    int   holdTimer;         /* offset 9  (in/out state) */
    float inputScaleSqDiv16; /* offset 10 (0.0625f * inputScale * inputScale) */
} sGateAsmParams;

void fxCompressorBlock_asm(const float* src, float* dst, sCompAsmParams* params);
void fxGateBlock_asm(float* src_dst, sGateAsmParams* params);

#ifdef __cplusplus
}
#endif

#endif /* OPENAUDIODSP_DYNAMICS_ASM_H */
