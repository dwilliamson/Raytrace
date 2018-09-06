#include "matrix.h"
#include "vm.h"


class Colour
{
public:
	Colour(void)
	{
		r = 0;
		g = 0;
		b = 0;
	}

	float			r, g, b;
};


class Intersect
{
public:
	Intersect(void)
	{
		hit = 0;
		where.x = 0;
		where.y = 0;
		where.z = 0;
	}

	bool			hit;
	Colour			colAmbient;
	Colour			colDiffuse;
	Colour			colSpecular;
	float			distance;
	Point3D			where;
};


class Image
{
public:
	int				Load(char *filename);
	void			Release(void);

	int32			*buffer;
	int				width, height;
};


class Texture : public Image
{
public:
	void			SetAxis(float x, float y, float z);
	void			SetOffset(int x, int y);

	Colour			GetUVColour(Point3D *where);

	Point3D			axis;
	float			scale;
	Point2D			offset;
};


class Light;


class Object
{
public:
					Object(void);
	void			SetPosition(float x, float y, float z);
	void			SetColour(float red, float green, float blue);

	void			SetAmbientColour(float red, float green, float blue);
	void			SetDiffuseColour(float red, float green, float blue);
	void			SetSpecularColour(float red, float green, float blue);

	virtual
		Intersect	CalculateIntersection(Point3D *origin, Point3D *ray_normal);

	virtual
		void		GetIntersectionNormal(Point3D *dest, Intersect *isect);

	Object			*next;
	Point3D			position;
	float			reflectivity;

	Colour			colAmbient;
	Colour			colDiffuse;
	Colour			colSpecular;

	Texture			*texAmbient;
	Texture			*texDiffuse;
	Texture			*texSpecular;

private:
};


class Plane : public Object
{
public:
					Plane(void);

	void			SetNormal(float x, float y, float z);
	Intersect		CalculateIntersection(Point3D *origin, Point3D *ray_normal);
	void			GetIntersectionNormal(Point3D *dest, Intersect *isect);

private:
	Point3D			normal;
};


class Sphere : public Object
{
public:
					Sphere(float r);
					Sphere(void);

	void			SetRadius(float r);
	Intersect		CalculateIntersection(Point3D *origin, Point3D *ray_normal);
	void			GetIntersectionNormal(Point3D *dest, Intersect *isect);

	float			radius;
private:
};


class Light : public Object
{
public:
	Colour			LightObject(Object *object, Intersect *isect, int make_ambient);
	void			SetColour(float red, float green, float blue);

	Colour			colour;
	float			strength;
};


class Camera : public Object
{
public:
	void			SetViewport(int width, int height);
	void			SetFOV(int x, int y);
	void			SetDirection(float x, float y, float z);
	void			SetBuffer(int32 *buf);
	void			CalculateRay(int x, int y);
	void			ReflectRay(Point3D *where, Point3D *normal);

	int32			*buffer;
	Point3D			current_ray;
	Point2D			viewport;

private:
	Matrix			direction;
	Point2D			fov;
};


class World
{
public:
	/* --- Functions --- */

					World(void);
					~World(void);

	Plane			*AddPlane(void);
	Light			*AddLight(void);
	Sphere			*AddSphere(void);

	Object			*AddObject(void *object);

	void			Render(void);
	void			SetCamera(Camera *cam);
	int32			CheckForIntersections(void);
	void			SetAmbientLight(float r, float g, float b);
	Colour			GetIntersectionColour(void);
	bool			MeCanSeePoint(Point3D *me, Point3D *point);

	Colour			ambient_light;

private:
	/* --- Variables --- */

	Object			*first_object;
	Object			*last_object;

	Light			*first_light;
	Light			*last_light;

	Camera			*camera;

	int				iteration;
};


#include <stdio.h>			// Eeeuuggh!


class Bitmap
{
public:
#pragma pack(push, 1)

	struct tagBITMAPFILEHEADER
	{
		unsigned short	bfType;					/* File type */
		unsigned long	bfSize;					/* Size of file */
		unsigned short	bfReserved1;			/* Reserved */
		unsigned short	bfReserved2;			/* Reserved */
		unsigned long	bfOffBits;				/* Offset of actual bitmap */
	} bfh;

	struct tagBITMAPINFOHEADER
	{
		unsigned long	biSize;					/* Number of bytes for BITMAPINFOHEADER */
		unsigned long	biWidth;				/* Width of bitmap */
		unsigned long	biHeight;				/* Height of bitmap */
		unsigned short	biPlanes;				/* Number of planes for target device (1) */
		unsigned short	biBitCount;				/* Bits per pixel */
		unsigned long	biCompression;			/* Compression type */
		unsigned long	biSizeImage;			/* Size of the image alone */
		unsigned long	biXPelsPerMeter;		/* Horizontal resolution - pixels per meter */
		unsigned long	biYPelsPerMeter;		/* Vertical resolution */
		unsigned long	biClrUsed;				/* Number of colour indexes used in bitmap */
		unsigned long	biClrImportant;			/* Number of important colours for display */
	} bmiHeader;

#pragma pack(pop)

					Bitmap(unsigned long width, unsigned long height, unsigned char type);
					~Bitmap(void);
	bool			WriteBuffer_BMP(unsigned char *filename);

	unsigned char	*buffer;
	unsigned char	bmp_type;

private:
	bool			WriteImage_8Bit(FILE *);
	bool			WriteImagePalette(FILE *);
	bool			WriteImage_RGB565(FILE *);
	bool			WriteImage_RGB555(FILE *);
	bool			WriteImage_24Bit(FILE *);
	bool			WriteImage_Alpha_RGB1555(FILE *);
	bool			WriteImage_32Bit(FILE *fp);
};


class VMScene
{
public:
	bool			Load(unsigned char *filename);
	void			Run(void);

private:
	void			Render(void);

	World			world;
	VMUnit			*vm_unit;
};