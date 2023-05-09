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
#include "MemoryFile.h"
#include "LogManager.h"


namespace Core
{

MemoryFile::MemoryFile()
{
	mData			= NULL;
	mCurrentPos		= NULL;
	mSize			= 0;
	mUsedSize		= 0;
	mPreAllocSize	= 1024;
	mAllocate		= false;
}


MemoryFile::~MemoryFile()
{
	Close();
}


void MemoryFile::Open()
{
	mData			= NULL;
	mCurrentPos		= NULL;
	mSize			= 0;
	mUsedSize		= 0;
	mPreAllocSize	= 1024;
	mAllocate		= true;
}


void MemoryFile::Close()
{
	if (mAllocate)
		Core::Free( mData );

	mData			= NULL;
	mCurrentPos		= NULL;
	mSize			= 0;
	mUsedSize		= 0;
}


void MemoryFile::Forward(uint32 numBytes)
{
	uint8* newPos = mCurrentPos + numBytes;
	if (newPos > (mData + mSize))
		return;
	
	mCurrentPos = newPos;
}


void MemoryFile::Seek(uint32 offset)
{
	if (offset > mSize)
		return;
	
	mCurrentPos = mData + offset;
	return;
}


void MemoryFile::Write(const void* data, uint32 length)
{
	if ((mCurrentPos + length > mData + mSize) && mAllocate)
	{	
		uint32 offset = mCurrentPos - mData;
		uint32 numBytesExtra = mCurrentPos + length - mData;
		numBytesExtra += mPreAllocSize;
		mData = (uint8*)Core::Realloc((uint8*)mData, mSize + numBytesExtra);
		mSize += numBytesExtra;
		mCurrentPos = mData + offset;
	}

	CORE_ASSERT((mCurrentPos + length) <= (mData + mSize));
	Core::MemCopy(mCurrentPos, data, length);
	Forward(length);

	mUsedSize = (uint32)(mCurrentPos - mData);
}

} // namespace Core
