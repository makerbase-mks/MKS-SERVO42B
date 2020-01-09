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

#include "IIC.h"

/**
	*					
	* @Brief	
	*	@Param	None
	**/
void IIC_Start (void)
{		
	SDA_H;
	delay_us(1);
	
	SCL_H;
	delay_us(1);	
	
	SDA_L;
	delay_us(1);
	
	SCL_L;
}


/**
	*
	* @Brief	
	*	@Param	None
	**/
void IIC_Stop (void)
{		
	SDA_L;
	delay_us(1);
	
	SCL_H;
	delay_us(1);
	
	SDA_H;
	delay_us(1);
	
	SCL_L;
}


/**
	*
	* @Brief	
	*	@Param	None
	**/
void IIC_Ack (void)
{
	
	SDA_L;
	delay_us(1);	
	
	SCL_H;
	delay_us(1);
		
	SCL_L;
}


/**
	*	
	* @Brief	
	*	@Param	None
	**/
void IIC_NAck (void)
{
	SDA_H;
	delay_us(1);
	
	SCL_H;
	delay_us(1);
		
	SCL_L;
}

/**
	*	
	* @Brief	
	*	@Param	
						ERROR
						SUCCESS	
	**/
ErrorStatus IIC_Get_Ack (void)
{
	uint8_t IIC_ErrTime = 0;
	
	SDA_IN();
	
	SDA_H;
	delay_us(1);
	
	SCL_H;
	delay_us(1);
	
	while(SDA_READ)
	{
		++IIC_ErrTime;
		
		if(IIC_ErrTime>10)
		{			
			SCL_L;
			
			SDA_OUT();
			
			return ERROR;
		}
	}
	SCL_L;
		
	SDA_OUT();
		
	return SUCCESS;
}


/**
	*
	* @Brief	
	*	@Param	
						BYTE
	**/
void IIC_ByteWrite (__IO uint8_t BYTE)
{
	uint8_t x=8;
	
	while(x--)
	{
		if (BYTE & 0x80)
			SDA_H;
		else
			SDA_L;
		
		BYTE<<=1;
		
		delay_us(1);
		
		SCL_H;
		
		delay_us(1);
			
		SCL_L;
	}	
}

/**
	* @Brief	
	*	@Param	
						1.Slave_Addr  		
						2.Resister_Addr		
						3.DataByte   		
	* @Retval  	
	**/
ErrorStatus IIC_SingleWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t DataByte)
{	
	IIC_Start ();	
	
	IIC_ByteWrite (Slave_Addr);
	
	IIC_Get_Ack ();	
	
	IIC_ByteWrite (Resister_Addr);
	
	IIC_Get_Ack ();	
	
	IIC_ByteWrite (DataByte);	
	
	IIC_Get_Ack ();	
	
	IIC_Stop ();
	
	return SUCCESS;
}

/**
	*	
	* @Brief	
	*	@Param	
						1.Slave_Addr
						2.Slave_Addr
						3.Length
						3.pBuffer
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_BytesWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length,const uint8_t* pBuffer)
{	
	if(Length<=0)
		return ERROR;
	
	IIC_Start () ;
	
	IIC_ByteWrite (Slave_Addr);
	
	IIC_Get_Ack ();
	
	IIC_ByteWrite (Resister_Addr);
	
	IIC_Get_Ack ();
	
	while (Length--)
	{
		IIC_ByteWrite ((*pBuffer));	
		IIC_Get_Ack ();
		++pBuffer;
	}
	
	IIC_Stop ();
	
	return SUCCESS;
}

/**
	*
	* @Brief	
	*	@Param	
						1.Slave_Addr
						2.Slave_Addr
						3.Length
	* @Retval  
						SUCCESS
						ERROR
	**/
ErrorStatus IIC_ZerosWrite (uint8_t Slave_Addr,uint8_t Resister_Addr,uint8_t Length)
{	
	if(Length<=0)
		return ERROR;
	
	IIC_Start () ;
	
	IIC_ByteWrite (Slave_Addr); 
	
	IIC_Get_Ack ();	
	
	IIC_ByteWrite (Resister_Addr);
	
	IIC_Get_Ack ();	
	
	while (Length--)
	{
		IIC_ByteWrite (0x00);
		IIC_Get_Ack ();	
	}
	
	IIC_Stop ();
	
	return SUCCESS;
}
