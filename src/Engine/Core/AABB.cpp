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
#include "AABB.h"
#include "Math.h"


namespace Core
{

// constructor
AABB::AABB()
{
	Init();
}


// constructor
AABB::AABB(const Vector3& min, const Vector3& max)
{
	mMin = min;
	mMax = max;
}


// destructor
AABB::~AABB()
{
	Init();
}


void AABB::Init()
{
	mMin.Set( CORE_FLOAT_MAX,  CORE_FLOAT_MAX,  CORE_FLOAT_MAX);
	mMax.Set(-CORE_FLOAT_MAX, -CORE_FLOAT_MAX, -CORE_FLOAT_MAX);
}

void AABB::Add(const AABB& box)
{
	Add(box.mMin);
	Add(box.mMax);
}

// add point and grow AABB
void AABB::Add(const Vector3& point)
{
	mMin.x = Min(mMin.x, point.x);
	mMin.y = Min(mMin.y, point.y);
	mMin.z = Min(mMin.z, point.z);

	mMax.x = Max(mMax.x, point.x);
	mMax.y = Max(mMax.y, point.y);
	mMax.z = Max(mMax.z, point.z);
}


bool AABB::IsValid() const
{
	if (mMin.x > mMax.x)
		return false;
	
	if (mMin.y > mMax.y)
		return false;
	
	if (mMin.z > mMax.z)
		return false;
	
	return true;
}


void AABB::SetMin(const Vector3& min)
{
	mMin = min;
}


void AABB::SetMax(const Vector3& max)
{
	mMax = max;
}


const Vector3& AABB::GetMin() const
{
	return mMin;
}


const Vector3& AABB::GetMax() const
{
	return mMax; 
}


Vector3 AABB::GetCenter() const
{ 
	return (mMin + mMax) * 0.5;
}


Vector3 AABB::GetExtents() const
{ 
	return (mMax - mMin) * 0.5;
}


float AABB::GetWidth() const
{ 
	return mMax.x - mMin.x;
}


float AABB::GetHeight() const
{ 
	return mMax.y - mMin.y;
}


float AABB::GetDepth() const
{ 
	return mMax.z - mMin.z;
}

} // namespace Core
