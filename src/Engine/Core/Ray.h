/*
 * neuromore Engine
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __CORE_RAY_H
#define __CORE_RAY_H

 // include required headers
#include "StandardHeaders.h"
#include "Vector.h"
#include "AABB.h"
#include "Plane.h"


namespace Core
{

	class ENGINE_API Ray
	{
	public:
		// constructor & destructor
		Ray();
		Ray(const Vector3& origin, const Vector3& destination);

		// mutators
		void Set(const Vector3& origin, const Vector3& destination);
		void SetOrigin(const Vector3& origin);
		void SetDestination(const Vector3& destination);

		// accessors
		const Vector3& GetOrigin() const { return mOrigin; }
		const Vector3& GetDestination() const { return mDestination; }
		const Vector3& GetDirection() const { return mDirection; }
		float GetLength() const;

		// intersection tests
		bool Intersects(const Plane& plane, Vector3* intersect = NULL) const;
		bool Intersects(const AABB& box, Vector3* intersectA = NULL, Vector3* intersectB = NULL) const;
		bool Intersects(const Vector3& p1, const Vector3& p2, const Vector3& p3, Vector3* intersect = NULL, float* baryU = NULL, float* baryV = NULL) const;

	private:
		Vector3 mOrigin;
		Vector3 mDestination;
		Vector3 mDirection;
	};

} // namespace Core


#endif