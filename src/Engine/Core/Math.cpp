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

// include precompiled header
#include <Engine/Precompiled.h>

// include the required headers
#include "Math.h"
#include "Vector.h"


namespace Core
{

// single precission
ENGINE_API const float Math::pi				= 3.14159265358979323846f;
ENGINE_API const float Math::twoPi			= 6.28318530717958647692f;
ENGINE_API const float Math::halfPi			= 1.57079632679489661923f;
ENGINE_API const float Math::invPi			= 0.31830988618f;
ENGINE_API const float Math::epsilon		= 0.000001f;
ENGINE_API const float Math::sqrt2			= 1.4142135623730950488016887242097f;
ENGINE_API const float Math::halfSqrt2		= 0.70710678118654752440084436210485f;
ENGINE_API const float Math::sqrt3			= 1.7320508075688772935274463415059f;
ENGINE_API const float Math::halfSqrt3		= 0.86602540378443864676372317075294f;
ENGINE_API const float	Math::sqrtHalf		= 0.7071067811865475244f;

// double precission
ENGINE_API const double Math::piD			= 3.14159265358979323846;
ENGINE_API const double Math::twoPiD		= 6.28318530717958647692;


// float
template <>
ENGINE_API bool IsClose<float>(const float& a, const float& b, const float threshold)
{
	return (Math::Abs(a - b) <= threshold);
}


// double
template <>
ENGINE_API bool IsClose<double>(const double& a, const double& b, const float threshold)
{
	return (Math::Abs(a - b) <= threshold);
}

// Vector2
template <>
ENGINE_API bool IsClose<Vector2>(const Vector2& a, const Vector2& b, const float threshold)
{
	return ((a - b).SquareLength() <= threshold);
}


// Vector3
template <>
ENGINE_API bool IsClose<Vector3>(const Vector3& a, const Vector3& b, const float threshold)
{
	return ((a - b).SquareLength() <= threshold);
}


// Vector4
template <>
ENGINE_API bool IsClose<Vector4>(const Vector4& a, const Vector4& b, const float threshold)
{
	return ((a - b).SquareLength() <= threshold);
}

/**
 * Remap the given value in range [fromMin, fromMax] to the new range [toMin, toMax].
 * A value of fromMin will result in toMin, fromMax will result in toMax. Everything in between will be interpolated and within [toMin, toMax].
 */
double RemapRange(double value, double fromMin, double fromMax, double toMin, double toMax)
{
	if (Core::Math::AbsD(fromMax - fromMin) > Core::Math::epsilon)
		return ( (value - fromMin) / (fromMax - fromMin) ) * (toMax - toMin) + toMin;
	
	return toMin;
}


double ClampedRemapRange(double value, double fromMin, double fromMax, double toMin, double toMax)
{
	const double remapped = RemapRange( value, fromMin, fromMax, toMin, toMax );
	return Clamp<double>( remapped, Min<double>(toMin, toMax), Max<double>(toMin, toMax) );
}


float CalcCosineInterpolationWeight(float linearValue)
{
	return (1.0f - Math::Cos(linearValue * Math::pi)) * 0.5f;
}


float SampleEaseInOutCurve(float t, float k1, float k2)
{
	const float f = k1 * 2.0f/Core::Math::pi + k2 - k1 + (1.0f - k2) * 2.0f/Core::Math::pi;

	// ease in section
	if (t < k1)
		return (k1*(2.0f/Core::Math::pi) * (Core::Math::Sin((t/k1)*Core::Math::halfPi - Core::Math::halfPi)+1.0f)) / f;
	else
	// mid section
	if (t < k2)	
		return (k1/Core::Math::halfPi + t-k1) / f;
	// ease out section
	else
		return ((k1 / Core::Math::halfPi) + k2 - k1 + ((1.0f - k2) * (2.0f/Core::Math::pi) * Core::Math::Sin( ((t-k2)/(1.0f-k2))*Core::Math::halfPi ) )) / f;
}


/**
 * Convert from cartesian coordinates into spherical coordinates.
 * This uses the y-axis (up) and x-axis (right) as basis.
 * The input vector needs to be normalized!
 * @param normalizedVector The normalized direction vector to convert into spherical coordinates.
 * @result The spherical angles, in radians.
 */
/*Vector2 ToSpherical(const Vector3& normalizedVector)
{	
	return Vector2( Math::ATan2( normalizedVector.y, normalizedVector.x ),
					Math::ACos( normalizedVector.z ) );
}*/


/**
 * Convert from spherical coordinates back into cartesian coordinates.
 * this uses the y-axis (up) and x-axis (right) as basis.
 * @param spherical The spherical coordinates, as returned by ToSpherical(...).
 * @result The unit direction vector that was converted from the spherical coordinates.
 */
/*Vector3 FromSpherical(const Vector2& spherical)
{
	return Vector3( Math::Cos(spherical.x),
					Math::Sin(spherical.x) * Math::Sin(spherical.y),
					Math::Sin(spherical.x) * Math::Cos(spherical.y) );
}*/

} // namespace Core
