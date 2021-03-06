/*
 * contrast pwm driver
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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
 */

#if !defined(_CONTRAST_H_)
#define _CONTRAST_H_ 1

typedef enum {
	Contrast_minimum = 0,
	Contrast_light   = 1500,
	Contrast_default = 2048,
	Contrast_dark    = 3000,
	Contrast_maximum = 4095,
} ContrastType;

void Contrast_initialise(void);

// clip between maximum and minimum
void Contrast_set_value(int value);

// clip between light and dark rather than full range
// as this is more useful since the display should always be visible
void Contrast_set(int value);

int Contrast_get(void);

#endif
