
#pragma once

typedef struct
{
    float tmp1, tmp2, out1, out2;
} Filter1Data;

void ResetFilter1Data(Filter1Data *p)
{
    p->tmp1 = 0, p->tmp2 = 0, p->out1 = 0, p->out2 = 0;
}

float LPF1(Filter1Data *p, float vin, float ctof, float reso)
{ // 一阶低通带反馈
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    return p->out1;
}
float LPF1_l(Filter1Data *p, float vin, float ctof, float reso, float limVol, float limK)
{ // 一阶低通带限制反馈
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->out1 = p->out1 > limVol ? ((p->out1 - limVol) * limK + limVol) : p->out1;
    p->out1 = p->out1 < -limVol ? ((p->out1 + limVol) * limK - limVol) : p->out1;
    p->tmp1 = p->tmp1 > limVol ? ((p->tmp1 - limVol) * limK + limVol) : p->tmp1;
    p->tmp1 = p->tmp1 < -limVol ? ((p->tmp1 + limVol) * limK - limVol) : p->tmp1;
    return p->out1;
}
float LPF1_ol(Filter1Data *p, float vin, float ctof, float reso, float limVol, float limK)
{ // 超采样一阶低通，可以和普通低通一样使用
    ctof *= 0.5;
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->out1 = p->out1 > limVol ? ((p->out1 - limVol) * limK + limVol) : p->out1;
    p->out1 = p->out1 < -limVol ? ((p->out1 + limVol) * limK - limVol) : p->out1;
    p->tmp1 = p->tmp1 > limVol ? ((p->tmp1 - limVol) * limK + limVol) : p->tmp1;
    p->tmp1 = p->tmp1 < -limVol ? ((p->tmp1 + limVol) * limK - limVol) : p->tmp1;
    return p->out1;
}
float LPF2(Filter1Data *p, float vin, float ctof, float reso)
{ // 二阶的，同上
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->tmp2 += ctof * (p->out1 - p->tmp2 + reso * (p->tmp2 - p->out2));
    p->out2 += ctof * (p->tmp2 - p->out2);
    return p->out2;
}
float LPF2_l(Filter1Data *p, float vin, float ctof, float reso, float limVol, float limK)
{
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->tmp2 += ctof * (p->out1 - p->tmp2 + reso * (p->tmp2 - p->out2));
    p->out2 += ctof * (p->tmp2 - p->out2);
    p->out1 = p->out1 > limVol ? ((p->out1 - limVol) * limK + limVol) : p->out1;
    p->out1 = p->out1 < -limVol ? ((p->out1 + limVol) * limK - limVol) : p->out1;
    p->tmp1 = p->tmp1 > limVol ? ((p->tmp1 - limVol) * limK + limVol) : p->tmp1;
    p->tmp1 = p->tmp1 < -limVol ? ((p->tmp1 + limVol) * limK - limVol) : p->tmp1;
    p->out2 = p->out2 > limVol ? ((p->out2 - limVol) * limK + limVol) : p->out2;
    p->out2 = p->out2 < -limVol ? ((p->out2 + limVol) * limK - limVol) : p->out2;
    p->tmp2 = p->tmp2 > limVol ? ((p->tmp2 - limVol) * limK + limVol) : p->tmp2;
    p->tmp2 = p->tmp2 < -limVol ? ((p->tmp2 + limVol) * limK - limVol) : p->tmp2;
    return p->out2;
}
float LPF2_ol(Filter1Data *p, float vin, float ctof, float reso, float limVol, float limK)
{
    ctof *= 0.5;
    reso += reso / (1.0 - ctof);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->tmp2 += ctof * (p->out1 - p->tmp2 + reso * (p->tmp2 - p->out2));
    p->out2 += ctof * (p->tmp2 - p->out2);
    p->tmp1 += ctof * (vin - p->tmp1 + reso * (p->tmp1 - p->out1));
    p->out1 += ctof * (p->tmp1 - p->out1);
    p->tmp2 += ctof * (p->out1 - p->tmp2 + reso * (p->tmp2 - p->out2));
    p->out2 += ctof * (p->tmp2 - p->out2);
    p->out1 = p->out1 > limVol ? ((p->out1 - limVol) * limK + limVol) : p->out1;
    p->out1 = p->out1 < -limVol ? ((p->out1 + limVol) * limK - limVol) : p->out1;
    p->tmp1 = p->tmp1 > limVol ? ((p->tmp1 - limVol) * limK + limVol) : p->tmp1;
    p->tmp1 = p->tmp1 < -limVol ? ((p->tmp1 + limVol) * limK - limVol) : p->tmp1;
    p->out2 = p->out2 > limVol ? ((p->out2 - limVol) * limK + limVol) : p->out2;
    p->out2 = p->out2 < -limVol ? ((p->out2 + limVol) * limK - limVol) : p->out2;
    p->tmp2 = p->tmp2 > limVol ? ((p->tmp2 - limVol) * limK + limVol) : p->tmp2;
    p->tmp2 = p->tmp2 < -limVol ? ((p->tmp2 + limVol) * limK - limVol) : p->tmp2;
    return p->out2;
}
