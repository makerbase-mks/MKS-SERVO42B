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
 #include "steppin.h"

extern volatile uint32_t NVM_address;
extern volatile int64_t numSteps;
volatile int64_t steps = 0;

void dirPinSetup(void)
{
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	EXTI_InitTypeDef  EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_ClearITPendingBit(EXTI_Line1);
	EXTI_Init(&EXTI_InitStructure);
}

void stepPinSetup(void)
{
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = INPUT_MAX;
	TIM_TimeBaseStructure.TIM_Prescaler = 0 ;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(INPUT_TIM, &TIM_TimeBaseStructure);
	
	TIM_TIxExternalClockConfig(INPUT_TIM,TIM_TS_TI1FP1, TIM_ICPolarity_Rising, 5);
	
	TIM_SetCounter(INPUT_TIM, 0);
	TIM_Cmd(INPUT_TIM, ENABLE);
}

void dirChanged_ISR(void)
{
	int dir = 0;
	
	if(GPIO_ReadInputDataBit(PIN_INPUT, PIN_INPUT_DIR) != (uint8_t)Bit_RESET)
	{
		dir = 1;
	}
	
	if(CW_ROTATION == NVM->SystemParams.dirPinRotation)
	{
		dir = !dir; //reverse the rotation
	}
	
	if(dir)
	{
		INPUT_TIM->CR1 &= 0xffef; //0 	TC4->COUNT16.CTRLBCLR.bit.DIR = 1;
	} else
	{
		INPUT_TIM->CR1 |= 0x0010; //1	TC4->COUNT16.CTRLBSET.bit.DIR = 1;
	}
}

void inputPinSetup(void)
{
	dirPinSetup(); //setup the dir pin

	stepPinSetup(); //setup the step pin
	
	dirChanged_ISR();
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{	
		EXTI_ClearITPendingBit(EXTI_Line1);
		
		dirChanged_ISR();
	}
}

int64_t getSteps(void)
{
	uint16_t y;
	static uint16_t lasty = 0;
	int16_t dy;
	
	y = INPUT_TIM->CNT;
	
	dy = (int16_t)(y - lasty);

	steps += dy;
	
	lasty = y;
	
	return (int64_t)dy;
}

void enableINPUTInterrupts(void)
{
	dirChanged_ISR();

	EXTI_ClearITPendingBit(EXTI_Line1);
	EXTI->IMR |= EXTI_Line1;
	
	TIM_Cmd(INPUT_TIM, ENABLE);
}

void disableINPUTInterrupts(void)
{
	TIM_Cmd(INPUT_TIM, DISABLE);
	
	EXTI->IMR &= (~EXTI_Line1);
	EXTI_ClearITPendingBit(EXTI_Line1);
}
