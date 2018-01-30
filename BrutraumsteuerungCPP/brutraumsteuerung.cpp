/*
 * Brutraumsteuerung.c
 *
 * Created: 12.01.2013 10:42:23
 *  Author: Johannes
 */ 

/*************************************************************************/
/******************** Variablen & Deklarationen **************************/
/*************************************************************************/

#define MAIN_DATA

#include "main.h"
#include "hardware.h"
#include "lcd.h"
#include "Menue.h"
#include "ds18x20lib.h"
#include <math.h>
#include "pid_controller.h"

void init();
void vUpdateDisplay();
void vCheckKeys();

PID_para pid_ch1, pid_ch2;

int16_t pwParameter[ePara_Last];
int16_t pwParameterSave[ePara_Last];
uint16_t pwParameterEprom[ePara_Last] EEMEM;

const int16_t pwParameterMin[ePara_Last] = {000,000,00,00,-50,-50,000,000,0,0,00,00,0,0,00000,00000,00000,00000,00000,00000};
const int16_t pwParameterMax[ePara_Last] = {400,400,50,50, 50, 50,180,180,9,9,60,60,1,1,20000,20000,20000,20000,20000,20000};
const int16_t pwParameterDef[ePara_Last] = {373,373,00,00, 00, 00, 60, 60,5,5,30,30,1,0, 3000,   75,  500, 3000,   75,  500};
uint8_t count = 0;

bool b_tempupdate1 = false;
bool b_tempupdate2 = false;
bool b_hygroupdate1 = false;
bool b_hygroupdate2 = false;
bool b_updateparam = false;
bool b_checkplausibility = false;

uint32_t sys_time = 0;
uint16_t display_active = 1200;

/*************************************************************************/
/********************Lokale Funktionen************************************/
/*************************************************************************/

/********************Temperatur lesen und PID Regler Updaten*************/
void temp_update(t_param param)
{

	cli();
	if ((param.status_ist == eHeat_NA) || (param.status_ist == eHeat_Off))  
	{
		softPWM_setvalue(param.channel, 0);
		sei();
		return;
	}
	*param.t_istwert = ds1820_read_temp(param.sensor);
	//sei();
	*param.t_istwert = *param.t_istwert + *param.t_offset;
	
	if(((sys_time / 100) < (DAY - *param.t_cooldown*60)) && (param.status_ist == eHeat_On))
	{
		softPWM_setvalue(param.channel, pid_update(param.pid_para, ((float)*param.t_sollwert) / 10, ((float)*param.t_istwert) / 10));
	}else
	{
		softPWM_setvalue(param.channel, 0);
	}
	ucUpdateDisplay = 1;
	//cli();
	ds1820_measure(param.sensor);
	sei();

}
/*********************Parameter aus dem EProm in Nutzdaten schreiben*****/
void vUpdateParam(t_param *temp1, t_param *temp2)
{
	if (temp1->status_soll != eHeat_NA)
	{
		if (pwParameterSave[ePara_Enable2] == 1)
		{
			temp2->status_soll = eHeat_On;
		}
		else
		{
			temp2->status_soll = eHeat_Off;
		}
	}
	if(temp2->status_soll != eHeat_NA)
	{
		if (pwParameterSave[ePara_Enable1] == 1)
		{
			temp1->status_soll = eHeat_On;
		}
		else
		{
			temp1->status_soll = eHeat_Off;
		}
	}
}

/*********************Plausibilitäts und Fehler Prüfung******************/
void checkplausibility(t_param *temp, AM2303_Data hygro)
{
	float temp_1 = (float)(*(temp->t_istwert) / 10);
	float temp_2 = hygro.f_temperatur;
	if ((temp->status_soll == eHeat_Off) || (temp->status_soll == eHeat_NA))
	{
		temp->status_ist = temp->status_soll; 
	}
	else if (temp_1 == 85.0)
	{
		temp->status_ist =  eHeat_SensorFail;
	}
	else if (temp_1 == -127)
	{
		temp->status_ist =  eHeat_SensorFail;
	}
	else if (temp_1 > 75.0)
	{		
		temp->status_ist =  eHeat_CriticalOvertemp;
		vSetSound_ms(250);
	}
	else if (temp_1 > 50.0)
	{
		temp->status_ist =  eHeat_Overtemp;
	}
	else if (fabsf(temp_1 - temp_2) >= 5.0)
	{
		temp->status_ist = eHeat_SensorFail;
	}
	else
		temp->status_ist = temp->status_soll;
}

/*********************Hygrometer Daten lesen****************************/
void hygro_update(uint8_t used_pin, AM2303_Data *data)
{
	*data = t_AM2303_Get_Data(used_pin);
}

