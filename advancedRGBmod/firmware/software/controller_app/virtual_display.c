/*********************************************************************************
 *
 * This file is part of the N64 RGB/YPbPr DAC project.
 *
 * Copyright (C) 2016-2017 by Peter Bartmann <borti4938@gmx.de>
 *
 * N64 RGB/YPbPr DAC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************************************************************************
 *
 * virtual_display.c
 *
 *  Created on: 06.01.2018
 *      Author: Peter Bartmann
 *
 ********************************************************************************/
 
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include "virtual_display.h"
#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"

alt_u16 _width = VD_WIDTH;
alt_u16 _height = VD_HEIGHT;

int VD_print_string(int horiz_offset, int vert_offset, alt_u8 color, char *font, char string[])
{
  int i = 0;
  int original_horiz_offset;

  original_horiz_offset = horiz_offset;

  // Print until we hit the '\0' char.
  while (string[i]) {
    //Handle newline char here.
    if (string[i] == '\n') {
      horiz_offset = original_horiz_offset;
      vert_offset += FONT_10PT_ROW+1;
      i++;
      continue;
    }
    // Lay down that character and increment our offsets.
    VD_print_char(horiz_offset, vert_offset, color, string[i], font);
    i++;
    horiz_offset += FONT_10PT_COLUMN;
  }
  return (0);
}

int VD_print_char (int horiz_offset, int vert_offset, alt_u8 color, char character, char *font)
{

  int i, j;

  char   temp_char, char_row;
  alt_u8 temp_color;

  // Convert the ASCII value to an array offset
  temp_char = (character - 0x20);

  //Each character is 8 pixels wide and 11 tall.
  for(i = 0; i < FONT_10PT_ROW; i++) {
      char_row = *(font + (temp_char * FONT_10PT_ROW) + i);
    for (j = 0; j < FONT_10PT_COLUMN; j++) {
      //If the font table says the pixel in this location is on for this character, then set it.
      temp_color = (char_row & (((unsigned char)0x80) >> j)) ? color : VD_NON;
      VD_draw_Pixel((horiz_offset + j), (vert_offset + i), temp_color); // draw the pixel to panel
    }
  }
  return(0);
}

void VD_draw_Pixel(alt_u16 x, alt_u8 y, alt_u8 color)
{
  if((x >= 0) && (x < _width) && (y >= 0) && (y < _height)){
    VD_SET_ADDR(x,y);
    VD_SET_DATA(color);
    VD_write_data();
  }
}

void VD_write_data()
{
  alt_u8 wrctrl;

  wrctrl = IORD_ALTERA_AVALON_PIO_DATA(TXT_WRCTRL_BASE) | VD_WRCTRL_WREN_ORMASK;
  IOWR_ALTERA_AVALON_PIO_DATA(TXT_WRCTRL_BASE,wrctrl);
  wrctrl = wrctrl & VD_WRCTRL_WREN_ANDMASK;
  IOWR_ALTERA_AVALON_PIO_DATA(TXT_WRCTRL_BASE,wrctrl);
}

