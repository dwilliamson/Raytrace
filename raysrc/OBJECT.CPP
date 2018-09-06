
#include "ptc.h"
#include "types.h"
#include "classes.h"


Object::Object(void)
{
	reflectivity = 0.0f;
	SetPosition(0, 0, 0);
	SetColour(0, 0, 0);
	next = NULL;

	texAmbient = NULL;
	texDiffuse = NULL;
	texSpecular = NULL;
}


void Object::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}


void Object::SetColour(float red, float green, float blue)
{
	SetAmbientColour(red, green, blue);
	SetDiffuseColour(red, green, blue);
	SetSpecularColour(red, green, blue);
}


void Object::SetAmbientColour(float red, float green, float blue)
{
	colAmbient.r = red;
	colAmbient.g = green;
	colAmbient.b = blue;
}


void Object::SetDiffuseColour(float red, float green, float blue)
{
	colDiffuse.r = red;
	colDiffuse.g = green;
	colDiffuse.b = blue;
}


void Object::SetSpecularColour(float red, float green, float blue)
{
	colSpecular.r = red;
	colSpecular.g = green;
	colSpecular.b = blue;
}


Intersect Object::CalculateIntersection(Point3D *origin, Point3D *normal)
{
	Intersect	ret;

	return (ret);
}


void Object::GetIntersectionNormal(Point3D *dest, Intersect *isect)
{
}
