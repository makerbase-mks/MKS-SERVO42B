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

#ifndef __OLED_H
#define __OLED_H
		  	 
#include "spi.h"
#include "board.h"

#define OLED_DC_DAT		PIN_OLED->BSRR = PIN_OLED_DC;		//GPIO_SetBits(PIN_OLED, PIN_OLED_DC)
#define OLED_DC_CMD		PIN_OLED->BRR  = PIN_OLED_DC;		//GPIO_ResetBits(PIN_OLED, PIN_OLED_DC)

#define OLED_CS_H			PIN_OLED->BSRR = PIN_OLED_CS;		//GPIO_SetBits(PIN_OLED, PIN_OLED_CS)
#define OLED_CS_L			PIN_OLED->BRR  = PIN_OLED_CS;		//GPIO_ResetBits(PIN_OLED, PIN_OLED_CS)

void oled_writeCmd(uint8_t command);
void oled_writeData(uint8_t data);
void oled_setCursor(uint8_t x, uint8_t y);
void oled_clearDisplay(void);
void oled_drawChar(uint8_t x, uint8_t y, char c);
void oled_drawStr(uint8_t x, uint8_t y, char* s);
void oled_clearLine(char* line1, char* line2, char* line3, char* line4);
void oled_begin(void);

#endif  
	 



