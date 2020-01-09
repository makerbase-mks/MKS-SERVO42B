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

#include "flash.h"
//clean£¬write uint16_t data
/*
 *	flashAddr:
 *	ptrData:
 *	size: uint16_t size
*/
void Flash_ProgramPage(uint32_t flashAddr, uint16_t* ptrData, uint16_t size)
{
	uint32_t i;
	
	FLASH_Unlock();
	
	FLASH_ErasePage(flashAddr);
	
	for(i=0; i < size; i++)
	{
		if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT) != FLASH_COMPLETE)
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		}
		FLASH_ProgramHalfWord(flashAddr, (*ptrData));
		flashAddr += 2;
		ptrData += 1;
	}
	
	FLASH_Lock();
}

//not clean£¬write uint16_t data
/*
 *	flashAddr:
 *	ptrData:
 *	size:
*/
void Flash_ProgramSize(uint32_t flashAddr, uint16_t* ptrData, uint16_t size)
{
	uint32_t i;
	
	FLASH_Unlock(); 
	
	for(i=0; i<size; i++)
	{
		if(FLASH_WaitForLastOperation(FLASH_WAIT_TIMEOUT) != FLASH_COMPLETE)
		{
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		}	
		FLASH_ProgramHalfWord(flashAddr, (*ptrData));
		flashAddr += 2;
		ptrData += 1;
	}
	
	FLASH_Lock();
}

//read uint16_t data
uint16_t Flash_readHalfWord(uint32_t address)
{
  return (*(__IO uint16_t*)address); 
}

//read uint32_t data
uint32_t Flash_readWord(uint32_t address)
{
  return (*(__IO uint32_t*)address); 
}

//CRC
bool Flash_checknvmFlash(uint32_t address)
{
	uint32_t i;
	for(i=0; i < (sizeof(nvm_t)/2); i++)
	{
		if( Flash_readHalfWord( address + (i * 2) ) != invalid )
			return false;
	}
	return true;
}
