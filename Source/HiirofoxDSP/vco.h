
#pragma once

#define MaxUnisonN (48)
typedef struct
{
    float v1[MaxUnisonN];
    float v2[MaxUnisonN];
} VCOData;
void InitVCOData(VCOData *p)
{
    for (int i = 0; i < MaxUnisonN; ++i)
    {
        p->v1[i] = (float)(rand() % 10000) / 10000.0 * (rand() % 2 ? 1 : -1);
        p->v2[i] = (float)(rand() % 10000) / 10000.0 * (rand() % 2 ? 1 : -1);
    }
}
StereoSignal VCO_Table(VCOData *p, float SawFreq, int UniN, float delta, float *table, int tableLen)
{
    float tmp1 = 0, tmp2 = 0, tmp3 = 0;
    for (int i = 0; i < UniN; ++i)
    {
        tmp3 += delta;
        p->v1[i] += SawFreq + tmp3;
        p->v2[i] += SawFreq - tmp3;
        p->v1[i] -= (int)p->v1[i];
        p->v2[i] -= (int)p->v2[i];
        tmp1 += table[(int)(p->v1[i] * tableLen)];
        tmp2 += table[(int)(p->v2[i] * tableLen)];
    }
    return StereoSignal{tmp1 + tmp2, tmp1 - tmp2};
}