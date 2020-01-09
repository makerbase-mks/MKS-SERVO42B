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
 
#ifndef __BOARD_H
#define __BOARD_H

#include "../CMSIS/stm32f10x.h"
#include "oled.h"
#include "delay.h"

#define	VERSON	"Ver:1.0.3"

//INPUT
#define PIN_INPUT     			GPIOA
#define PIN_INPUT_STEP  		GPIO_Pin_0
#define PIN_INPUT_DIR  			GPIO_Pin_1
#define PIN_INPUT_ENABLE   	GPIO_Pin_2
#define	INPUT_TIM						TIM2
#define INPUT_MAX						(uint16_t)65535 //65535

//OLED
#define PIN_OLED						GPIOA
#define PIN_OLED_CS					GPIO_Pin_4
#define PIN_OLED_D0					GPIO_Pin_5
#define PIN_OLED_DC					GPIO_Pin_6
#define PIN_OLED_D1    			GPIO_Pin_7

//SW
#define	PIN_SW							GPIOA
#define	PIN_SW4_EXIT				GPIO_Pin_8	//exit
#define	PIN_SW3_ENTER				GPIO_Pin_9	//enter
#define	PIN_SW1_NEXT				GPIO_Pin_10	//next

//A4950
#define PIN_A4950     			GPIOB
#define PIN_A4950_VREF12    GPIO_Pin_0	//TIM3_CH3
#define PIN_A4950_VREF34    GPIO_Pin_1	//TIM3_CH4
#define PIN_A4950_IN1   		GPIO_Pin_6
#define PIN_A4950_IN2   		GPIO_Pin_7
#define PIN_A4950_IN3  			GPIO_Pin_8
#define PIN_A4950_IN4  		  GPIO_Pin_9
#define	VREF_TIM						TIM3
#define VREF_MAX						(uint16_t)511 //511

//A1333
#define PIN_A1333     			GPIOB
#define PIN_A1333_CS    		GPIO_Pin_12
#define PIN_A1333_SCK   		GPIO_Pin_13
#define PIN_A1333_MISO   		GPIO_Pin_14
#define PIN_A1333_MOSI   		GPIO_Pin_15

//LED
#define PIN_RED							GPIOA
#define	PIN_LED_RED					GPIO_Pin_3
#define PIN_BLUE						GPIOC
#define PIN_LED_BLUE				GPIO_Pin_13

static void NVIC_init(void);
static void CLOCK_init(void);
static void A4950_init(void);
static void A1333_init (void);
static void OLED_init(void);
static void SWITCH_init(void);
static void INPUT_init(void);
static void LED_init(void);
void board_init(void);
void RED_LED(bool state);
void BLUE_LED(bool state);

#endif
