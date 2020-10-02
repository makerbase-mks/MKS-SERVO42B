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
 
#include "calibration.h"
#include "nonvolatile.h"
#include "usart.h"

extern volatile int64_t currentLocation;

volatile CalData_t CalData[CALIBRATION_TABLE_SIZE];
volatile bool	fastCalVaild = false;

uint16_t CalibrationTable_getTableIndex(uint16_t value)
{
	int32_t x;
	x = ( ( (int32_t)value * CALIBRATION_TABLE_SIZE ) / ANGLE_STEPS );
	return (uint16_t)x;
}

bool CalibrationTable_updateTableValue(uint16_t index, uint16_t value)
{
	CalData[index].value =	value;
	CalData[index].error = CALIBRATION_STEPS / CALIBRATION_TABLE_SIZE;	//Îó²î
	return true;
}

bool CalibrationTable_calValid(void)
{
	uint16_t i;
	for (i=0; i < CALIBRATION_TABLE_SIZE; i++)
	{
		if (CalData[i].error == CALIBRATION_ERROR_NOT_SET)
		{
			return false;
		}
	}
	
	if (nvmFlashCalData->status != valid)
	{
		CalibrationTable_saveToFlash();
	}
	
	return true;
}

uint16_t CalibrationTable_fastReverseLookup(uint16_t fastEncoderAngle)
{
	if (fastCalVaild == true)	//assume calibration is good
	{  									//we only have 16384 values in table
		return (uint16_t)(nvmFastCal->angle[fastEncoderAngle >> 1] + ((fastEncoderAngle & 0x0001) << 1)); //((fastEncoderAngle % 2) << 1)
	}else
	{
		return CalibrationTable_reverseLookup(fastEncoderAngle);
	}
}

//·´Ïò²éÕÒ
uint16_t CalibrationTable_reverseLookup(uint16_t encoderAngle)
{
	int32_t i = 0;	
	int32_t a1,a2;
	int32_t b1,b2;	
	int32_t x;
	uint16_t y;

	x = (int32_t)encoderAngle;
	if (x < ((int32_t)nvmFlashCalData->MIN))
	{
		x = x + CALIBRATION_STEPS;
	}

	i = 0;
	while(i < CALIBRATION_TABLE_SIZE)
	{
		a1 = (int32_t)CalData[i].value;

		//handle index wrap around
		if(i == (CALIBRATION_TABLE_SIZE - 1))
		{
			a2 = (int32_t)CalData[0].value;
		}else
		{
			a2 = (int32_t)CalData[i+1].value;
		}

		//wrap
		if (fastAbs(a1 - a2) > CALIBRATION_WRAP)
		{
			if (a1 < a2)
			{
				a1 = a1 + CALIBRATION_STEPS;
			}else
			{
				a2 = a2 + CALIBRATION_STEPS;
			}
		}

		//finding matching location
		if ( (x >= a1 && x <= a2) || (x >= a2 && x <= a1) )
		{
			//x1=a1  y1=b1=i*(65536/200)+0.5   x2=a2  y2=b2=(i+1)*(65536/200)+0.5   x=encoderAngle
			b1 = (int32_t)DIVIDE_WITH_ROUND(((int64_t)i     * ANGLE_STEPS),CALIBRATION_TABLE_SIZE);
			b2 = (int32_t)DIVIDE_WITH_ROUND(((int64_t)(i+1) * ANGLE_STEPS),CALIBRATION_TABLE_SIZE);
			y = interp(a1,b1,a2,b2,x); //y=y1+k(x-x1), k=(y2-y1)/(x2-x1)
			return y;
		}
		i++;
	}
	return 0;	//we did some thing wrong
}

static uint16_t interp(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	int32_t dx,dy,dx2,y;
	dx = x2 - x1;
	dy = y2 - y1;
	dx2 = x - x1;
	y = y1 + (int32_t)DIVIDE_WITH_ROUND((dx2 * dy),dx); //(y-y1)=k(x-x1), k=(y2-y1)/(x2-x1)
	if (y < 0)
	{
		y = y + ANGLE_STEPS;
	}
	if (y > ANGLE_MAX)
	{
		y = y - ANGLE_STEPS;
	}
	return (uint16_t)y;
}

static uint16_t interp2(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	int32_t dx,dy,dx2,y;
	dx = x2 - x1;
	dy = y2 - y1;
	dx2 = x - x1;
	y = y1 + (int32_t)DIVIDE_WITH_ROUND((dx2 * dy),dx); //(y-y1)=k(x-x1), k=(y2-y1)/(x2-x1)

	if (y < 0)
	{
		y = y + CALIBRATION_STEPS;
	}
	if (y >= CALIBRATION_STEPS)
	{
		y = y - CALIBRATION_STEPS;
	}
	return (uint16_t)y;
}

