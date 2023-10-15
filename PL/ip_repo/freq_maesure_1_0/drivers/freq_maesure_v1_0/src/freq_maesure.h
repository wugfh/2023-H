
#ifndef FREQ_MAESURE_H
#define FREQ_MAESURE_H


/****************** Include Files ********************/
#include "xil_types.h"
#include "xstatus.h"
    
#define GATE_OFFSET         0
#define SIGNAL_CNT_OFFSET   4 //can not be writen if en = 0, signal_cnt = 0
#define CONTROL_OFFSET      8
#define STATUS_OFFSET       12 //can not be writen

//en in control register
#define EN_OFFSET           0   

// ok in status register
// if en = 0,ok = 0
#define OK_OFFSET           0 



/**************************** Type Definitions *****************************/
/**
 *
 * Write a value to a FREQ_MAESURE register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the FREQ_MAESUREdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void FREQ_MAESURE_mWriteReg(u32 BaseAddress, unsigned RegOffset, u32 Data)
 *
 */
#define FREQ_MAESURE_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/**
 *
 * Read a value from a FREQ_MAESURE register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the FREQ_MAESURE device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	u32 FREQ_MAESURE_mReadReg(u32 BaseAddress, unsigned RegOffset)
 *
 */
#define FREQ_MAESURE_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/************************** Function Prototypes ****************************/
/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the FREQ_MAESURE instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus FREQ_MAESURE_Reg_SelfTest(void * baseaddr_p);
int get_freq(int gate_cnt);

#endif // FREQ_MAESURE_H
