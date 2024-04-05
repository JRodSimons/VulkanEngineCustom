#include "PCH_Engine.h"

#include "Math_Types.h"

//=================================
// 
// Vector 2
// 
//=================================
void Vec2_t::scalar_add(float s)
{
	x = x + s;
	y = y + s;
}

void Vec2_t::scalar_subtract(float s)
{
	x = x - s;
	y = y - s;
}

void Vec2_t::scalar_multiply(float s)
{
	x = x * s;
	y = y * s;
}

void Vec2_t::negate()
{
	x = -x;
	y = -y;
}

float Vec2_t::length()
{
	float x2 = x * x;
	float y2 = y * y;

	float result = x2 + y2;
	return sqrt(result);
}

Vec2_t Vec2_t::scale(Vec2_t* vec, float scale_x, float scale_y)
{
	Vec2_t outVector;

	outVector.x = vec->x * scale_x;
	outVector.y = vec->y * scale_y;

	if (!scale_x)
		outVector.x = 1;

	if (!scale_y)
		outVector.y = 1;

	return outVector;
}

Vec2_t Vec2_t::normalize()
{
	Vec2_t outvector;
	float length = this->Vec2_t::length();

	outvector.x = x / length;
	outvector.y = y / length;
	return outvector;
}

Vec2_t Vec2_t::add(Vec2_t* vecA, Vec2_t* vecB)
{
	Vec2_t outVector{};
	outVector.x = vecA->x + vecB->x;
	outVector.y = vecA->y + vecB->y;
	return outVector;
}

Vec2_t Vec2_t::subtract(Vec2_t* vecA, Vec2_t* vecB)
{
	Vec2_t outVector{};
	outVector.x = vecA->x - vecB->x;
	outVector.y = vecA->y - vecB->y;
	return outVector;
}

float Vec2_t::dotProduct(Vec2_t* vecA, Vec2_t* vecB)
{
	float x_ = vecA->x * vecB->x;
	float y_ = vecA->y * vecB->y;
	float Result = x_ + y_;
	return Result;
}

//=================================
// 
// Vector 3
// 
//=================================
void Vec3_t::scalar_add(float s)
{
	x = x + s;
	y = y + s;
	z = z + s;
}

void Vec3_t::subtract(float s)
{
	x = x - s;
	y = y - s;
	z = z - s;
}

void Vec3_t::multiply(float s)
{
	x = x * s;
	y = y * s;
	z = z * s;
}

void Vec3_t::negate()
{
	x = -x;
	y = -y;
	z = -z;
}

float Vec3_t::length()
{
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;

	float result = x2 + y2 + z2;
	return sqrt(result);
}

Vec3_t Vec3_t::normalize()
{
	Vec3_t outvector;
	float length = this->length();

	outvector.x = x / length;
	outvector.y = y / length;
	outvector.z = z / length;
	return outvector;
}

Vec3_t Vec3_t::add(Vec3_t* vecA, Vec3_t* vecB)
{
	Vec3_t outVector{};
	outVector.x = vecA->x + vecB->x;
	outVector.y = vecA->y + vecB->y;
	outVector.z = vecA->z + vecB->z;
	return outVector;
}

Vec3_t Vec3_t::subtract(Vec3_t* vecA, Vec3_t* vecB)
{
	Vec3_t outVector{};
	outVector.x = vecA->x - vecB->x;
	outVector.y = vecA->y - vecB->y;
	outVector.z = vecA->z - vecB->z;
	return outVector;
}

float Vec3_t::dotProduct(Vec3_t* vecA, Vec3_t* vecB)
{
	float x_ = vecA->x * vecB->x;
	float y_ = vecA->y * vecB->y;
	float z_ = vecA->z * vecB->z;
	float Result = x_ + y_ + z_;
	return Result;
}

Vec3_t Vec3_t::crossProduct(Vec3_t* vecA, Vec3_t* vecB)
{
	Vec3_t outVector;

	outVector.x = (vecA->y * vecB->z) - (vecA->z * vecB->y);
	outVector.y = (vecA->z * vecB->x) - (vecA->x * vecB->z);
	outVector.z = (vecA->x * vecB->y) - (vecA->y * vecB->x);

	return outVector;
}

Vec3_t Vec3_t::scale(Vec3_t* original, Vec3_t* scale)
{
	Vec3_t outVector;

	outVector.x = original->x * scale->x;
	outVector.y = original->y * scale->y;
	outVector.z = original->z * scale->z;

	return outVector;
}

Vec3_t Vec3_t::translate(Vec3_t* original, Vec3_t* translate)
{
	Vec3_t outVector;

	outVector.x = original->x + translate->x;
	outVector.y = original->y + translate->y;
	outVector.z = original->z + translate->z;

	return outVector;
}

//=================================
// 
// Matrix 2x2
// 
//=================================
Mat2x2_t Mat2x2_t::add(Mat2x2_t* matA, Mat2x2_t* matB)
{
	Mat2x2_t outMatrix;

	for (int i = 0; i < 4; i++)
	{
		outMatrix.data[i] = matA->data[i] + matB->data[i];
	}

	return outMatrix;
}

Mat2x2_t Mat2x2_t::subtract(Mat2x2_t* matA, Mat2x2_t* matB)
{
	Mat2x2_t outMatrix;

	for (int i = 0; i < 4; i++)
	{
		outMatrix.data[i] = matA->data[i] - matB->data[i];
	}

	return outMatrix;
}

Mat2x2_t Mat2x2_t::multiply(Mat2x2_t* matA, Mat2x2_t* matB)
{
	Mat2x2_t outMatrix;

	outMatrix.data[0] = (matA->data[0] * matB->data[0]) + (matA->data[1] * matB->data[2]);
	outMatrix.data[1] = (matA->data[0] * matB->data[1]) + (matA->data[1] * matB->data[3]);
	outMatrix.data[2] = (matA->data[2] * matB->data[0]) + (matA->data[3] * matB->data[2]);
	outMatrix.data[3] = (matA->data[2] * matB->data[1]) + (matA->data[3] * matB->data[3]);

	return outMatrix;
}

Mat2x2_t Mat2x2_t::scalar_multiply(Mat2x2_t* mat, float s)
{
	Mat2x2_t outMatrix;
	for (int i = 0; i < 4; i++)
	{
		outMatrix.data[i] = mat->data[i] * s;
	}

	return outMatrix;
};

//=================================
// 
// Matrix 3x3
// 
//=================================
Mat3x3_t Mat3x3_t::multiply(Mat3x3_t* matA, Mat3x3_t* matB)
{
	Mat3x3_t outMatrix;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			outMatrix.data[i] = (matA->data[i] * matB->data[j]) + (matA->data[i + 1] * matB->data[j + 3]) + (matA->data[i + 2] * matB->data[j + 6]);
			j++;

			outMatrix.data[i + 1] = (matA->data[i] * matB->data[j]) + (matA->data[i + 1] * matB->data[j + 3]) + (matA->data[i + 2] * matB->data[j + 6]);
			j++;

			outMatrix.data[i + 2] = (matA->data[i] * matB->data[j]) + (matA->data[i + 1] * matB->data[j + 3]) + (matA->data[i + 2] * matB->data[j + 6]);
		}
		i += 2;
	}

	return outMatrix;
}