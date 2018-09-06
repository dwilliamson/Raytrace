
#include "ptc.h"
#include <math.h>
#include "types.h"
#include "classes.h"


void Camera::SetViewport(int width, int height)
{
	viewport.x = width;
	viewport.y = height;
}


void Camera::SetFOV(int x, int y)
{
	fov.x = x;
	fov.y = y;
}


void Camera::SetDirection(float x, float y, float z)
{
	Matrix	rot[3];

	rot[0].RotateX(x);
	rot[1].RotateY(y);
	rot[2].RotateZ(z);

	direction = rot[1] * rot[0] * rot[2];
}


void Camera::SetBuffer(int32 *buf)
{
	buffer = buf;
}


void Camera::CalculateRay(int x, int y)
{
	Point3D		pt_ray;
	float		angle_h, angle_v, len;

	// Calculate the horizontal and vertical angles of the point through screen ray

	angle_h = (((float)fov.x / (float)viewport.x) * (float)x) - ((float)fov.x / 2.0f);
	angle_v = (((float)fov.y / (float)viewport.y) * (float)y) - ((float)fov.y / 2.0f);
	angle_h *= (3.141593f / 180.0f);
	angle_v *= (3.141593f / 180.0f);

	// Calculate the point through screen ray

	pt_ray.x = (float)sin(angle_h);
	pt_ray.y = (float)sin(angle_v);
	pt_ray.z = 1;

	// Normalise ray

	len = (float)sqrt(1 + pt_ray.x * pt_ray.x + pt_ray.y * pt_ray.y);
	pt_ray.x /= len;
	pt_ray.y /= len;
	pt_ray.z /= len;

	// Need a fish-eye effect?
//	pt_ray.z = (float)sqrt(1 - (pt_ray.x * pt_ray.x + pt_ray.y * pt_ray.y));

	direction.MulVector(&current_ray, &pt_ray);
}


void Camera::ReflectRay(Point3D *where, Point3D *normal)
{
	float		cos_angle, len;
	Point3D		a, i, b;
	Point3D		p, R, N;

	p = *where;
	N = *normal;
	R = current_ray;

	// a = p - R

	a.x = p.x - R.x;
	a.y = p.y - R.y;
	a.z = p.z - R.z;

	// cos(x) = -R.N
	
	cos_angle = -R.x * N.x + -R.y * N.y + -R.z * N.z;

	// i = (|R|cos(x)).N + p

	len = (float)sqrt(R.x * R.x + R.y * R.y + R.z * R.z) * cos_angle;
	i.x = len * N.x + p.x;
	i.y = len * N.y + p.y;
	i.z = len * N.z + p.z;

	// b = a + 2(i - a)
	// b = a + 2i - 2a
	// b = 2i - a

	b.x = 2.0f * i.x - a.x;
	b.y = 2.0f * i.y - a.y;
	b.z = 2.0f * i.z - a.z;

	// F = b - p

	current_ray.x = b.x - p.x;
	current_ray.y = b.y - p.y;
	current_ray.z = b.z - p.z;

	// Normalise the resultant ray

	len = (float)sqrt(current_ray.x * current_ray.x + current_ray.y * current_ray.y + current_ray.z * current_ray.z);
	current_ray.x /= len;
	current_ray.y /= len;
	current_ray.z /= len;
}