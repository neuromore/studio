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

#ifndef __CORE_CONFIG_H
#define __CORE_CONFIG_H

#include <wchar.h>

// cpu
#if defined(_M_X64) || defined(__amd64__) || defined(__x86_64__)
#define NEUROMORE_CPU_X64
#define NEUROMORE_CPU_X86ORX64
#define NEUROMORE_CPU_64BIT
#elif defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(__i686__)
#define NEUROMORE_CPU_X86
#define NEUROMORE_CPU_X86ORX64
#define NEUROMORE_CPU_32BIT
#elif defined(_M_ARM64) || defined(__aarch64__)
#define NEUROMORE_CPU_ARM64
#define NEUROMORE_CPU_ARMORARM64
#define NEUROMORE_CPU_64BIT
#elif defined (_M_ARM) || defined(__arm__)
#define NEUROMORE_CPU_ARM
#define NEUROMORE_CPU_ARMORARM64
#define NEUROMORE_CPU_32BIT
#else
#error UNKNOWN CPU
#endif


// compilers
#define CORE_COMPILER_MSVC			0x01
#define CORE_COMPILER_GCC			0x04

// finds the compiler type and version
#if (defined(__GNUC__) || defined(__GNUC) || defined(__gnuc))
	#define CORE_COMPILER CORE_COMPILER_GCC
	#define CORE_COMPILERVERSION __VERSION__
#elif defined _MSC_VER
	#define CORE_COMPILER CORE_COMPILER_MSVC
	#define CORE_COMPILERVERSION _MSC_VER
#else
	// unsupported compiler!
#endif


// disable conversion compile warning
#if (CORE_COMPILER == CORE_COMPILER_MSVC)
	#pragma warning (disable : 4244)	// conversion from 'double' to 'float', possible loss of data
	#pragma warning (disable : 4800)	// 'int' : forcing value to bool 'true' or 'false' (performance warning)
	#pragma warning (disable : 4324)	// structure was padded due to __declspec(align())
	#pragma warning (disable : 4714)	// __forceinline function not expanded									  
	#pragma warning (disable : 4996)	// sprintf (etc) was declared deprecated (Visual Studio 2005)
	#pragma warning (disable : 4068)	// unknown pragma (for OpenMP support that isn't recognised by .NET 2003)
	#pragma warning (disable : 4100)	// unreferenced formal parameter
	#pragma warning (disable : 4251)	// class 'type' needs to have dll-interface to be used by clients of class 'type2'
	#pragma warning (disable : 4103)	// alignment changed after including header, may be due to missing #pragma pack(pop)
	#pragma warning (disable : 4530)	// C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc (Visual Studio 2008)
#endif


// define the basic types
#ifndef CORE_DISABLE_STANDARD_TYPEDEFS
	typedef unsigned long long	uint64;		// a 64 bit unsigned integer
	typedef unsigned int		uint32;		// a 32-bit unsigned integer
	typedef unsigned short int	uint16;		// a 16-bit unsigned integer
	typedef unsigned char		uint8;		// an 8-bit unsigned integer
	typedef signed long long	int64;		// a signed 64 bit integer
	typedef signed int			int32;		// a signed 32-bit integer
	typedef signed short int	int16;		// a signed 16-bit integer
	typedef signed char			int8;		// a signed 8-bit integer
#endif


//---------------------
// stringised version of line number (must be done in two steps)
#define CORE_STRINGISE(N) #N
#define CORE_EXPAND_THEN_STRINGISE(N) CORE_STRINGISE(N)
#define CORE_LINE_STR CORE_EXPAND_THEN_STRINGISE(__LINE__)

// MSVC-suitable routines for formatting <#pragma message>
#define CORE_LOC __FILE__ "(" CORE_LINE_STR ")"
#define CORE_OUTPUT_FORMAT(type) CORE_LOC " : [" type "] "

// specific message types for <#pragma message>
#define CORE_WARNING	CORE_OUTPUT_FORMAT("WARNING")
#define CORE_ERROR		CORE_OUTPUT_FORMAT("ERROR")
#define CORE_MESSAGE	CORE_OUTPUT_FORMAT("INFO")
#define CORE_TODO		CORE_OUTPUT_FORMAT("TODO")

// USAGE:
// #pragma message ( CORE_MESSAGE "my message" )
//---------------------


