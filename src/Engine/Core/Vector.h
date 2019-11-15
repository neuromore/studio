/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __CORE_VECTOR_H
#define __CORE_VECTOR_H

// include required header files
#include "StandardHeaders.h"
#include "Math.h"


namespace Core
{

class ENGINE_API Vector2
{
	public:
		Vector2()																	{}
		Vector2(float inX, float inY)												{ x=inX; y=inY; }
		Vector2(const Vector2& vec)													{ x=vec.x; y=vec.y; }

		void Set(float inX, float inY)												{ x=inX; y=inY; }
		void Zero()																	{ x=0.0; y=0.0; }

		float SquareLength() const													{ return (x*x + y*y); }
		float Length() const														{ return Math::Sqrt(x*x + y*y); }
		float SafeLength() const													{ return Math::SafeSqrt(x*x + y*y); }
		float Dot(const Vector2& vec) const											{ return (x*vec.x + y*vec.y); }

		Vector2& Normalize()														{ const float inv=1.0/Length(); x*=inv; y*=inv; return *this; }
		Vector2 Normalized() const													{ const float inv=1.0/Length(); return Vector2(x*inv, y*inv); }

		bool operator==(const Vector2& vec) const									{ return ((x==vec.x) && (y==vec.y)); }
		bool operator!=(const Vector2& vec) const									{ return ((x!=vec.x) || (y!=vec.y)); }

		Vector2 operator-() const													{ return Vector2(-x, -y); }

		const Vector2& operator*=(float value)										{ x*=value; y*=value; return *this; }
		const Vector2& operator*=(double value)										{ x*=value; y*=value; return *this; }
		const Vector2& operator/=(float value)										{ const float inv=1.0f/value; x*=inv; y*=inv; return *this; }
		const Vector2& operator/=(double value)										{ const float inv=1.0f/value; x*=inv; y*=inv; return *this; }
	
		const Vector2& operator+=(const Vector2& vec)								{ x+=vec.x; y+=vec.y; return *this; }
		const Vector2& operator-=(const Vector2& vec)								{ x-=vec.x; y-=vec.y; return *this; }	
		const Vector2& operator=(const Vector2& vec)								{ x =vec.x; y =vec.y; return *this; }
		const Vector2& operator*=(const Vector2& vec)								{ x*=vec.x; y*=vec.y; return *this; }