/*********************System Initialisieren*******************************/
void init()
{
	hw_init();
	set_status(eStatus_Failure);
	bl_timer_init();
	lcd_init();
	set_bl(BL_BRIGHTNESS_MAX);
	vWriteChar(nDisplayPuffer.z1,"Brutraum-");
	vWriteChar(nDisplayPuffer.z2,"Steuerung");
	vUpdateDisplay();
	_delay_ms(3000);
	set_status(eStatus_Warning);
	vWriteChar(nDisplayPuffer.z1,"Selbsttest");
	vWriteChar(nDisplayPuffer.z2,"bitte warten...");
	vUpdateDisplay();
	_delay_ms(3000);
	hw_POST();
	vUpdateDisplay();
	vSoftPWM_init();
	_delay_ms(3000);
	systick_timer_init();
	vSetSound_ms(25);
	
	const uint8_t ZeichenHeizen[8] = {
		0x12,
		0x09,
		0x09,
		0x12,
		0x12,
		0x09,
		0x00,
		0x1f
	};
	lcd_generatechar(1,ZeichenHeizen);
	
	vActionExec(eAction_SelectGrundMenue,0);
	vAnzeigeBearbeiten();
	
	t_param1.sensor = DS1820_SENS1;
	t_param1.t_hysterese = &pwParameterSave[ePara_Hysterese1];
	t_param1.t_offset = &pwParameterSave[ePara_Offset1];
	t_param1.t_sollwert = &pwParameterSave[ePara_Sollwert1];
	t_param1.t_cooldown = &pwParameterSave[ePara_CoolDown1];
	t_param1.t_istwert = &Temp1;
	t_param1.channel = e_softPWM_ch1;
	t_param1.pid_para = &pid_ch1;
	
	t_param2.sensor = DS1820_SENS2;
	t_param2.t_hysterese = &pwParameterSave[ePara_Hysterese2];
	t_param2.t_offset = &pwParameterSave[ePara_Offset2];
	t_param2.t_sollwert = &pwParameterSave[ePara_Sollwert2];
	t_param2.t_cooldown = &pwParameterSave[ePara_CoolDown2];
	t_param2.t_istwert = &Temp2;
	t_param2.channel = e_softPWM_ch2;
	t_param2.pid_para = &pid_ch2;
	
	for(int i = 0; i < ePara_Last; ++i)
	{
		pwParameter[i] =  eeprom_read_word(&pwParameterEprom[i]);
		if(pwParameter[i] == 0xFFFF) pwParameter[i] = pwParameterDef[i];
		
		pwParameterSave[i] = pwParameter[i];
	}
	Temp1 = 0;
	Temp2 = 0;
	disp = 0;
	
	pid_reset(&pid_ch1);
	pid_init(&pid_ch1, &pwParameterSave[ePara_Regelparam_Kp1], &pwParameterSave[ePara_Regelparam_Ki1], &pwParameterSave[ePara_Regelparam_Kd1]);
	
	pid_reset(&pid_ch2);
	pid_init(&pid_ch2, &pwParameterSave[ePara_Regelparam_Kp2], &pwParameterSave[ePara_Regelparam_Ki2], &pwParameterSave[ePara_Regelparam_Kd2]);
	
	sei();
}
/*************************************************************************/
/****************************** Globale Funktionen ***********************/
/*************************************************************************/

/**************************** Parameter Speichern ***********************/
void vSavePara(int16_t cPara)
{
	eeprom_write_word (&pwParameterEprom[cPara], pwParameter[cPara]);
	pwParameterSave[cPara] = pwParameter[cPara];
}


/**************************** Parameter Speichern ***********************/
void vSaveParaWert(int16_t cPara,int16_t wWert)
{
	pwParameter[cPara] = wWert;
	pwParameterSave[cPara] = wWert;
	eeprom_write_word (&pwParameterEprom[cPara], wWert);
}


/*************************************************************************/
/****************************** Main Schleife ****************************/
/*************************************************************************/
int main(void)
{	
	init();

    while(1)
    {
		if(b_tempupdate1){
			b_tempupdate1 = 0;
			temp_update(t_param1);		
		}
		if(b_tempupdate2){
			b_tempupdate2 = 0;
			temp_update(t_param2);	
		}
		if(b_hygroupdate1)
		{
			hygro_update(AM2303_HYG_1, &Hygrometer_1);
			b_hygroupdate1 = 0;
			sei();
		}
		if(b_hygroupdate2)
		{
			hygro_update(AM2303_HYG_2, &Hygrometer_2);
			b_hygroupdate2 = 0;
			sei();
		}
		if (ucUpdateDisplay != 0)
		{
			vAnzeigeBearbeiten();
			vUpdateDisplay();
		}
		if (b_updateparam)
		{
			vUpdateParam(&t_param1, &t_param2);
			b_updateparam = false;
		}
		if (b_checkplausibility)
		{
			checkplausibility(&t_param1, Hygrometer_1);
			checkplausibility(&t_param2, Hygrometer_2);
			b_checkplausibility = false;
		}
    }
}

/*************************************************************************/
/************************** Interrupt Routinen ***************************/
/*************************************************************************/


/********************Interrupt für Sensorbearbeitung**********************/
ISR(TIMER0_COMP_vect)
{
	const uint32_t u32UpdateInterval = 500;
	cli();
	
	sys_time++;

	vCheckKeys();
	vBlControl();
	vSoundControl();
	softPWM_update();
	sei();
	
	if ((sys_time % u32UpdateInterval == 0) && (t_param2.status_soll != eHeat_NA)) b_tempupdate1 = true;
	if (((sys_time + 250) % u32UpdateInterval == 0) && (t_param2.status_soll != eHeat_NA)) b_tempupdate2 = true;
	if ((sys_time + 100) % u32UpdateInterval == 0) b_hygroupdate1 = true;
	if ((sys_time + 150) % u32UpdateInterval == 0) b_hygroupdate2 = true;
	if ((sys_time + 200) % u32UpdateInterval == 0)
	{
		b_checkplausibility = true;
		if ((disp == 0) && (t_param2.status_soll != eHeat_NA))
		{
			disp = 1;
		}
		else
		{
			disp = 0;
		}
	}
	if ((sys_time + 250) % u32UpdateInterval == 0)
	{
		b_updateparam = true;
	}
	if((sys_time / 100) > DAY) sys_time = 0;
}