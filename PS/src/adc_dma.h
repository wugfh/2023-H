#ifndef __ADC_DMA_H
#define __ADC_DMA_H
#include "parameter.h"
#include "xaxidma.h"

typedef struct {
    u8 rx_done;
    u8 error;
    uint32_t* adc_buf;
    u32 adc_buf_len;
    float* sin_volt, *cos_volt;
    XAxiDma* adc_dma;
}Adc_control;

int adc_translation(int recv_len);
int adc_translation_init();


#endif