
#include <math.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


void Light::SetColour(float red, float green, float blue)
{
	colour.r = red;
	colour.g = green;
	colour.b = blue;
}


Colour Light::LightObject(Object *object, Intersect *isect, int make_ambient)
{
	Colour	lcol;
	Point3D	ray, normal;
	float	d, cos_angle;

	// No calculations for no light

	if (strength == 0)
		return (colour);

	// Calculate the point->light ray

	ray.x = position.x - isect->where.x;
	ray.y = position.y - isect->where.y;
	ray.z = position.z - isect->where.z;

	d = (float)sqrt(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z);
	ray.x /= d;
	ray.y /= d;
	ray.z /= d;

	// Get the surface normal at the point of contact

	object->GetIntersectionNormal(&normal, isect);

	// Calculate the diffuse term

	cos_angle = (float)sqrt(normal.x * ray.x + normal.y * ray.y + normal.z * ray.z);

	lcol.r += isect->colDiffuse.r * cos_angle * colour.r;
	lcol.g += isect->colDiffuse.g * cos_angle * colour.g;
	lcol.b += isect->colDiffuse.b * cos_angle * colour.b;

	// Add some linear fall-off to the light

	if (d > strength) d = strength;
	lcol.r *= ((strength - d) / strength);
	lcol.g *= ((strength - d) / strength);
	lcol.b *= ((strength - d) / strength);

	return (lcol);
}


