#include "stdafx.h"
#include "Ray.h"

#include "BoundingSphere.h"
#include "../Utilities/Math.h"


Ray::Ray()
	:origin(0.f, 0.f, 0.f), direction(0.f, 0.f, 0.f)
{

}

Ray::Ray(D3DXVECTOR3 position, D3DXVECTOR3 direction)
	:origin(position),direction(direction)
{

}

bool Ray::Intersects(D3DXPLANE & plane, float * value)
{
	return false;
}

bool Ray::Intersects(BoundingSphere & sphere, float * value)
{
	float x = sphere.center.x - this->origin.x;
	float y = sphere.center.y - this->origin.y;
	float z = sphere.center.z - this->origin.z;

	float single = x * x + y * y + z * z;
	float radius = sphere.radius * sphere.radius;
	if (single <= radius)
	{
		*value = 0;
		return true;
	}
	float x1 = x * this->direction.x + y * this->direction.y + z * this->direction.z;
	if (x1 < 0.0f)
	{
		*value = 0;
		return false;
	}
	float single1 = single - x1 * x1;
	if (single1 > radius)
	{
		*value = 0;
		return false;
	}
	float single2 = (float)sqrt((double)(radius - single1));
	*value = x1 - single2;
	return true;
}



