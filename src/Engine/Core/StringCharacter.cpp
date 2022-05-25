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
#include "StringCharacter.h"
#include "StringIterator.h"
#include "LogManager.h"

namespace Core
{

// statics
ENGINE_API const StringCharacter StringCharacter::space(' ');
ENGINE_API const StringCharacter StringCharacter::tab('\t');
ENGINE_API const StringCharacter StringCharacter::endLine('\n');
ENGINE_API const StringCharacter StringCharacter::comma(',');
ENGINE_API const StringCharacter StringCharacter::dot('.');
ENGINE_API const StringCharacter StringCharacter::colon(':');
ENGINE_API const StringCharacter StringCharacter::backSlash('\\');
ENGINE_API const StringCharacter StringCharacter::forwardSlash('/');
ENGINE_API const StringCharacter StringCharacter::semiColon(';');
ENGINE_API const StringCharacter StringCharacter::doubleQuote('\"');
ENGINE_API const StringCharacter StringCharacter::dash('-');
ENGINE_API const StringCharacter StringCharacter::asterisk('*');


// init from a UTF16 code point
void StringCharacter::InitFromUTF16(uint16 first, uint16 second, uint32 index)
{
	if (first >= StringIterator::UNICODE_SURHIGH_START && first <= StringIterator::UNICODE_SURHIGH_END)
		mCodeUnit = ((first - StringIterator::UNICODE_SURHIGH_START) << StringIterator::UNICODE_HALFSHIFT) + (second - StringIterator::UNICODE_SURLOW_START) + StringIterator::UNICODE_HALFBASE;
	else
		mCodeUnit = first;

	mIndex = index;
}


// convert to UTF16
uint16* StringCharacter::AsUTF16(uint16* output) const
{
	if (mCodeUnit <= StringIterator::UNICODE_MAX_BMP)
	{
		output[0] = mCodeUnit;
		output[1] = 0;
	}
	else
	{
		const uint32 value = mCodeUnit - StringIterator::UNICODE_HALFBASE;
		output[0] = (uint16)((value >> StringIterator::UNICODE_HALFSHIFT) + StringIterator::UNICODE_SURHIGH_START);
		output[1] = (uint16)((value & StringIterator::UNICODE_HALFMASK) + StringIterator::UNICODE_SURLOW_START);
	}

	return output;
}


// convert to UTF8
char* StringCharacter::AsUTF8(char* output, uint32* outNumBytes, bool addNullTerminator) const
{
	// get the UTF32 character code
	uint32 ch = AsUTF32();

	// convert that into UTF8
	unsigned char* target = (unsigned char*)output;

	uint16 bytesToWrite = 0;
	const uint32 byteMask = 0xBF;
	const uint32 byteMark = 0x80; 
		
	// figure out how many bytes the result will require
	// turn any illegally large UTF32 things (> Plane 17) into replacement chars
	if (ch < (uint32)0x80)				bytesToWrite = 1;
	else if (ch < (uint32)0x800)		bytesToWrite = 2;
	else if (ch < (uint32)0x10000)		bytesToWrite = 3;
	else if (ch <= StringIterator::UNICODE_MAX_LEGAL_UTF32) bytesToWrite = 4;
	else
	{
		bytesToWrite = 3;
		ch = StringIterator::UNICODE_REPLACEMENT_CHAR;
	}
	
	target += bytesToWrite;
	switch (bytesToWrite)
	{
		case 4: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
		case 3: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
		case 2: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
		case 1: *--target = (uint8) (ch | StringIterator::firstByteMark[bytesToWrite]);
	}
	target += bytesToWrite;

	if (addNullTerminator == true) 
		*target = 0;

	*outNumBytes = bytesToWrite;
	return output;
}



// init from UTF8
bool StringCharacter::InitFromUTF8(const char* utf8Data, uint32 index)
{
	uint32 ch = 0;
	const unsigned char* source = (unsigned char*)utf8Data;
	const unsigned char firstByteValue = *source;
	uint32 extraBytesToRead = (uint32)StringIterator::trailingBytesForUTF8[firstByteValue];

	// do this check whether lenient or strict
	#ifdef CORE_DEBUG
		if (StringIterator::IsLegalUTF8((uint8*)source, extraBytesToRead+1) == false)
		{
			Core::LogWarning("Core::StringCharacter::InitFromUTF8() - The input data is not valid UTF8!");
			return false;
		}
	#endif

	switch (extraBytesToRead)
	{
		case 0:
			ch += *source++; 
			break;
		case 1:		
			ch += *source++; ch <<= 6;
			ch += *source++;		
			break;
		case 2:
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++;
			break;
		case 3:
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++;
			break;
		case 4:
			ch += *source++; ch <<= 6; // remember, illegal UTF-8
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++;
			break;
		case 5:
			ch += *source++; ch <<= 6; // remember, illegal UTF-8
			ch += *source++; ch <<= 6; // remember, illegal UTF-8
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++; ch <<= 6;
			ch += *source++;
			break;
	};

/*
		switch (extraBytesToRead)
		{
			case 5: ch += *source++; ch <<= 6; // remember, illegal UTF-8
			case 4: ch += *source++; ch <<= 6; // remember, illegal UTF-8
			case 3: ch += *source++; ch <<= 6;
			case 2: ch += *source++; ch <<= 6;
			case 1: ch += *source++; ch <<= 6;
			case 0: ch += *source++;
			default:;
		}
*/

	ch -= StringIterator::offsetsFromUTF8[extraBytesToRead];
	InitFromUTF32( ch, index );

	return true;
}


// calculate the number of required bytes for this character when writing to UTF8
uint32 StringCharacter::CalcNumRequiredUTF8Bytes() const
{
	const uint32 ch = AsUTF32();

	// figure out how many bytes the result will require
	uint32 bytesToWrite = 0;
	if (ch < (uint32)0x80)			bytesToWrite = 1;
	else if (ch < (uint32)0x800)	bytesToWrite = 2;
	else if (ch < (uint32)0x10000)	bytesToWrite = 3;
	else if (ch < (uint32)0x110000)	bytesToWrite = 4;
	else { bytesToWrite = 3; /* StringIterator::UNICODE_REPLACEMENT_CHAR */ }

	return bytesToWrite;
}


}	// namespace Core

