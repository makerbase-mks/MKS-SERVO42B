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

#include "MKS.h"

extern volatile bool StepperCtrl_Enabled;
extern volatile uint32_t NVM_address;

volatile bool enableState = true;
nvm_t nvmParams = {0};

int menuCalibrate(int argc, char *argv[])
{
	display_show("", "Calibrating...", "", "");

	StepperCtrl_calibrateEncoder();

	return 1;
}

int menuTestCal(int argc, char *argv[])
{
	uint16_t error;
	int32_t x,y;
	char str[25];

	display_show("", "Testing Cal...", "", "");

	error = StepperCtrl_maxCalibrationError();

	x = (36000 * (int32_t)error) / ANGLE_STEPS;
	y = x / 100;
	x = x - (y * 100);
	x = fastAbs(x);
	sprintf(str, "%d.%02d deg",y,x);
	display_show("Cal Error", str, "", "");

	while(GPIO_ReadInputDataBit(PIN_SW, PIN_SW3_ENTER) == 1)
	{
		//wait for button press
	}
	while(GPIO_ReadInputDataBit(PIN_SW, PIN_SW3_ENTER) == 0)
	{
		//wait for button release
	}
	return 1;
}

options_t stepOptions[] = {
		{"200"},
		{"400"},
		{""},
};

//returns the index of the stepOptions when called
// with no arguments.
int motorSteps(int argc, char *argv[])
{
	if (argc == 0)
	{
		uint16_t i;
		i = NVM->motorParams.fullStepsPerRotation;
		if (i == 400)
		{
			return 1;
		}
		return 0; //default to 200
	}
	if (argc > 0)
	{
		uint16_t i;
		nvm_t params;
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		i = atol(argv[0]);
		if (i != params.motorParams.fullStepsPerRotation)
		{
			params.motorParams.fullStepsPerRotation = i;
			nvmWriteConfParms(&params);
		}
	}
	return 0;
}

options_t currentOptions[] = {
		{"0"},
		{"100"},
		{"200"},
		{"300"},
		{"400"},
		{"500"},
		{"600"},
		{"700"},
		{"800"},
		{"900"},
		{"1000"},
		{"1100"},
		{"1200"},
		{"1300"},
		{"1400"},
		{"1500"},
#ifndef MKS_SERVO42B
		{"1600"},
		{"1700"},
		{"1800"},
		{"1900"},
		{"2000"},
		{"2100"},
		{"2200"},
		{"2300"},
		{"2400"},
		{"2500"},
		{"2600"},
		{"2700"},
		{"2800"},
		{"2900"},
		{"3000"},
		{"3100"},
		{"3200"},
		{"3300"},
#endif
		{""},
};

//int motorCurrent(int argc, char *argv[])
//{
//	if (argc == 1)
//	{
//		int i;
//		nvm_t params;
//		i = atol(argv[0]);
//		i = i * 100;
//		memcpy((void *)&params, (void *)NVM, sizeof(params));
//		if (i != params.motorParams.currentMa)
//		{
//			params.motorParams.currentMa = i;
//			nvmWriteConfParms(&params);
//		}
//		return i / 100;
//	}else
//	{
//		int i;
//		i = NVM->motorParams.currentMa / 100;
//		return i;
//	}
//}

int motorHoldCurrent(int argc, char *argv[])
{
	if (argc == 1)
	{
		uint16_t i;
		nvm_t params;
		i = atol(argv[0]);
		i = i * 100;
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		if (i != params.motorParams.currentHoldMa)
		{
			params.motorParams.currentHoldMa = i;

/*********************************************************************/
			params.motorParams.currentMa = (uint16_t)(i + i);

			if(params.motorParams.currentMa > 3300)
			{
				params.motorParams.currentMa = 3300;
			}
/*********************************************************************/

			nvmWriteConfParms(&params);
		}
		return i / 100;

	}else
	{
		uint16_t i;
		i = NVM->motorParams.currentHoldMa / 100;
		return i;
	}
}

