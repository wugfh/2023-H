#ifndef _TJC_SCREEN_H_
#define _TJC_SCREEN_H_

#include "xuartps.h"
#define SCREEN_BUF_SIZE  256

typedef struct{
    XUartPs* device;
    char* send_buf;
    char* recv_buf;
    uint32_t recv_ptr;
    uint8_t recv_end;
}hmi_screen;

int screen_init();

void send_hmi_command(char* buf);
void package_process();
void screen_clear_ans();
void send_ab_information(ab_wave* ab);

#endif