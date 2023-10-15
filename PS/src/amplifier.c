#include "parameter.h"
#include "sleep.h"
#include "xgpiops.h"
#include "amplifier.h"
#include "math.h"

XGpioPs ad8366_gpio;

enum{
    AD8366_SENB_OFFSET          = 0,
    AD8366_CS_OFFSET            = 1,
    AD8366_SDAT_OFFSET          = 2,      
    AD8366_SCLK_OFFSET          = 3 
};

enum{
    AD8324_TXEN_OFFSET          = 3,
    AD8324_SDAT_OFFSET          = 1,
    AD8324_SCLK_OFFSET          = 5,
    AD8324_DATAEN_OFFSET        = 7,
    AD8324_SLEEP_OFFSET         = 0,    
};

void ad8366_gpio_init(){
    XGpioPs_Config *config_ptr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    XGpioPs_CfgInitialize(&ad8366_gpio, config_ptr, config_ptr->BaseAddr);
    XGpioPs_SetDirection(&ad8366_gpio, EMIO_BANK1, GPIO_BANK_OUT);
    XGpioPs_SetOutputEnable(&ad8366_gpio, EMIO_BANK1, GPIO_BANK_OUT);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SENB_OFFSET), 1);
}


uint32_t ad8366_gain_2_code(float gain, uint32_t ch){
    if(ch == 1){
        if(gain < 1.12)
            return 0;
        return logf(gain/1.043)/0.02758-1;
    }
    else {
        if(gain < 4.5)
            return 0;
        return logf(gain/3.78)/0.03235;
    }
}

uint32_t f_3db(uint32_t gain_code){
    return 0;
}



void ad8366_set_gain_serial(float gaina, float gainb){
    uint32_t gaincode_b = ad8366_gain_2_code(gainb,1)&0x3f;
    uint32_t gaincode_a = ad8366_gain_2_code(gaina,0)&0x3f;
    uint32_t gaincode = (gaincode_a<<6)|gaincode_b;
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 1);
    usleep(500);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_CS_OFFSET), 1);
    usleep(500);
    for(uint32_t i = 0; i < 12; ++i){
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SDAT_OFFSET), gaincode&0x1);
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 0);
        usleep(50);
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 1);
        usleep(50);
        gaincode >>= 1;
    }
    usleep(50);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_CS_OFFSET), 0);
}

void ad8366_self_test(){
    for(uint32_t i = 0; i < 64; ++i){
        uint8_t tmp = i&0x3f;
        uint16_t gaincode = (tmp<<6)|tmp;
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 1);
        usleep(500);
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_CS_OFFSET), 1);
        usleep(500);
        for(uint32_t i = 0; i < 12; ++i){
            XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SDAT_OFFSET), gaincode&0x1);
            XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 0);
            usleep(50);
            XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_SCLK_OFFSET), 1);
            usleep(50);
            gaincode >>= 1;
        }
        usleep(50);
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8366_CS_OFFSET), 0);
    }
}

void ad8324_gpio_init(){
    XGpioPs_Config *config_ptr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    XGpioPs_CfgInitialize(&ad8366_gpio, config_ptr, config_ptr->BaseAddr);
    XGpioPs_SetDirection(&ad8366_gpio, EMIO_BANK1, GPIO_BANK_OUT);
    XGpioPs_SetOutputEnable(&ad8366_gpio, EMIO_BANK1, GPIO_BANK_OUT);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SLEEP_OFFSET), 1);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_TXEN_OFFSET), 1);
}

void ad8324_set_gain(float gain){
    if(gain > 30) gain = 30;
    uint8_t gain_code = gain+30;
    gain_code &= 0x3f;
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SCLK_OFFSET), 1);
    usleep(500);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_DATAEN_OFFSET), 0);
    usleep(500);
    uint32_t tmp = 0x20;
    for(uint32_t i = 0; i < 12; ++i){
        if((tmp&gain_code) != 0){
            XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SDAT_OFFSET), 1);
        }
        else {
            XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SDAT_OFFSET), 1);
        }
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SCLK_OFFSET), 0);
        usleep(50);
        XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_SCLK_OFFSET), 1);             
        usleep(50);
        tmp >>= 1;
    }
    usleep(50);
    XGpioPs_WritePin(&ad8366_gpio, EMIO_PIN(AD8324_DATAEN_OFFSET), 1);
}
