#pragma once

#include <cmath>
#include "Math/Math_Types.h"

class Mat4x4_t;

#define mPI 3.14159265

#define mSqrt(x) sqrt(x)

#define mSin(x)	sin(x)
#define mCos(x) cos(x)
#define mTan(x) tan(x)
#define mASin(x) asin(x)
#define mACos(x) acos(x)
#define mATan(x) atan(x)

#define mDegrees_to_Radians(x) x * (mPI / 180.0)
#define mRadians_to_Degrees(x) x * (180 / mPI)

namespace Transform
{
	Mat4x4_t Rotate(float angle, Vec3_t axis, Mat4x4_t matrix)
	{
		float angCos = mCos(angle);
		float angSin = mSin(angle);

		matrix.data[0] = angCos + (axis.x * axis.x) * (1 - angCos);
		matrix.data[1] = axis.x * axis.y * (1 - angCos) - axis.z * angSin;
		matrix.data[2] = axis.x * axis.z * (1 - angCos) + axis.y * angSin;
		matrix.data[3] = 0;
		matrix.data[4] = axis.y * axis.x * (1 - angCos) + axis.z * angSin;
		matrix.data[5] = angCos + (axis.y * axis.y) * (1 - angCos);
		matrix.data[6] = axis.y * axis.z * (1 - angCos) - axis.x * angSin;
		matrix.data[7] = 0;
		matrix.data[8] = axis.z * axis.x * (1 - angCos) - axis.y * angSin;
		matrix.data[9] = axis.z * axis.y * (1 - angCos) + axis.x * angSin;
		matrix.data[10] = angCos + (axis.z * axis.z) * (1 - angCos);
		matrix.data[11] = 0;
		matrix.data[12] = 0;
		matrix.data[13] = 0;
		matrix.data[14] = 0;
		matrix.data[15] = 1;

		return matrix;
	}

	Mat4x4_t LookAt(Vec3_t cameraPos, Vec3_t forward, Vec3_t up)
	{
		Vec3_t right = Vec3_t::crossProduct(&forward, &up);

		Mat4x4_t outMatrix(1.0f);

		outMatrix.data[0] = right.x;
		outMatrix.data[1] = right.y;
		outMatrix.data[2] = right.z;
		outMatrix.data[3] = -(cameraPos.x);
		outMatrix.data[4] = up.x;
		outMatrix.data[5] = up.y;
		outMatrix.data[6] = up.z;
		outMatrix.data[7] = -(cameraPos.y);
		outMatrix.data[8] = forward.x;
		outMatrix.data[9] = forward.y;
		outMatrix.data[10] = forward.z;
		outMatrix.data[11] = -(cameraPos.z);
		outMatrix.data[12] = 0;
		outMatrix.data[13] = 0;
		outMatrix.data[14] = 0;
		outMatrix.data[15] = 1;
		
		
		return outMatrix;
	}
}