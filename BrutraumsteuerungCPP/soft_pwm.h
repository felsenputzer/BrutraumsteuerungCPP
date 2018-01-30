/*
 * soft_pwm.h
 *
 * Created: 10.01.2017 22:21:07
 *  Author: Johannes
 */ 
#include "main.h"

#ifndef SOFT_PWM_H_
#define SOFT_PWM_H_

#define PWM_CHANNELS	2

typedef enum e_softPWM_mode{
	e_softPWM_off,
	e_softPWM_normal,
	e_softPWM_inverted
} e_softPWM_mode;

typedef enum e_softPWM_channel{
	e_softPWM_ch1 = 0,
	e_softPWM_ch2 = 1,
	e_softPWM_ch3 = 2,
	e_softPWM_ch4 = 3,
	e_softPWM_ch5 = 4,
	e_softPWM_ch6 = 5,
	e_softPWM_ch7 = 6,
	e_softPWM_ch8 = 7
} e_softPWM_channel;

typedef struct {
	volatile uint8_t *DDR;
	volatile uint8_t *PORT;
	volatile uint8_t PIN;
	uint8_t value;
	e_softPWM_mode modus;
} softPWM_info;

void softPWM_init();
void softPWM_reset();
void softPWM_update();
void softPWM_setvalue(e_softPWM_channel channel, uint8_t value);
void softPWM_assignchannel(e_softPWM_channel channel, volatile uint8_t *DDR, volatile uint8_t *PORT, uint8_t PIN);
void softPWM_setmode(e_softPWM_channel channel, e_softPWM_mode modus);

#endif /* SOFT_PWM_H_ */