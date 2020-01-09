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

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "board.h"
#include "delay.h"
#include "nonvolatile.h"
#include "stepper_controller.h"

typedef struct{
	char str[15];
}options_t;

typedef struct{
	char str[15];
	int (*func)(int argc, char *argv[]);
	options_t *ptrOptions;
}menuItem_t;

void display_begin(void);
void display_show(char* line1, char* line2, char* line3, char* line4);
void display_setMenu(menuItem_t *pMenu);
void display_showMenu(void);
void display_showOptions(void);
void display_updateMenu(void);
void display_forceMenuActive(void);
void display_process(void);
void display_updateLCD(void);

#endif
