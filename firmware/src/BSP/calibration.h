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
#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include "../CMSIS/stm32f10x.h"
#include "stdlib.h"
#include "string.h"
#include "flash.h"

#define	CALIBRATION_TABLE_SIZE	(256)
#define CALIBRATION_WRAP 				((int32_t)16384)
#define CALIBRATION_STEPS 			((uint32_t)32768)	

#define ANGLE_WRAP 							((int32_t)32768)
#define ANGLE_STEPS 						((uint32_t)65536) 
#define ANGLE_MAX 							((uint16_t)65535)

#define CALIBRATION_ERROR_NOT_SET (-1) //indicated that the calibration value is not set.
#define CALIBRATION_MIN_ERROR (2)  //the minimal expected error on our calibration 4 ~=+/0.2 degrees

//Here are some useful macros
#define DIVIDE_WITH_ROUND(x,y)  ( ( x + (y >> 1) ) / y)	//DIVIDE_WITH_ROUND(x,y) = (x/y)+0.5

typedef struct {
	uint16_t FlashCalData[CALIBRATION_TABLE_SIZE];
	uint16_t status;
	uint16_t MIN;
	uint16_t MAX;
} FlashCalData_t;

typedef struct {
  uint16_t value;  //cal value
  int16_t error; 	 //error assuming it is constantly updated
} CalData_t;

uint16_t CalibrationTable_getTableIndex(uint16_t value);
bool CalibrationTable_updateTableValue(uint16_t index, uint16_t value);
void CalibrationTable_saveToFlash(void);
bool CalibrationTable_calValid(void);
uint16_t CalibrationTable_fastReverseLookup(uint16_t fastEncoderAngle);
uint16_t CalibrationTable_reverseLookup(uint16_t encoderAngle);
static uint16_t interp(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);
static uint16_t interp2(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);
int CalibrationTable_getValue(uint16_t actualAngle, CalData_t *ptrData);
uint16_t CalibrationTable_getCal(uint16_t actualAngle);
void CalibrationTable_saveToFlash(void);
void CalibrationTable_createFastCal(void);
void CalibrationTable_loadFromFlash(void);
void CalibrationTable_init(void);
void CalibrationTable_updateFastCal(void);
uint16_t CalibrationTable_getCal(uint16_t actualAngle);
void CalibrationTable_printCalTable(void);

#endif