// some special types that are missing inside Visual C++ 6
#if (CORE_COMPILER == CORE_COMPILER_MSVC)
	#if (CORE_COMPILERVERSION < 1300)
		typedef unsigned long uintPointer;
	#else
		#include <stddef.h>
		typedef uintptr_t uintPointer;
	#endif
#else
	#include <stddef.h>
	#include <stdint.h>
	typedef uintptr_t uintPointer;
#endif


// debug macro
#ifdef _DEBUG
	#define CORE_DEBUG
#endif

// date define
#define CORE_DATE __DATE__

#ifdef _DEBUG
	#define CORE_FILE __FILE__
	#define CORE_LINE __LINE__
#else
	#define CORE_FILE NULL
	#define CORE_LINE 0xFFFFFFFF
#endif


// define the __cdecl 
#if (CORE_COMPILER == CORE_COMPILER_MSVC)
	#define CORE_CDECL __cdecl
#else
	#define CORE_CDECL
#endif


// redefine NULL to nullptr
#ifdef NULL
 #undef NULL
#endif
#define NULL nullptr


// alignment macro
#if (CORE_COMPILER == CORE_COMPILER_MSVC)
	#define CORE_ALIGN(NUMBYTES, X) __declspec(align(NUMBYTES)) X
	#define CORE_ALIGN_PRE(NUMBYTES) __declspec(align(NUMBYTES))
	#define CORE_ALIGN_POST(NUMBYTES)
#elif (CORE_COMPILER == CORE_COMPILER_GCC)
	#define CORE_ALIGN(NUMBYTES, X) X __attribute__((aligned(NUMBYTES)))
	#define CORE_ALIGN_PRE(NUMBYTES)
	#define CORE_ALIGN_POST(NUMBYTES) __attribute__((aligned(NUMBYTES)))
#else
	#define CORE_ALIGN(NUMBYTES, X) X
#endif


// define a custom assert macro
#ifndef CORE_NO_ASSERT
	#define CORE_ASSERT(x) assert(x)
#else
	#define CORE_ASSERT(x)
#endif

// DLL import & export
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#ifdef ENGINE_DLL_IMPORT
		#define ENGINE_API	__declspec(dllimport)
	#elif ENGINE_DLL_EXPORT
		#define ENGINE_API	__declspec(dllexport)
	#else
		#define ENGINE_API
	#endif
#else
	#define ENGINE_API
#endif


// platform strings
#define NEUROMORE_PLATFORM_STRING_WINDOWS "win"
#define NEUROMORE_PLATFORM_STRING_OSX "osx"
#define NEUROMORE_PLATFORM_STRING_LINUX "linux"
#define NEUROMORE_PLATFORM_STRING_ANDROID "android"
#define NEUROMORE_PLATFORM_STRING_IOS "ios"
#define NEUROMORE_PLATFORM_STRING_UNKNOWN "unknown"
#if defined(NEUROMORE_PLATFORM_WINDOWS)
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_WINDOWS
#elif defined(NEUROMORE_PLATFORM_OSX)
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_OSX
#elif defined(NEUROMORE_PLATFORM_LINUX)
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_LINUX
#elif defined(NEUROMORE_PLATFORM_ANDROID)
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_ANDROID
#elif defined(NEUROMORE_PLATFORM_IOS)
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_IOS
#else
#define NEUROMORE_PLATFORM_STRING NEUROMORE_PLATFORM_STRING_UNKNOWN
#endif

// cpu strings
#define NEUROMORE_CPU_STRING_X64 "x64"
#define NEUROMORE_CPU_STRING_X86 "x86"
#define NEUROMORE_CPU_STRING_ARM64 "arm64"
#define NEUROMORE_CPU_STRING_ARM "arm"
#define NEUROMORE_CPU_STRING_UNKNOWN "unknown"
#if defined (NEUROMORE_CPU_X64)
#define NEUROMORE_CPU_STRING NEUROMORE_CPU_STRING_X64
#elif defined (NEUROMORE_CPU_X86)
#define NEUROMORE_CPU_STRING NEUROMORE_CPU_STRING_X86
#elif defined (NEUROMORE_CPU_ARM64)
#define NEUROMORE_CPU_STRING NEUROMORE_CPU_STRING_ARM64
#elif defined (NEUROMORE_CPU_ARM)
#define NEUROMORE_CPU_STRING NEUROMORE_CPU_STRING_ARM
#else
#define NEUROMORE_CPU_STRING NEUROMORE_CPU_STRING_UNKNOWN
#endif

#endif
