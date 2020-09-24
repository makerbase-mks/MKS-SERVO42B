
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
#include "usart.h"

///**
//	* @brief   USART
//	* @param  
//	* @retval  
//	*/
//static void NVIC_Configuration (void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

///**
//	* @brief   USART
//	* @param   
//	* @retval  
//	*/
//void USART_Config (void)
//{
//	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB| RCC_APB2Periph_USART1,ENABLE);	
//	
//	/***********************GPIO INIT**********************/
//	//RX
//	GPIO_InitTypeDef USART_GPIO_InitStructure;
//	USART_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	USART_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init (GPIOB,&USART_GPIO_InitStructure);
//	
//	//Tx
//	USART_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	USART_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	USART_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init (GPIOB,&USART_GPIO_InitStructure);

//	/***********************USART INIT**********************/
//	USART_InitTypeDef USART_InitStructure;	
//	USART_InitStructure.USART_BaudRate = 9600;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
//	USART_Init (USART1,&USART_InitStructure);
//	
//	NVIC_Configuration ();
//	
//	//USART
//	USART_Cmd (USART1,ENABLE);
//}


#define USE_DEBUG_VIEWER
#ifdef USE_DEBUG_VIEWER

/**
	* @brief 
	* @param   int ch
	* @param   FILE* f
	* @retval  ch
	*/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))        
#define TRCENA          0x01000000
 
//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;
 
int fputc(int ch, FILE *f)
{
    if (DEMCR & TRCENA)
    {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    return ch;
}

#else

/**
	* @brief 
	* @param   int ch
	* @param   FILE* f
	* @retval  ch
	*/
	int fputc (int ch,FILE *f)
	{
		USART_SendData (USART1,(uint8_t)ch);
		while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		return (ch);
	}	
		
	
	int fgetc (FILE *f)
	{
		while (USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
		return (int)USART_ReceiveData(USART1);
	}

#endif
