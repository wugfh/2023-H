#ifndef _WAVE_GUI_H_
#define _WAVE_GUI_H_

#define LCD_LENGTH          480
#define LCD_WIDTH           320
#define WAVE_WINDOW_LENTH   (LCD_LENGTH*3/4)
#define WAVE_WINDOW_WIDTH   LCD_WIDTH

int gui_init();
void ps_dma_transfer(uint32_t src, uint32_t dst, uint32_t len);
void draw_fft(ne10_fft_cpx_float32_t* fft_buf, uint32_t len, uint32_t begin, uint32_t end);

#endif