void CalibrationTable_saveToFlash(void)
{
	uint16_t i = 0;	
	uint16_t min = 0, max = 0;
	FlashCalData_t data;
	
	max = min = CalData[0].value;

	for (i=0; i < CALIBRATION_TABLE_SIZE; i++ )
	{
		if(CalData[i].value < min)	{min = CalData[i].value;}
		if(CalData[i].value > max)	{max = CalData[i].value;}
		
		data.FlashCalData[i] = (uint16_t)CalData[i].value;
	}
	data.status = valid;
	data.MIN = min;
	data.MAX = max;
	
//	CalibrationTable_printCalTable();	//print the calibration to screen
//	printf("data.MIN %d\n\r",data.MIN);
//	printf("data.MAX %d\n\r",data.MAX);
	
	nvmWriteCalTable(&data); //CalTable

	CalibrationTable_createFastCal(); //FastCalTable
}

void CalibrationTable_createFastCal(void)
{
	uint32_t i,j;
	uint16_t checkSum = 0;
	uint16_t data[512]; //1K
	for (i=0,j=0; i < 16384; i++)
	{
		uint16_t x = 0;
		x = CalibrationTable_reverseLookup(i << 1); //setting fast calibration
		data[j] = x;
		j++;
		if (j >= FLASH_ROW_SIZE) //1k bytes = 512 uint16_t
		{
			Flash_ProgramPage( (FLASH_PAGE32_ADDR + (uint32_t)(((i + 1) * 2) - FLASH_PAGE_SIZE)), data, FLASH_ROW_SIZE );
			j=0;
		}
		checkSum += x;
	}
	Flash_ProgramSize(FLASH_checkSum_ADDR, &checkSum, 1);	//checkSum
	
	fastCalVaild = true;
}

void CalibrationTable_loadFromFlash(void)
{
	uint16_t i;

	for(i=0; i < CALIBRATION_TABLE_SIZE; i++)	//Reading Calbiration from Flash
	{
		CalData[i].value = nvmFlashCalData->FlashCalData[i];
		CalData[i].error = CALIBRATION_MIN_ERROR;
	}
}

void CalibrationTable_init(void)
{
	uint16_t i;

	if(valid == nvmFlashCalData->status)
	{
		CalibrationTable_loadFromFlash();
		CalibrationTable_updateFastCal();
		
	}else
	{
		for(i=0; i < CALIBRATION_TABLE_SIZE; i++)
		{
			CalData[i].value = 0;
			CalData[i].error = CALIBRATION_ERROR_NOT_SET;
		}
	}
}

void CalibrationTable_updateFastCal(void)
{
	uint16_t i;
	uint16_t checkSum = 0;
	bool NonZero = false;
	for(i=0; i < 16384; i++)
	{
		checkSum += nvmFastCal->angle[i];
		if(checkSum != 0)
		{
			NonZero = true;
		}
	}
	
	if(checkSum != Flash_readHalfWord(FLASH_checkSum_ADDR) || NonZero != true)
	{		
		CalibrationTable_saveToFlash();
	}
	else
	{
		fastCalVaild = true;
	}
}

//We want to linearly interpolate between calibration table angle
uint16_t CalibrationTable_getCal(uint16_t actualAngle)
{
	uint16_t indexLow,indexHigh;
	int32_t x1,x2,y1,y2;
	uint16_t value;

	indexLow  = CalibrationTable_getTableIndex(actualAngle);
	indexHigh = indexLow + 1;

	x1 = ((int32_t)indexLow  * ANGLE_STEPS) / CALIBRATION_TABLE_SIZE;
	x2 = ((int32_t)indexHigh * ANGLE_STEPS) / CALIBRATION_TABLE_SIZE;
	
	if(indexHigh >= CALIBRATION_TABLE_SIZE)
	{
		indexHigh -= CALIBRATION_TABLE_SIZE;
	}
	y1 = CalData[indexLow].value;
	y2 = CalData[indexHigh].value;

	//handle the wrap condition
	if (fastAbs((int32_t)(y2 - y1)) > CALIBRATION_WRAP)
	{
		if (y2 < y1)
		{
			y2 = y2 + CALIBRATION_STEPS;
		}else
		{
			y1 = y1 + CALIBRATION_STEPS;
		}
	}

	value = interp2(x1, y1, x2, y2, actualAngle);

	return value;
}

void CalibrationTable_printCalTable(void)
{
	uint16_t i;
	printf("\n\r");
	for (i=0; i < CALIBRATION_TABLE_SIZE; i++)
	{
		printf("%d\n\r",CalData[i].value);
	}
	printf("\n\r");
}
