/*
 * hardware.c
 *
 * Created: 25.05.2015 18:09:32
 *  Author: Johannes
 */ 

#include "Main.h"
#include "hardware.h"
#include "ds18x20lib.h"
#include "Menue.h"


uint16_t u16SoundTime;

void hw_init(void)
{
	//Set Ports
	DDRC &= ~(1<<PC0) | ~(1<<PC1) | ~(1<<PC2) |  ~(1<<PC3);
	DDRA |= (1 << PA6) | (1 << PA7);
	SOUNDDDR |= (1 << SOUND1);
	SOUNDPORT &= ~(1 << SOUND1);
	DDRB |= (1 << PB7);
	DDRB &= ~(1 << PB5);
	PORTB &= (1 << PB5);
}

void hw_POST(void)
{
	uint8_t error = 0;
	
	switch(get_confpins())
	{
		case 3:	// Alles verfügbar 2 Räume
		{
			error |= ds1820_reset(DS1820_SENS1);
			error |= ds1820_reset(DS1820_SENS2) << 2;
			vWriteChar(nDisplayPuffer.z1,"Zwei R\xe1ume");
			vWriteChar(nDisplayPuffer.z2,"mit Wendung");
			if (error & 0x01)
			{
				t_param1.status_soll = eHeat_SensorFail;
			} 
			else
			{
				t_param1.status_soll = eHeat_On;
			}
			if (error & 0x02)
			{
				t_param2.status_soll = eHeat_SensorFail;
			}
			else
			{
				t_param2.status_soll = eHeat_On;
			}
			break;

		}
		case 2:	// Alles verfügbar 1 Raum
		{
			error |= ds1820_reset(DS1820_SENS1);
			vSaveParaWert(ePara_Enable2, 0);
			vWriteChar(nDisplayPuffer.z1,"Ein Raum");
			vWriteChar(nDisplayPuffer.z2,"mit Wendung");
			if (error & 0x01)
			{
				t_param1.status_soll = eHeat_SensorFail;
			}
			else
			{
				t_param1.status_soll = eHeat_On;
			}
			t_param2.status_soll = eHeat_NA;
			break;
		}
		case 1:	// Keine Motorwendung 2 Räume
		{
			error |= ds1820_reset(DS1820_SENS1);
			error |= ds1820_reset(DS1820_SENS2) << 2;
			vSaveParaWert(ePara_WendeAnzahl1, 0);
			vSaveParaWert(ePara_WendeAnzahl2, 0);
			vWriteChar(nDisplayPuffer.z1,"Zwei R\xe1ume");
			vWriteChar(nDisplayPuffer.z2,"ohne Wendung");
			if (error & 0x01)
			{
				t_param1.status_soll = eHeat_SensorFail;
			}
			else
			{
				t_param1.status_soll = eHeat_On;
			}
			if (error & 0x02)
			{
				t_param2.status_soll = eHeat_SensorFail;
			}
			else
			{
				t_param2.status_soll = eHeat_On;
			}
			break;
		}
		case 0:	// Keine Motorwendung 1 Raum
		{
			error |= ds1820_reset(DS1820_SENS1);
			vSaveParaWert(ePara_Enable2, 0);
			vSaveParaWert(ePara_WendeAnzahl1, 0);
			vSaveParaWert(ePara_WendeAnzahl2, 0);
			vWriteChar(nDisplayPuffer.z1,"Ein Raum");
			vWriteChar(nDisplayPuffer.z2,"ohne Wendung");
			if (error & 0x01)
			{
				t_param1.status_soll = eHeat_SensorFail;
			}
			else
			{
				t_param1.status_soll = eHeat_On;
			}
			t_param2.status_soll = eHeat_NA;
			break;
		}
	}
	
	if (error == 0)
	{
		set_status(eStatus_OK);
	}
	else
	{
		set_status(eStatus_Failure);
	}
}


