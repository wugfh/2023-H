#ifndef __AMP_FREQ_H_
#define __AMP_FREQ_H_

enum{
    SIN_WAVE     = 0,
    TRI_WAVE     = 1
};

enum{
    PORTA       = 0,
    PORTB       = 1
};

typedef struct{
    uint32_t a_freq, b_freq;
    uint8_t a_form, b_form; 
    float phase;
    float b_phase;
}ab_wave;

void set_dac_freq(uint32_t freq, float phase, uint32_t port, uint8_t wave);
void dac_gpio_init();
void get_fft(float* fft_out, uint32_t len);
void get_ab(float* fft_out, ab_wave* ab, uint32_t len);
void sync(ab_wave* ab_inst);
float comp_phase(ab_wave* ab);
float circuit_comp(ab_wave* ab);

#endif