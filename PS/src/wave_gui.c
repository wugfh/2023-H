#include "NE10.h"
#include "xdmaps.h"
#include "parameter.h"
#include "xstatus.h"
#include <math.h>
#include <stdlib.h>
#include "rtdef.h"
#include "interrupt.h"
#include <string.h>
#include "my_func.h"
#include "amp_freq.h"
#include "tjc_screen.h"
#include "wave_gui.h"
#include <stdio.h>

#define WINDOW_HRIGHT      	100
#define SCREEN_WIDTH		480
#define DMA_CHANNEL 		0
XDmaPs gui_dma;

void ps_dma_0_handler(int vector, void* callback){
    rt_interrupt_enter();
    XDmaPs* ptr = (XDmaPs*)callback;
    XDmaPs_ChannelData* chandata = ptr->Chans+DMA_CHANNEL;
    XDmaPs_WriteReg(ptr->Config.BaseAddress,XDMAPS_INTCLR_OFFSET,1 << chandata->ChanId);
    XDmaPs_Cmd* dma_cmd = chandata->DmaCmdToHw;
    if (dma_cmd) {
		if (!chandata->HoldDmaProg) {
            void *DmaProgBuf;
			DmaProgBuf = (void *)dma_cmd->GeneratedDmaProg;
			if (DmaProgBuf){
                	int Index;
                    XDmaPs_ProgBuf* Pool = chandata->ProgBufPool;
                    void* Buf = DmaProgBuf;
	                for (Index = 0; Index < XDMAPS_MAX_CHAN_BUFS; Index++) {
		                if (Pool[Index].Buf == Buf) {
			                if (Pool[Index].Allocated) {
				                Pool[Index].Allocated = 0;
			                }
	                    }
                    }
			    dma_cmd->GeneratedDmaProg = NULL;
		    }
		dma_cmd->DmaStatus = 0;
		chandata->DmaCmdToHw = NULL;
		chandata->DmaCmdFromHw = dma_cmd;
	}
    rt_interrupt_leave();
    }
}

void ps_dma_transfer(uint32_t src, uint32_t dst, uint32_t len){
    XDmaPs_Cmd cmd;
    memset(&cmd, 0, sizeof(XDmaPs_Cmd));
    
	cmd.ChanCtrl.SrcBurstSize = 4;
	cmd.ChanCtrl.SrcBurstLen = 4;
	cmd.ChanCtrl.SrcInc = 1;
	cmd.ChanCtrl.DstBurstSize = 4;
	cmd.ChanCtrl.DstBurstLen = 4;
	cmd.ChanCtrl.DstInc = 0;
	cmd.BD.SrcAddr = (u32) src;
	cmd.BD.DstAddr = (u32) dst;
	cmd.BD.Length = len * sizeof(int);
    cmd.ChanCtrl.SrcBurstSize = 4;

    XDmaPs_Start(&gui_dma, DMA_CHANNEL, &cmd, 0);
}

void gui_dma_init(){
    XDmaPs_Config* dma_ptr = XDmaPs_LookupConfig(XPAR_PS7_DMA_NS_DEVICE_ID);
    XDmaPs_CfgInitialize(&gui_dma, dma_ptr, dma_ptr->BaseAddress);
    XDmaPs_ResetManager(&gui_dma);
    rt_hw_interrupt_install(XPS_DMA0_INT_ID, ps_dma_0_handler, &gui_dma, "gui_dma");
    rt_hw_interrupt_umask(XPS_DMA0_INT_ID);
}

int gui_init(){
    gui_dma_init();
	screen_init();
    return 1;
}
INIT_APP_EXPORT(gui_init);

void draw_fft(ne10_fft_cpx_float32_t* fft_buf, uint32_t len, uint32_t begin, uint32_t end){
	float* buf = rt_malloc(sizeof(float)*len);
	char* send_buf = rt_malloc(sizeof(char)*256);
	for(uint32_t i = 0; i < len; ++i){
		buf[i] = sqrtf(fft_buf[i].i*fft_buf[i].i+fft_buf[i].r*fft_buf[i].r);
	}
	normalize(buf, len/2);
	for(uint32_t i = begin; i < end; ++i){
		int pos = buf[i]*WINDOW_HRIGHT;
		snprintf(send_buf, 256, "add s0.id,0,%d", pos);
		send_hmi_command(send_buf);
	}
	rt_free(buf);
	rt_free(send_buf);
}

void draw_wave(float* wave, uint32_t len, uint8_t which){
	char* send_buf = rt_malloc(sizeof(char)*256);
	float aver = average(wave, len);
	for(uint32_t i = 0; i < len; ++i){
		wave[i] -= aver;
	}
	normalize(wave, len);
	int step = len/SCREEN_WIDTH+1;
	uint32_t cnt = 0;
	for(uint32_t i = 0; i < len; i+=step){
		int pos;
		++cnt;
		if(which == 0){
			pos = (wave[i])*WINDOW_HRIGHT/2+WINDOW_HRIGHT/2;
		}
		else{
			pos = (-wave[i])*WINDOW_HRIGHT/2+WINDOW_HRIGHT/2;
		}
		snprintf(send_buf, 256, "add s%d.id,0,%d", which, pos);
		send_hmi_command(send_buf);
	}
	while(cnt < 480){
		++cnt;
		snprintf(send_buf, 256, "add s%d.id,0,0",which);
		send_hmi_command(send_buf);
	}
	rt_free(send_buf);
}




