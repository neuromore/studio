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

// include required headers
#include "Ray.h"

namespace Core
{

// constructor
Ray::Ray()
{
}


// constructor
Ray::Ray(const Vector3& origin, const Vector3& destination)
{
	Set(origin, destination);
}


void Ray::Set(const Vector3& origin, const Vector3& destination)
{
	mOrigin			= origin;
	mDestination	= destination;
	mDirection		= (destination-origin).Normalized();
}


void Ray::SetOrigin(const Vector3& origin)
{
	Set(origin, mDestination);
}


void Ray::SetDestination(const Vector3& destination)
{
	Set(mOrigin, destination);
}


float Ray::GetLength() const
{
	return (mDestination-mOrigin).SafeLength();
}


// ray-axis aligned bounding box intersection test
bool Ray::Intersects(const AABB& box, Vector3* intersectA, Vector3* intersectB) const
{
	float tNear = -FLT_MAX, tFar=FLT_MAX;

	const Vector3& minVec = box.GetMin();
	const Vector3& maxVec = box.GetMax();

	// For all three axes, check the near and far intersection point on the two slabs
	for (int32 i=0; i<3; i++)
	{
		if (Math::Abs(mDirection[i]) < Math::epsilon)
		{
			// direction is parallel to this plane, check if we're somewhere between min and max
			if ((mOrigin[i] < minVec[i]) || (mOrigin[i] > maxVec[i]))
				return false;
		}
		else
		{
			// calculate t's at the near and far slab, see if these are min or max t's
			float t1 = (minVec[i] - mOrigin[i]) / mDirection[i];
			float t2 = (maxVec[i] - mOrigin[i]) / mDirection[i];
			if (t1 > t2)
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			if (t1 > tNear) tNear = t1;							// accept nearest value
			if (t2 < tFar)  tFar  = t2;							// accept farthest value
			if ((tNear > tFar) || (tFar < 0.0f))
				return false;
		}
	}

	if (intersectA)
		*intersectA = mOrigin + mDirection * tNear;

	if (intersectB)
		*intersectB = mOrigin + mDirection * tFar;

	return true;
}


// ray-triangle intersection test
bool Ray::Intersects(const Vector3& p1, const Vector3& p2, const Vector3& p3, Vector3* intersect, float* baryU, float* baryV) const
{
	// calculate two vectors of the polygon
	const Vector3 edge1 = p2 - p1;
	const Vector3 edge2 = p3 - p1;

	// begin calculating determinant - also used to calculate U parameter
	const Vector3 dir = mDestination - mOrigin;
	const Vector3 pvec = dir.Cross(edge2);

	// if determinant is near zero, ray lies in plane of triangle
	const float det = edge1.Dot(pvec);
	if (det > -Math::epsilon && det < Math::epsilon)
		return false;

	// calculate distance from vert0 to ray origin
	const Vector3 tvec = mOrigin - p1;

	// calculate U parameter and test bounds
	const float inv_det = 1.0f / det;
	const float u = tvec.Dot(pvec) * inv_det;
	if (u < 0.0f || u > 1.0f)
		return false;

	// prepare to test V parameter
	const Vector3 qvec = tvec.Cross(edge1);

	// calculate V parameter and test bounds
	const float v = dir.Dot(qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f)
		return false;

	// calculate t, ray intersects triangle
	const float t = edge2.Dot(qvec) * inv_det;
	if (t < 0.0f || t > 1.0f)
		return false;

	// output the results
	if (baryU)		*baryU = u;
	if (baryV)		*baryV = v;
	if (intersect)	*intersect = mOrigin + t * dir;

	// yes, there was an intersection
	return true;
}

} // namespace Core
