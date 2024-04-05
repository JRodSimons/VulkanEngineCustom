#pragma once

#include "EngineGlobals.h"

class ENGINE_API Vec2_t
{
public:
	float x;
	float y;

public:
	Vec2_t()
	{
		x = 0;
		y = 0;
	}
	Vec2_t(float x_, float y_)
	{
		x = x_;
		y = y_;
	}

	void scalar_add(float s);

	void scalar_subtract(float s);

	void scalar_multiply(float s);

	void negate();

	float length();

	static Vec2_t scale(Vec2_t* vec, float scale_x, float scale_y);

	Vec2_t normalize();

	Vec2_t add(Vec2_t* vecA, Vec2_t* vecB);

	Vec2_t subtract(Vec2_t* vecA, Vec2_t* vecB);

	static float dotProduct(Vec2_t* vecA, Vec2_t* vecB);
};

class ENGINE_API Vec3_t
{
public:
	float x;
	float y;
	float z;

public:
	Vec3_t()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vec3_t(float x_, float y_, float z_)
	{
		x = x_;
		y = y_;
		z = z_;
	}

	void scalar_add(float s);

	void subtract(float s);

	void multiply(float s);

	void negate();

	float length();

	Vec3_t normalize();

	static Vec3_t add(Vec3_t* vecA, Vec3_t* vecB);

	static Vec3_t subtract(Vec3_t* vecA, Vec3_t* vecB);

	static float dotProduct(Vec3_t* vecA, Vec3_t* vecB);

	static Vec3_t crossProduct(Vec3_t* vecA, Vec3_t* vecB);

	Vec3_t scale(Vec3_t* original, Vec3_t* scale);

	Vec3_t translate(Vec3_t* original, Vec3_t* translate);
};

class ENGINE_API Vec4_t
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Vec4_t()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	Vec4_t(float x_, float y_, float z_, float w_)
	{
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}
};

class ENGINE_API Mat2x2_t
{
public:
	float data[4];

public:
	Mat2x2_t()
	{
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
		data[3] = 0;
	};
	Mat2x2_t(float _00, float _01, float _10, float _11)
	{
		data[0] = _00;
		data[1] = _01;
		data[2] = _10;
		data[3] = _11;
	};

	static Mat2x2_t add(Mat2x2_t* matA, Mat2x2_t* matB);

	static Mat2x2_t subtract(Mat2x2_t* matA, Mat2x2_t* matB);

	static Mat2x2_t multiply(Mat2x2_t* matA, Mat2x2_t* matB);

	static Mat2x2_t scalar_multiply(Mat2x2_t* mat, float s);
};

class ENGINE_API Mat3x3_t
{
public:
	float data[9];

public:
	Mat3x3_t()
	{
		for (int i = 0; i < 8; i++)
		{
			data[i] = 0;
		}
	};
	Mat3x3_t(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
	{
		data[0] = _00;
		data[1] = _01;
		data[2] = _02;
		data[3] = _10;
		data[4] = _11;
		data[5] = _12;
		data[6] = _20;
		data[7] = _21;
		data[8] = _22;
	};

	static Mat3x3_t multiply(Mat3x3_t* matA, Mat3x3_t* matB);
};

class ENGINE_API Mat4x4_t
{
public:
	float data[16];

public:
	Mat4x4_t()
	{
		for (int i = 0; i < 15; i++)
		{
			data[i] = 0;
		}
	}

	Mat4x4_t(float x)
	{
		if (x == 1.0f)
		{
			// Identity matrix
			data[0] = 1;
			data[1] = 0;
			data[2] = 0;
			data[3] = 0;
			data[4] = 0;
			data[5] = 1;
			data[6] = 0;
			data[7] = 0;
			data[8] = 0;
			data[9] = 0;
			data[10] = 1;
			data[11] = 0;
			data[12] = 0;
			data[13] = 0;
			data[14] = 0;
			data[15] = 1;
		}
	}
	Mat4x4_t(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30, float _31, float _32, float _33)
	{
		data[0] = _00;
		data[1] = _01;
		data[2] = _02;
		data[3] = _03;
		data[4] = _10;
		data[5] = _11;
		data[6] = _12;
		data[7] = _13;
		data[8] = _20;
		data[9] = _21;
		data[10] = _22;
		data[11] = _23;
		data[12] = _30;
		data[13] = _31;
		data[14] = _32;
		data[15] = _33;
	};
};

struct ENGINE_API Vertex_t
{
	Vec2_t	pos;
	Vec3_t	color;
};