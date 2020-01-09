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
 
#ifndef __NONVOLATILE_H
#define __NONVOLATILE_H

#include "calibration.h"
#include "flash.h"
#include "stepper_controller.h"

typedef enum {
	CW_ROTATION = 0,
	CCW_ROTATION = 1,
} RotationDir_t; //sizeof(RotationDir_t)=1

typedef enum {
	ERROR_PIN_MODE_ENABLE = 0, //error pin works like enable on step sticks
	ERROR_PIN_MODE_ACTIVE_LOW_ENABLE = 1, //error pin works like enable on step sticks
} ErrorPinMode_t; //sizeof(ErrorPinMode_t)=1

typedef enum {
	CTRL_SIMPLE = 0, //simple error controller
	CTRL_POS_PID =1, //PID  Position controller
	CTRL_POS_VELOCITY_PID = 2, //PID  Velocity controller
} feedbackCtrl_t; //sizeof(feedbackCtrl_t)=1

#pragma pack(4)
typedef struct {
	__attribute__((__aligned__(4))) float Kp;
	__attribute__((__aligned__(4))) float Ki;
	__attribute__((__aligned__(4))) float Kd;
} PIDparams_t;

#pragma pack(2)
typedef struct {
	__attribute__((__aligned__(2))) uint16_t currentMa;   //maximum current for the motor
	__attribute__((__aligned__(2))) uint16_t currentHoldMa; //hold current for the motor
	__attribute__((__aligned__(2))) bool motorWiring;  //forward wiring of motor or reverse
	__attribute__((__aligned__(2))) uint16_t fullStepsPerRotation; //how many full steps per rotation is the motor
	__attribute__((__aligned__(2))) uint16_t parametersValid;
} MotorParams_t; //sizeof(MotorParams_t)=14

#pragma pack(2)
typedef struct {
	__attribute__((__aligned__(2))) uint16_t microsteps;    //number of microsteps on the dir/step pin interface from host
	__attribute__((__aligned__(2))) RotationDir_t dirPinRotation;  //is the direction pin high for clockwise or counterClockWise
	__attribute__((__aligned__(2))) uint16_t errorLimit;    //error limit before error pin asserts 65536==360degrees
	__attribute__((__aligned__(2))) ErrorPinMode_t errorPinMode;  //is error pin used for enable, error, or bidirectional
	__attribute__((__aligned__(2))) feedbackCtrl_t controllerMode; //feedback mode for the controller
	__attribute__((__aligned__(2))) uint16_t parametersValid;
} SystemParams_t; //sizeof(SystemParams_t)=18

typedef struct {
	__attribute__((__aligned__(2))) uint16_t angle[16384];
}FastCal_t;

typedef struct {
	SystemParams_t 	SystemParams;
	MotorParams_t 	motorParams;
	PIDparams_t 		sPID; //simple PID parameters
	PIDparams_t 		pPID; //position PID parameters
	PIDparams_t 		vPID; //velocity PID parameters
} nvm_t;

#define NVM										((nvm_t*)NVM_address)
#define nvmFlashCalData				((FlashCalData_t*)FLASH_PAGE31_ADDR)
#define nvmFastCal 						((FastCal_t *)FLASH_PAGE32_ADDR)

#define	valid									(uint16_t)0x0001
#define invalid								(uint16_t)0xffff
#define NONVOLATILE_STEPS				((uint32_t)62)		//sizeof(nvm_t) = 60

void nonvolatile_begin(void);
bool nvmWriteCalTable(void *ptrData);
bool nvmWriteConfParms(nvm_t* ptrNVM);

#endif
