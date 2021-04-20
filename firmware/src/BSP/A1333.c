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
bool a1333 = false;
uint16_t prev_WriteCommand = 0; 
#ifndef SINGLE_A1333_SPI
	#define SINGLE_A1333_SPI true // for dual ASIL-D A1333 last SPI command (prev_WriteCommand) needs to be tracked seperatly for each cheap 
#endif
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
	if (SINGLE_A1333_SPI && prev_WriteCommand != command){// (when false) it enables faster (sequential) read from a register if the same register was accessed previously 
		// take the chip select low to select the device
		GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);
		// send the device the register you want to read
		SPI_WriteAndRead(SPI2, command);

		GPIO_SetBits(PIN_A1333, PIN_A1333_CS);
	}

	GPIO_ResetBits(PIN_A1333, PIN_A1333_CS);
	// send the command again to read the contents
	value = SPI_WriteAndRead(SPI2, command);
	// take the chip select high to de-select
	GPIO_SetBits(PIN_A1333, PIN_A1333_CS);

	prev_WriteCommand = command;
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
	
	prev_WriteCommand = command;
	return kNOERROR;
}

/*
 * ExtendedRead
 *
 * Write to extended locations
 */
uint32_t ExtendedRead(uint16_t address)
{	
	uint32_t value;
	/*Extended access is provided to additional memory space via the
	direct registers. This access includes the EEPROM and EEPROM
	shadow registers. All extended registers are up to 32 bits wide.
	Invoking an extended read access is a three-step process:*/

	PrimaryWrite(0x0A, address); //write ERA address first
	PrimaryWrite(0x0C, 1<<15); //write 1 into ercs.exr to invoke 
	value = PrimaryRead(0x10); //read ERDLow data
	value |= ((uint32_t) PrimaryRead(0x0E))<<16; //read ERDHigh data

	// add here checks for eeprom extended access progress //
	return value;
}

/*
 * ExtendedWrite
 *
 * Write to extended locations
 */
uint16_t ExtendedWrite(uint16_t address, uint32_t value)
{
	//Invoking an extended write access is a three-step process:
	
	PrimaryWrite(0x02, address); //write EWA address first
	PrimaryWrite(0x04, (value >> 16)); //write EWDHigh data
	PrimaryWrite(0x06, value & 0x0000FFFF); //write EWDLow data
	PrimaryWrite(0x08, 1<<15); //lastly write 1 into EWCS.EXW to invoke 
	

	// add here checks for eeprom extended access progress //
	return kNOERROR;
}

void A1333_setRegister_ANG(uint8_t orate, uint8_t hysteresis, uint16_t zero_offset,  uint8_t ro, uint8_t rd){
	uint8_t ANG_reg = 0x5C;
	ExtendedWrite(ANG_reg, ((orate & 0xF) << 20) | (rd <<  19) | (ro << 18) | ((hysteresis & 0x3F) << 12) | (zero_offset & 0xFFF));
	ExtendedRead(ANG_reg); //read back for debugging
}

//A1333 SPI uses mode=3 (CPOL=1, CPHA=1)
bool A1333_begin(void)
{
	uint8_t t0 = 0;
	bool test_completed = false;
	delay_ms(100);

	// Unlock A1333 to write to eeprom or shadow memory using keycode:
	PrimaryWrite(0x3C, 0x0000);
	PrimaryWrite(0x3C, 0x2700);
	PrimaryWrite(0x3C, 0x8100);
	PrimaryWrite(0x3C, 0x1F00);
	PrimaryWrite(0x3C, 0x7700);
	// Make sure the device is unlocked
	if  (!(PrimaryRead(0x3C) & 0x0001)){
		return false; //device didn't unlock
	}else{
		a1333 = true; 
	}

	if ((ExtendedRead(0x5e)>>20) & 0x3){
		return false; //check if writing to eeprom or shadown memory is pernamently locked
	}

	PrimaryWrite(0x1E, 0xB7B9);//start A1333 self test, also clears SDN and BDN
	while (!test_completed) 
	{
		delay_ms(1);
		++t0;
		test_completed = (PrimaryRead(0x22) & 0x0020) > 0; //STA.SDN [5]- wait for test completion (should be 30ms)
		if(t0 > 35){  //typically it takes 30ms to run the test. If not, time-out.
			return false;
		}
	}
	if((PrimaryRead(0x24) >> 10) & 0x1){
		return false; //ERR.STF - self-test failed
	}
	delay_ms(40); //cooldown after test - this seems to be needed before writing to extended registers


	// Disable hardware averaging and enable hysteresis filter:
	// 	orate | hysteresis | zero_offset | ro | rd
	A1333_setRegister_ANG(0, 4, 0, 0, 0); //averaging cause delay and doesn't deal well with low freq noise according to datasheet
	
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
		read_angle = PrimaryRead(0x32);
	}
	return read_angle;
}
