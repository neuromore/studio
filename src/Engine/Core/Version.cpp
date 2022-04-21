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
#include "Version.h"
#include "LogManager.h"


namespace Core
{

// constructor
Version::Version()
{
	mMajor = 0;
	mMinor = 0;
	mPatch = 0;
}


// constructor
Version::Version(uint32 major, uint32 minor, uint32 patch)
{
	mMajor = major;
	mMinor = minor;
	mPatch = patch;
}


// destructor
Version::~Version()
{
}


String Version::AsString() const
{
	String result;

	result.Format("%i.%i.%i", mMajor, mMinor, mPatch);

	return result;
}


bool Version::operator==(const Version& other) const
{
	if (mMajor == other.mMajor && mMinor == other.mMinor && mPatch == other.mPatch)
		return true;

	return false;
}


bool Version::operator!=(const Version& other) const
{
	if (mMajor != other.mMajor || mMinor != other.mMinor || mPatch != other.mPatch)
		return true;

	return false;
}


bool Version::operator<(const Version& other) const
{
	// major version already is smaller
	if(mMajor < other.mMajor)
		return true;

	// major is equal, minor is smaller
	if(mMajor == other.mMajor && mMinor < other.mMinor)
		return true;

	// major and minor are equal, patch is smaller
	if(mMajor == other.mMajor && mMinor == other.mMinor && mPatch < other.mPatch)
		return true;

	return false;
}


bool Version::operator<=(const Version& other) const
{
	return ( (*this < other) || (*this == other) );
}


} // namespace Core
