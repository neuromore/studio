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

#ifndef __CORE_STANDARDHEADERS_H
#define __CORE_STANDARDHEADERS_H

// include standard core headers
#include "Config.h"

// standard includes
#include <math.h>
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <stdarg.h>
#include <wchar.h>


#if (CORE_COMPILER != CORE_COMPILER_GCC)
	#include <new.h>
#else
	#include <new>
	#include <ctype.h>	// strupr and strlwr
#endif

// include the Windows header
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0600
	#endif
	#ifndef WIN32
		#define WIN32
	#endif
	#define WIN32_LEAN_AND_MEAN
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <windows.h>
#endif

#define CORE_INVALIDINDEX32		0xFFFFFFFF
#define CORE_INVALIDINDEX64		0xFFFFFFFFFFFFFFFF
#define CORE_INVALIDINDEX16		0xFFFF
#define CORE_INVALIDINDEX8		0xFF

#define CORE_INT64_MAX			9223372036854775807L
#define CORE_UINT64_MAX			0xFFFFFFFFFFFFFFFF
#define CORE_INT32_MAX			2147483647
#define CORE_UINT32_MAX			0xFFFFFFFF
#define CORE_INT16_MAX			32767
#define CORE_UINT16_MAX			0xFFFF
#define CORE_FLOAT_MAX			FLT_MAX

// specifies the folder separator slash type used on the different supported platforms
#if (defined(NEUROMORE_PLATFORM_WINDOWS))
	#define CORE_FOLDERSEPARATORCHAR	'\\'
#else
	#define CORE_FOLDERSEPARATORCHAR	 '/'
#endif

namespace Core
{

inline void* Allocate(size_t numBytes)
{
	return malloc(numBytes);
}

inline void* Realloc(void* memory, size_t numBytes)
{
	void* newMemPtr = realloc( memory, numBytes );

	if (newMemPtr == NULL)
	{
        #ifdef CORE_DEBUG
            #ifdef NEUROMORE_PLATFORM_WINDOWS
        		OutputDebugStringA("CRITICAL ERROR: REALLOCATION FAILURE");
            #endif
        #endif

		CORE_ASSERT(newMemPtr != 0);

		return memory;
	}

	return newMemPtr;
}

inline void Free(void* memory)
{ 
	free(memory);
}

inline void* MemCopy(void* dest, const void* source, const size_t numBytes)
{
	return memcpy(dest, source, numBytes);
}

inline void* MemMove(void* dest, const void* source, const size_t numBytes)
{
	return memmove(dest, source, numBytes);
}

inline void* MemSet(void* address, const uint32 value, const size_t numBytes)
{
	return memset(address, value, numBytes);
}

};

#endif
