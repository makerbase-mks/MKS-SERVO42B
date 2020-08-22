 /**
 * MKS SERVO42B
 * Copyright (c) 2020 Makerbase. 
 *
 * Based on nano_stepper project by Misfittech
 * Copyright (C) 2018  MisfitTech LLC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "A1333.h"

#define kNOERROR 0
#define kPRIMARYREADERROR 1
#define kEXTENDEDREADTIMEOUTERROR 2
#define kPRIMARYWRITEERROR 3
#define kEXTENDEDWRITETIMEOUTERROR 4

/*
		 1			1					6						8								4
		sync	(R/W)	 address bits	 data bits	 optinal CRC bits
*/
const uint16_t WRITE = 0x40;
const uint16_t READ = 0x00;
const uint16_t COMMAND_MASK = 0xC0;
const uint16_t ADDRESS_MASK = 0x3F;

bool error = false;
bool a1333 = true;

/*
 * PrimaryRead
 *
 * Read from the primary serial registers
 */
uint16_t PrimaryRead(uint16_t address)
{
  	uint16_t value;

	// Combine the register address and the command into one byte
  	uint16_t command = (((address & ADDRESS_MASK) | READ) << 8);

  	// take the chip select low to select the device
	GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);

  	// send the device the register you want to read
	SPI_WriteAndRead(SPI2, command);

	GPIO_SetBits(PIN_A1333, PIN_A1333_CS);
	GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);

	// send the command again to read the contents
	value = SPI_WriteAndRead(SPI2, command);

	// take the chip select high to de-select
	GPIO_SetBits(PIN_A1333, PIN_A1333_CS);

  	return value;
}

// SubsequentRead read from the same register - to be used for fast continuous reading from a sensor 
uint16_t SubsequentRead(uint16_t address)
{
  	uint16_t value;

	// Combine the register address and the command into one byte
  	uint16_t command = (((address & ADDRESS_MASK) | READ) << 8);

	GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);

	// send the command again to read the contents
	value = SPI_WriteAndRead(SPI2, command);

	// take the chip select high to de-select
	GPIO_SetBits(PIN_A1333, PIN_A1333_CS);

  	return value;
}

/*
 * PrimaryWrite
 *
 * Write to the primary serial registers
 */
uint16_t PrimaryWrite(uint16_t address, uint16_t value)
{
	// Combine the register address and the command into one byte
	uint16_t command = (((address & ADDRESS_MASK) | WRITE) << 8);

  // take the chip select low to select the device:
	GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);

	// Send most significant byte of register data
  SPI_WriteAndRead(SPI2, (command | ((value >> 8) & 0x00FF)));

  // take the chip select high to de-select:
  GPIO_SetBits(PIN_A1333, PIN_A1333_CS);

  command = ((((address + 1) & ADDRESS_MASK) | WRITE) << 8);

  // take the chip select low to select the device:
  GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);

	// Send least significant byte of register data
  SPI_WriteAndRead(SPI2, command | (value & 0x00FF));

  // take the chip select high to de-select:
	GPIO_SetBits(PIN_A1333, PIN_A1333_CS);

  return kNOERROR;
}

//A1333 SPI uses mode=3 (CPOL=1, CPHA=1)
bool A1333_begin(void)
{
	uint8_t t0 = 0;
	uint16_t flags = 0;

	error = false;
	delay_ms(100);

	// Unlock the device
  PrimaryWrite(0x3C, 0x2700);
  PrimaryWrite(0x3C, 0x8100);
  PrimaryWrite(0x3C, 0x1F00);
  PrimaryWrite(0x3C, 0x7700);

	// Make sure the device is unlocked
	flags = PrimaryRead(0x3C);
	while ((flags & 0x0001) != 0x0001)
	{
		++t0;
		if (t0>=200)
		{
			error = true;
			a1333 = false;
			return false;
		}
		flags = PrimaryRead(0x3C);
	}
	return true;
}

//read the encoders
uint16_t A1333_readAddress(uint16_t addr)
{
	uint16_t data;

	data = PrimaryRead(addr);

	return (data);
}

//read the encoders
uint16_t A1333_readEncoderAngle(void)
{
	uint16_t read_angle = 0;
	if (a1333)
	{
		read_angle = SubsequentRead(0x32);
	}
	return ((uint16_t)(read_angle & 0x7FFF));
}