void systick_timer_init(void)	//Systicktimer to 10 ms
{
	TCCR0 |= (1 << WGM01) | (1 << CS02) | (1 << CS00);
	OCR0 = 157;
	TIMSK |= (1 << OCIE0);
}

void bl_timer_init(void)
{
	TCCR2 |= (1 << WGM21) | (1 << WGM20); // Set Timer 2 as fast PWM
	TCCR2 |= (1 << COM21); // Enable Output Compare Pin
	TCCR2 |= (1 << CS22) | (1 << CS21);	  // Set BL Clock to Prescaler 256
	
	DDRD |= (1 << PD7);
	set_bl(BL_BRIGHTNESS_MAX);										
}

void vBlControl()
{
	static uint8_t brightnes = BL_BRIGHTNESS_MAX;
	if (display_active > 0)
	{
		display_active--;
		brightnes = BL_BRIGHTNESS_MAX;
		set_bl(brightnes);
	}
	else
	{
		if (brightnes > BL_BRIGHTNESS_MIN)
		{
			brightnes--;
			set_bl(brightnes);
		}
	}
}

void set_bl(uint8_t brightnes)
{
	if (brightnes >= 0xFF) brightnes = 0xFF; 
	if (brightnes <= 0x00) brightnes = 0x00;
	

	if(brightnes == 0x00)
	{
		TCCR2 &= ~(1 << COM21); // Disable Output Compare Pin
		PORTD &= ~(1 << PD7);
	}
	else if(brightnes == 0xFF)
	{
		TCCR2 |= (1 << COM21);
		OCR2 = 0xFF;
	}
	else
	{
		TCCR2 |= (1 << COM21);
		OCR2 = brightnes;
	}
}

void set_status(ESTATUS status)
{
	switch(status)
	{
		case eStatus_OK:
		{
			PORTA &= ~(1 << PA6);
			PORTA |= (1 << PA7);
			break;	
		}
		case eStatus_Warning:
		{
			PORTA |= (1 << PA6);
			PORTA |= (1 << PA7);
			break;
		}
		case eStatus_Failure:
		{
			PORTA |= (1 << PA6);
			PORTA &= ~(1 << PA7);
			break;
		}
		case eStatus_Off:
		{
			PORTA &= ~(1 << PA6);
			PORTA &= ~(1 << PA7);
			break;
		}
	}
}

uint8_t get_confpins(void)
{
	uint8_t config = 0;
	DDRA &= ~(1 << PA2) & ~(1 << PA3);
	config = PINA & ((1 << PA2) | (1 << PA3));
	config = config >> 2;
	return config;
}

uint8_t u8check_admin(void)
{
	uint8_t admin = 0;
	PORTB |= (1 << PB7);
	_delay_us(10);
	if (PINB & (1 << PB5))
	{
		PORTB &= ~(1 << PB7);
		_delay_us(10);
		if (!(PINB & (1 << PB5)))
		{
			admin = 1;
		}
	}
	PORTB &= ~(1 << PB7);
	return admin;	
	//return 1;
}


void vSoundControl(void)
{
	if (u16SoundTime > 0)
	{
		u16SoundTime--;
		SOUNDPORT |= (1 << SOUND1);
	} 
	else
	{
		SOUNDPORT &= ~(1 << SOUND1);
	}
}

void vSetSound_ms(uint16_t sTime)
{
	u16SoundTime = sTime;
}

void vSoftPWM_init()
{
	softPWM_reset();

	softPWM_assignchannel(e_softPWM_ch1, &HEATDDR, &HEATPORT, HEAT1);
	softPWM_setmode(e_softPWM_ch1, e_softPWM_normal);
	softPWM_setvalue(e_softPWM_ch1, 0);
	
	if((get_confpins() == 1 ) || (get_confpins() == 3))
	{
		softPWM_assignchannel(e_softPWM_ch2, &HEATDDR, &HEATPORT, HEAT2);
		softPWM_setmode(e_softPWM_ch2, e_softPWM_normal);
		softPWM_setvalue(e_softPWM_ch2, 0);
	}
	
	softPWM_init();
}