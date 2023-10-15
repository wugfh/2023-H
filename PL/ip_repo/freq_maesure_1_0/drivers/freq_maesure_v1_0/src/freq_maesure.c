#include "freq_maesure.h"     
#include "xparameters.h"
#include "xil_io.h"

/************************** Function Definitions ***************************/
#define FREQ_MEASURE_BASEADDRESS    XPAR_FREQ_MAESURE_0_S00_AXI_BASEADDR
#define PL_CLK  100000000

int get_freq(int gate_cnt){
    FREQ_MAESURE_mWriteReg(FREQ_MEASURE_BASEADDRESS, GATE_OFFSET, gate_cnt);
    FREQ_MAESURE_mWriteReg(FREQ_MEASURE_BASEADDRESS,CONTROL_OFFSET, 0);
    FREQ_MAESURE_mWriteReg(FREQ_MEASURE_BASEADDRESS,CONTROL_OFFSET, 1);
    int status = FREQ_MAESURE_mReadReg(FREQ_MEASURE_BASEADDRESS, STATUS_OFFSET);
    while((status&(1<<OK_OFFSET)) != 1){
        status = FREQ_MAESURE_mReadReg(FREQ_MEASURE_BASEADDRESS, STATUS_OFFSET);
    }
    int freq_cnt = FREQ_MAESURE_mReadReg(FREQ_MEASURE_BASEADDRESS, SIGNAL_CNT_OFFSET);
    float freq = (1.0*freq_cnt)/gate_cnt*PL_CLK;
    return freq;
}
