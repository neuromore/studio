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

#ifndef __CORE_BYTEARRAY_H
#define __CORE_BYTEARRAY_H

// include the required headers
#include "StandardHeaders.h"
#include "Array.h"
#include "String.h"


namespace Core
{

class ENGINE_API ByteArray : public Array<uint8>
{
	public:
		// constructor and destructor
		ByteArray();
		ByteArray(const uint8* data, uint32 size);
		virtual ~ByteArray();

		void Add(const uint8* data, uint32 size);
		void FromString(const String& text);

		ByteArray Left(uint32 size);
		ByteArray Mid(uint32 pos, uint32 size);

		void Remove(uint32 pos, uint32 size);

		uint8 GetByte(uint32 pos)							{ return mData[pos]; }
		unsigned char* Data() const							{ return mData; }
};

} // namespace Core


#endif
