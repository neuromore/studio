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

#ifndef __CORE_VERSION_H
#define __CORE_VERSION_H

// include required headers
#include "StandardHeaders.h"
#include "String.h"


namespace Core
{

class ENGINE_API Version
{
	public:
		// constructor & destructor
		Version();
		Version(uint32 major, uint32 minor, uint32 patch);
		~Version();

		uint32 GetMajor() const					{ return mMajor; }
		uint32 GetMinor() const					{ return mMinor; }
		uint32 GetPatch() const					{ return mPatch; }

		bool operator==(const Version& other) const;
		bool operator!=(const Version& other) const;
		bool operator<(const Version& other) const;
		bool operator<=(const Version& other) const;

		// conversion to string helper
		Core::String AsString() const;

	private:
		uint32	mMajor;
		uint32	mMinor;
		uint32	mPatch;
};

} // namespace Core


#endif
