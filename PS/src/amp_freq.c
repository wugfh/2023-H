#include "adc_dma.h"
#include "amp_freq.h"
#include "parameter.h"
#include "sleep.h"
#include "xgpio.h"
#include "NE10.h"
#include "tjc_screen.h"
#include "my_func.h"

extern Adc_control adc_inst;
ab_wave ab_inst;

#define A_TRI_CH                2
#define B_TRI_CH                1
#define TRI_GPIO_BASEADDR       XPAR_A_B_OUT_A_B_TRI_BASEADDR

#define A_DDS_BASEADDR          XPAR_A_B_OUT_A_DDS_BASEADDR

#define B_DDS_BASEADDR          XPAR_A_B_OUT_B_DDS_BASEADDR

#define CONTROL_BASEADDR        XPAR_A_B_OUT_A_B_WHICH_BASEADDR
#define A_CONTOL                0
#define B_CONTOL                1

#define DDS_FREQ_CH             1
#define DDS_PHASE_CH            2

#define A_SYNC_CH               1
#define B_SYNC_CH               2

XGpio tri_gpio , A_dds, B_dds, ABcontrol, ab_sync, mp_gpio;

void axi_gpio_init(uint16_t id, XGpio* inst){
    XGpio_Config* config_ptr = XGpio_LookupConfig(id);
    XGpio_CfgInitialize(inst, config_ptr, config_ptr->BaseAddress);
    XGpio_SetDataDirection(inst, 1, 0);
    if(inst->IsDual == 1){
        XGpio_SetDataDirection(inst, 2, 0);
    }

}

void dac_gpio_init(){
    axi_gpio_init(XPAR_A_B_OUT_A_B_TRI_DEVICE_ID, &tri_gpio);
    axi_gpio_init(XPAR_A_B_OUT_A_B_WHICH_DEVICE_ID, &ABcontrol);
    axi_gpio_init(XPAR_A_B_OUT_A_DDS_DEVICE_ID, &A_dds);
    axi_gpio_init(XPAR_A_B_OUT_B_DDS_DEVICE_ID, &B_dds);
    axi_gpio_init(XPAR_SYNC1_AXI_GPIO_0_DEVICE_ID, &ab_sync);
    axi_gpio_init(XPAR_PHASE_GPIO_DEVICE_ID, &mp_gpio);
}

void set_dac_freq(uint32_t freq, float phase, uint32_t port, uint8_t wave){
    if(wave == SIN_WAVE){
        uint32_t fword = FREQ_WORD(freq, DAC_DDS_WIDTH);
        uint32_t pword = (1.0*phase*(((long long)1)<<(DAC_DDS_WIDTH))/(2*PI));
        if(port == PORTA){
            XGpio_DiscreteWrite(&A_dds, DDS_FREQ_CH, fword);
            XGpio_DiscreteWrite(&A_dds, DDS_PHASE_CH, pword);
            uint32_t now_wave = XGpio_DiscreteRead(&ABcontrol, 1);
            now_wave |= (1<<(A_CONTOL));
            XGpio_DiscreteWrite(&ABcontrol, 1, now_wave);
        }
        else if(port == PORTB){
            XGpio_DiscreteWrite(&B_dds, DDS_FREQ_CH, fword);
            XGpio_DiscreteWrite(&B_dds, DDS_PHASE_CH, pword);
            uint32_t now_wave = XGpio_DiscreteRead(&ABcontrol, 1);
            now_wave |= (1<<(B_CONTOL));
            XGpio_DiscreteWrite(&ABcontrol, 1, now_wave);
        }
    }
    else if(wave == TRI_WAVE){
        uint32_t step = TRI_STEP(freq);
        if(port == PORTA){
            XGpio_DiscreteWrite(&tri_gpio, A_TRI_CH, step);
            uint32_t now_wave = XGpio_DiscreteRead(&ABcontrol, 1);
            now_wave &= ~(1<<(A_CONTOL));
            XGpio_DiscreteWrite(&ABcontrol, 1, now_wave);
        }
        else if(port == PORTB){
            XGpio_DiscreteWrite(&tri_gpio, B_TRI_CH, step);
            uint32_t now_wave = XGpio_DiscreteRead(&ABcontrol, 1);
            now_wave &= ~(1<<(B_CONTOL));
            XGpio_DiscreteWrite(&ABcontrol, 1, now_wave);
        }
    }
}

void get_fft(float* fft_out, uint32_t len){
    adc_translation(SAMPLE_LEN);
    normalize(adc_inst.sin_volt, SAMPLE_LEN);
    ne10_fft_cpx_float32_t* fft_i = rt_malloc(sizeof(ne10_fft_cpx_float32_t)*len);
    fft_volt(adc_inst.sin_volt, len, fft_i);
    for(uint32_t i = 0; i < len; ++i){
        fft_out[i] = fft_i[i].i*fft_i[i].i+fft_i[i].r*fft_i[i].r;
    }
    rt_free(fft_i);
}


