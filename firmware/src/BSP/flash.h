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

#ifndef __FLASH_H
#define __FLASH_H

//#include "stm32f10x.h"
#include "../CMSIS/stm32f10x.h"
#include "nonvolatile.h"

#define FLASH_WAIT_TIMEOUT			100000
#define FLASH_PAGE_SIZE   			(1024)
#define FLASH_ROW_SIZE   			  (512)

#define	FLASH_PAGE21_ADDR				0x08005400
#define	FLASH_PAGE30_ADDR				0x08007800
#define	FLASH_PAGE31_ADDR				0x08007C00
#define FLASH_PAGE32_ADDR				0x08008000
#define FLASH_checkSum_ADDR			    0x08007FFC
#define FLASH_chipID_ADDR				0x080054F0

void Flash_ProgramPage(uint32_t flashAddr, uint16_t* ptrData, uint16_t size);
void Flash_ProgramSize(uint32_t flashAddr, uint16_t* ptrData, uint16_t size);
uint16_t Flash_readHalfWord(uint32_t address);
uint32_t Flash_readWord(uint32_t address);
bool Flash_checknvmFlash(uint32_t address);

#endif
