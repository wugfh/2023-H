#include "xaxidma.h"
#include "adc_dma.h"
#include "my_func.h"
#include "xscugic.h"
#include "my_func.h"
#include "parameter.h"
#include "interrupt.h"
#include "math.h"

static XAxiDma axi_dma_inst;
Adc_control adc_inst;

int dma_check(){
    return (XAxiDma_ReadReg(axi_dma_inst.RegBase, XAXIDMA_RX_OFFSET+XAXIDMA_SR_OFFSET) & XAXIDMA_IRQ_ERROR_MASK);
}

static void s2mm_handler(int vector, void* CallbackRef){
    rt_interrupt_enter();
    u32 irq_status;
    XAxiDma* p_inst = (XAxiDma*)CallbackRef;
    XAxiDma_IntrDisable(p_inst, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

    irq_status = XAxiDma_IntrGetIrq(p_inst, XAXIDMA_DEVICE_TO_DMA);

    if(!(irq_status&XAXIDMA_IRQ_ALL_MASK))
        return ;

    if (irq_status & XAXIDMA_IRQ_ERROR_MASK){
//        error_say(1, "dma s2mm intrrupt error\n");
        adc_inst.error = XST_FAILURE;
		XAxiDma_Reset(p_inst);
		int time_out = RESET_TIMEOUT;

		while(time_out){
			if (XAxiDma_ResetIsDone(p_inst))
				break;
			time_out -= 1;
		}
	}
    
    if(irq_status & XAXIDMA_IRQ_IOC_MASK)
        adc_inst.rx_done = 1;

    XAxiDma_IntrAckIrq(p_inst, irq_status, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrEnable(p_inst, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    rt_interrupt_leave();
}

static int dma_intr_init(int intr_s2mm_id){

    rt_hw_interrupt_install(intr_s2mm_id, s2mm_handler, &axi_dma_inst, "adc_dma");
    rt_hw_interrupt_umask(intr_s2mm_id);

    XAxiDma_IntrEnable(&axi_dma_inst, (XAXIDMA_IRQ_IOC_MASK|XAXIDMA_IRQ_ERROR_MASK), XAXIDMA_DEVICE_TO_DMA);

    return XST_SUCCESS;
}

static int dma_init(int device_id, int intr_id, int s2mm_intr_id){
    XAxiDma_Config* dma_config = XAxiDma_LookupConfig(device_id);
    if(dma_config == NULL){
        xil_printf("dma look up config error");
        xil_printf("can not find target dma device\n");
        return XST_FAILURE;
    }

    if(XAxiDma_CfgInitialize(&axi_dma_inst, dma_config) != XST_SUCCESS)
        return XST_FAILURE;

    XAxiDma_Reset(&axi_dma_inst);
    while(XAxiDma_ResetIsDone(&axi_dma_inst) == 0);

    if(dma_check() != 0) {
        xil_printf("dma return an error\n");
        return XST_FAILURE;
    }

    return dma_intr_init(s2mm_intr_id);

    return XST_SUCCESS;
}

int adc_translation_init(){
    int status = XST_SUCCESS;
    status = dma_init(
            XPAR_AXIDMA_0_DEVICE_ID, 
            XPAR_SCUGIC_0_DEVICE_ID, 
            XPAR_FABRIC_AXIDMA_0_VEC_ID);
    
    adc_inst.adc_dma = &axi_dma_inst;
    adc_inst.rx_done = 0;
    adc_inst.error = status;
    adc_inst.adc_buf = (uint32_t*)rt_malloc_align((adc_inst.adc_dma->TxBdRing.MaxTransferLen)*sizeof(uint32_t), 4);
    adc_inst.adc_buf_len = (adc_inst.adc_dma->TxBdRing.MaxTransferLen);
    adc_inst.sin_volt = (float*)rt_malloc_align(adc_inst.adc_buf_len*sizeof(float), sizeof(float));
    adc_inst.cos_volt = (float*)rt_malloc_align(adc_inst.adc_buf_len*sizeof(float), sizeof(float));
    return status;
}
INIT_DEVICE_EXPORT(adc_translation_init);

static void adc_volt(uint32_t* iq_data, uint32_t len){
    int* ptr = (int*)iq_data;
    for(int i = 0; i < len; ++i){
        (adc_inst.sin_volt)[i] = ptr[i];
    }
    
}


int adc_translation(int recv_len){
    // if(recv_len > adc_inst.adc_buf_len)
    //     error_say(1, "receive length is larger than adc buf lenghth\n");

    int time_out = RESET_TIMEOUT;
    if(time_out == 0)
        return XST_FAILURE;

    int status = XAxiDma_SimpleTransfer(adc_inst.adc_dma, (UINTPTR)adc_inst.adc_buf, recv_len, XAXIDMA_DEVICE_TO_DMA);
    if(status != XST_SUCCESS)
        return status;
    
    while(adc_inst.rx_done == 0);
    adc_inst.rx_done = 0;

    Xil_DCacheFlushRange((INTPTR)adc_inst.adc_buf, recv_len);

    adc_volt(adc_inst.adc_buf, recv_len);

    return XST_SUCCESS;
}

