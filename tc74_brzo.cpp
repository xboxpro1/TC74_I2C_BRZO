/*
  tc74_brzo.ccp - ESP8266 I2C library for TC74 temperature sensor. 
  This library uses the BRZO I2C library for the ESP8266.
  
  Created Jan 2017
  By Martin Stahl	https://github.com/xboxpro1/tc74_brzo

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  TC74 Address
  A0	0x48
  A1	0x49
  A2	0x4A
  A3	0x4B
  A4	0x4C
  A5	0x4D // This is the default address
  A6	0x4E
  A7	0x4F
  
  
  TC74 SCL frequency 100kHz
*/

#include <stdint.h>
#include "brzo_i2c.h"
#include "tc74_brzo.h"

uint8_t tc74_speed = 100;				//TC74 SCL frequency 100kHz
uint8_t _command[] = {0x00, 0x01, 0x80};
uint8_t _buffer[2];

TC74_BRZO::TC74_BRZO(uint8_t TC74_Addr)
{  
	_Addr = TC74_Addr;
}

uint8_t TC74_BRZO::standby(void){
	uint8_t	ecode;
	
	brzo_i2c_start_transaction(_Addr, tc74_speed);
	brzo_i2c_write(&_command[1], 1, false);
	brzo_i2c_write(&_command[2], 1, false);
	ecode = brzo_i2c_end_transaction();   
	return ecode;
}

uint8_t TC74_BRZO::on(void){
	uint8_t	ecode;
	
	brzo_i2c_start_transaction(_Addr, tc74_speed);
	brzo_i2c_write(&_command[1], 1, false);
	brzo_i2c_write(_command, 1, false);
	ecode = brzo_i2c_end_transaction();
	delay(250);		// Maximum ensured conversion time after Power-on Reset (POR to DATA_RDY) is 250 msec.
	
	return ecode;	
}

uint8_t TC74_BRZO::temp(int8_t &t){
	uint8_t	ecode;
		
	brzo_i2c_start_transaction(_Addr, tc74_speed);
	brzo_i2c_write(_command, 1, true);
	brzo_i2c_read(_buffer, 1, false);
	ecode = brzo_i2c_end_transaction();

	if(_buffer[0] & 0x80){				// Negative temperatures have bit 8 set, and range from -1 to -65 deg Celcius , see datasheet page 8 table 4.4
		t = -1*((_buffer[0] ^ 0xFF )+1);		// Convert back from two's complement and multiply by -1 to make it negative again
	}
	else{
		t = _buffer[0];	
	}	
	return ecode;
}

