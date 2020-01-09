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
 
#ifndef __STEPPER_CONTROLLER_H
#define __STEPPER_CONTROLLER_H

#include "A4950.h"
#include "delay.h"
#include "nonvolatile.h"
#include "calibration.h"
#include "A1333.h"
#include "math.h"
#include "steppin.h"

typedef enum {
	STEPCTRL_NO_ERROR=0,
	STEPCTRL_NO_POWER=1, 	//no power to motor
	STEPCTRL_NO_CAL=2,	 	//calibration not set
	STEPCTRL_NO_ENCODER=3,//encoder not working
} stepCtrlError_t;

typedef struct {
	int32_t Kp;
	int32_t Ki;
	int32_t Kd;
} PID_t;

#define ANGLE_FROM_DEGREES(x) ((int32_t) ( ((float)ANGLE_STEPS * (float)(x)+ 180.0) / 360.0 ) )
#define ANGLE_T0_DEGREES(x) ( (float) ((float(x) * 360.0) / ((float)ANGLE_STEPS) ))

//this scales the PID parameters from Flash to floating point
// to fixed point int32_t values
#define CTRL_PID_SCALING 			(uint16_t)(1024)
#define MKS_CONTROL_LOOP_HZ		(uint16_t)(6000) //update rate of control loop

void setupTCInterrupts(void);
void enableTCInterrupts(void);
void disableTCInterrupts(void);
void  StepperCtrl_motorReset(void);
void StepperCtrl_setLocationFromEncoder(void);
int64_t StepperCtrl_getCurrentLocation(void);
int64_t StepperCtrl_getDesiredLocation(void);
bool StepperCtrl_calibrateEncoder(void);
uint16_t StepperCtrl_sampleMeanEncoder(uint16_t numSamples);
uint16_t StepperCtrl_getEncoderAngle(void);
void StepperCtrl_updateParamsFromNVM(void);
void StepperCtrl_setRotationDirection(bool forward);
float StepperCtrl_measureStepSize(void);
stepCtrlError_t StepperCtrl_begin(void);
void StepperCtrl_enable(bool enable);
bool StepperCtrl_processFeedback(void);
bool StepperCtrl_simpleFeedback(int64_t desiredLoc, int64_t currentLoc);
void StepperCtrl_moveToAngle(int32_t a, uint32_t ma);
uint16_t StepperCtrl_maxCalibrationError(void);

#endif
