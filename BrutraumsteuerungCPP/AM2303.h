/*
 * AM2303.h
 *
 * Created: 19.08.2015 23:33:21
 *  Author: Johannes
 */ 

#ifndef AM2303_H_
#define AM2303_H_

#define AM2303_PORT		PORTA
#define AM2303_PIN		PINA
#define AM2303_DDR		DDRA
#define AM2303_HYG_1	PA1
#define AM2303_HYG_2	PA0

typedef struct{
	uint8_t u8_humidity_H;
	uint8_t u8_humidity_L;
	uint8_t u8_temperatur_H;
	uint8_t	u8_temperatur_L;
	uint8_t u8_parity;
}AM2303_Registers;

typedef struct{
	float f_humidity;
	float f_temperatur;
}AM2303_Data;


uint8_t u8_AM2303_ReadBit(uint8_t);
uint8_t u8_AM2303_ReadByte(uint8_t);
uint8_t u8_AM2303_Init(uint8_t);
AM2303_Registers u8_AM2303_Readout(uint8_t);
AM2303_Data t_AM2303_Get_Data(uint8_t);
uint8_t u8_AM2303_CheckTelegramm(AM2303_Registers);
float AM2302_u16_to_float(uint16_t);


#endif /* AM2303_H_ */