options_t microstepOptions[] = {
		{"1"},
		{"2"},
		{"4"},
		{"8"},
		{"16"},
		{"32"},
		{"64"},
		{"128"},
		{"256"},
		{""}
};

int microsteps(int argc, char *argv[])
{
	if (argc == 1)
	{
		uint16_t i,steps;
		nvm_t params;
		i = atol(argv[0]);
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		steps = 0x01 << i;
		if (steps != params.SystemParams.microsteps)
		{
			params.SystemParams.microsteps = steps;
			nvmWriteConfParms(&params);
		}
		return i;
	}else
	{
		uint16_t i,j;
		i = NVM->SystemParams.microsteps;
		for (j=0; j<9; j++)
		{

			if ((0x01<<j) == i)
			{
				return j;
			}
		}
		return 0;
	}
}

options_t controlLoopOptions[] = {
		{"Simple"},
		{"Pos PID"},
		{"Vel PID"},
		{""}
};

int controlLoop(int argc, char *argv[])
{
	if (argc == 1)
	{
		uint16_t i;
		nvm_t params;
		i = atol(argv[0]);
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		if (i != params.SystemParams.controllerMode)
		{
			params.SystemParams.controllerMode = (feedbackCtrl_t)i;
			nvmWriteConfParms(&params);
		}
		return i;
	}
	return NVM->SystemParams.controllerMode;
}

options_t enablePinOptions[] = {
		{"Enable"},
		{"!Enable"},
		{""}
};

int enablePin(int argc, char *argv[])
{
	if (argc == 1)
	{
		uint16_t i;
		nvm_t params;
		i = atol(argv[0]);
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		if (i != params.SystemParams.errorPinMode)
		{
			params.SystemParams.errorPinMode = (ErrorPinMode_t)i;
			nvmWriteConfParms(&params);
		}
		return i;
	}
	return NVM->SystemParams.errorPinMode;
}

options_t dirPinOptions[] = {
		{"High CW"},
		{"High CCW"},
		{""}
};

int dirPin(int argc, char *argv[])
{
	if (argc == 1)
	{
		uint16_t i;
		nvm_t params;
		i = atol(argv[0]);
		memcpy((void *)&params, (void *)NVM, sizeof(params));
		if (i != params.SystemParams.dirPinRotation)
		{
			params.SystemParams.dirPinRotation = (RotationDir_t)i;
			nvmWriteConfParms(&params);
		}
		return i;
	}
	return NVM->SystemParams.dirPinRotation;
}

menuItem_t MenuMain[] = {	//���˵�
		{"Calibrate", menuCalibrate, NULL},
		{"Test Cal", menuTestCal, NULL},
//		{"Motor mA", motorCurrent, currentOptions},
//		{"Hold mA", motorHoldCurrent, currentOptions},
		{"Current mA", motorHoldCurrent, currentOptions},
		{"Microstep", microsteps, microstepOptions},
		{"EnablePin", enablePin, enablePinOptions},
		{"Dir Pin", dirPin, dirPinOptions},
		{ "", NULL, NULL}
};

menuItem_t MenuCal[] = {	//Calibrate�˵�
		{"Calibrate", menuCalibrate, NULL},
		{ "", NULL, NULL}
};

