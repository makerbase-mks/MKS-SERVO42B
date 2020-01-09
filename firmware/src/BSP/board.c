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

#include "board.h"

//Init clock
static void CLOCK_init(void)
{	
//	RCC->APB2ENR |= (1<<0);	//开启AFIO时钟
	RCC->APB2ENR |= (1<<2);	//开启GPIOA时钟
	RCC->APB2ENR |= (1<<3);	//开启GPIOB时钟
	RCC->APB2ENR |= (1<<4);	//开启GPIOC时钟
	RCC->APB2ENR |= (1<<12);//开启SPI1时钟

	RCC->APB2ENR |= (1<<11);	//开启TIM1时钟

	RCC->APB1ENR |= (1<<0);	//开启TIM2时钟
	RCC->APB1ENR |= (1<<1);	//开启TIM3时钟
//	RCC->APB1ENR |= (1<<2);	//开启TIM4时钟
	RCC->APB1ENR |= (1<<14);//开启SPI2时钟
}

//Init NVIC
static void NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //第4组:4位抢占优先级
	NVIC_SetPriority(SysTick_IRQn,15); //设置SysTick_IRQn抢占优先级最低
	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; //抢占优先级0(dir引脚)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //抢占优先级为1(控制循环)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

//Init IO
static void INPUT_init(void)
{	
	GPIOA->CRL &= 0xfffff000;	//clean STEP(PA0) DIR(PA1) ENABLE(PA2) control bit
	GPIOA->CRL |= 0x00000888;	//config STEP(PA0) DIR(PA1) ENABLE(PA2) input mode	
	GPIOA->ODR |= 0x00000007;	//default STEP(PA0) DIR(PA1) ENABLE(PA2) pullup
}

//Init SSD1306				    
static void OLED_init(void)
{ 			
	GPIOA->CRL &= 0x0000ffff;
	GPIOA->CRL |= 0xb3b30000;
	GPIOA->ODR |= 0x000000f0;
	
	SPI_InitTypeDef SPI_InitStructure;	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init (SPI1,&SPI_InitStructure);	
	SPI_Cmd (SPI1,ENABLE);
}

//Init switch IO
static void SWITCH_init(void)
{
	GPIOA->CRH &= 0xfffff000;	//SW4 SW3 SW1 (PA8 PA9 PA10)	
	GPIOA->CRH |= 0x00000888;	//SW4 SW3 SW1 (PA8 PA9 PA10)	
	GPIOA->ODR |= 0x00000700;	//SW4 SW3 SW1 (PA8 PA9 PA10)
}

//Init A4950
static void A4950_init(void)
{
	//Init A4950 IO
	GPIOB->CRL &= 0x00ffff00;	//clean VREF12(PB0) VREF34(PB1) IN1(PB6) IN2(PB7) control bit
	GPIOB->CRL |= 0x330000bb;	//config VREF12(PB0) VREF34(PB1)Multiplexed push-pull output IN1(PB6) IN2(PB7) Universal push-pull output
	GPIOB->CRH &= 0xffffff00;	//clean IN3(PB8) IN4(PB9) control bit	
	GPIOB->CRH |= 0x00000033;	//config IN3(PB8) IN4(PB9) Universal push-pull output	
	GPIOB->ODR &= 0xfffffc3f;	//defualt IN1(PB6) IN2(PB7) IN3(PB8) IN4(PB9) 
	
	//Init TIM3
	TIM_TimeBaseInitTypeDef  		TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = VREF_MAX;									//reload c
	TIM_TimeBaseStructure.TIM_Prescaler = 0;											//72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(VREF_TIM, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitTypeDef  	        TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(VREF_TIM, &TIM_OCInitStructure);	//TIM3 CH3
	TIM_OC4Init(VREF_TIM, &TIM_OCInitStructure);	//TIM3 CH4
 
	TIM_OC3PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(VREF_TIM, TIM_OCPreload_Enable);
 
	TIM_Cmd(VREF_TIM, ENABLE);
}

//Init A1333
static void A1333_init (void)
{	
	GPIOB->CRH &= 0x0000ffff;	//clean CS SCK MOSI MISO control bit
	GPIOB->CRH |= 0xb8b30000;	//config CS Universal push-pull output，SCK MOSI Multiplexed push-pull output，MISO pulldown input
	GPIOB->ODR |= 0x0000f000;	//default CS SCK MOSI MISO output high

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//CPOL=1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//CPHA=1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init (SPI2,&SPI_InitStructure);	
	SPI_Cmd (SPI2,ENABLE);
}

static void LED_init(void)
{
	//RED_LED
	GPIOA->CRL &= 0xffff0fff; //PA3
	GPIOA->CRL |= 0x00003000; //Universal push-pull output
	GPIOA->ODR |= 0x00000008;

	//BLUE_LED
	GPIOC->CRH &= 0xff0fffff; //PC13
	GPIOC->CRH |= 0x00300000; //Universal push-pull output
	GPIOC->ODR |= 0x00002000;
}

void board_init(void)
{
	CLOCK_init();
	NVIC_init(); 
	INPUT_init();
	A4950_init();
	A1333_init();
	OLED_init();
	SWITCH_init();
	LED_init();
//	USART_Config ();
}

//red led
//state = true  open
//state = false close
void RED_LED(bool state)
{
	GPIO_WriteBit(PIN_RED, PIN_LED_RED, (BitAction)state);
}

//blue led
//state(error) = true  open
//state(error) = false close
void BLUE_LED(bool state)
{
	GPIO_WriteBit(PIN_BLUE, PIN_LED_BLUE, (BitAction)(!state));
}
