
#include "ptc.h"
#include "types.h"
#include "classes.h"


void Texture::SetAxis(float x, float y, float z)
{
	axis.x = x;
	axis.y = y;
	axis.z = z;
}


void Texture::SetOffset(int x, int y)
{
	offset.x = x;
	offset.y = y;
}


Colour Texture::GetUVColour(Point3D *where)
{
	Colour	colour;

	if (axis.x)
	{
	}
	else if (axis.y)
	{
		int		u, v;
		int		index;

		u = ((int)(where->x * scale) - offset.x) % width;
		v = ((int)(where->z * scale) - offset.y) % height;

		if (u < 0) u = width + u;
		if (v < 0) v = height + v;

		index = v * width + u;

		colour.r = ((buffer[index] >> 16) & 255) / 255.0f;
		colour.g = ((buffer[index] >> 8) & 255) / 255.0f;
		colour.b = ((buffer[index] >> 0) & 255) / 255.0f;
	}
	else if (axis.z)
	{
	}

	return (colour);
}