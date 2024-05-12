#pragma once

#include <math.h>

#define SAMPLE_RATE (48000)
#define M_PI (3.14159265358979323846264338327)

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "dsptype.h"

#include "biquad.h"  //biquad filter
#include "filter1.h" //my filter
#include "hilbert.h" //hilbert iir

#include "vco.h"     //vco

#include "fft.h"

#include "ParticleQueue.h" //particle synth

#include "tableconnect.h"//连接两个波表，起到平滑的作用

//#include "SynthData.h"

// #include "drumachine.h"

//#ifdef __cplusplus
//}
//#endif
