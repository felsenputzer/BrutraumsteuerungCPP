/*
 * main.h
 *
 * Created: 12.01.2013 13:11:13
 *  Author: Johannes
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#ifdef F_CPU
#undef F_CPU
#endif
#define F_CPU 16000000UL

#ifdef MAIN_DATA
#define MAIN_LOC
#else
#define MAIN_LOC extern
#endif

#define DAY 86400
extern "C" {
	#include <stdint.h>
};
#include "soft_pwm.h"
#include "AM2303.h"
#include "PID_controller.h"

enum EParameter {
	ePara_Sollwert1,
	ePara_Sollwert2,
	ePara_Hysterese1,
	ePara_Hysterese2,
	ePara_Offset1,
	ePara_Offset2,
	ePara_CoolDown1,
	ePara_CoolDown2,
	ePara_WendeAnzahl1,
	ePara_WendeAnzahl2,
	ePara_WendeZeit1,
	ePara_WendeZeit2,
	ePara_Enable1,
	ePara_Enable2,
	ePara_Regelparam_Kp1,
	ePara_Regelparam_Ki1,
	ePara_Regelparam_Kd1,
	ePara_Regelparam_Kp2,
	ePara_Regelparam_Ki2,
	ePara_Regelparam_Kd2,
	ePara_Last
};

enum ESTATUS {
	eStatus_OK,
	eStatus_Warning,
	eStatus_Failure,
	eStatus_Off
};

enum eHeat_status {
	eHeat_Off,
	eHeat_On,
	eHeat_SensorFail,
	eHeat_Cooldown,
	eHeat_Overtemp,
	eHeat_CriticalOvertemp,
	eHeat_NA
};

typedef struct {			// Struktur für temp_update()
	int16_t *t_sollwert;
	int16_t *t_offset;
	int16_t *t_hysterese;
	int16_t *t_cooldown;
	uint8_t sensor;
	uint16_t *t_istwert;
	e_softPWM_channel channel;
	PID_para *pid_para;
	eHeat_status status_ist;
	eHeat_status status_soll;
}t_param;

MAIN_LOC t_param t_param1, t_param2;

MAIN_LOC unsigned char ucUpdateDisplay;
MAIN_LOC AM2303_Data Hygrometer_1;
MAIN_LOC AM2303_Data Hygrometer_2;
MAIN_LOC uint16_t Temp1;
MAIN_LOC uint16_t Temp2;
MAIN_LOC uint8_t disp;

void vSavePara(int16_t cPara);
void vSaveParaWert(int16_t cPara,int16_t wWert);
extern int16_t pwParameter[ePara_Last];
extern int16_t pwParameterSave[ePara_Last];

extern uint32_t sys_time;
extern uint16_t display_active;

extern const int16_t pwParameterMin[ePara_Last];
extern const int16_t pwParameterMax[ePara_Last];
extern const int16_t pwParameterDef[ePara_Last];

#define HEATDDR		DDRA
#define HEATPIN		PINA
#define HEATPORT	PORTA
#define HEAT1		PA5
#define HEAT2		PA4

#define SOUNDDDR	DDRB
#define SOUNDPIN	PINB
#define SOUNDPORT	PORTB
#define SOUND1		PB4

#define MCTRL_DDR	DDRC
#define MCTRL_PORT	PORTC
#define MCTRL_PIN	PINC
#define MEN_DDR		DDRB
#define MEN_PORT	PORTB
#define MEN_PIN		PINB
#define MOT_EN_A	PB3
#define MOT_EN_B	PB2
#define MOT_IN_A1	PC7
#define MOT_IN_A2	PC6
#define MOT_IN_B1	PC5
#define MOT_IN_B2	PC4

#define BL_BRIGHTNESS_MAX	179
#define BL_BRIGHTNESS_MIN	10

extern "C" {
	#include <avr/io.h>
	#include <avr/eeprom.h>
	#include <util/delay.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
};

#endif /* MAIN_H_ */