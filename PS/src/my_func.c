#include "xtime_l.h"
#include "arm_neon.h"
#include "xil_printf.h"
#include <stdlib.h>
#include "NE10.h"
#include "parameter.h"
#include "xil_io.h"
#include "adc_dma.h"
#include "rtthread.h"
#include "my_func.h"

void EnableNEON(){
    asm volatile ("MRC p15,0,r0,c1,c0,2"); 
    asm volatile ("ORR r0,r0,#0x00f00000"); 
    asm volatile ("MCR p15,0,r0,c1,c0,2");
    asm volatile ("ISB");
    asm volatile ("MOV r0,#0x40000000");
    asm volatile ("fmxr FPEXC,r0 ");
}

float sum(float* num, uint32_t len){
	return 0;
}

void delay_ms(unsigned int mseconds)
{
	XTime tEnd, tCur;

	XTime_GetTime(&tCur);
	tEnd = tCur + (((XTime) mseconds) * (COUNTS_PER_SECOND / 1000));
	do
	{
		XTime_GetTime(&tCur);
	} while (tCur < tEnd);
}

void error_say(int error_code, char str[]){
    xil_printf(str);
}

inline static void swap(float* a, float* b){
    float temp = *a;
    *a = *b;
    *b = temp;
}

float average(float* volt, uint32_t len){
    float average = 0;
    for(uint32_t i = 0; i < len; ++i){
        if(volt[i] > 1e17)
            return 0;
        average += volt[i];
    }
    average /= 1.0*len;
    return average;
}

float variance(float* volt, float aver, uint32_t len){
    float var = 0;
    for(uint32_t i = 0; i < len; ++i){
        var += (volt[i]-aver)*(volt[i]-aver);
    }
    var /= len;
    return var;
}

float var(float* volt, uint32_t len){
	return 0;
}

soft_status normalize(float* volt, uint32_t len){
    float max_volt = 0;
    for(uint32_t i = 0; i < len; ++i)
        max_volt = max(fabs(volt[i]), max_volt);
    if(max_volt < 1e-20)
        return SOFT_ERR;
    for(uint32_t i = 0; i < len; ++i)
        volt[i] /= max_volt;
    return SOFT_OK;
}

void del_exception_volt(float* volt, uint32_t len){
    for(uint32_t i = 1; i < len-1; ++i){
        if(fabs(volt[i]) > fabs(volt[i-1])+fabs(volt[i+1]))
            volt[i] = (volt[i-1]+volt[i+1])/2;
    }
}
soft_status fft_volt(ne10_float32_t* volt, int fft_size, ne10_fft_cpx_float32_t* fft_buf){
    ne10_fft_r2c_cfg_float32_t cfg = ne10_fft_alloc_r2c_float32(fft_size);
    if(cfg != NULL){
        ne10_fft_r2c_1d_float32_c(fft_buf, volt, cfg);
        NE10_FREE(cfg);
        return SOFT_OK;
    }
    else return SOFT_ERR;
}

soft_status sample(float* restrict volt, uint32_t len, float* restrict dst, uint32_t sample_len, uint8_t flag){
    uint32_t step = 0;
    step = SOFT_SAMPLE_RATE;
    for(uint32_t i = 0; i < (sample_len); ++i){
    	dst[i] = volt[i*step];
    }
    float aver = average(dst, sample_len);
    for(uint32_t i = 0; i < sample_len; ++i){
        dst[i] -= aver;
    }
    normalize(dst, sample_len);
    del_exception_volt(volt, sample_len);
    return SOFT_OK;
}

soft_status am_volt(float* restrict sin_volt, float* restrict cos_volt, uint32_t len, float* dst){
    for(uint32_t i = 0; i < len; ++i){
    	dst[i] = sqrtf(sin_volt[i]*sin_volt[i]+cos_volt[i]*cos_volt[i]);
    }
    return SOFT_OK;
}

soft_status fm_volt(float* restrict sin_volt, float* restrict cos_volt, uint32_t len, float* dst){
    float pre = 0;
    for(uint32_t i = 0; i < len; ++i){
        float temp;
        temp = atan2f(sin_volt[i],cos_volt[i]);
        dst[i] = temp-pre;
        pre = temp;
    }
    return SOFT_OK;
}

