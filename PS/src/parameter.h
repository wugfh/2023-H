#ifndef _PARAMETER_H_
#define _PARAMETER_H_
#include "xparameters.h"

#define PI		                    3.14159265358979323846f
#define PL_CLK                      50000000

#define DDS_FREQ                    PL_CLK
#define DAC_DDS_WIDTH               27  
#define DDS_PHASE_WIDTH             12 
#define FREQ_WORD(freq, width)      (1.0*freq*(((long long)1)<<(width))/DDS_FREQ)
#define PHASE_WORD(phase, width)    (1.0*phase*(((long long)1)<<(width))/(2*PI))
#define TRI_STEP(freq)              ((long long)0xffffffff*freq*2/(PL_CLK+4*freq)+1)

#define DMA_BASEADDR                XPAR_ZYNQ_DMA_AXI_DMA_0_BASEADDR
#define COUNTER_RES_WIDTH           16

#define DDR_BASEADDR                XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define DDR_HIGHADDR                XPAR_PS7_DDR_0_S_AXI_HIGHADDR

#define RESET_TIMEOUT               5000

#define ADC_WIDTH                   14
#define ADC_REVOLUTION              16384
#define ADC_REF                     0.55
#define ADC_CLK                     50000000
#define ADC_SNR                     72

#define GUI_TIMER_INT_ID            XPAR_SCUTIMER_INTR
#define GUI_TIMER_DEVICE_ID         XPAR_SCUTIMER_DEVICE_ID

#define CIC_SAMPLE_RATE             25
#define SOFT_SAMPLE_RATE            1

#define FFT_SAMPLE_LEN              (8192*2)
#define SAMPLE_LEN                  (adc_inst.adc_buf_len)
#define JUDGE_FFT_LEN               (4096*2)


#define TYPE_JUDGE_THRESHOLD        1e-1

#define FIR_ORDER                   100

#define ANS_POINT_NUM               1024
#define MAX_FREQ                    50e6
#define MIN_FREQ					5e6

#define GPIO_DEVICE_ID                          XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_BASE_ADDR                          XPAR_PS7_GPIO_0_BASEADDR
#define GPIO_HIGH_ADDR                          XPAR_PS7_GPIO_0_HIGHADDR

#define EMIO_BANK1                              XGPIOPS_BANK2
#define EMIO_BANK2                              XGPIOPS_BANK3
#define EMIO_MIN_PIN                            54
#define EMIO_PIN(num)                           (EMIO_MIN_PIN+num)
#define DATA_EMIO_MASK                          0x001fffe0
#define DATA_PIN_OFFSET                         5

#define GPIO_IN                                 0
#define GPIO_OUT                                1
#define GPIO_OUT_DISABLE                        0
#define GPIO_OUT_ENABLE                         1
#define GPIO_BANK_OUT                           0xffffffff
#define GPIO_BANK_IN                            0
#endif
