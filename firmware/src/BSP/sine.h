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
#ifndef __SINE_H
#define __SINE_H

//#include "stm32f10x.h"
#include "../CMSIS/stm32f10x.h"

#define SINE_STEPS	((uint16_t)1024)

#define SINE_MAX	(uint16_t)(32767)

int16_t sine(uint16_t angle);
int16_t cosine(uint16_t angle);
int32_t fastAbs(int32_t v);

#endif