float get_max_freq(ne10_fft_cpx_float32_t* fft, uint32_t flag, uint32_t len){
    float* buf = (float*)rt_malloc(sizeof(float)*len);
    for(int i = 0; i < len/2; ++i){
        buf[i] = sqrtf(fft[i].r*fft[i].r+fft[i].i*fft[i].i);
    }
    float fft_step = 0;
    normalize(buf, len/2);
    fft_step = 1.0*ADC_CLK/(CIC_SAMPLE_RATE*SOFT_SAMPLE_RATE*len/2);
    uint32_t end_pos = MAX_FREQ/fft_step;
    uint32_t start_pos = MIN_FREQ/fft_step;
    float max_freq_amp = 0;
    uint32_t max_pos = 0;   
    for(uint32_t i = start_pos; i < end_pos; ++i){
        if(fabsf(buf[i]) > max_freq_amp){
            max_freq_amp = fabsf(buf[i]);
            max_pos = i;
        }
    }
    rt_free(buf);
    return max_pos*fft_step;
}

float fm_index(float* sin_volt, uint32_t len, float mod_freq, float max_freq){
    float* buf = (float*)rt_malloc(sizeof(float)*FFT_SAMPLE_LEN);
    ne10_fft_cpx_float32_t* fft_buf = (ne10_fft_cpx_float32_t*)rt_malloc(sizeof(ne10_fft_cpx_float32_t)*FFT_SAMPLE_LEN);
    sample(sin_volt, len, buf, FFT_SAMPLE_LEN, FM);
    fft_volt(buf, FFT_SAMPLE_LEN, fft_buf);
    rt_free(buf);
    rt_free(fft_buf);
    float index = (max_freq)/mod_freq;
    if(index >= 0) return index;
    else return -1;
}

float am_index(float* sin_volt, uint32_t len){
	return 0;
}

int judge_type(float* sin_volt, float* cos_volt, uint32_t len, float* last_peak){
    ne10_fft_cpx_float32_t *buf = rt_malloc(sizeof(ne10_fft_cpx_float32_t)*JUDGE_FFT_LEN);
    float* fft_mod = rt_malloc(sizeof(float)*JUDGE_FFT_LEN/2);
    fft_volt(sin_volt, JUDGE_FFT_LEN, buf);
    uint32_t left_peak = 0, right_peak = 0;
    for(uint32_t i = 0; i < JUDGE_FFT_LEN/2; ++i){
        fft_mod[i] = sqrtf(buf[i].r*buf[i].r+buf[i].i*buf[i].i);
    }
    normalize(fft_mod, JUDGE_FFT_LEN/2);
    float fft_step = 0;
    float aver = average(fft_mod, JUDGE_FFT_LEN/2);
    float std = sqrtf(variance(fft_mod, aver, JUDGE_FFT_LEN/2));
    fft_step = 1.0*ADC_CLK/(CIC_SAMPLE_RATE*JUDGE_FFT_LEN);
    uint32_t end_pos = MAX_FREQ/fft_step+1;
    for(uint32_t i = 1; i < end_pos; ++i){
        if(fft_mod[i] > aver+10*std){
            if(left_peak == 0) left_peak = i*fft_step;
            else right_peak = i*fft_step;
        }
    }
    rt_free(buf);
    rt_free(fft_mod);
    *last_peak = right_peak;
    if(right_peak-left_peak > 20e3) return FM;
    else return AM;
}


int kmp(char* tar, char* in, uint32_t tar_len, uint32_t in_len){
    char* next = rt_malloc(sizeof(char)*tar_len);
    next[0] = 0;
    int next_j = 0;
    for(uint32_t i = 1; i < tar_len; ++i){
        while(next_j > 0 && next[i] != next[next_j]){
            next_j = next[next_j-1];
        }
        if(next[i] == next[next_j]){
            ++next_j;
        }
        next[i] = next_j;
    }
    next_j = 0;
    for(uint32_t i = 0; i < in_len; ++i){
        while(next_j > 0 && tar[next_j] != in[i]){
                next_j = next[next_j-1];
            }
        if(tar[next_j] == in[i]){
            ++next_j;
        }
        if(next_j == tar_len)
            return i-tar_len+1;
        if(tar[next_j] == in[i]){
            ++next_j;
            if(next_j == tar_len)
                return i;
        }
    }
    return -1;
}


int gcd(int a, int b){
    if(a<b){
        return gcd(b,a);
    }
    int c = 1;
    while(a%b != 0){
        c = a%b;
        a = b;
        b = c;
    }
    return b;
}
