/*
 * AM2303.c
 *
 * Created: 19.08.2015 23:33:02
 *  Author: Johannes
 */ 

#include "Main.h"
#include "AM2303.h"
#include <util/delay.h>

uint8_t u8_AM2303_ReadBit(uint8_t used_pin)
{
	uint8_t max_wait;
	uint8_t bittime;
	max_wait = 60;
	while((AM2303_PIN & (1 << used_pin)) )
	{
		max_wait--;
		if (max_wait == 00)
		{
			return 0xFF;
		}
		_delay_us(1);
	}
	max_wait = 60;
	while(!(AM2303_PIN & (1 << used_pin)))
	{
		max_wait--;
		if (max_wait == 00)
		{
			return 0xFF;
		}
		_delay_us(1);
	}
	bittime = 0;
	while((AM2303_PIN & (1 << used_pin)))
	{
		bittime += 5;
		if (255 == bittime)
		{
			return 0xFF;
		}
		_delay_us(5);
	}
	if ((10 <= bittime) && (45 >= bittime))
	{	
		return 0;
	} 
	else if ((55 <= bittime) && (90 >= bittime))
	{
		return 1;
	}
	else
	{
		return 0xFF;
	}
	
}

uint8_t u8_AM2303_ReadByte(uint8_t used_pin)
{
	uint8_t rbyte = 0;
	for (int i = 0; i < 8; i++)
	{
		rbyte <<= 1;
		rbyte |= u8_AM2303_ReadBit(used_pin);
	}
	return rbyte;
}

uint8_t u8_AM2303_Init(uint8_t used_pin)
{
	uint8_t error = 0;
	uint8_t max_wait;
	AM2303_DDR |= (1 << used_pin);
	AM2303_PORT &= ~(1 << used_pin);
	_delay_ms(20);
	AM2303_PORT |= (1 << used_pin);
	AM2303_DDR &= ~(1 << used_pin); // Bus wird losgelassen und muss zwischen 20us und 200us High bleiben
	cli();
	max_wait = 200;
	while((AM2303_PIN & (1 << used_pin))) // Warte bis der Bus auf LOW gezogen wird
	{
		if (max_wait == 0)
		{
			error++;
			break;
		}
		max_wait--;
		_delay_us(1);
	}
	max_wait = 85;
	while(!(AM2303_PIN & (1 << used_pin))) // Warten das der Bus wieder los gelassen wird
	{
		if (max_wait == 0)
		{
			error++;
			break;
		}
		max_wait--;
		_delay_us(1);
	}
	max_wait = 85;
	while((AM2303_PIN & (1 << used_pin)))
	{
		if (max_wait == 0)
		{
			error++;
			break;
		}
		max_wait--;
		_delay_us(1);
	}
	return error;
}	

AM2303_Registers t_AM2303_Readout(uint8_t used_pin)
{
	uint8_t error = 0;
	AM2303_Registers data;
	error += u8_AM2303_Init(used_pin);
	if (error == 0)
	{
		data.u8_humidity_H = u8_AM2303_ReadByte(used_pin);
		data.u8_humidity_L = u8_AM2303_ReadByte(used_pin);
		data.u8_temperatur_H = u8_AM2303_ReadByte(used_pin);
		data.u8_temperatur_L = u8_AM2303_ReadByte(used_pin);
		data.u8_parity = u8_AM2303_ReadByte(used_pin);
		if (u8_AM2303_CheckTelegramm(data))
		{	
			return data;
		}
		else
		{
			return data;
		}
	}
	else
	{
		return data;
	}	
}

AM2303_Data t_AM2303_Get_Data(uint8_t used_pin)
{
	AM2303_Registers data_in;
	AM2303_Data data_out;
	uint16_t help = 0;
	
	data_in = t_AM2303_Readout(used_pin);
	help |= (uint16_t)(data_in.u8_humidity_H << 8);
	help |= data_in.u8_humidity_L;
	data_out.f_humidity= AM2302_u16_to_float(help);
	help = 0;
	help |= (uint16_t)data_in.u8_temperatur_H << 8;
	help |= data_in.u8_temperatur_L;
	data_out.f_temperatur = AM2302_u16_to_float(help);
	return data_out;
}

uint8_t u8_AM2303_CheckTelegramm(AM2303_Registers data) 
{
	if (data.u8_humidity_H + data.u8_humidity_L + data.u8_temperatur_H + data.u8_temperatur_L == data.u8_parity)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

float AM2302_u16_to_float(uint16_t data)
{
	float erg = 0;
	if ((data & 0x8000) == 0)
	{
		erg = data;
		erg = erg / 10;
	} 
	else
	{
		erg = data;
		erg = erg / 10;
		erg *= -1;
	}
	return erg;
}