//check the NVM and set to defaults if there is any
void validateAndInitNVMParams(void)
{
	if (NVM->SystemParams.parametersValid != valid) //SystemParams invalid
	{
		nvmParams.sPID.Kp = 0.6;  nvmParams.sPID.Ki = 0.001;  nvmParams.sPID.Kd = 0.001;
		nvmParams.pPID.Kp = 1.0;  nvmParams.pPID.Ki = 0.0; 	  nvmParams.pPID.Kd = 0.0;
		nvmParams.vPID.Kp = 2.0;  nvmParams.vPID.Ki = 1.0; 	  nvmParams.vPID.Kd = 1.0;

		nvmParams.SystemParams.microsteps = 16;
		nvmParams.SystemParams.controllerMode = CTRL_SIMPLE;
		nvmParams.SystemParams.dirPinRotation = CCW_ROTATION; //default to clockwise rotation when dir is high
		nvmParams.SystemParams.errorLimit = (int32_t)ANGLE_FROM_DEGREES(1.8);
		nvmParams.SystemParams.errorPinMode = ERROR_PIN_MODE_ACTIVE_LOW_ENABLE;  //default to !enable pin

		if(NVM->motorParams.parametersValid == valid)
		{
			nvmParams.motorParams = NVM->motorParams;
			nvmWriteConfParms(&nvmParams);
		}
	}
	//the motor parameters are check in the stepper_controller code
	// as that there we can auto set much of them.
}

void MKS_begin(void)
{
	stepCtrlError_t stepCtrlError;

	board_init();	//set up the pins correctly on the board.

	nonvolatile_begin(); //����NVM_address

	validateAndInitNVMParams(); //systemParams init

	oled_begin();
	display_begin(); //display init
/*
#ifndef MKS_SERVO42B
	display_show("MKS", "Servo57B", VERSON, ""); //��ʾ57LOGO
#else
	display_show("MKS", "Servo42B", VERSON, ""); //��ʾ42LOGO
#endif
*/
  #ifdef MKS_SERVO42B
  	display_show("MKS", "Servo42B", VERSON, ""); //��ʾ57LOGO
  #else
	  display_show("MKS", "Servo57B", VERSON, ""); //��ʾ42LOGO
  #endif
	delay_ms(800);

	stepCtrlError = STEPCTRL_NO_CAL;
	while(STEPCTRL_NO_ERROR != stepCtrlError)
	{
		//start controller before accepting step inputs
		stepCtrlError = StepperCtrl_begin();

		//start up encoder
		if (STEPCTRL_NO_ENCODER == stepCtrlError)
		{
			display_show("Encoder", " Error!", "REBOOT", "");
			while(1);	//����
		}

		if(STEPCTRL_NO_POWER == stepCtrlError)
		{
			display_show("Waiting", "MOTOR", "POWER", "");
			while(STEPCTRL_NO_POWER == stepCtrlError)
			{
				stepCtrlError = StepperCtrl_begin(); //start controller before accepting step inputs
			}
		}

		if(STEPCTRL_NO_CAL == stepCtrlError)
		{
			display_show("   NOT ", "Calibrated", " ", "");
			delay_ms(800);
			display_setMenu(MenuCal);
			display_forceMenuActive();

			while(CalibrationTable_calValid() != true)
			{
				display_process();
			}
		}
	}
	display_setMenu(MenuMain);

	inputPinSetup(); //setup the step pin and dir pin

	RED_LED(false); //��ʼ������Ϩ������
}

void MKS_loop(void)
{
	//read the enable pin and update
	// this is also done as an edge interrupt but does not always see
	// to trigger the ISR.
	enableInput();

	if(enableState != StepperCtrl_Enabled)
	{
		StepperCtrl_enable(enableState);
	}

	display_process();
}

static void enableInput(void)
{
	if(NVM->SystemParams.errorPinMode == ERROR_PIN_MODE_ENABLE) //�ߵ�ƽ��Ч
	{
		enableState = (bool)GPIO_ReadInputDataBit(PIN_INPUT, PIN_INPUT_ENABLE); //read our enable pin
	}
	if(NVM->SystemParams.errorPinMode == ERROR_PIN_MODE_ACTIVE_LOW_ENABLE) //Ĭ�ϵ͵�ƽ��Ч
	{
		enableState = !(bool)GPIO_ReadInputDataBit(PIN_INPUT, PIN_INPUT_ENABLE); //read our enable pin
	}
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		bool error = false;

		error = StepperCtrl_processFeedback(); //handle the control loop
		BLUE_LED(error);

		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);	//writing a one clears the flag ovf flag
	}
}
