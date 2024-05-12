
#pragma once

#include <math.h>

/*
BiquadFilter v1.0.0 by Hiirofox
2023/12/14
*/
typedef struct
{
    float coeffs[6]; // a0,a1,a2,1/b0,b1,b2
} BiquadCoeffs;

typedef struct
{
    float dx1, dx2, dy1, dy2;
} BiquadData;

void ResetBiquadCoeffs(BiquadCoeffs *p)
{
    p->coeffs[0] = p->coeffs[1] = p->coeffs[2] = p->coeffs[3] = p->coeffs[4] = p->coeffs[5] = 0;
}
void ResetBiquadData(BiquadData *p)
{
    p->dx1 = 0;
    p->dx2 = 0;
    p->dy1 = 0;
    p->dy2 = 0;
}
void UpdataLPF(BiquadCoeffs *p, float ctof, float reso)
{
    ctof *= M_PI;
    float ncos = cos(ctof);
    float alpha = sin(ctof) / reso * 0.5;
    p->coeffs[1] = 1.0 - ncos;
    p->coeffs[2] = p->coeffs[0] = p->coeffs[1] * 0.5;
    p->coeffs[3] = 1.0 / (1.0 + alpha); // 取倒数优化除法
    p->coeffs[4] = -ncos * 2.0;
    p->coeffs[5] = 1.0 - alpha;

    p->coeffs[4] *= p->coeffs[3];
    p->coeffs[5] *= p->coeffs[3];
    p->coeffs[3] = 1.0;
}
void UpdataBPF(BiquadCoeffs *p, float ctof, float reso)
{
    ctof *= M_PI;
    float ncos = cos(ctof);
    float alpha = sin(ctof) / reso * 0.5;
    p->coeffs[0] = alpha;
    p->coeffs[1] = 0;
    p->coeffs[2] = -alpha;
    p->coeffs[3] = 1.0 / (1.0 + alpha);
    p->coeffs[4] = -ncos * 2.0;
    p->coeffs[5] = 1.0 - alpha;

    p->coeffs[4] *= p->coeffs[3];
    p->coeffs[5] *= p->coeffs[3];
    p->coeffs[3] = 1.0;
}
void UpdataHPF(BiquadCoeffs *p, float ctof, float reso)
{
    ctof *= M_PI;
    float ncos = cos(ctof);
    float alpha = sin(ctof) / reso * 0.5;
    p->coeffs[1] = -1.0 - ncos;
    p->coeffs[2] = p->coeffs[0] = -p->coeffs[1] * 0.5;
    p->coeffs[3] = 1.0 / (1.0 + alpha);
    p->coeffs[4] = -ncos * 2.0;
    p->coeffs[5] = 1.0 - alpha;

    p->coeffs[4] *= p->coeffs[3];
    p->coeffs[5] *= p->coeffs[3];
    p->coeffs[3] = 1.0;
}
void UpdataBCF(BiquadCoeffs *p, float ctof, float reso)
{
    ctof *= M_PI;
    float ncos = cos(ctof);
    float alpha = sin(ctof) / reso * 0.5;
    p->coeffs[0] = 1.0;
    p->coeffs[1] = -2.0 * ncos;
    p->coeffs[2] = 1.0;
    p->coeffs[3] = 1.0 / (1.0 + alpha);
    p->coeffs[4] = p->coeffs[1];
    p->coeffs[5] = 1.0 - alpha;

    p->coeffs[4] *= p->coeffs[3];
    p->coeffs[5] *= p->coeffs[3];
    p->coeffs[3] = 1.0;
}

float BiquadProc(BiquadData *p, float vin, const BiquadCoeffs *coeffs) // 直接型1
{
    // float vout = (coeffs->coeffs[0] * vin + coeffs->coeffs[1] * p->dx1 + coeffs->coeffs[2] * p->dx2 - coeffs->coeffs[4] * p->dy1 - coeffs->coeffs[5] * p->dy2) * coeffs->coeffs[3];
    float vout = coeffs->coeffs[0] * vin + coeffs->coeffs[1] * p->dx1 + coeffs->coeffs[2] * p->dx2 - coeffs->coeffs[4] * p->dy1 - coeffs->coeffs[5] * p->dy2;
    p->dy2 = p->dy1;
    p->dy1 = vout;
    p->dx2 = p->dx1;
    p->dx1 = vin;
    return vout;
}
float BiquadProc2(BiquadData *p, float vin, const BiquadCoeffs *coeffs) // 直接型2
{
    p->dy1 += vin + p->dx1 * coeffs->coeffs[4] + p->dx2 * coeffs->coeffs[5];
    vin = p->dy1 * coeffs->coeffs[0] + p->dx1 * coeffs->coeffs[1] + p->dx2 * coeffs->coeffs[2];
    p->dx2 = p->dx1;
    p->dx1 = p->dy1;
    return vin;
}