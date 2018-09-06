

#include <math.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


Sphere::Sphere(float r)
{
	radius = r;
}


Sphere::Sphere(void)
{
	radius = 1;
}


void Sphere::SetRadius(float r)
{
	radius = r;
}


Intersect Sphere::CalculateIntersection(Point3D *origin, Point3D *ray_normal)
{
	Intersect	inter;
	float		A, B, C;
	Point3D		Q, w;
	float		a, b, c;
	float		sval, t[2], use_t;

	Q = *origin;
	w = *ray_normal;
	a = position.x;
	b = position.y;
	c = position.z;

	// Calculate quadratic equation co-efficients

	A = w.x * w.x + w.y * w.y + w.z * w.z;

	B = (2.0f * w.x * (Q.x - a)) + (2.0f * w.y * (Q.y - b)) + (2.0f * w.z * (Q.z - c));

	C = (Q.x * Q.x - 2.0f * (Q.x * a) + a * a) + (Q.y * Q.y - 2.0f * (Q.y * b) + b * b) +
		(Q.z * Q.z - 2.0f * (Q.z * c) + c * c) - (radius * radius);

	// Check for imaginary roots and roots at infinity

	if (A == 0)
		return (inter);

	sval = B * B - 4 * A * C;
	if (sval < 0)
		return (inter);

	// Calculate real roots

	t[0] = (-B + (float)sqrt(sval)) / (2.0f * A);
	t[1] = (-B - (float)sqrt(sval)) / (2.0f * A);

	if (t[0] <= -1e-4 || t[1] <= -1e-4)
		return (inter);

	// Use the nearest intersection point

	if (t[0] > t[1])
		use_t = t[1];
	else
		use_t = t[0];

	// Reflected colour

	inter.colAmbient = colAmbient;
	inter.colDiffuse = colDiffuse;
	inter.colSpecular = colSpecular;

	// Stuff

	inter.hit = 1;
	inter.distance = use_t;

	// Point of intersection

	inter.where.x = Q.x + use_t * w.x;
	inter.where.y = Q.y + use_t * w.y;
	inter.where.z = Q.z + use_t * w.z;

	return (inter);
}


void Sphere::GetIntersectionNormal(Point3D *dest, Intersect *isect)
{
	float	len;

	dest->x = isect->where.x - position.x;
	dest->y = isect->where.y - position.y;
	dest->z = isect->where.z - position.z;

	len = (float)sqrt(dest->x * dest->x + dest->y * dest->y + dest->z * dest->z);

	dest->x /= len;
	dest->y /= len;
	dest->z /= len;
}
