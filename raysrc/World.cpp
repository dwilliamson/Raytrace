
#include <math.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


World::World(void)
{
	first_object = NULL;
	last_object = NULL;
	first_light = NULL;
	last_light = NULL;
}


World::~World(void)
{
	if (first_object)
	{
		Object	*cur_object, *next_object;
		Light	*cur_light, *next_light;

		for (cur_object = first_object; cur_object; cur_object = next_object)
		{
			next_object = cur_object->next;
			delete cur_object;
		}

		for (cur_light = first_light; cur_light; cur_light = next_light)
		{
			next_light = (Light *)cur_light->next;
			delete cur_light;
		}
	}
}


Plane *World::AddPlane(void)
{
	Plane	*plane;

	// Allocate plane structure
	plane = new Plane;

	// Fill in point on the plane
	plane->SetPosition(0, 0, 0);
	plane->SetNormal(0, 1, 0);

	plane->next = NULL;

	// Link object in the world
	if (first_object == NULL)
	{
		first_object = plane;
		last_object = plane;
	}
	else
	{
		last_object->next = plane;
		last_object = plane;
	}

	return (plane);
}


Object *World::AddObject(void *object)
{
	// Link object in the world
	if (first_object == NULL)
	{
		first_object = (Object *)object;
		last_object = (Object *)object;
	}
	else
	{
		last_object->next = (Object *)object;
		last_object = (Object *)object;
	}

	return ((Object *)object);
}


Light *World::AddLight(void)
{
	Light	*light;

	// Allocate light structure
	light = new Light;

	// Fill in stuff
	light->SetPosition(0, 0, 0);
	light->strength = 1.0f;

	light->next = NULL;

	// Link light in the world list
	if (first_light == NULL)
	{
		first_light = light;
		last_light = light;
	}
	else
	{
		last_light->next = light;
		last_light = light;
	}

	return (light);
}


Sphere *World::AddSphere(void)
{
	Sphere	*sphere;

	// Allocate light structure
	sphere = new Sphere;

	// Fill in position
	sphere->SetPosition(0, 0, 0);
	sphere->SetRadius(1);

	sphere->next = NULL;

	// Link object in the world list
	if (first_object == NULL)
	{
		first_object = sphere;
		last_object = sphere;
	}
	else
	{
		last_object->next = sphere;
		last_object = sphere;
	}

	return (sphere);
}


void World::SetCamera(Camera *cam)
{
	camera = cam;
}


bool World::MeCanSeePoint(Point3D *me, Point3D *point)
{
	Object		*cur_object;
	Intersect	inter;
	Point3D		ray;
	float		len;

	// Generate the me->point direction vector

	ray.x = point->x - me->x;
	ray.y = point->y - me->y;
	ray.z = point->z - me->z;

	len = (float)sqrt(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z);
	ray.x /= len;
	ray.y /= len;
	ray.z /= len;

	// Check for intersections with each object in the scene

	for (cur_object = first_object; cur_object; cur_object = cur_object->next)
	{
		inter = cur_object->CalculateIntersection(me, &ray);

		// If there is an intersection before the ray hits the point

		if (inter.hit && inter.distance <= len)
		{
			return (0);
		}
	}

	return (1);
}


Colour World::GetIntersectionColour(void)
{
	Object		*cur_object;
	Light		*cur_light;
	int32		colour = 0;
	Intersect	inter;
	Colour		retcol;
	float		distance = 1e24f;

	// Check for intersections with each object in the scene

	for (cur_object = first_object; cur_object; cur_object = cur_object->next)
	{
		inter = cur_object->CalculateIntersection(&camera->position, &camera->current_ray);

		// Closest hit yet?

		if (inter.hit && inter.distance < distance)
		{
			Colour	colour;
			Intersect	isect;

			// Ambient term

			colour.r = ambient_light.r * inter.colAmbient.r;
			colour.g = ambient_light.g * inter.colAmbient.g;
			colour.b = ambient_light.b * inter.colAmbient.b;

			// Sum each light in the scene

			for (cur_light = first_light; cur_light; cur_light = (Light *)cur_light->next)
			{
				Colour	col;

				// If the light can be seen from here use it, else leave shadowed

				if (MeCanSeePoint(&inter.where, &cur_light->position))
				{
					col = cur_light->LightObject(cur_object, &inter, 0);

					colour.r += col.r;
					colour.g += col.g;
					colour.b += col.b;
				}
			}

			// Clamp the colour to 0...1

			if (colour.r > 1) colour.r = 1; if (colour.r < 0) colour.r = 0;
			if (colour.g > 1) colour.g = 1; if (colour.g < 0) colour.g = 0;
			if (colour.b > 1) colour.b = 1; if (colour.b < 0) colour.b = 0;

			// If this object is a mirror-type object, reflect the ray and cast it again

			if (cur_object->reflectivity != 0.0f && iteration < 4)
			{
				Point3D	normal;
				Point3D old_ray, old_pos;
				Colour bcol;

				// Backup the currently active ray

				old_ray = camera->current_ray;
				old_pos = camera->position;

				// Reflect the ray about the object normal

				cur_object->GetIntersectionNormal(&normal, &inter);
				camera->ReflectRay(&inter.where, &normal);
				camera->position = inter.where;

				// Now start tracing the scene again with this new ray

				iteration++;
				bcol = GetIntersectionColour();

				// Alpha blend the two colours together with the reflectivity value

				colour.r = (colour.r * (1.0f - cur_object->reflectivity)) + (bcol.r * cur_object->reflectivity);
				colour.g = (colour.g * (1.0f - cur_object->reflectivity)) + (bcol.g * cur_object->reflectivity);
				colour.b = (colour.b * (1.0f - cur_object->reflectivity)) + (bcol.b * cur_object->reflectivity);

				camera->current_ray = old_ray;
				camera->position = old_pos;
			}

			distance = inter.distance;

			retcol = colour;
		}
	}

	// Can check here for NULL pointer, incase a sky sphere is needed later on

	return (retcol);
}


int32 World::CheckForIntersections(void)
{
	Colour		col;
	int32		colour = 0, r, g, b;

	col = GetIntersectionColour();
	iteration = 0;

	r = (int32)(255.0f * col.r);
	g = (int32)(255.0f * col.g);
	b = (int32)(255.0f * col.b);

	colour = b | (g << 8) | (r << 16);

	return (colour);
}


void World::Render(void)
{
	int	x, y, z;

	for (y = 0, z = 0; y < camera->viewport.y; y++)
	{
		for (x = 0; x < camera->viewport.x; x++, z++)
		{
			camera->CalculateRay(x, camera->viewport.y - y - 1);
			camera->buffer[z] = CheckForIntersections();
		}
	}
}


void World::SetAmbientLight(float r, float g, float b)
{
	ambient_light.r = r;
	ambient_light.g = g;
	ambient_light.b = b;
}
