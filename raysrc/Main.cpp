
#include <stdio.h>
#include <math.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"
#include "bmpwrite.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	Console		console;
	Camera		camera;
	World		world;
	Plane		*plane;
	Light		*light;
	Sphere		*sphere;
	Texture		texture;

	float	x = 0, y = 0, t = 0;
	int		frame = 0;

	Format format(32, 0x00FF0000, 0x0000FF00, 0x000000FF);

	console.open("Simple Raytracer", 640, 480, format);

	const int width = console.width();
	const int height = console.height();

	Bitmap bitmap(width, height, BMP_TYPE_32BIT);

	camera.SetFOV(90, 60);
	camera.SetViewport(width, height);
	camera.SetPosition(0, 0, 0);
	camera.SetBuffer((int32 *)bitmap.buffer);

	plane = world.AddPlane();
	plane->SetPosition(0, -1, 0);
	plane->SetColour(1, 1, 1);
	plane->reflectivity = 0.3f;

	texture.Load("marblem.pcx");
	texture.SetAxis(0, 1, 0);
	texture.scale = 40;
	texture.SetOffset(-120, 20);
	plane->texDiffuse = &texture;
	plane->texAmbient = &texture;

	plane = world.AddPlane();
	plane->SetPosition(0, 1, 0);
	plane->SetColour(0, 1, 0);
	plane->SetNormal(0, -1, 0);

	plane = world.AddPlane();
	plane->SetPosition(0, 0, 7);
	plane->SetNormal(0, 0, -1);
	plane->SetColour(0, 0, 1);

	plane = world.AddPlane();
	plane->SetPosition(-3, 0, 0);
	plane->SetNormal(1, 0, 0);
	plane->SetColour(1, 1, 0);

	plane = world.AddPlane();
	plane->SetPosition(3, 0, 0);
	plane->SetNormal(-1, 0, 0);
	plane->SetColour(0, 1, 1);

	plane = world.AddPlane();
	plane->SetPosition(0, 0, -7);
	plane->SetNormal(0, 0, 1);
	plane->SetColour(1, 0, 1);

	sphere = world.AddSphere();
	sphere->SetPosition(-1.5, 0, 5.5);
	sphere->SetRadius(0.75f);
	sphere->SetColour(1, 0, 0);
	sphere->reflectivity = 0.8f;

	world.SetCamera(&camera);
	world.SetAmbientLight(0.2f, 0.2f, 0.2f);

	light = world.AddLight();
	light->SetColour(1, 1, 1);
	light->SetPosition(2.8f, 0.8f, 6.8f);
	light->strength = 10;

	camera.SetDirection(t, 0, 0);

	while (!console.key())
	{
		t += 0.1f;

		x = 2.0f * (float)sin(t);
		y = 2.0f * (float)cos(t) + 4.0f;
		light->SetPosition(x, 0.8f, y);

		world.Render();
		{
			unsigned char filename[256];

			sprintf((char *)filename, "output%d.bmp", frame++);
			bitmap.WriteBuffer_BMP(filename);
		}

		console.load(bitmap.buffer, width, height, width * 4, format, Palette());

		console.update();

		if (frame == 64)
			break;
	}

	texture.Release();

	return (0);
}