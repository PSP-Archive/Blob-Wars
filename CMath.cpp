/*
Copyright (C) 2004 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "headers.h"

void Math::limitChar(signed char *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

void Math::limitChar(unsigned char *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

void Math::limitInt(int *in, int low, int high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

void Math::limitFloat(float *in, float low, float high)
{
	if (*in < low)
		*in = low;
	if (*in > high)
		*in = high;
}

void Math::wrapChar(signed char *in, signed char low, signed char high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

void Math::wrapInt(int *in, int low, int high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

void Math::wrapFloat(float *in, float low, float high)
{
	if (*in < low)
		*in = high;
	if (*in > high)
		*in = low;
}

int Math::rrand(int min, int max)
{
	int r = min;

	max++;

	if ((max - min) == 0)
		return min;

	r += rand() % (max - min);

	return r;
}

void Math::addBit(long *currentBits, long newBits)
{
	if (!(*currentBits & newBits))
		*currentBits += newBits;
}

void Math::removeBit(long *currentBits, long oldBits)
{
	if (*currentBits & oldBits)
		*currentBits -= oldBits;
}

void Math::calculateSlope(float x, float y, float x2, float y2, float *dx, float *dy)
{
	int steps = (int)max(fabs(x - x2), fabs(y - y2));
	if (steps == 0)
	{
		*dx = 0;
		*dy = 0;
		return;
	}

	*dx = (x - x2);
	*dx /= steps;
	*dy = (y - y2);
	*dy /= steps;
}
