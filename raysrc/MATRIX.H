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
