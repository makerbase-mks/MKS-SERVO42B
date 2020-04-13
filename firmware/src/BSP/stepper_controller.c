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

#include "stepper_controller.h"

extern volatile bool forwardRotation;
extern volatile bool A4950_Enabled;
extern volatile uint8_t DIR;
extern volatile uint32_t NVM_address;
extern nvm_t nvmParams;

volatile MotorParams_t motorParams;
volatile SystemParams_t systemParams;
volatile PID_t sPID; //simple control loop PID parameters
volatile PID_t pPID; //positional current based PID control parameters
volatile PID_t vPID; //velocity PID control parameters

volatile int32_t loopError = 0;
volatile bool StepperCtrl_Enabled = true;
volatile bool TC1_ISR_Enabled = false;
volatile bool enableFeedback = false; //true if we are using PID control algorithm
volatile uint32_t fullMicrosteps = 4096;
volatile int32_t fullStep = 327;
volatile int64_t numSteps = 0; //this is the number of steps we have taken from our start angle
volatile int64_t zeroAngleOffset = 0;
volatile int64_t currentLocation = 0;
//estimate of the current location from encoder feedback
//the current location lower 16 bits is angle (0-360 degrees in 65536 steps) while upper
//bits is the number of full rotations.

void setupTCInterrupts(void)
{
	TIM_DeInit(TIM1);
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 124;				//8k = 125us
	TIM_TimeBaseStructure.TIM_Prescaler = (72-1);	//72��1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	TIM_SetCounter(TIM1, 0);
	TIM_Cmd(TIM1, ENABLE);
}

void enableTCInterrupts(void)
{
	TC1_ISR_Enabled = true;
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}

