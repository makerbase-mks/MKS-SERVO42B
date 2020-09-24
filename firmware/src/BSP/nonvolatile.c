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

#include "nonvolatile.h"
#include "usart.h"

extern volatile MotorParams_t motorParams;
extern volatile SystemParams_t systemParams;
extern volatile bool TC1_ISR_Enabled;
extern volatile uint32_t fullMicrosteps;

volatile uint32_t NVM_address = FLASH_PAGE30_ADDR;

//NVM_address
void nonvolatile_begin(void)
{
	uint32_t i = ((FLASH_PAGE_SIZE / NONVOLATILE_STEPS) - 1); //(1024/62) = 16(0~15)
	
	NVM_address = FLASH_PAGE30_ADDR;
	
	for(i=((FLASH_PAGE_SIZE / NONVOLATILE_STEPS) - 1); i > 0; i--)
	{
		if( Flash_readHalfWord( (FLASH_PAGE30_ADDR + (i * NONVOLATILE_STEPS)) ) != invalid )
		{
			NVM_address = (FLASH_PAGE30_ADDR + (i * NONVOLATILE_STEPS));
			return;
		}
	}
}

bool nvmWriteCalTable(void *ptrData)
{
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts(); 
	disableINPUTInterrupts();
	
	Flash_ProgramPage(FLASH_PAGE31_ADDR, ptrData, (sizeof(FlashCalData_t)/2));
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();
	return true;
}

bool nvmWriteConfParms(nvm_t* ptrNVM)
{		
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts();
	disableINPUTInterrupts();
	
	ptrNVM->motorParams.parametersValid  = valid;
	ptrNVM->SystemParams.parametersValid = valid;
	
	if(Flash_readHalfWord(NVM_address) != invalid && ((NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR))
	{
		NVM_address += NONVOLATILE_STEPS;
		
		while( Flash_checknvmFlash(NVM_address) == false )
		{																													 
			if( (NVM_address + NONVOLATILE_STEPS) < FLASH_PAGE31_ADDR )
			{
				NVM_address += NONVOLATILE_STEPS;
			}
			else
			{
				NVM_address = FLASH_PAGE30_ADDR;
				Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
				return true;
			}
		}
		Flash_ProgramSize(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	else 
	{
		NVM_address = FLASH_PAGE30_ADDR;
		Flash_ProgramPage(NVM_address, (uint16_t*)ptrNVM, (sizeof(nvm_t)/2));
	}
	
	motorParams = NVM->motorParams; //update motorParams
	systemParams = NVM->SystemParams; //update systemParams
	fullMicrosteps = (uint32_t)(ANGLE_STEPS / systemParams.microsteps);
	StepperCtrl_setLocationFromEncoder(); //measure new starting point
	
	enableINPUTInterrupts();
	if (state) enableTCInterrupts();	
	return true;
}
