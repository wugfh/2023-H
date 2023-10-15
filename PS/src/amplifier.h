#ifndef _AMPLIFIER_H_
#define _AMPLIFIER_H_

void ad8366_gpio_init();
uint32_t ad8366_gain_2_code(float gain, uint32_t ch);
void ad8366_set_gain_serial(float gaina, float gainb);
void ad8324_gpio_init();
void ad8324_set_gain(float gain);
void ad8366_self_test();
#endif