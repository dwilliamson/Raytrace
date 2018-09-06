
#include <math.h>
#include "types.h"


class Matrix
{
public:
	/* --- Variables --- */

	float	e[3][3];

	/* --- Functions --- */

	void Identity(void);
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
	void MulVector(Point3D *dest, Point3D *vector);

	/* --- Overloaded operators --- */

	Matrix operator *(Matrix &other);
};


void Matrix::Identity(void)
{
	e[0][0] = 1; e[1][0] = 0; e[2][0] = 0;
	e[0][1] = 0; e[1][1] = 1; e[2][1] = 0;
	e[0][2] = 0; e[1][2] = 0; e[2][2] = 1;
}


void Matrix::RotateX(float angle)
{
	e[0][0] = 1;			e[1][0] = 0;					e[2][0] = 0;
	e[0][1] = 0;			e[1][1] = (float)cos(angle);	e[2][1] = (float)sin(angle);
	e[0][2] = 0;			e[1][2] = (float)-sin(angle);	e[2][2] = (float)cos(angle);
}


void Matrix::RotateY(float angle)
{
	e[0][0] = (float)cos(angle);	e[1][0] = 0;			e[2][0] = (float)sin(angle);
	e[0][1] = 0;					e[1][1] = 1;			e[2][1] = 0;
	e[0][2] = (float)-sin(angle);	e[1][2] = 0;			e[2][2] = (float)cos(angle);
}


void Matrix::RotateZ(float angle)
{
	e[0][0] = (float)cos(angle);	e[1][0] = (float)-sin(angle);	e[2][0] = 0;
	e[0][1] = (float)sin(angle);	e[1][1] = (float)cos(angle);	e[2][1] = 0;
	e[0][2] = 0;					e[1][2] = 0;					e[2][2] = 1;
}


Matrix Matrix::operator *(Matrix &other)
{
	Matrix	answer;
	int		x, y;

	for (y = 0; y < 3; y++)
	{
		for (x = 0; x < 3; x++)
		{
			answer.e[x][y] = e[0][y] * other.e[x][0] +
							 e[1][y] * other.e[x][1] +
							 e[2][y] * other.e[x][2];
		}
	}

	return (answer);
}

void Matrix::MulVector(Point3D *dest, Point3D *vector)
{
	dest->x = vector->x * e[0][0] + vector->y * e[0][1] + vector->z * e[0][2];
	dest->y = vector->x * e[1][0] + vector->y * e[1][1] + vector->z * e[1][2];
	dest->z = vector->x * e[2][0] + vector->y * e[2][1] + vector->z * e[2][2];
}