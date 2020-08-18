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
#ifndef __A1333_H
#define __A1333_H

//#include "stm32f10x.h"
#include "../CMSIS/stm32f10x.h"
#include "board.h"
#include "delay.h"
#include "spi.h"

uint16_t PrimaryRead(uint16_t address);
uint16_t SubsequentRead(uint16_t address);
uint32_t ExtendedRead(uint16_t address);
uint16_t PrimaryWrite(uint16_t address, uint16_t value);
uint16_t ExtendedWrite(uint16_t address, uint32_t value);
bool A1333_begin(void);
uint16_t A1333_readAddress(uint16_t addr);
uint16_t A1333_readEncoderAngle(void);

void A1333_setRegister_ANG(uint8_t orate, uint8_t hysteresis, uint16_t zero_offset,  uint8_t ro, uint8_t rd);

#endif
