#ifndef _DRUMACHINE_
#define _DRUMACHINE_

#include <math.h>

float randf() //-1 -> 1
{
    return (float)(rand() % 10000) / 10000.0 * (rand() % 2 ? 1 : -1);
}
float Freq2Pitch(float freq)
{
    return log(freq) / log(2.0);
}
float Pitch2Freq(float pitch)
{
    return pow(2.0, pitch);
}
float MapValue(float n, float x, float y, float u, float v)
{
    return (n - x) / (x - y) * (u - v) + u;
}
float DistVal(float v, float k)
{
    v = v > 1.05 ? (1.05 + k * (v - 1.05)) : v;
    v = v < -1.0 ? (-1.0 + k * (v + 1.0)) : v;
    return v;
}

#define MaxTableLen (2048)
float sinv[MaxTableLen];
float triv[MaxTableLen];
float sawv[MaxTableLen];
void InitWaveTableBuf() ///////////////////!!!
{
    for (int i = 0; i < MaxTableLen; ++i)
    {
        sinv[i] = sin(2.0 * M_PI * i / MaxTableLen);
        triv[i] = asin(sin(2.0 * M_PI * i / MaxTableLen));
        sawv[i] = (float)i / MaxTableLen * 2.0 - 1.0;
    }
}

typedef struct
{
    double t;
    double t2;
} WaveTableData;
void ResetWaveTableData(WaveTableData *p)
{
    p->t = p->t2 = 0;
}
float WaveTable_fast(WaveTableData *p, float f, float phi, float wt)
{
    p->t += f;
    int16_t pos = (int)((p->t + phi) * MaxTableLen) % MaxTableLen;
    if (wt < 1.0)
        return (1.0 - wt) * sinv[pos] + (wt)*triv[pos];
    else if (wt < 2.0)
        return (2.0 - wt) * triv[pos] + (wt - 1.0) * (triv[pos] > 0 ? 1 : -1);
    else
        return (3.0 - wt) * (triv[pos] > 0 ? 1 : -1) + (wt - 2.0) * sawv[pos];
    return 0;
}
/*
float WaveTable(float f, float wt)
{
    t2 += f;
    if (wt < 1.0)
        return (1.0 - wt) * sin(t2) + (wt)*asin(sin(t2));
    else if (wt < 2.0)
        return (2.0 - wt) * asin(sin(t2)) + (wt - 1.0) * (sin(t2) > 0 ? 1 : -1);
    else
        return (3.0 - wt) * (sin(t2) > 0 ? 1 : -1) + (wt - 2.0) * atan(tan(t2 * 0.5));
    return 0;
}*/

////////////////////////////////////////////////////////////

typedef struct
{
    BiquadCoeffs lpfilt, bpfilt, hpfilt;
    BiquadCoeffs outbpfilt, outbcfilt; //"eq gain" reso = 4

    float osc_a, osc_d;   // 振荡器包络参数
    float osc_freq;       // 振荡器频率
    float osc_wt;         // 波表
    float VCODecayAmount; // 啊啊那一堆调制写一起了
    float VCODecayRate;
    float VCOModAmount;
    float VCOModRate;
    float VCOModType; // lfo的反馈，往锯齿和方波渐变
    float VCONoiseAmount;
    float VCONoiseRate;

    float noi_a, noi_d;
    float noi_envetype; // 包络种类
    float noi_ftype;    // 滤波器种类的过渡
    float noi_ctof;
    float noi_reso;

    float outmix;
    float eqgain;
    float eqfreq;
    float dist;
    float volume;
} DrumParams;

typedef struct
{
    float vEnve, vLFO, vNoise;

    float OscVol;
    float lfofdbk;
    int OscEnveMode;
    WaveTableData vco1;
    WaveTableData fm_lfo;
    BiquadData lpfilt, bpfilt, hpfilt;
    BiquadData outbpfilt, outbcfilt; //"eq gain" reso = 4

    float NoiVol, NoiVolClap;
    int NoiEnveMode;
} DrumData;

