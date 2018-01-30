//#################################################################################
//#################################################################################
//#################################################################################
/*	Library to use ds18x20 with ATMEL Atmega family.
	For short ds18x20 wires there is no need for an external pullup resistor.
	If the wire length exceeds one meter you should use a 4.7k pullup resistor 
	on the data line. This library does not work for parasite power. 
	You can just use one ds18x20 per Atmega Pin.
	
	Copyright (C) 2010 Stefan Sicklinger

	For support check out http://www.sicklinger.com
    
	This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/
//#################################################################################
//#################################################################################
//#################################################################################

#include "Main.h"
#include "ds18x20lib.h"

extern "C" {
#include <util/delay.h>
};

//----------------------------------------
// Reset DS18S20
//----------------------------------------
uint8_t ds1820_reset(uint8_t used_pin)
{
  uint8_t err=100;
  DS1820_DDR |= 1<<used_pin;						// define as ouput
  DS1820_PORT &= ~(1<<used_pin);					//Pull low
  _delay_us(480);				;					// 480 us
  DS1820_DDR &= ~(1<<used_pin);						// define as input
  DS1820_PORT |= 1<<used_pin;						//Pullup on
  _delay_us(66);										// 66 us
  err = (DS1820_PIN & (1<<used_pin)) >> used_pin;	// no presence detect --> err=1 otherwise err=0
  _delay_us(240);									// 240 us
  if( (DS1820_PIN & (1<<used_pin)) == 0 ){			// short circuit --> err=2
    err = 2;
	}
  return err;
}
//-----------------------------------------
// Write one bit to DS18S20
//-----------------------------------------
void ds1820_wr_bit(uint8_t wrbit,uint8_t used_pin)
{
	if (wrbit ==0)	{
		DS1820_DDR |= 1<<used_pin;					// define as ouput
  		DS1820_PORT &= ~(1<<used_pin);				//Pull low
		_delay_us(60);
		DS1820_DDR &= ~(1<<used_pin);				// define as input
	  	DS1820_PORT |= 1<<used_pin;					//Pullup on
		_delay_us(4);
	}
	if (wrbit ==1)	{
		DS1820_DDR |= 1<<used_pin;					// define as ouput
  		DS1820_PORT &= ~(1<<used_pin);				//Pull low
		_delay_us(10);
		DS1820_DDR &= ~(1<<used_pin);				// define as input
	  	DS1820_PORT |= 1<<used_pin;					//Pullup on
		_delay_us(54);
	}
}
//-----------------------------------------
// Read one bit from DS18S20
//-----------------------------------------
uint8_t ds1820_re_bit(uint8_t used_pin)
{
	uint8_t rebit;
	DS1820_DDR |= 1<<used_pin;							// define as ouput
  	DS1820_PORT &= ~(1<<used_pin);						//Pull low
	_delay_us(1);
	DS1820_DDR &= ~(1<<used_pin);						// define as input
	DS1820_PORT |= 1<<used_pin;							//Pullup on
	_delay_us(10);
	rebit = (DS1820_PIN & (1<<used_pin)) >> used_pin; 	//Read bit
	_delay_us(50);
	return rebit;								
}
//-----------------------------------------
// Read 1 byte from DS18S20
//-----------------------------------------
uint8_t ds1820_re_byte(uint8_t used_pin)
{
	uint8_t rebyte =0x00;
	uint8_t rebit;
	uint8_t i;

	for (i=0;i<8;i++)
	{
		rebit=ds1820_re_bit(used_pin);
		//_delay_us(2);									//be on the save side
		if (rebit==1){
			rebyte|=(1<<i);
		}
	}
	return(rebyte);
}
//-----------------------------------------
// Write 1 byte to DS18S20
//-----------------------------------------
void ds1820_wr_byte(uint8_t wrbyte,uint8_t used_pin)
{
	uint8_t i;
	for (i=0; i<8; i++) // writes byte, one bit at a time
	{	    
		ds1820_wr_bit((wrbyte & 0b00000001),used_pin);
		wrbyte = wrbyte >> 1;
	}
	_delay_us(5);
}
//-----------------------------------------
// Measure temperature
//-----------------------------------------
void  ds1820_measure(uint8_t used_pin)
{
	uint8_t error;
	
	error=ds1820_reset(used_pin);									//1. Reset
	if (error==0)
	{
		ds1820_wr_byte(0xCC,used_pin);  							//2. skip ROM
		ds1820_wr_byte(0x44,used_pin);  							//3. ask for temperature conversion
	}
}
//-----------------------------------------
// Read temperature
//-----------------------------------------
uint16_t  ds1820_read_temp(uint8_t used_pin)	
{
	uint8_t error,i;
    uint8_t scratchpad[9];
	uint16_t temp=0;
	scratchpad[0]=0;
	scratchpad[1]=0;
	scratchpad[2]=0;
	scratchpad[3]=0;
	scratchpad[4]=0;
	scratchpad[5]=0;
	scratchpad[6]=0;
	scratchpad[7]=0;
	scratchpad[8]=0;
								 
	    error=ds1820_reset(used_pin);								//5. Reset
	    ds1820_wr_byte(0xCC,used_pin);  							//6. skip ROM
	    ds1820_wr_byte(0xBE,used_pin);  							//7. Read entire scratchpad 9 bytes
    
	    for (i=0; i<9; i++)     									//8. Get scratchpad byte by byte
	    {
	       scratchpad[i] = ds1820_re_byte(used_pin); 					//9. read one DS18S20 byte
	    }
		if((scratchpad[1] & 0xF8) == 0){
			temp = ((float)scratchpad[0]*0.625);
			temp += ((float)(scratchpad[1] & 0x07)*160.0);
		}else{
			//temp = ((float)scratchpad[0]*0.625);
			//temp += ((float)(scratchpad[1] & 0x07)*160.0);
			//temp *= -1;
			temp = 0;
		}

	return temp;
}
//-----------------------------------------
// Initialize DS18S20
//-----------------------------------------
void  ds1820_init(uint8_t used_pin)	
{
	uint8_t error;
	uint16_t i =0;
	error=ds1820_reset(used_pin);									//1. Reset
	if (error==0){
	    ds1820_wr_byte(0xCC,used_pin);  							//2. skip ROM
	    ds1820_wr_byte(0x44,used_pin);  							//3. ask for temperature conversion
	    while (ds1820_re_byte(used_pin)==0xFF && i<1000){			//4. wait until conversion is finished 
			_delay_us(1);
			i++;
		}
		error=ds1820_reset(used_pin);								//5. Reset
	    ds1820_wr_byte(0xCC,used_pin);  							//6. skip ROM
	    ds1820_wr_byte(0xBE,used_pin);  							//7. Read entire scratchpad 9 bytes
		}
    
	   
}