uint32_t get_delta(float* fft_out, uint32_t* delta_pos, uint32_t len){
    float freq_step = ADC_CLK/CIC_SAMPLE_RATE/FFT_SAMPLE_LEN;
    uint32_t fft_end = len/2;
    normalize(fft_out, fft_end);
    uint32_t pos = 0;
    float aver = 0;
    for(uint32_t i = 0; i < fft_end; ++i)
    	aver += fft_out[i];
    aver /= fft_end;
    uint32_t width = 2.5e3/freq_step;
    for(uint32_t i = 15e3/freq_step; i < fft_end; ++i){
        if(fft_out[i]>3e-3){
        	if(delta_pos[pos-1] < i-width || pos == 0){
        			delta_pos[pos] = i;
					++pos;
        	}
        	else if(delta_pos[pos-1] > i-width && fft_out[delta_pos[pos-1]] < fft_out[i]){
        		delta_pos[pos-1] = i;
        	}
        }
    }
    return pos;
}


void get_ab(float* fft_out, ab_wave* ab, uint32_t len){
    float freq_step = ADC_CLK/CIC_SAMPLE_RATE/FFT_SAMPLE_LEN;
    uint32_t width = 1e3/freq_step;
    uint32_t* deltas = rt_malloc(100*sizeof(uint32_t));
    uint32_t delta_end = get_delta(fft_out, deltas, len);
    ab->a_freq = freq_step*deltas[0];
    uint32_t apos_3 = 0;
    uint32_t bpos_3 = 0, b_pos = 0;
    for(uint32_t i = 1; i < delta_end; ++i){
        if((deltas[i] > deltas[0]*3-width && deltas[i] < deltas[0]*3+width)){
            apos_3 = i;
        }
    }
    if(apos_3 != 0){
    	if(fft_out[deltas[apos_3]] < fft_out[deltas[0]]/10){
    		ab->a_form = TRI_WAVE;
    	}
    	else {
    		uint8_t flag = 0;
    		for(uint32_t i = deltas[0]*5-width; i < deltas[0]*5+width; ++i){
    			if(fft_out[i] > 4e-4)
    				flag = 1;
    		}
    		if(!flag)
    			ab->a_form = SIN_WAVE;
    		else {
    			ab->a_form = TRI_WAVE;
    		}
    	}
    }
    else{
        ab->a_form = SIN_WAVE;
    } 
 
    if(ab->a_form == SIN_WAVE){
        b_pos = 1;
    }
    else{
        if(apos_3 > 1){
            b_pos = 1;
        }
        else if(b_pos == 0) {
        	if(deltas[2]*freq_step<110e3)
        		b_pos = 2;
        	else
        		b_pos = 1;
        }
    }
    ab->b_freq = deltas[b_pos]*freq_step;
    for(uint32_t i = b_pos+1; i < delta_end; ++i){
        if((deltas[i] > deltas[b_pos]*3-width && deltas[i] < deltas[b_pos]*3+width)){
            bpos_3 = i;
        }
    }
    if(bpos_3 != 0){
        ab->b_form = TRI_WAVE;
    }
    else {
        ab->b_form = SIN_WAVE;
    }
    uint32_t tmp = (ab->a_freq)/5e3;
    if(ab->a_freq-tmp*5e3 > 2.5e3) ab->a_freq = (tmp+1)*5e3;
    else ab->a_freq = tmp*5e3;
    tmp = (ab->b_freq)/5e3;
    if(ab->b_freq-tmp*5e3 > 2.5e3) ab->b_freq = (tmp+1)*5e3;
    else ab->b_freq = tmp*5e3;
}

void sync(ab_wave* ab_inst){
    XGpio_DiscreteWrite(&ab_sync, A_SYNC_CH, ab_inst->a_freq/5e3-1);
    XGpio_DiscreteWrite(&ab_sync, B_SYNC_CH, ab_inst->b_freq/5e3-1);
}

float comp_phase(ab_wave* ab){
    uint32_t phase_cnt = XGpio_DiscreteRead(&mp_gpio, 1);
    float out_phase = 1.0*360*phase_cnt*ab_inst.b_freq/PL_CLK;
    float offset = ab_inst.phase - out_phase;
    while(offset < 0) offset += 360;
    while(offset > 360) offset -= 360;
    if((offset<5 && offset>-5)||(offset-360<5&&offset-360>-5)||(offset+360<5&&offset+360>-5)){
    	return 0;
    }
    return offset;

}

float circuit_comp(ab_wave* ab){
    ab_inst.phase += 5;
    while(ab_inst.phase < 0) ab_inst.phase += 360;
    while(ab_inst.phase > 360) ab_inst.phase -= 360;
    return 0;
}
