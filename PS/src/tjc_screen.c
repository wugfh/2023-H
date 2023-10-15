#include "parameter.h"
#include "NE10.h"
#include "xuartps.h"
#include "interrupt.h"
#include "rtthread.h"
#include <stdio.h>
#include "amp_freq.h"
#include <math.h>
#include "tjc_screen.h"
#include "wave_gui.h"

hmi_screen tjc_screen;
extern ab_wave ab_inst;;
uint8_t work_mode = 0;
float phase_offset[] = {5.8, 4, 7.9, 8.8, 6.4, 10.5, 8.7, 12.7, 10.5, 11.2, 12.6, 17.2, 17.5, 15.2, 19.3, 20.6, 18.2};
static void sceen_handler(int vector, void* callback){
    rt_interrupt_enter();
    u32 IsrStatus;
    hmi_screen* screen_ptr = (hmi_screen*)callback;
	IsrStatus = XUartPs_ReadReg(screen_ptr->device->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);

	IsrStatus &= XUartPs_ReadReg(screen_ptr->device->Config.BaseAddress,
				   XUARTPS_ISR_OFFSET);
	if(((IsrStatus & ((u32)XUARTPS_IXR_RXOVR)) != (u32)0)) {
        if((XUartPs_IsReceiveData(screen_ptr->device->Config.BaseAddress)) == 1){
            while((XUartPs_IsReceiveData(screen_ptr->device->Config.BaseAddress)) == 1){
                (screen_ptr->recv_buf)[screen_ptr->recv_ptr] = XUartPs_ReadReg(screen_ptr->device->Config.BaseAddress, XUARTPS_FIFO_OFFSET);
                ++screen_ptr->recv_ptr;
            }
        }
        XUartPs_WriteReg(screen_ptr->device->Config.BaseAddress, XUARTPS_ISR_OFFSET,XUARTPS_IXR_RXOVR);
	}
	else if(((IsrStatus & ((u32)XUARTPS_IXR_TOUT)) != (u32)0)) {
        XUartPs_WriteReg(screen_ptr->device->Config.BaseAddress, XUARTPS_ISR_OFFSET,XUARTPS_IXR_TOUT);
        tjc_screen.recv_end = 1;
    }
    rt_interrupt_leave();
}

void screen_uart_init(XUartPs* uart){
    XUartPs_Config* esp_uart_config;
    esp_uart_config = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
    XUartPs_CfgInitialize(uart, esp_uart_config, esp_uart_config->BaseAddress);
    XUartPsFormat uart_format;
    uart_format.BaudRate = 115200;
    uart_format.DataBits = XUARTPS_FORMAT_8_BITS;
    uart_format.Parity = XUARTPS_FORMAT_NO_PARITY;
    uart_format.StopBits = XUARTPS_FORMAT_1_STOP_BIT;
    XUartPs_SetFifoThreshold(uart, 1);
    XUartPs_SetRecvTimeout(uart, 1);
    XUartPs_SetDataFormat(uart, &uart_format);
    XUartPs_EnableUart(uart);
    XUartPs_SetInterruptMask(uart, XUARTPS_IXR_RXOVR|XUARTPS_IXR_TOUT);
    rt_hw_interrupt_install(XPS_UART1_INT_ID, sceen_handler, &tjc_screen, "screen");
    rt_hw_interrupt_umask(XPS_UART1_INT_ID);
}

int screen_init(){
    tjc_screen.device = (XUartPs*)rt_malloc(sizeof(XUartPs));
    tjc_screen.recv_buf = (char*)rt_malloc(SCREEN_BUF_SIZE);
    tjc_screen.send_buf = (char*)rt_malloc(SCREEN_BUF_SIZE);
    tjc_screen.recv_ptr = 0;
    screen_uart_init(tjc_screen.device);
    return 0;
}


void send_hmi_command(char* buf){
    uint32_t len = snprintf(tjc_screen.send_buf, SCREEN_BUF_SIZE,"%s", buf);
    uint32_t send_cnt = 0;
    while(send_cnt < len){
        while ((!XUartPs_IsTransmitFull(tjc_screen.device->Config.BaseAddress)) && 
        (len > send_cnt)) {
            /* Fill the FIFO from the buffer */
            XUartPs_WriteReg(tjc_screen.device->Config.BaseAddress,
                    XUARTPS_FIFO_OFFSET,
                    buf[send_cnt]);
            /* Increment the send count. */
            ++send_cnt;
        }
    }
    XUartPs_WriteReg(tjc_screen.device->Config.BaseAddress,
        XUARTPS_FIFO_OFFSET,0xff);
    XUartPs_WriteReg(tjc_screen.device->Config.BaseAddress,
        XUARTPS_FIFO_OFFSET,0xff);
    XUartPs_WriteReg(tjc_screen.device->Config.BaseAddress,
        XUARTPS_FIFO_OFFSET,0xff);
    while(XUartPs_IsSending(tjc_screen.device) == TRUE);
}

void slipper_process(uint8_t* data){
    char buf[50];
    static int phase = 0;
    if(*data == 2) phase = (phase <= 0) ? 359:phase-1;
    else if(*data==1) phase = (phase >= 359) ? 0:phase+1;
    else if(*data==3){
        ab_inst.phase = phase;
        work_mode = 1;
    }
    uint32_t a_offset = ab_inst.a_freq/5e3-4;
    uint32_t b_offset = ab_inst.b_freq/5e3-4;
    if(phase < 180){
        int dis_phase = 180-phase-(phase_offset[b_offset]-phase_offset[a_offset]);
        while(dis_phase > 180) dis_phase -= 180;
        while(dis_phase < 0) dis_phase += 180;
    	snprintf(buf, 50, "x2.val=%d", dis_phase);
    }
    else
    	snprintf(buf, 50, "x2.val=%d", -180-(phase-360));
    send_hmi_command(buf);
}

void work_change(uint8_t* data){
    if(*data == 1) work_mode = 0;
    else work_mode = 1;
}

void process_select(uint8_t* data, uint8_t flag){
    switch (flag){
    case 1:
        slipper_process(data);
        break;
    case 2:
        work_change(data);
        break;
    default:
        break;
    }
}

void package_process(){
    if(tjc_screen.recv_end != 0){
        tjc_screen.recv_end = 0;
        tjc_screen.recv_ptr = 0;
        uint8_t* buf = (uint8_t*)tjc_screen.recv_buf;
        if(buf[0] == 0x55 && buf[1] == 0xaa){
            process_select(buf+3, buf[2]);
        }
    }
}


void screen_clear_ans(){
    tjc_screen.recv_end = 0;
    tjc_screen.recv_ptr = 0;
    XUartPs_SetRecvTimeout(tjc_screen.device, 4);
}

void send_ab_information(ab_wave* ab){
    char buf[100];
    snprintf(buf, 100, "x0.val=%ld", ab->a_freq);
    send_hmi_command(buf);
    snprintf(buf, 100, "x1.val=%ld", ab->b_freq);
    send_hmi_command(buf);
    if(ab->a_form == SIN_WAVE)
        snprintf(buf, 100, "t3.txt=\"SIN\"");
    else snprintf(buf, 100, "t3.txt=\"TRI\"");
    send_hmi_command(buf);
    if(ab->b_form == SIN_WAVE)
        snprintf(buf, 100, "t5.txt=\"SIN\"");
    else snprintf(buf, 100, "t5.txt=\"TRI\"");
    send_hmi_command(buf);
}
