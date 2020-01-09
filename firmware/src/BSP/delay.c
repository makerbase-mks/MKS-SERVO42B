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

#include "delay.h"

void delay_us (__IO uint32_t us)
{
	uint32_t i;
	/**
		* SystemCoreClock/1000000     1us
		* SystemCoreClock/100000			10us
		* SystemCoreClock/1000				1ms
		*/
	SysTick_Config (SystemCoreClock / 1000000); //1us

	//value = 0£¬CTRL's bit16 will set 1
	 for(i = 0; i < us; i++)
			while (!((SysTick -> CTRL) & (1 << 16))); 
	 
	 //close SysTick tim
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk; //SysTick_CTRL_ENABLE_Msk,¼´Îª(1<<0)	 
}

void delay_ms (__IO uint32_t ms)
{
		uint32_t i, t0;
	/**
		* SystemCoreClock/1000000     1us
		* SystemCoreClock/100000			10us
		* SystemCoreClock/1000				1ms
		*/
	SysTick_Config (SystemCoreClock / 1000); //1ms
	
	//value = 0£¬CTRL's bit16 will set 1
	 for(i = 0; i < ms; i++)
	 {
			t0 = 0;
			while (!((SysTick -> CTRL) & (1 << 16)))
			{
				t0++;
				if(t0 > 20000) //1.3ms
				{
					break;
				}
			}
	 }	 
	 
	 //close SysTick tim
	 SysTick -> CTRL &= ~SysTick_CTRL_ENABLE_Msk;  
}
