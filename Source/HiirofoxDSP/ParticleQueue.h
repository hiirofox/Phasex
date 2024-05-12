
#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct
{
    int memlen;
    int buflen;
    int16_t *bufl;
    int16_t *bufr;

    int pos;

    void (*FillBufferCallback)(int16_t *, int16_t *, int *, int);
    // void FillBufferCallback(int16_t *bufl,int16_t *bufr,int &FillBufferLen/*in*/,int MaxBufferLen/*out*/);
    int8_t UseFillBufferCB;
} ParticleData;
void ResetParticleData(ParticleData *p, int keeplen)
{
    free(p->bufl);
    free(p->bufr);
    p->buflen = keeplen;
    p->memlen = keeplen;
    p->bufl = (int16_t *)malloc(sizeof(int16_t) * keeplen);
    p->bufr = (int16_t *)malloc(sizeof(int16_t) * keeplen);
    p->pos = 0;
    p->FillBufferCallback = NULL;
    p->UseFillBufferCB = 0;
}
void UpdataParticle(ParticleData *p, const int16_t *wavl, const int16_t *wavr, int wavlen)
{
    if (wavlen > p->memlen)
    {
        p->bufl = (int16_t *)realloc(p->bufl, sizeof(int16_t) * (wavlen + 512));
        p->bufr = (int16_t *)realloc(p->bufr, sizeof(int16_t) * (wavlen + 512));
    }
    p->memlen = wavlen + 512;
    p->buflen = wavlen;
    if (wavl != NULL)
    {
        memcpy(p->bufl, wavl, sizeof(int16_t) * wavlen);
    }
    if (wavr != NULL)
    {
        memcpy(p->bufr, wavr, sizeof(int16_t) * wavlen);
    }
}

void ParticleToBuf(ParticleData *p, int16_t *outl, int16_t *outr, int wavlen, float diffusion) // diffusion 0.0 -> 0.5
{
    int mixsize = diffusion * p->buflen;
    int width = p->buflen - mixsize;
    for (int i = 0; i < wavlen;)
    {
        for (; p->pos < mixsize && i < wavlen; p->pos++, i++)
        {
            float mix = (float)p->pos / mixsize;
            mix = sqrt(mix);
            outl[i] = mix * p->bufl[p->pos] + (1.0 - mix) * p->bufl[p->pos + width - 1];
            outr[i] = mix * p->bufr[p->pos] + (1.0 - mix) * p->bufr[p->pos + width - 1];
        }
        for (; p->pos >= mixsize && p->pos < width && i < wavlen; p->pos++, i++)
        {
            outl[i] = p->bufl[p->pos];
            outr[i] = p->bufr[p->pos];
        }
        if (p->pos >= width)
        {
            p->pos = 0;
            if (p->UseFillBufferCB)
            {
                p->FillBufferCallback(p->bufl, p->bufr, &p->buflen, p->memlen);
                mixsize = diffusion * p->buflen;
                width = p->buflen - mixsize;
            }
        }
    }
}
