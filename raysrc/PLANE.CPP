
#include <math.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


Plane::Plane(void)
{
	SetNormal(0, 1, 0);
}


void Plane::SetNormal(float x, float y, float z)
{
	normal.x = x;
	normal.y = y;
	normal.z = z;
}


Intersect Plane::CalculateIntersection(Point3D *origin, Point3D *ray_normal)
{
	Intersect	inter;
	Point3D		sub;
	float		upper_val, lower_val, t;

	sub.x = position.x - origin->x;
	sub.y = position.y - origin->y;
	sub.z = position.z - origin->z;

	upper_val = sub.x * normal.x + sub.y * normal.y + sub.z * normal.z;

	lower_val = normal.x * ray_normal->x +
				normal.y * ray_normal->y +
				normal.z * ray_normal->z;

	if (lower_val == 0)
		return (inter);

	t = upper_val / lower_val;

	if (t <= 1e-4)
		return (inter);

	sub.x = ray_normal->x * t;
	sub.y = ray_normal->y * t;
	sub.z = ray_normal->z * t;

	t = (float)sqrt(sub.x * sub.x + sub.y * sub.y + sub.z * sub.z);
	inter.distance = t;

	sub.x += origin->x;
	sub.y += origin->y;
	sub.z += origin->z;

	// If an ambient texture exists, use that instead of the colour

	if (texAmbient == NULL)
		inter.colAmbient = colAmbient;
	else
		inter.colAmbient = texAmbient->GetUVColour(&sub);

	// If a diffuse texture exists, use that instead of the colour

	if (texDiffuse == NULL)
		inter.colDiffuse = colDiffuse;
	else
		inter.colDiffuse = texDiffuse->GetUVColour(&sub);

	// If a specular texture exists, use that instead of the colour

	if (texSpecular == NULL)
		inter.colSpecular = colSpecular;
	else
		inter.colSpecular = texSpecular->GetUVColour(&sub);

	inter.hit = 1;

	inter.where.x = sub.x;
	inter.where.y = sub.y;
	inter.where.z = sub.z;

	return (inter);
}


void Plane::GetIntersectionNormal(Point3D *dest, Intersect *isect)
{
	dest->x = normal.x;
	dest->y = normal.y;
	dest->z = normal.z;
}
