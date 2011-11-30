#ifndef PC_TARGET

#include <nds.h>

/* 
   NDS trigonometry routines.
   Copied from sin() and cos() in nehe11.cpp, written by Dovoto.
*/

float my_sin(float angle)
{
	int32 s = sinLerp((int)((angle * DEGREES_IN_CIRCLE) / 360.0));

	return f32tofloat(s);
}

float my_cos(float angle)
{
	int32 c = cosLerp((int)((angle * DEGREES_IN_CIRCLE) / 360.0));

	return f32tofloat(c);
}

#endif