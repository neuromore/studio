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

#ifndef __CORE_MEMORYFILE_H
#define __CORE_MEMORYFILE_H

// include the Core headers
#include "StandardHeaders.h"


namespace Core
{

class ENGINE_API MemoryFile
{
	public:
		MemoryFile();
		~MemoryFile();

		void Open();
		void Close();
		
		void Seek(uint32 offset);
		void Write(const void* data, uint32 length);

		uint32 GetSize() const							{ return mUsedSize; }
		uint8* GetData() const							{ return mData; }

	private:
		void Forward(uint32 numBytes);

		uint8*	mData;
		uint8*	mCurrentPos;
		uint32	mSize;
		uint32	mUsedSize;
		uint32	mPreAllocSize;
		bool	mAllocate;
};

} // namespace Core

#endif