void disableTCInterrupts(void)
{
	TC1_ISR_Enabled = false;
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

void StepperCtrl_updateParamsFromNVM(void)
{
	if(NVM->SystemParams.parametersValid == valid)
	{
		pPID.Kp = NVM->pPID.Kp * CTRL_PID_SCALING;
		pPID.Ki = NVM->pPID.Ki * CTRL_PID_SCALING;
		pPID.Kd = NVM->pPID.Kd * CTRL_PID_SCALING;

		vPID.Kp = NVM->vPID.Kp * CTRL_PID_SCALING;
		vPID.Ki = NVM->vPID.Ki * CTRL_PID_SCALING;
		vPID.Kd = NVM->vPID.Kd * CTRL_PID_SCALING;

		sPID.Kp = NVM->sPID.Kp * CTRL_PID_SCALING;
		sPID.Ki = NVM->sPID.Ki * CTRL_PID_SCALING;
		sPID.Kd = NVM->sPID.Kd * CTRL_PID_SCALING;

		systemParams = NVM->SystemParams;
	}else
	{
		pPID.Kp = nvmParams.pPID.Kp * CTRL_PID_SCALING;
		pPID.Ki = nvmParams.pPID.Ki * CTRL_PID_SCALING;
		pPID.Kd = nvmParams.pPID.Kd * CTRL_PID_SCALING;

		vPID.Kp = nvmParams.vPID.Kp * CTRL_PID_SCALING;
		vPID.Ki = nvmParams.vPID.Ki * CTRL_PID_SCALING;
		vPID.Kd = nvmParams.vPID.Kd * CTRL_PID_SCALING;

		sPID.Kp = nvmParams.sPID.Kp * CTRL_PID_SCALING;
		sPID.Ki = nvmParams.sPID.Ki * CTRL_PID_SCALING;
		sPID.Kd = nvmParams.sPID.Kd * CTRL_PID_SCALING;

		systemParams = nvmParams.SystemParams;
	}

	//default the error pin to input, if it is an error pin the
	// handler for this will change the pin to be an output.
	// for bidirection error it has to handle input/output it's self as well.
	// This is not the cleanest way to handle this...
	// TODO implement this cleaner
	if(NVM->motorParams.parametersValid == valid)
	{
		motorParams = NVM->motorParams;
	} else
	{
		motorParams.fullStepsPerRotation = 200;

#ifndef MKS_SERVO42B
		motorParams.currentHoldMa = 800;
		motorParams.currentMa = 1440;
#else
		motorParams.currentHoldMa = 400;
		motorParams.currentMa = 800;
#endif

		motorParams.motorWiring = true;
	}

	StepperCtrl_setRotationDirection(motorParams.motorWiring);
}

void StepperCtrl_motorReset(void)
{
	//when we reset the motor we want to also sync the motor
	//phase.Therefore we move forward a few full steps then back
	//to sync motor phasing, leaving the motor at "phase 0"
	A4950_move(0,motorParams.currentMa);
	delay_ms(1200);

  //measure new starting point
	StepperCtrl_setLocationFromEncoder();
}

void StepperCtrl_setLocationFromEncoder(void)
{
	numSteps = 0;
	currentLocation = 0;

	if (CalibrationTable_calValid())
	{
		uint16_t x,a;

		//set our angles based on previous cal data
		x = StepperCtrl_sampleMeanEncoder(102);
		a = CalibrationTable_fastReverseLookup(x); //start angle

		//we need to set our numSteps( numSteps = ((a/65536)*200)*16 ) (0-65535) is (200step * micro)
		numSteps = (int64_t)DIVIDE_WITH_ROUND(((uint32_t)a * (motorParams.fullStepsPerRotation >> 3)), (fullMicrosteps >> 3));

		currentLocation = (int64_t)a; //save position
	}

	zeroAngleOffset = StepperCtrl_getCurrentLocation(); //zero the angle shown on LCD
}

int64_t StepperCtrl_getCurrentLocation(void)
{
	int32_t a,x;

	a = (int32_t)StepperCtrl_getEncoderAngle();

	x = a - (int32_t)(currentLocation & (int64_t)ANGLE_MAX);

	if ( x > ANGLE_WRAP )
	{
		currentLocation -= ANGLE_STEPS;
	}
	if ( x < -ANGLE_WRAP )
	{
		currentLocation += ANGLE_STEPS;
	}

	currentLocation = (currentLocation & 0xFFFFFFFFFFFF0000) | (int64_t)a;

	return currentLocation;
}

int64_t StepperCtrl_getDesiredLocation(void)
{
	int64_t ret;

	ret = (int64_t)DIVIDE_WITH_ROUND((numSteps * (fullMicrosteps >> 2)), (motorParams.fullStepsPerRotation >> 2));

	return ret;
}

//The encoder needs to be calibrated to the motor.
// we will assume full step detents are correct,
// ex 1.8 degree motor will have 200 steps for 360 degrees.
// We also need to calibrate the phasing of the motor
// to the A4950. This requires that the A4950 "step angle" of
// zero is the first entry in the calibration table.
bool StepperCtrl_calibrateEncoder(void)
{
	uint16_t j = 0;
	uint16_t mean = 0;
	int32_t steps = 0;

	bool done = false;
	bool feedback = enableFeedback;
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts();
	disableINPUTInterrupts();

	A4950_Enabled = true;
	enableFeedback = false;
	systemParams.microsteps = 1;

	A4950_move(0, motorParams.currentMa);
	delay_ms(1200);

	while(!done) //Starting calibration
	{
		delay_ms(180);
		mean = StepperCtrl_sampleMeanEncoder(202);

		CalibrationTable_updateTableValue(j,mean);	//CalibrationTable[j] = mean

		//move one half step at a time, a full step move could cause a move backwards
		steps += A4950_NUM_MICROSTEPS;
		A4950_move(steps,motorParams.currentMa);
		delay_ms(60);
		steps += A4950_NUM_MICROSTEPS;
		A4950_move(steps,motorParams.currentMa);
		delay_ms(60);

		if(400 == motorParams.fullStepsPerRotation)
		{
			steps += A4950_NUM_MICROSTEPS;
			A4950_move(steps,motorParams.currentMa);
			delay_ms(60);
			steps += A4950_NUM_MICROSTEPS;
			A4950_move(steps,motorParams.currentMa);
			delay_ms(60);
		}

		j++;
		if(j >= CALIBRATION_TABLE_SIZE)
		{
			done = true;
		}
	}
	CalibrationTable_saveToFlash(); //saves the calibration to flash

	StepperCtrl_updateParamsFromNVM(); //update the local cache from the NVM

	StepperCtrl_motorReset();

	enableFeedback = feedback;

	enableINPUTInterrupts();
	if (state) enableTCInterrupts();

	return done;
}

// when sampling the mean of encoder if we are on roll over
// edge we can have an issue so we have this function
// to do the mean correctly
uint16_t StepperCtrl_sampleMeanEncoder(uint16_t numSamples)
{
	uint16_t i;
	int32_t lastx = 0,x = 0;
	int32_t min = 0,max = 0;
	int64_t sum = 0;
	int32_t mean = 0;

	for(i=0; i < numSamples; i++)
	{
		lastx = x;
		x = (int32_t)A1333_readEncoderAngle();

		if(i == 0)
		{
			lastx = x;
			min = x;
			max = x;
		}

		//wrap
		if (fastAbs(lastx - x) > CALIBRATION_WRAP) //2^15-1 = 32767(max)
		{
			if (lastx > x)
			{
				x = x + CALIBRATION_STEPS;
			} else
			{
				x = x - CALIBRATION_STEPS;
			}
		}

		if (x > max)
		{
			max = x;
		}
		if (x < min)
		{
			min = x;
		}

		sum = sum + (x);
	}

	mean = (int32_t)(sum - min - max) / (numSamples - 2); //remove the min and the max.

	//mean 0~32767
	if(mean >= CALIBRATION_STEPS)
	{
		mean = (mean - (int32_t)CALIBRATION_STEPS);
	}
	if(mean < 0)
	{
		mean = (mean + (int32_t)CALIBRATION_STEPS);
	}

	return ((uint16_t)mean);
}

uint16_t StepperCtrl_getEncoderAngle(void)
{
	uint16_t EncoderAngle;

	EncoderAngle = CalibrationTable_fastReverseLookup(StepperCtrl_sampleMeanEncoder(3));

	return EncoderAngle;
}

void StepperCtrl_setRotationDirection(bool forward)
{
	forwardRotation = forward;
}

// TODO This function does two things, set rotation direction
// and measures step size, it should be two functions.
// return is anlge in degreesx100 ie 360.0 is returned as 36000
float StepperCtrl_measureStepSize(void)
{
	int32_t angle1,angle2,x;
	bool feedback = enableFeedback;
	uint16_t microsteps = systemParams.microsteps;

	A4950_Enabled = true;
	systemParams.microsteps = 1;
	enableFeedback = false;
	motorParams.motorWiring = true; //assume we are forward wiring to start with
	StepperCtrl_setRotationDirection(motorParams.motorWiring);
	/////////////////////////////////////////
	//// Measure the full step size /////
	/// Note we assume machine can take one step without issue///

	A4950_move(0,motorParams.currentMa); //this puts stepper motor at stepAngle of zero
	delay_ms(1200);

	angle1 = StepperCtrl_sampleMeanEncoder(102); //angle1
	A4950_move(A4950_STEP_MICROSTEPS/2,motorParams.currentMa); //move one half step 'forward'
	delay_ms(100);
	A4950_move(A4950_STEP_MICROSTEPS,motorParams.currentMa); //move one half step 'forward'
	delay_ms(500);
	angle2 = StepperCtrl_sampleMeanEncoder(102); //angle2

	if ( fastAbs(angle2 - angle1) > CALIBRATION_WRAP )
	{
		//we crossed the wrap around
		if (angle1 > angle2)
		{
			angle1 = angle1 + (int32_t)CALIBRATION_STEPS;
		}else
		{
			angle2 = angle2 + (int32_t)CALIBRATION_STEPS;
		}
	}

	// if x is ~180 we have a 1.8 degree step motor, if it is ~90 we have 0.9 degree step
	x = (int32_t)(((int64_t)(angle2 - angle1) * 36000) / (int32_t)CALIBRATION_STEPS);

	//move back
	A4950_move(A4950_STEP_MICROSTEPS/2,motorParams.currentMa);
	delay_ms(100);
	A4950_move(0,motorParams.currentMa);

	systemParams.microsteps = microsteps;
	enableFeedback = feedback;

	return ((float)x)/100.0;
}

stepCtrlError_t StepperCtrl_begin(void)
{
	float x;
	enableFeedback = false;
	currentLocation = 0;
	numSteps = 0;

	//we have to update from NVM before moving motor
	StepperCtrl_updateParamsFromNVM(); //update the local cache from the NVM

	//start up encoder
	if (false == A1333_begin())
	{
		return STEPCTRL_NO_ENCODER;
	}

	//cal table init
	CalibrationTable_init();

//	A4950_begin();

	if (NVM->motorParams.parametersValid == valid)
	{
		//lets read the motor voltage
		if (0/*GetMotorVoltage()<5*/)
		{
			//if we have less than 5 volts the motor is not powered
//			uint32_t x;
//			x=(uint32_t)(GetMotorVoltage()*1000.0);
			return STEPCTRL_NO_POWER;
		}
	}else
	{
		x = StepperCtrl_measureStepSize();
		if (fabs(x) < 0.5)
		{
			return STEPCTRL_NO_POWER; //Motor may not have power
		}
	}

	//Checking the motor parameters
	//todo we might want to move this up a level to the NZS
	//especially since it has default values
	if (NVM->motorParams.parametersValid != valid) //NVM motor parameters are not set, we will update
	{
		// power could have just been applied and step size read wrong
		// if we are more than 200 steps/rotation which is most common
		// lets read again just to be sure.
		if (fabs(x) < 1.5)
		{
			//run step test a second time to be sure
			x = StepperCtrl_measureStepSize();
		}

		if (x > 0)
		{
			motorParams.motorWiring = true;
		} else
		{
			motorParams.motorWiring = false;
		}
		if (fabs(x) <= 1.2)
		{
			motorParams.fullStepsPerRotation = 400;
		}else
		{
			motorParams.fullStepsPerRotation = 200;
		}
		//Motor params are now good
		nvmParams.motorParams = motorParams;
		nvmWriteConfParms(&nvmParams);
	}

	StepperCtrl_setLocationFromEncoder(); //measure new starting point

	fullMicrosteps = (uint32_t)(ANGLE_STEPS / systemParams.microsteps);
	fullStep = (int32_t)(ANGLE_STEPS / motorParams.fullStepsPerRotation);

	if (false == CalibrationTable_calValid())
	{
		return STEPCTRL_NO_CAL;
	}

	enableFeedback = true;
	setupTCInterrupts();
	enableTCInterrupts();

	return STEPCTRL_NO_ERROR;
}

void StepperCtrl_enable(bool enable)
{
	A4950_enable(enable); //enable or disable the stepper driver as needed

	if(StepperCtrl_Enabled == true && enable == false)
	{
		enableFeedback = false;
		disableTCInterrupts();
	}
	if(StepperCtrl_Enabled == false && enable == true) //if we are enabling previous disabled motor
	{
		StepperCtrl_setLocationFromEncoder();
		enableFeedback = true;
		enableTCInterrupts();
	}

	StepperCtrl_Enabled = enable;
}

bool StepperCtrl_processFeedback(void)
{
	bool ret = false;
	int64_t desiredLoc = 0;
	int64_t currentLoc = 0;
	static __IO int64_t mean = 0;

	numSteps += (int64_t)getSteps(); //numSteps
	desiredLoc = StepperCtrl_getDesiredLocation(); //DesiredLocation
	currentLoc = StepperCtrl_getCurrentLocation(); //CurrentLocation
	mean = (31 * mean + currentLoc + 16) / 32;
	if(abs((currentLoc - mean)) < 27)	//ANGLE_FROM_DEGREES(0.15) = 27
	{
		currentLoc = mean;
	}

	ret = StepperCtrl_simpleFeedback(desiredLoc, currentLoc);

//	switch (systemParams.controllerMode)
//	{
//		case CTRL_POS_PID:
//		{
////			ret = StepperCtrl_pidFeedback(desiredLoc, currentLoc, &ctrl);
//			break;
//		}
//		default:
//		case CTRL_SIMPLE:
//		{
//			ret = StepperCtrl_simpleFeedback(desiredLoc, currentLoc);
//			break;
//		}
//		case CTRL_POS_VELOCITY_PID:
//		{
////			ret = StepperCtrl_vpidFeedback(desiredLoc, currentLoc, &ctrl);
//			break;
//		}
//	}
	return ret;
}

//this was written to do the PID loop not modeling a DC servo
// but rather using features of stepper motor.
bool StepperCtrl_simpleFeedback(int64_t desiredLoc, int64_t currentLoc)
{
	static int64_t lastError = 0;
	static int64_t iTerm = 0;
	static int32_t errorCount = 0;

	if(enableFeedback)
	{
		int64_t error;
		int32_t u;
		int32_t x;
		int32_t ma = 0;

		//error is in units of degrees when 360 degrees == 65536
		error = desiredLoc - currentLoc; //error is desired - PoscurrentPos

		iTerm += (sPID.Ki * error);
		x = (int32_t)(iTerm >> 10);

		if(fastAbs(x) > fullStep)
		{
			if(x > fullStep)
			{
				x = fullStep;
				iTerm = fullStep;
			}else
			{
				x = -fullStep;
				iTerm = -fullStep;
			}
		}

		//PID��Kp*e(k) + Ki*��e(k) + Kd[e��k��-e(k-1)]
		//PD��Kp*e(k) + Kd[e��k��-e(k-1)]
		u = ((sPID.Kp * error) >> 10) + x + ((sPID.Kd * (error - lastError)) >> 10);

		//limit error to full step
		if(fastAbs(u) > fullStep)
		{
			if (u > fullStep)
			{
				u = fullStep;
			}else
			{
				u = -fullStep;
			}
		}

		ma = (int32_t)((fastAbs(u) * fastAbs((int32_t)(motorParams.currentMa - motorParams.currentHoldMa))) / fullStep) + motorParams.currentHoldMa;

		StepperCtrl_moveToAngle((int32_t)(currentLoc + u), ma);

		lastError = error;
		loopError = error;
	} //end if enableFeedback

  // error needs to exist for some time period
	if (abs(lastError) > systemParams.errorLimit)
	{
		++errorCount;
		if (errorCount > (MKS_CONTROL_LOOP_HZ >> 7))
		{
			return true;
		}
		return false;
	}

	if(errorCount > 0)
	{
		--errorCount;
	}

	return false;
}

int sgn(int32_t val) {
    return (val > 0) ? 1 : ((val < 0) ? -1 : 0);
}

void StepperCtrl_moveToAngle(int32_t a, uint32_t ma)
{
	//we need to convert 'Angle' to A4950 steps
	// a = a & ANGLE_MAX;  //a = a & ANGLE_STEPS;	//we only interested in the current angle
	if (abs(a) > ANGLE_MAX)
		a = sgn(a) * ANGLE_MAX;
	
	//a = DIVIDE_WITH_ROUND((a * (motorParams.fullStepsPerRotation >> 3)), ((ANGLE_STEPS / A4950_STEP_MICROSTEPS) >> 3));
	a = DIVIDE_WITH_ROUND((a * (motorParams.fullStepsPerRotation >> 3)), (A4950_STEP_MICROSTEPS >> 3));

	A4950_move(a, ma);
}

uint16_t StepperCtrl_maxCalibrationError(void)
{
	uint16_t j = 0;
	int32_t steps = 0;
	uint16_t microSteps = systemParams.microsteps;

	uint16_t mean;
	uint16_t desiredAngle;
	uint16_t cal;

	int32_t dist;
	uint16_t maxError = 0;

	bool done = false;
	bool feedback = enableFeedback;
	bool state = TC1_ISR_Enabled;
	disableTCInterrupts();
	disableINPUTInterrupts();

	if (false == CalibrationTable_calValid())
	{
		return ANGLE_MAX;
	}

	A4950_Enabled = true;
	enableFeedback = false; //Running calibration test
	systemParams.microsteps = 1;
	fullMicrosteps = ANGLE_STEPS;

	StepperCtrl_motorReset(); //reset and measure new starting point

	while(!done)
	{
		delay_ms(180); //todo we should measure mean and wait until stable.

		desiredAngle = (uint16_t)(StepperCtrl_getDesiredLocation() & (int64_t)0x000000000000FFFF);
		cal = CalibrationTable_getCal(desiredAngle); //(0-32767)

		mean = StepperCtrl_sampleMeanEncoder(202);

		dist = mean - cal;

		// move one half step at a time, a full step move could cause a move backwards depending on how current ramps down
		steps += A4950_STEP_MICROSTEPS/2;
		A4950_move(steps,motorParams.currentMa);
		delay_ms(60);
		steps += A4950_STEP_MICROSTEPS/2;
		A4950_move(steps,motorParams.currentMa);
		delay_ms(60);
		++numSteps;

		if(400 == motorParams.fullStepsPerRotation)
		{
			steps += A4950_STEP_MICROSTEPS/2;
			A4950_move(steps,motorParams.currentMa);
			delay_ms(60);
			steps += A4950_STEP_MICROSTEPS/2;
			A4950_move(steps,motorParams.currentMa);
			delay_ms(60);
			++numSteps;
		}

		if(fastAbs(dist) > maxError)
		{
			maxError = (uint16_t)fastAbs(dist);
		}

		j++;
		if(j >= (CALIBRATION_TABLE_SIZE - 56))
		{
			done = true;
		}
	}
	systemParams.microsteps = microSteps;
	fullMicrosteps = (int32_t)(ANGLE_STEPS / systemParams.microsteps);

	StepperCtrl_motorReset();

	enableFeedback = feedback;

	enableINPUTInterrupts();
	if (state) enableTCInterrupts();

	return maxError;
}
