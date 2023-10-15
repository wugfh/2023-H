#ifndef __BOARD_H__
#define __BOARD_H__

#include "xparameters.h"
#include "xscugic_hw.h"

#if defined(__CC_ARM)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN      ((void*)&Image$$RW_IRAM1$$ZI$$Limit)
#elif defined(__GNUC__)
extern int __bss_end;
#define HEAP_BEGIN      ((void*)0x00280000)
#endif

#define HEAP_END        ((void*)0X1FFFFFFF)
#define ARM_GIC_MAX_NR          RT_CPUS_NR
#define ARM_GIC_NR_IRQS         XSCUGIC_MAX_NUM_INTR_INPUTS
#define MAX_HANDLERS            XSCUGIC_MAX_NUM_INTR_INPUTS
#define GIC_IRQ_START           0

void rt_hw_board_init(void);
uint32_t platform_get_gic_dist_base();
uint32_t platform_get_gic_cpu_base();
void rt_hw_secondary_cpu_idle_exec();
void rt_hw_secondary_cpu_up();
void secondary_cpu_c_start();
void rt_hw_secondary_cpu_bsp_start();

#endif
