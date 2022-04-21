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
#include "ByteArray.h"


namespace Core
{

// constructor
ByteArray::ByteArray() : Array<uint8>()
{
}


// constructor
ByteArray::ByteArray(const uint8* data, uint32 size) : Array<uint8>()
{
	Add(data, size);
}


// destructor
ByteArray::~ByteArray()
{
}


void ByteArray::Add(const uint8* data, uint32 size)
{
	// resize
	const uint32 sizeBefore = Size();
	Resize( sizeBefore + size );

	// copy data
	Core::MemCopy( (void*)&mData[sizeBefore], data, size );
}


void ByteArray::FromString(const String& text)
{
	const uint32 stringLength = text.GetLength();
	if (text.IsEmpty() == true)
		return;

	Add( (const uint8*)text.AsChar(), stringLength );
}


ByteArray ByteArray::Left(uint32 size)
{
	ByteArray result;

	const uint32 sourceSize = Size();
	if (size >= sourceSize)
		size = sourceSize;

	result.Add(mData, size);

	return result;
}


ByteArray ByteArray::Mid(uint32 pos, uint32 size)
{
	ByteArray result;
	
	const uint32 sourceSize = Size();
	if (pos + size >= sourceSize)
		size = sourceSize - pos;

	result.Add( &mData[pos], size );

	return result;
}


void ByteArray::Remove(uint32 pos, uint32 size)
{
	const uint32 sourceSize = Size();
	if (pos + size >= sourceSize)
		size = sourceSize - pos;

	for (uint32 i=0; i<size; ++i)
		Array<uint8>::Remove(pos);
}

} // namespace Core
