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

#ifndef __CORE_AABB_H
#define __CORE_AABB_H

// include the required headers
#include "StandardHeaders.h"
#include "Vector.h"


namespace Core
{

class ENGINE_API AABB
{
	public:
		// constructors & destructor
		AABB();
		AABB(const Vector3& min, const Vector3& max);
		~AABB();

		// initialization
		void Init();
		void Add(const Vector3& point);
		bool IsValid() const;

		// mutators
		void SetMin(const Vector3& min);
		void SetMax(const Vector3& mac);

		// accessors
		const Vector3& GetMin() const;
		const Vector3& GetMax() const;
		Vector3 GetCenter() const;
		Vector3 GetExtents() const;
		float GetWidth() const;
		float GetHeight() const;
		float GetDepth() const;

	private:
		Vector3 mMin;
		Vector3 mMax;
};

} // namespace Core


#endif
