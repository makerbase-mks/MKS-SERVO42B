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

#ifndef __STEPPIN_H
#define __STEPPIN_H

#include "board.h"
#include "nonvolatile.h"
#include "stepper_controller.h"

void dirPinSetup(void);
void stepPinSetup(void);
void inputPinSetup(void);
void dirChanged_ISR(void);
int64_t getSteps(void);
void disableINPUTInterrupts(void);
void enableINPUTInterrupts(void);

#endif
