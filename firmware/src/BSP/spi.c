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

#include "spi.h"
#include "usart.h"

//SPI Write and Read
uint16_t SPI_WriteAndRead(SPI_TypeDef* SPIx, uint16_t Data)
{
	uint16_t timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET)
	{
		++timeout;
		if(timeout >= 400)
		{
			return 0;
		}		
	}
	SPIx->DR = Data;
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET)
		{
		++timeout;
		if(timeout >= 400)
		{
			return 0;
		}
	}
	return (SPIx->DR);
}

//SPI write
bool SPI_Write(SPI_TypeDef* SPIx, uint8_t data)
{
	uint16_t timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_BSY) != RESET)
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;		
		}
	}
	
	SPIx->DR = data;
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET)
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;
		}
	}
	
	timeout = 0;
	while((SPIx->SR & SPI_I2S_FLAG_BSY) != RESET)
	{
		++timeout;
		if(timeout >= 400)
		{
			return false;		
		}
	}
	
	return true;
}
