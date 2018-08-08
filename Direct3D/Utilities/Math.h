#pragma once
class Math
{
public:
	static const float PI;
	static const float Epsilon;

	static const int IntMin;
	static const int IntMax;
	static const float FloatMin;
	static const float FloatMax;


	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Lerp(float val1, float val2,float amount);
	static float Clamp(float value, float min, float max);

	static float Modulo(float val1, float val2);
};