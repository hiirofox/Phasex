
#pragma once

#include <math.h>

typedef struct
{
	float z2, z1;
} PolyPhaseAPFData;
float PolyPhaseAPFProc(PolyPhaseAPFData* p, float sign, float a)
{ // faster
	sign += p->z2 * a;
	a = p->z2 - sign * a;
	p->z2 = p->z1;
	p->z1 = sign;
	return a;
}

#define MaxApfN (4)
const float a_re[4] = { 0.4021921162426, 0.8561710882420, 0.9722909545651, 0.9952884791278 }; // hilbert iir apf常数
const float a_im[4] = { 0.6923878000000, 0.9360654322959, 0.9882295226860, 0.9987488452737 };
typedef struct
{
	PolyPhaseAPFData filt_re[MaxApfN];
	PolyPhaseAPFData filt_im[MaxApfN];
	float im_delay;
} HilbertData;
void HilbertTrans(HilbertData* p, float* sign_re, float* sign_im)
{
	for (int i = 0; i < MaxApfN; i++)
		*sign_re = PolyPhaseAPFProc(&p->filt_re[i], *sign_re, a_re[i]);
	for (int i = 0; i < MaxApfN; i++)
		*sign_im = PolyPhaseAPFProc(&p->filt_im[i], *sign_im, a_im[i]);
	float get_im = p->im_delay;
	p->im_delay = *sign_im;
	*sign_im = get_im;
}
float HilbertTransRe(HilbertData* p, float sign_re)
{
	for (int i = 0; i < MaxApfN; i++)
		sign_re = PolyPhaseAPFProc(&p->filt_re[i], sign_re, a_re[i]);
	return sign_re;
}
float HilbertTransIm(HilbertData* p, float sign_im)
{
	for (int i = 0; i < MaxApfN; i++)
		sign_im = PolyPhaseAPFProc(&p->filt_im[i], sign_im, a_im[i]);
	float get_im = p->im_delay;
	p->im_delay = sign_im;
	return get_im;
}

#define MaxPhaserAPFN (800)
typedef struct
{
	PolyPhaseAPFData apfs[MaxPhaserAPFN];
} PolyPhasePhaserData;
float PolyPhasePhaser(PolyPhasePhaserData* p, float in, float a, int step)
{
	for (int i = 0; i < step; ++i)
		in = PolyPhaseAPFProc(&p->apfs[i], in, a);
	return in;
}



#define MaxNewAPF1DelayLen (120)
typedef struct
{
	float delayline[MaxNewAPF1DelayLen];
	unsigned int delaypos;
} NewAPF1Data;
void InitNewAPF1Proc(NewAPF1Data* p)
{
	memset(p->delayline, 0, sizeof(float) * MaxNewAPF1DelayLen);
	p->delaypos = 0;
}
float NewAPF1Proc(NewAPF1Data* p, float sign, float a, int delayn)
{
	float lastv = p->delayline[p->delaypos % MaxNewAPF1DelayLen];
	sign += lastv * a;
	p->delayline[(p->delaypos + delayn) % MaxNewAPF1DelayLen] = sign;
	p->delaypos++;
	return lastv - sign * a;
}
typedef struct
{
	NewAPF1Data apfs[MaxPhaserAPFN];
}PhasexPhaserData;
void InitPhasexPhaser(PhasexPhaserData* p)
{
	for (int i = 0; i < MaxPhaserAPFN; ++i)
	{
		InitNewAPF1Proc(&p->apfs[i]);
	}
}
float PhasexPhaser(PhasexPhaserData* p, float in, float a, int step, int delayn)
{
	for (int i = 0; i < step; ++i)
		in = NewAPF1Proc(&p->apfs[i], in, a, delayn);
	return in;
}
float PhasexPhaserMultiCtof(PhasexPhaserData* p, float in, float* ctofs, float ctofv, int step, int delayn)
{
	for (int i = 0; i < step; ++i)
		in = NewAPF1Proc(&p->apfs[i], in, ctofs[i] + ctofv, delayn);
	return in;
}