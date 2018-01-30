/*
 * soft_pwm.c
 *
 * Created: 10.01.2017 22:20:40
 *  Author: Johannes
 */ 

#include "soft_pwm.h"
#include "hardware.h"
 
softPWM_info softPWM_channel[PWM_CHANNELS];
 
void softPWM_init(){
	for(int i = 0; i < PWM_CHANNELS; i++){
		if(softPWM_channel[i].modus != e_softPWM_off){		
			switch(softPWM_channel[i].modus){
				case e_softPWM_off:
					break;
				case e_softPWM_normal:
					*softPWM_channel[i].PORT &= ~(1 << softPWM_channel[i].PIN);
					break;
				case e_softPWM_inverted:
					*softPWM_channel[i].PORT |= (1 << softPWM_channel[i].PIN);
					break;
				default: break;
			}
			*softPWM_channel[i].DDR |= (1 << softPWM_channel[i].PIN);
		}
	}
}

void softPWM_reset(){
	for(int i = 0; i < PWM_CHANNELS; i++){
		*softPWM_channel[i].DDR = 0;
		*softPWM_channel[i].PORT = 0;
		softPWM_channel[i].PIN = 0;
		softPWM_channel[i].value = 0;
		softPWM_channel[i].modus = e_softPWM_off;
	}
}

void softPWM_update(){
	static uint8_t softPWM_count = 0;
	softPWM_count++;

	if(softPWM_count == 0){
		for(int i = 0; i < PWM_CHANNELS; i++){
			if(softPWM_channel[i].value != 0){
				switch(softPWM_channel[i].modus){
					case e_softPWM_off:
						break;
					case e_softPWM_normal:
						*softPWM_channel[i].PORT |= (1 << softPWM_channel[i].PIN);
						break;
					case e_softPWM_inverted:
						*softPWM_channel[i].PORT &= ~(1 << softPWM_channel[i].PIN);
						break;
					default: break;
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < PWM_CHANNELS; i++){
			if(softPWM_channel[i].value <= softPWM_count){
				switch(softPWM_channel[i].modus){
					case e_softPWM_off:
						break;
					case e_softPWM_normal:
						*softPWM_channel[i].PORT &= ~(1 << softPWM_channel[i].PIN);
						break;
					case e_softPWM_inverted:
						*softPWM_channel[i].PORT |= (1 << softPWM_channel[i].PIN);
						break;
					default: break;
				}
			}
		}
	}
}

void softPWM_setvalue(e_softPWM_channel channel, uint8_t value){
	softPWM_channel[channel].value = value;
}

void softPWM_assignchannel(e_softPWM_channel channel, volatile uint8_t *DDR, volatile uint8_t *PORT, uint8_t PIN){
	softPWM_channel[channel].DDR = DDR;
	softPWM_channel[channel].PORT = PORT;
	softPWM_channel[channel].PIN = PIN;	
}

void softPWM_setmode(e_softPWM_channel channel, e_softPWM_mode modus){
	softPWM_channel[channel].modus = modus;
}