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

#ifndef __IIC_H
#define __IIC_H

//#include "stm32f10x.h"
#include "../CMSIS/stm32f10x.h"
#include "delay.h"
#include "../LIB/inc/stm32f10x_i2c.h"

//SCL-->PB6
#define SCL_L		GPIOB->BRR = GPIO_Pin_6
#define SCL_H		GPIOB->BSRR = GPIO_Pin_6
//SDA-->PB7
#define SDA_L		GPIOB->BRR = GPIO_Pin_7
#define SDA_H		GPIOB->BSRR = GPIO_Pin_7

#define SDA_READ   GPIOB->IDR & GPIO_Pin_7

#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x70000000;}

void IIC_Start (void);
void IIC_Stop (void);
void IIC_Ack (void);
void IIC_NAck (void);
ErrorStatus IIC_Get_Ack (void);
void IIC_ByteWrite (__IO uint8_t BYTE);

ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte);
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer);
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length);

#endif
