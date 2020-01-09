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

#ifndef __MKS_H
#define __MKS_H

#include "display.h"
#include "stepper_controller.h"
#include "stdlib.h"
#include "steppin.h"

int menuCalibrate(int argc, char *argv[]);
int menuTestCal(int argc, char *argv[]);
int motorSteps(int argc, char *argv[]);
int motorCurrent(int argc, char *argv[]);
int motorHoldCurrent(int argc, char *argv[]);
int microsteps(int argc, char *argv[]);
int controlLoop(int argc, char *argv[]);
int enablePin(int argc, char *argv[]);
int dirPin(int argc, char *argv[]);

void validateAndInitNVMParams(void);
static void enableInput(void);
void MKS_begin(void);
void MKS_loop(void);

#endif