void ResetDrumData(DrumData *p)
{
    p->vEnve = 0;
    p->vLFO = 0; // sin this = 1
    p->vNoise = 0;
    p->OscVol = 0.0;
    p->OscEnveMode = 0;
    ResetWaveTableData(&(p->vco1));
    ResetWaveTableData(&(p->fm_lfo));

    p->NoiVol = 0.0;
    p->NoiVolClap = 0.0;
    p->NoiEnveMode = 0;

    ResetBiquadData(&(p->lpfilt));
    ResetBiquadData(&(p->bpfilt));
    ResetBiquadData(&(p->hpfilt));
    ResetBiquadData(&(p->outbpfilt));
    ResetBiquadData(&(p->outbcfilt));
}
void DrumSetGate(DrumData *p, int acc, int fill, DrumParams *prm)
{
    p->vEnve = prm->VCODecayAmount;
    p->vLFO = M_PI / 2.0; // sin this = 1
    p->vNoise = randf();
    p->OscVol = 0.0;
    p->OscEnveMode = 1;
    ResetWaveTableData(&(p->vco1));
    ResetWaveTableData(&(p->fm_lfo));

    p->NoiVol = 0.0;
    p->NoiVolClap = 1.0;
    p->NoiEnveMode = 1;

    ResetBiquadData(&(p->lpfilt));
    ResetBiquadData(&(p->bpfilt));
    ResetBiquadData(&(p->hpfilt));
    ResetBiquadData(&(p->outbpfilt));
    ResetBiquadData(&(p->outbcfilt));
}
float proc(DrumData *dat, DrumParams *prm)
{
    if (dat->OscVol < 0.00001 && dat->NoiVol < 0.00001 && dat->OscEnveMode == 0 && dat->NoiEnveMode == 0)
        return 0;

    float oscFreq = osc_freq * (dat->vEnve += VCODecayRate * (1.0 - dat->vEnve));
    float val1 = (dat->vNoise += VCONoiseRate * (randf() - dat->vNoise)) * VCONoiseAmount;
    float lfo = (1.0 + WaveTable_fast(dat.fm_lfo, VCOModRate, 0, VCOModType)) * VCOModAmount;
    oscFreq += val1;
    oscFreq = oscFreq < 0 ? 0 : oscFreq;
    float out1 = vco1.WaveTable_fast(oscFreq, lfo, osc_wt) * OscVol * OscVol;

    float NoiEnve = 0; // 噪声部分
    if (noi_envetype < 1.0)
    {
        NoiEnve = (1.0 - noi_envetype) * NoiVol + (noi_envetype)*NoiVol * NoiVol;
    }
    else
    {
        NoiEnve = (2.0 - noi_envetype) * NoiVol * NoiVol + (noi_envetype - 1.0) * NoiVolClap * NoiVolClap;
    }
    float noise = randf() * NoiEnve;
    float lpv = lpfilt.proc(noise);
    float bpv = bpfilt.proc(noise);
    float hpv = hpfilt.proc(noise);
    if (noi_ftype < 1.0)
    {
        noise = (1.0 - noi_ftype) * lpv + (noi_ftype)*bpv;
    }
    else
    {
        noise = (2.0 - noi_ftype) * bpv + (noi_ftype - 1.0) * hpv;
    }
    float out2 = noise;

    if (OscEnveMode == 1)
    { // vco的包络更新
        OscVol += osc_a;
        if (OscVol >= 1.0)
        {
            OscVol = 1.0;
            OscEnveMode = 0;
        }
    }
    else if (OscEnveMode == 0)
    {
        OscVol -= osc_d * OscVol;
    }

    if (NoiEnveMode == 1)
    { // 噪音的包络更新
        NoiVol += noi_a;

        NoiVolClap -= sqrt(noi_a) * 0.1; // clap
        if (NoiVolClap <= 0)
            NoiVolClap = 1.0;

        if (NoiVol >= 1.0)
        {
            NoiVol = 1.0;
            NoiVolClap = 1.0;
            NoiEnveMode = 0;
        }
    }
    else if (NoiEnveMode == 0)
    {
        NoiVol -= noi_d * NoiVol;
        NoiVolClap = NoiVol;
    }

    float mixv = (1.0 - outmix) * out1 + (outmix)*out2;
    float distk = (1.0 - dist);
    mixv = DistVal(mixv * (1.0 + dist * 25.0), distk * distk * distk * distk);
    float bpmixv = outbpfilt.proc(mixv);
    float bcmixv = outbcfilt.proc(mixv);
    if (eqgain > 0)
    {
        mixv = (1.0 - eqgain) * mixv + eqgain * bpmixv;
    }
    else
    {
        mixv = (1.0 + eqgain) * mixv - eqgain * bcmixv;
    }
    return mixv * volume; // mix
}

void updatafilter()
{
    outbpfilt.UpdataBPF(eqfreq, 2.0);
    outbcfilt.UpdataBCF(eqfreq, 2.0);
    lpfilt.UpdataLPF(noi_ctof, noi_reso);
    bpfilt.UpdataBPF(noi_ctof, noi_reso);
    hpfilt.UpdataHPF(noi_ctof, noi_reso);
}

float proc_buf(float *buf, int bufsize)
{
    gate(0, 0);
    for (int i = 0; i < bufsize; ++i)
        buf[i] = proc();
    return 0;
}
void InputParam(float *params)
{
    float *dst = &osc_a;
    memcpy(dst, params, 22 * sizeof(float));
}
#endif