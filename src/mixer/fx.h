#ifndef __FX_H__
#define __FX_H__

#include "dsp1.h"

// function prototypes
float linearToDb(float linear);
float dbToLinear(float dB);
float calcRMS(const float* __restrict src);
void fxSetPeqCoeffs(int channel, int index, float coeffs[]);
void fxSmoothCoeffs(void);
#endif
