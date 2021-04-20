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
#include "A4950.h"

volatile bool forwardRotation = true;
volatile bool A4950_Enabled = true;

//phase 1
inline static void bridge1(int state)
{
	if (state == 0)
	{
		PIN_A4950->BSRR = PIN_A4950_IN1;	//GPIO_SetBits(PIN_A4950, PIN_A4950_IN1);		//IN1=1
		PIN_A4950->BRR = PIN_A4950_IN2;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN2);	//IN2=0
	}
	if (state == 1)
	{
		PIN_A4950->BRR = PIN_A4950_IN1;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN1);	//IN1=0	
		PIN_A4950->BSRR = PIN_A4950_IN2;	//GPIO_SetBits(PIN_A4950, PIN_A4950_IN2);		//IN2=1	
	}
	if (state == 3)
	{
		PIN_A4950->BRR = PIN_A4950_IN1;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN1);	//IN1=0
		PIN_A4950->BRR = PIN_A4950_IN2;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN2);	//IN2=0
	}
}

//phase 2
inline static void bridge2(int state)
{
	if (state == 0)
	{
		PIN_A4950->BSRR = PIN_A4950_IN3;	//GPIO_SetBits(PIN_A4950, PIN_A4950_IN3);		//IN3=1
		PIN_A4950->BRR = PIN_A4950_IN4;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN4);	//IN4=0
	}
	if (state == 1)
	{
		PIN_A4950->BRR = PIN_A4950_IN3;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN3);	//IN3=0
		PIN_A4950->BSRR = PIN_A4950_IN4;	//GPIO_SetBits(PIN_A4950, PIN_A4950_IN4);		//IN4=1
	}
	if (state == 3)
	{
		PIN_A4950->BRR = PIN_A4950_IN3;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN3);	//IN3=0
		PIN_A4950->BRR = PIN_A4950_IN4;		//GPIO_ResetBits(PIN_A4950, PIN_A4950_IN4);	//IN4=0
	}
}

//Vref
inline static void setVREF(uint16_t VREF12, uint16_t VREF34)
{
	//VREF_SCALER reduces PWM resolution by 2^VREF_SCALER,
	//but also increasesPWM freq - needed for low pass filter to effectively filter V reference)
	VREF_TIM->CCR3 = VREF12>>VREF_SCALER;						//TIM_SetCompare3(VREF_TIM, VREF12);
	VREF_TIM->CCR4 = VREF34>>VREF_SCALER;						//TIM_SetCompare4(VREF_TIM, VREF34);
}


void A4950_enable(bool enable)
{
	A4950_Enabled = enable;
	
	if (A4950_Enabled == false)
	{
		setVREF(0,0); //turn current off
		bridge1(3); //tri state bridge outputs
		bridge2(3); //tri state bridge outputs
	}
}

// this is precise move and modulo of A4950_STEP_MICROSTEPS is a full step.
// stepAngle is in A4950_STEP_MICROSTEPS units..
// The A4950 has no idea where the motor is, so the calling function has to
// tell the A4950 what phase to drive motor coils.
// A4950_STEP_MICROSTEPS is 256 by default so stepAngle of 1024 is 360 degrees
// Note you can only move up to +/-A4950_STEP_MICROSTEPS from where you
// currently are.
int32_t A4950_move(int32_t stepAngle, uint32_t mA)
{
	uint16_t angle = 0;
	int32_t sin,cos;
	uint16_t vrefSin,vrefCos;

	if (A4950_Enabled == false)
	{
		setVREF(0,0); //turn current off
		bridge1(3); 	//tri state bridge outputs
		bridge2(3); 	//tri state bridge outputs
		return stepAngle;
	}

	//handle roll overs, could do with modulo operator
	angle = (uint16_t)(stepAngle & 0x000003ff);

	//calculate the sine and cosine of our angle
	sin 	= 	 sine(angle);
	cos 	= cosine(angle);

	//if we are reverse swap the sign of one of the angels
	if(false == forwardRotation)
	{
		cos = -cos;
	}

	//convert value into DAC scaled to 3300mA max
	vrefSin = (uint16_t)(mA * fastAbs(sin) / 3300);
	//convert value into DAC scaled to 3300mA max
	vrefCos = (uint16_t)(mA * fastAbs(cos) / 3300);

	setVREF(vrefSin,vrefCos); //VREF12	VREF34

	if (sin > 0)
	{
		bridge1(1);
	}else
	{
		bridge1(0);
	}
	if (cos > 0)
	{
		bridge2(1);
	}else
	{
		bridge2(0);
	}
	
	return stepAngle;
}

