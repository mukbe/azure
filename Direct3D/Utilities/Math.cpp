#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::Epsilon = 1E-6f;
const int Math::IntMin = -2147483647;
const int Math::IntMax = 2147483647;
const float Math::FloatMin = -3.402823E+38f;
const float Math::FloatMax = 3.402823E+38f;

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

float Math::Lerp(float val1, float val2, float amount)
{
	return val1 + (val2 - val1)* amount;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}