	public:
		float x, y;
};

inline Vector2 operator+(const Vector2& v1, const Vector2& v2)						{ return Vector2(v1.x + v2.x, v1.y + v2.y); }
inline Vector2 operator-(const Vector2& v1, const Vector2& v2)						{ return Vector2(v1.x - v2.x, v1.y - v2.y); }
inline Vector2 operator*(const Vector2& v1, const Vector2& v2)						{ return Vector2(v1.x*v2.x, v1.y*v2.y); }
inline Vector2 operator*(const Vector2& vec, float value)							{ return Vector2(vec.x*value, vec.y*value); }
inline Vector2 operator*(const Vector2& vec, double value)							{ return Vector2(vec.x*value, vec.y*value); }
inline Vector2 operator*(float value, const Vector2& vec)							{ return Vector2(value*vec.x, value*vec.y); }
inline Vector2 operator*(double value, const Vector2& vec)							{ return Vector2(value*vec.x, value*vec.y); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class ENGINE_API Vector3
{
	public:
		Vector3()																	{}
		Vector3(float inX, float inY, float inZ)									{ x=inX; y=inY; z=inZ; }
		Vector3(const Vector3& vec)													{ x=vec.x; y=vec.y; z=vec.z; }

		void Set(float inX, float inY, float inZ)									{ x=inX; y=inY; z=inZ; }
		void Zero()																	{ x=0.0; y=0.0; z=0.0; }

		float Dot(const Vector3& vec) const											{ return (x*vec.x + y*vec.y + z*vec.z); }
		Vector3 Cross(const Vector3& vec) const										{ return Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x); }

		float SquareLength() const													{ return (x*x + y*y + z*z); }
		float Length() const														{ return Math::Sqrt(x*x + y*y + z*z); }
		float SafeLength() const													{ return Math::SafeSqrt(x*x + y*y + z*z); }

		Vector3& Normalize() 														{ const float inv = 1.0f/Length();	x*= inv; y*= inv; z*= inv; return *this; }
		Vector3 Normalized() const													{ const float inv = 1.0f/Length(); return Vector3(x*inv, y*inv, z*inv); }
		Vector3& SafeNormalize()													{ const float sqrLen = SquareLength(); if (sqrLen > 0.0f) { const float invLen = 1.0/Math::Sqrt(sqrLen); x*=invLen; y*=invLen; z*=invLen; } return *this; }
		Vector3 SafeNormalized() const												{ const float sqrLen = SquareLength(); if (sqrLen > 0.0f) { const float invLen = 1.0/Math::Sqrt(sqrLen); return Vector3(x*invLen, y*invLen, z*invLen); } return Vector3(*this); }

		bool operator==(const Vector3& v) const										{ return ((v.x == x) && (v.y == y) && (v.z == z)); }
		bool operator!=(const Vector3& v) const										{ return ((v.x != x) || (v.y != y) || (v.z != z)); }

		operator float*()															{ return (float*)&x; }
		operator const float*() const												{ return (const float*)&x; }

		Vector3 operator-() const													{ return Vector3(-x, -y, -z); }
		
		const Vector3& operator*=(float value)										{ x*=value; y*=value; z*=value; return *this; }
		const Vector3& operator*=(double value)										{ x*=value; y*=value; z*=value; return *this; }
		const Vector3& operator/=(float value)										{ const float inv = 1.0/value; x*=inv; y*=inv; z*=inv; return *this; }
		const Vector3& operator/=(double value)										{ const float inv = 1.0/value; x*=inv; y*=inv; z*=inv; return *this; }
		
		const Vector3& operator+=(const Vector3& vec)								{ x+= vec.x; y+= vec.y; z+= vec.z; return *this; }
		const Vector3& operator-=(const Vector3& vec)								{ x-= vec.x; y-= vec.y; z-= vec.z; return *this; }
		const Vector3& operator=(const Vector3& vec)								{ x = vec.x; y = vec.y; z = vec.z; return *this; }
		const Vector3& operator/=(const Vector3& vec)								{ x/= vec.x; y/= vec.y; z/= vec.z; return *this; }

	public:
		float x, y, z;
};

inline Vector3 operator*(const Vector3& v1, const Vector3& v2)						{ return Vector3(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z); }
inline Vector3 operator+(const Vector3& v1, const Vector3& v2)						{ return Vector3(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z); }
inline Vector3 operator-(const Vector3& v1, const Vector3& v2)						{ return Vector3(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z); }
inline Vector3 operator*(const Vector3& vec, float value)							{ return Vector3(vec.x*value, vec.y*value, vec.z*value); }
inline Vector3 operator*(float value, const Vector3& vec)							{ return Vector3(value*vec.x, value*vec.y, value*vec.z); }
inline Vector3 operator*(const Vector3& vec, double value)							{ return Vector3(vec.x*value, vec.y*value, vec.z*value); }
inline Vector3 operator*(double value, const Vector3& vec)							{ return Vector3(value*vec.x, value*vec.y, value*vec.z); }
inline Vector3 operator/(const Vector3& v1, const Vector3& v2)						{ return Vector3(v1.x/ v2.x, v1.y/ v2.y, v1.z/ v2.z); }
inline Vector3 operator/(const Vector3& vec, float value)							{ float inv=1.0/value; return Vector3(vec.x*inv, vec.y*inv, vec.z*inv); }
inline Vector3 operator/(float value, const Vector3& vec)							{ return Vector3(value/vec.x, value/vec.y, value/vec.z); }
inline Vector3 operator/(const Vector3& vec, double value)							{ float inv=1.0/ value; return Vector3(vec.x*inv, vec.y*inv, vec.z*inv); }
inline Vector3 operator/(double value, const Vector3& vec)							{ return Vector3(value/vec.x, value/vec.y, value/vec.z); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class ENGINE_API Vector4
{
	public:
		Vector4()																	{}
		Vector4(float inX, float inY, float inZ, float inW)							{ x=inX; y=inY; z=inZ; w=inW; }
		Vector4(const Vector4& vec)													{ x=vec.x; y=vec.y; z=vec.z; w=vec.w; }

		void Set(float inX, float inY, float inZ, float inW)						{ x=inX; y=inY; z=inZ; w=inW; }
		void Zero()																	{ x=0.0; y=0.0; z=0.0; w=0.0; }

		float Dot(const Vector4& vec) const											{ return (x*vec.x + y*vec.y + z*vec.z + w*vec.w); }

		float SquareLength() const													{ return (x*x + y*y + z*z + w*w); }
		float Length() const														{ return Math::Sqrt(x*x + y*y + z*z + w*w); }
		Vector4& Normalize()														{ const float invLen = 1.0/Length(); x*= invLen; y*= invLen; z*= invLen; w*= invLen; return *this; }
		Vector4 Normalized() const													{ const float invLen = 1.0/Length(); return Vector4(x*invLen, y*invLen, z*invLen, w*invLen); }

		bool operator==(const Vector4& vec) const									{ return ((vec.x == x) && (vec.y == y) && (vec.z == z) && (vec.w == w)); }
		bool operator!=(const Vector4& vec) const									{ return ((vec.x != x) || (vec.y != y) || (vec.z != z) || (vec.w != w)); }

		Vector4 operator-() const													{ return Vector4(-x, -y, -z, -w); }

		const Vector4& operator*=(float value)										{ x*=value; y*=value; z*=value; w*=value; return *this; }
		const Vector4& operator*=(double value)										{ x*=value; y*=value; z*=value; w*=value; return *this; }
		const Vector4& operator/=(float value)										{ const float inv=1.0/value; x*=inv; y*=inv; z*=inv; w*=inv; return *this; }
		const Vector4& operator/=(double value)										{ const float inv=1.0/value; x*=inv; y*=inv; z*=inv; w*=inv; return *this; }

		const Vector4& operator+=(const Vector4& vec)								{ x+=vec.x; y+=vec.y; z+=vec.z; w+=vec.w; return *this; }
		const Vector4& operator-=(const Vector4& vec)								{ x-=vec.x; y-=vec.y; z-=vec.z; w-=vec.w; return *this; }
		const Vector4& operator=(const Vector4& vec)								{ x=vec.x; y=vec.y; z=vec.z; w=vec.w; return *this; }
		const Vector4& operator+=(const Vector3& vec)								{ x+=vec.x; y+=vec.y; z+=vec.z; return *this; }
		const Vector4& operator-=(const Vector3& vec)								{ x-=vec.x; y-=vec.y; z-=vec.z; return *this; }

	public:
		float x, y, z, w;
};

inline Vector4 operator*(const Vector4& v1, const Vector4& v2)						{ return Vector4(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z, v1.w*v2.w); }
inline Vector4 operator+(const Vector4& v1, const Vector4& v2)						{ return Vector4(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w); }
inline Vector4 operator-(const Vector4& v1, const Vector4& v2)						{ return Vector4(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w); }
inline Vector4 operator/(const Vector4& v1, const Vector4& v2)						{ return Vector4(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z, v1.w/v2.w); }
inline Vector4 operator*(const Vector4& vec, float value)							{ return Vector4(vec.x*value, vec.y*value, vec.z*value, vec.w*value); }
inline Vector4 operator*(float value, const Vector4& vec)							{ return Vector4(value*vec.x, value*vec.y, value*vec.z, value*vec.w); }
inline Vector4 operator*(const Vector4& vec, double value)							{ return Vector4(vec.x*value, vec.y*value, vec.z*value, vec.w*value); }
inline Vector4 operator*(double value, const Vector4& vec)							{ return Vector4(value*vec.x, value*vec.y, value*vec.z, value*vec.w); }

} // namespace Core


#endif
