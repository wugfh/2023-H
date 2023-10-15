#include "board.h"
#include "parameter.h"
#include "adc_dma.h"
#include "mmu.h"
#include "xil_io.h"
#include "xtime_l.h"
#include "interrupt.h"
#include "gic.h"
#include "interrupt.h"
#include "rtthread.h"

struct mem_desc platform_mem_desc[] = {
    {0x00000000, 0x30000, 0x00000000, NORMAL_MEM},
    {0x100000, 0X1FFFFFFF, 0x100000, NORMAL_MEM},
	{0xFFFF0000, 0xFFFFFE00, 0xFFFF0000, NORMAL_MEM},
    {0x40000000, 0xFFFEFFFF, 0x40000000, DEVICE_MEM}
};
u32 platform_mem_desc_size = 4;

#define GT_WRITE_REG(addr, value)       Xil_Out32(addr, value)
#define GT_COUNTER_LO                   (GLOBAL_TMR_BASEADDR+GTIMER_COUNTER_LOWER_OFFSET)
#define GT_COUNTER_HI                   (GLOBAL_TMR_BASEADDR+GTIMER_COUNTER_UPPER_OFFSET)
#define GT_CONTROL                      (GLOBAL_TMR_BASEADDR+GTIMER_CONTROL_OFFSET)
#define GT_INTERRUPT_STATUS             (GLOBAL_TMR_BASEADDR+0x0c)
#define GT_COMPARATOR_LO                (GLOBAL_TMR_BASEADDR+0x10)   
#define GT_COMPARATOR_HI                (GLOBAL_TMR_BASEADDR+0x14)
#define GT_AUTO_INCREMENT               (GLOBAL_TMR_BASEADDR+0x18)

#define PRESCALER_OFFSET                8
#define AUTO_INCREAMENT_OFFSET          3
#define IRQ_ENABLE_OFFSET               2
#define COMP_ENABLE_OFFSET              1
#define TIMER_ENABLE_OFFSET             0

static void global_timer_handler(int vector, void* Callbackred){
    rt_interrupt_enter();
    GT_WRITE_REG(GT_INTERRUPT_STATUS, 0);
    rt_tick_increase();
    rt_interrupt_leave();
}

static s32 global_timer_intr_init(){
    GT_WRITE_REG(GT_CONTROL, 0);
    GT_WRITE_REG(GT_COUNTER_LO, 0);
    GT_WRITE_REG(GT_COUNTER_HI, 0);
    GT_WRITE_REG(GT_COMPARATOR_LO, COUNTS_PER_SECOND/1000);
    GT_WRITE_REG(GT_COMPARATOR_HI, 0);
    GT_WRITE_REG(GT_AUTO_INCREMENT, COUNTS_PER_SECOND/1000);
    GT_WRITE_REG(GT_CONTROL, (1<<AUTO_INCREAMENT_OFFSET)|
                                (1<<IRQ_ENABLE_OFFSET)|
                                (1<<COMP_ENABLE_OFFSET)|1);
    GT_WRITE_REG(GT_INTERRUPT_STATUS, 0);

    rt_hw_interrupt_install(XPAR_GLOBAL_TMR_INTR, global_timer_handler, NULL, "global timer");          
    rt_hw_interrupt_umask(XPAR_GLOBAL_TMR_INTR);
    return XST_SUCCESS;
}

static void rt_hw_tick_init(){
    global_timer_intr_init();
}

uint32_t platform_get_gic_dist_base(){
    return XPAR_SCUGIC_DIST_BASEADDR;
}

uint32_t platform_get_gic_cpu_base(){
    return XPAR_SCUGIC_CPU_BASEADDR;
}

void rt_hw_board_init(){
    rt_hw_interrupt_init();
    rt_system_heap_init(HEAP_BEGIN, HEAP_END);
    rt_hw_tick_init();
    rt_components_board_init();
}

void rt_hw_secondary_cpu_idle_exec(){
    asm volatile ("wfe":::"memory","cc");
}

void rt_hw_secondary_cpu_up(){
    // secondary_cpu_c_start();
}

void secondary_cpu_c_start(){
    // rt_hw_vector_init();
    // rt_hw_spin_lock(&_cpus_lock);
    
}

void rt_hw_secondary_cpu_bsp_start(){
    
}
