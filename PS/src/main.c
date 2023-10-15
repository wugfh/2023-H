#include <stdio.h>
#include "xil_printf.h"
#include "adc_dma.h"
#include "NE10.h"
#include "xtime_l.h"
#include "string.h"
#include "xstatus.h"
#include "parameter.h"
#include "xaxidma.h"
#include "my_func.h"
#include "interrupt.h"
#include "amp_freq.h"
#include "sleep.h"
#include "tjc_screen.h"
#include "amplifier.h"

extern Adc_control adc_inst;
extern ab_wave ab_inst;
extern hmi_screen tjc_screen;
extern uint8_t work_mode;


int main(){   
    if(ne10_init() != NE10_OK){
        while(1);
    }

    dac_gpio_init();
    ab_inst.phase = 0;
    float* fft_buf = rt_malloc(sizeof(float)*FFT_SAMPLE_LEN);
    XUartPs_SetRecvTimeout(tjc_screen.device, 4);
    send_hmi_command("x2.val=0");
    ab_inst.phase = 0;
    ab_inst.b_phase = 20;
    while(1){
        package_process();
		get_fft(fft_buf, FFT_SAMPLE_LEN);
        get_ab(fft_buf, &ab_inst, FFT_SAMPLE_LEN);
        set_dac_freq(ab_inst.a_freq, 0, PORTA, ab_inst.a_form);
        if(work_mode == 1){
            float error = 0.25*comp_phase(&ab_inst);
            ab_inst.b_phase = 0.1*error+ab_inst.b_phase;
            while(ab_inst.b_phase > 360) ab_inst.b_phase -= 360;
            while(ab_inst.b_phase < 0) ab_inst.b_phase += 360;
            set_dac_freq(ab_inst.b_freq, (ab_inst.b_phase)*PI/180, PORTB, ab_inst.b_form);
        }
        else {
        	set_dac_freq(ab_inst.b_freq, 0, PORTB, ab_inst.b_form);
        }
        sync(&ab_inst);
        send_ab_information(&ab_inst);
    }
    return 0;
}


