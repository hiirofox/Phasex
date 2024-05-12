
#pragma once

#include <math.h>
#include "complex.h"

void fft_f32(complex_f32_t *a, int n, int inv)
{
    for (int i = 1, j = 0; i < n - 1; ++i)
    {
        for (int s = n; j ^= s >>= 1, ~j & s;)
            ;
        if (i < j)
        {
            complex_f32_t tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }
    complex_f32_t t;
    int p1,p2;
    for (int m = 2; m <= n; m <<= 1)
    {
        complex_f32_t wm = complex_f32_t{(float)cos(2.0 * M_PI / m), (float)sin(2.0 * M_PI / m) * inv};//��ת���ٶ�
        /*
        ���и��뷨���ǣ��ܷ񱣴���ת���Ǹ�������ʹ�ÿ��Ա�����һ��fft����λ��
        ������λ���ܽ������ˡ�
        ͬʱwmҲ���Ա�ɲ���������Ƶ�ʱ�������ת����
        */
        for (int k = 0; k < n; k += m)
        {
            complex_f32_t w = complex_f32_t{1.0, 0};//Ҫ��ת���Ǹ�(��ʵ�Ǹ���m)
            p1 = m>>1;
            for (int j = 0; j < p1; ++j, w = cmul(w, wm))//������ת
            {
                p2 = k+j;
                t = cmul(w, a[p2 + p1]);
                a[p2 + p1] = csub(a[p2], t);
                a[p2] = cadd(a[p2], t);
            }
        }
    } 
}
void fft_f64(complex_f64_t *a, int n, int inv)
{
    for (int i = 1, j = 0; i < n - 1; ++i)
    {
        for (int s = n; j ^= s >>= 1, ~j & s;)
            ;
        if (i < j)
        {
            complex_f64_t tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }
    for (int m = 2; m <= n; m <<= 1)
    {
        complex_f64_t wm = complex_f64_t{cos(2.0 * M_PI / m), sin(2.0 * M_PI / m) * inv};

        for (int k = 0; k < n; k += m)
        {
            complex_f64_t w = complex_f64_t{1.0, 0};
            for (int j = 0; j < (m >> 1); ++j, w = cmul64(w, wm))
            {
                complex_f64_t t = cmul64(w, a[k + j + (m >> 1)]);
                a[k + j + (m >> 1)] = csub64(a[k + j], t);
                a[k + j] = cadd64(a[k + j], t);
            }
        }
    }
}
