/*
 * hardware.h
 *
 * Created: 25.05.2015 18:09:46
 *  Author: Johannes
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

enum eMotor_Mode{
	eMotor_off,
	eMotor_stop,
	eMotor_left,
	eMotor_right
	};

void hw_init(void);
void hw_POST(void);
void systick_timer_init(void);
void bl_timer_init(void);
void vBlControl(void);
void set_bl(uint8_t brightnes);
void set_status(ESTATUS status);
uint8_t get_confpins(void);
void vSoundControl(void);
void vSetSound_ms(uint16_t sTime);
void motor_init(void);
void motor_a_set(enum eMotor_Mode mode);
void motor_b_set(enum eMotor_Mode mode);
enum eMotor_Mode get_motor_a(void);
enum eMotor_Mode get_motor_b(void);
void vSoftPWM_init(void);
uint8_t u8check_admin(void);

#endif /* HARDWARE_H_ */