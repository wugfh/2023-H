#ifndef _MY_FUNC_H_
#define _MY_FUNC_H_

#define min(x,y)  (((x)<(y))?(x):(y))
#define max(x,y)  (((x)>(y))?(x):(y))

#include "NE10.h"
#include "math.h"

enum{
    FM = 0,
    AM = 1
};

typedef enum{
    SOFT_OK = 0,
    SOFT_ERR = -1
}soft_status;


void delay_ms(unsigned int mseconds);
void error_say(int error_code, char str[]);
int dds_set_freq(float freq, uint32_t addr);
soft_status am_volt(float* sin_volt, float* cos_volt, uint32_t len, float* dst);
soft_status fm_volt(float* sin_volt, float* cos_volt, uint32_t len, float* dst);
soft_status sample(float* volt, uint32_t len, float* dst, uint32_t sample_len, uint8_t flag);
soft_status fft_volt(float* volt, int fft_size, ne10_fft_cpx_float32_t* fft_out);
soft_status normalize(float* volt, uint32_t len);
float average(float* volt, uint32_t len);
float get_max_freq(ne10_fft_cpx_float32_t* fft, uint32_t flag, uint32_t len);
float fm_index(float* sin_volt, uint32_t len, float mod_freq, float max_freq);
int judge_type(float* sin_volt, float* cos_volt, uint32_t len, float* last_peak);
void filt_inputwave(float* volt, int len);
int kmp(char* tar, char* in, uint32_t tar_len, uint32_t in_len);
int gcd(int a, int b);
void del_exception_volt(float* volt, uint32_t len);
void EnableNEON();
float variance(float* volt, float aver, uint32_t len);
#endif