/******************************************************************
*  Data: cour10_font (first 0x20 symbols omitted)
*
******************************************************************/
char cour10_font_array[95][11] = {
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00},
 {0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x14, 0x14, 0x7E, 0x28, 0x28, 0x28, 0xFC, 0x50, 0x50, 0x00, 0x00},
 {0x10, 0x38, 0x44, 0x40, 0x38, 0x04, 0x44, 0x38, 0x10, 0x00, 0x00},
 {0x40, 0xA2, 0x44, 0x08, 0x10, 0x20, 0x44, 0x8A, 0x04, 0x00, 0x00},
 {0x30, 0x40, 0x40, 0x20, 0x60, 0x92, 0x94, 0x88, 0x76, 0x00, 0x00},
 {0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x08, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x08},
 {0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20},
 {0x00, 0x00, 0x6C, 0x38, 0xFE, 0x38, 0x6C, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x10, 0x10, 0x10, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00},
 {0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00},
 {0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00},
 {0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00},
 {0x38, 0x44, 0x04, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7C, 0x00, 0x00},
 {0x38, 0x44, 0x04, 0x04, 0x18, 0x04, 0x04, 0x44, 0x38, 0x00, 0x00},
 {0x08, 0x18, 0x18, 0x28, 0x28, 0x48, 0x7C, 0x08, 0x1C, 0x00, 0x00},
 {0x7C, 0x40, 0x40, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38, 0x00, 0x00},
 {0x18, 0x20, 0x40, 0x40, 0x78, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00},
 {0x7C, 0x44, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00},
 {0x38, 0x44, 0x44, 0x44, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00},
 {0x38, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x04, 0x08, 0x30, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00, 0x00, 0x00},
 {0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00},
 {0x38, 0x44, 0x04, 0x04, 0x08, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00},
 {0x3C, 0x42, 0x9A, 0xAA, 0xAA, 0xAA, 0x9C, 0x40, 0x38, 0x00, 0x00},
 {0x30, 0x10, 0x10, 0x28, 0x28, 0x44, 0x7C, 0x44, 0xEE, 0x00, 0x00},
 {0xFC, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0xFC, 0x00, 0x00},
 {0x3C, 0x42, 0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00},
 {0xF8, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0xF8, 0x00, 0x00},
 {0xFE, 0x42, 0x40, 0x48, 0x78, 0x48, 0x40, 0x42, 0xFE, 0x00, 0x00},
 {0xFE, 0x42, 0x40, 0x48, 0x78, 0x48, 0x40, 0x40, 0xF0, 0x00, 0x00},
 {0x3C, 0x42, 0x80, 0x80, 0x80, 0x8E, 0x82, 0x42, 0x3C, 0x00, 0x00},
 {0xEE, 0x44, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00},
 {0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00},
 {0x1E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38, 0x00, 0x00},
 {0xE6, 0x44, 0x48, 0x48, 0x50, 0x70, 0x48, 0x44, 0xE6, 0x00, 0x00},
 {0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0xFE, 0x00, 0x00},
 {0xC6, 0x44, 0x6C, 0x6C, 0x54, 0x54, 0x44, 0x44, 0xEE, 0x00, 0x00},
 {0xCE, 0x44, 0x64, 0x64, 0x54, 0x4C, 0x4C, 0x44, 0xE4, 0x00, 0x00},
 {0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00},
 {0xFC, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0xF0, 0x00, 0x00},
 {0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x36, 0x00},
 {0xFC, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x48, 0x44, 0xE6, 0x00, 0x00},
 {0x7C, 0x82, 0x80, 0x80, 0x7C, 0x02, 0x02, 0x82, 0x7C, 0x00, 0x00},
 {0xFE, 0x92, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00},
 {0xEE, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00},
 {0xEE, 0x44, 0x44, 0x44, 0x28, 0x28, 0x28, 0x10, 0x10, 0x00, 0x00},
 {0xEE, 0x44, 0x44, 0x44, 0x54, 0x54, 0x54, 0x28, 0x28, 0x00, 0x00},
 {0xEE, 0x44, 0x28, 0x28, 0x10, 0x28, 0x28, 0x44, 0xEE, 0x00, 0x00},
 {0xEE, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00},
 {0xFE, 0x84, 0x08, 0x08, 0x10, 0x20, 0x20, 0x42, 0xFE, 0x00, 0x00},
 {0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x00},
 {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00},
 {0x1C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1C, 0x00, 0x00},
 {0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00},
 {0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x78, 0x04, 0x7C, 0x84, 0x84, 0x7A, 0x00, 0x00},
 {0xC0, 0x40, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x42, 0xFC, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7C, 0x82, 0x80, 0x80, 0x82, 0x7C, 0x00, 0x00},
 {0x0C, 0x04, 0x04, 0x7C, 0x84, 0x84, 0x84, 0x84, 0x7E, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7C, 0x82, 0xFE, 0x80, 0x82, 0x7C, 0x00, 0x00},
 {0x30, 0x40, 0x40, 0xF0, 0x40, 0x40, 0x40, 0x40, 0xF0, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7E, 0x84, 0x84, 0x84, 0x7C, 0x04, 0x04, 0x78},
 {0xC0, 0x40, 0x40, 0x58, 0x64, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00},
 {0x08, 0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00},
 {0x08, 0x00, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x70},
 {0xC0, 0x40, 0x40, 0x4C, 0x48, 0x50, 0x70, 0x48, 0xC6, 0x00, 0x00},
 {0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xE8, 0x54, 0x54, 0x54, 0x54, 0xD6, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xD8, 0x64, 0x44, 0x44, 0x44, 0xEE, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7C, 0x82, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xFC, 0x42, 0x42, 0x42, 0x42, 0x7C, 0x40, 0xE0},
 {0x00, 0x00, 0x00, 0x7E, 0x84, 0x84, 0x84, 0x7C, 0x04, 0x0E, 0x00},
 {0x00, 0x00, 0x00, 0xEC, 0x32, 0x20, 0x20, 0x20, 0xF8, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0x7C, 0x82, 0x70, 0x0C, 0x82, 0x7C, 0x00, 0x00},
 {0x00, 0x20, 0x20, 0x78, 0x20, 0x20, 0x20, 0x24, 0x18, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xCC, 0x44, 0x44, 0x44, 0x4C, 0x36, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xEE, 0x44, 0x44, 0x28, 0x28, 0x10, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xEE, 0x44, 0x54, 0x54, 0x28, 0x28, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xEE, 0x44, 0x38, 0x38, 0x44, 0xEE, 0x00, 0x00},
 {0x00, 0x00, 0x00, 0xEE, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x60},
 {0x00, 0x00, 0x00, 0xFC, 0x88, 0x10, 0x20, 0x44, 0xFC, 0x00, 0x00},
 {0x0C, 0x10, 0x10, 0x10, 0x10, 0x60, 0x10, 0x10, 0x10, 0x10, 0x0C},
 {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},
 {0x60, 0x10, 0x10, 0x10, 0x10, 0x0C, 0x10, 0x10, 0x10, 0x10, 0x60},
 {0x00, 0x00, 0x62, 0x92, 0x8C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };

//Pointer to our font table
char* cour10_font = &cour10_font_array[0][0];
