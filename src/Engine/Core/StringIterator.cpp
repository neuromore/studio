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

// includd required headers
#include "StringIterator.h"
#include "LogManager.h"


namespace Core
{

// Index into the table below with the first byte of a UTF-8 sequence to
// get the number of trailing bytes that are supposed to follow it.
// Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
// left as-is for anyone who may want to do such conversion, which was
// allowed in earlier algorithms.
const char StringIterator::trailingBytesForUTF8[256] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};


// magic values subtracted from a buffer value during UTF8 conversion.
// this table contains as many values as there might be trailing bytes in a UTF-8 sequence.
const uint32 StringIterator::offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL };


// Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
// into the first byte, depending on how many bytes follow.  There are
// as many entries in this table as there are UTF-8 sequence types.
// (I.e., one byte sequence, two byte... etc.). Remember that sequences for *legal* UTF-8 will be 4 or fewer bytes total.
const uint8 StringIterator::firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };


// check if we're a legal UTF8 input
bool StringIterator::IsLegalUTF8(const uint8* source, uint32 length)
{
    uint8 a;
    const uint8* srcptr = source + length;
    switch (length)
	{
	    default: 
			return false;

		// everything else falls through when "true"...
		case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 2: if ((a = (*--srcptr)) > 0xBF) return false;

		switch (*source)
		{
			// no fall-through in this inner switch
			case 0xE0: if (a < 0xA0) return false; break;
			case 0xED: if (a > 0x9F) return false; break;
			case 0xF0: if (a < 0x90) return false; break;
			case 0xF4: if (a > 0x8F) return false; break;
			default:   if (a < 0x80) return false;
		}

		case 1: if (*source >= 0x80 && *source < 0xC2) return false;
    }

    if (*source > 0xF4)
		return false;

    return true;
}


// check if a given sequence is a valid UTF8 string
bool StringIterator::IsLegalUTF8Sequence(const uint8* source, const uint8* sourceEnd)
{
    uint32 length = trailingBytesForUTF8[*source] + 1;
    if (source+length > sourceEnd)
		return false;
    return IsLegalUTF8(source, length);
}


// calculate the number of characters in a given UTF8 string
uint32 StringIterator::CalcNumCharacters(const char* utf8String)
{
	return CalcNumCharacters(utf8String, (uint32)strlen(utf8String));
}


// calculate the number of characters in a given UTF8 string
uint32 StringIterator::CalcNumCharacters(const char* utf8String, uint32 numBytes)
{
	uint32 numCharacters = 0;

	uint32 offset = 0;
	while (offset < numBytes)
	{
		uint32 extraBytesToRead = StringIterator::trailingBytesForUTF8[(unsigned char)utf8String[offset] ];
		offset += extraBytesToRead + 1;
		numCharacters++;
	}

	return numCharacters;
}


// get the number of characters in the string
uint32 StringIterator::CalcNumCharacters() const
{
	return CalcNumCharacters(mData, mNumCodeUnits);
}


// set the index to the start of the string
void StringIterator::Rewind()
{ 
	mCodeUnitIndex = 0;
}


// set the index to a given character's location
void StringIterator::SetIndexToCharacter(uint32 characterIndex)
{ 
	mCodeUnitIndex = FindCharacterCodeUnitIndex( characterIndex );
}


// set the index to a given value
void StringIterator::SetIndex(uint32 codeUnitIndex)
{ 
	mCodeUnitIndex = codeUnitIndex;
}


// set the index to the end of the string
void StringIterator::SetIndexToEnd()
{
	mCodeUnitIndex = mNumCodeUnits;
}


// forward the index to the next character
void StringIterator::ForwardToNextCharacter()
{
	uint32 index = mCodeUnitIndex;
	mCodeUnitIndex += StringIterator::trailingBytesForUTF8[ (unsigned char)mData[index] ] + 1;
	CORE_ASSERT( mCodeUnitIndex < mNumCodeUnits );
}


// get next character without returning a copy
void StringIterator::GetNextCharacter(StringCharacter* outCharacter)
{
	uint32 index = mCodeUnitIndex;
	mCodeUnitIndex += StringIterator::trailingBytesForUTF8[ (unsigned char)mData[index] ] + 1;
	outCharacter->InitFromUTF8(&mData[index], index);
}


// get the next character
StringCharacter StringIterator::GetNextCharacter()
{
	uint32 index = mCodeUnitIndex;
	CORE_ASSERT( mCodeUnitIndex < mNumCodeUnits );
	mCodeUnitIndex += StringIterator::trailingBytesForUTF8[ (unsigned char)mData[index] ] + 1;
	return StringCharacter((const char*)&mData[index], index );
}


// reverse to the previous character
void StringIterator::ReverseToPreviousCharacter()
{
	CORE_ASSERT( mCodeUnitIndex > 0 );
	mCodeUnitIndex--;
	if (mData[mCodeUnitIndex] & 128)
	{
		mCodeUnitIndex--;
		if ((mData[mCodeUnitIndex] & 64) == 0)
		{
			mCodeUnitIndex--;
			if ((mData[mCodeUnitIndex] & 64) == 0)
				mCodeUnitIndex--;
		}
	}
}


// get the previous character
StringCharacter StringIterator::GetPreviousCharacter()
{
	CORE_ASSERT( mCodeUnitIndex > 0 );
	mCodeUnitIndex--;
	if (mData[mCodeUnitIndex] & 128)
	{
		mCodeUnitIndex--;
		if ((mData[mCodeUnitIndex] & 64) == 0)
		{
			mCodeUnitIndex--;
			if ((mData[mCodeUnitIndex] & 64) == 0)
				mCodeUnitIndex--;
		}
	}

	return StringCharacter((const char*)&mData[mCodeUnitIndex], mCodeUnitIndex);
}


// get the current character
StringCharacter StringIterator::GetCurrentCharacter() const
{
	return StringCharacter( (const char*)&mData[mCodeUnitIndex], mCodeUnitIndex );
}


// get a given character
StringCharacter StringIterator::GetCharacter(uint32 characterIndex) const
{
	uint32 numCharacters = 0;

	uint32 offset = 0;
	while (offset < mNumCodeUnits)
	{
		if (numCharacters == characterIndex)
			return StringCharacter( (const char*)&mData[offset], offset );

		uint32 extraBytesToRead = StringIterator::trailingBytesForUTF8[ (unsigned char)mData[offset] ];
		offset += extraBytesToRead + 1;
		numCharacters++;
	}

	return StringCharacter();
}


// find the code index for a given character number
uint32 StringIterator::FindCharacterCodeUnitIndex(uint32 characterIndex) const
{
	uint32 numCharacters = 0;
	uint32 offset = 0;
	while (offset < mNumCodeUnits)
	{
		if (numCharacters == characterIndex)
			return offset;

		uint32 extraBytesToRead = StringIterator::trailingBytesForUTF8[ (unsigned char)mData[offset] ];
		offset += extraBytesToRead + 1;
		numCharacters++;
	}

	return CORE_INVALIDINDEX32;
}

/*
// calculate the number of required UTF8 code units to represent this string (includes null terminator)
uint32 StringIterator::CalcNumRequiredUTF8CodeUnits()
{
	Rewind();

	uint32 numBytes = 0;
	while (HasReachedEnd() == false)
		numBytes += GetNextCharacter().CalcNumRequiredUTF8Bytes();		

	numBytes += 1;	// add the null terminator

	return numBytes;
}
*/
//-----------------------------------

// convert a buffer from uint16 to uint32
bool StringIterator::ConvertUTF16toUTF32(const uint16** sourceStart, const uint16* sourceEnd, uint32** targetStart, uint32* targetEnd, bool strictConversion)
{
	bool result = true;

	const uint16* source = *sourceStart;
	uint32* target = *targetStart;
	uint32 ch	= CORE_INVALIDINDEX32;
	uint32 ch2	= CORE_INVALIDINDEX32;
	while (source < sourceEnd)
	{
		const uint16* oldSource = source; //  in case we have to back up because of target overflow
		ch = *source++;

		// if we have a surrogate pair, convert to UTF32 first
		if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURHIGH_END)
		{
			// if the 16 bits following the high surrogate are in the source buffer...
			if (source < sourceEnd)
			{
				ch2 = *source;

				// if it's a low surrogate, convert to UTF32
				if (ch2 >= StringIterator::UNICODE_SURLOW_START && ch2 <= StringIterator::UNICODE_SURLOW_END)
				{
					ch = ((ch - StringIterator::UNICODE_SURHIGH_START) << StringIterator::UNICODE_HALFSHIFT) + (ch2 - StringIterator::UNICODE_SURLOW_START) + StringIterator::UNICODE_HALFBASE;
					++source;
				}
				else 
				if (strictConversion == true)
				{
					// it's an unpaired high surrogate
					--source; // return to the illegal value itself
					result = false;	// source illegal
					Core::LogWarning("String::ConvertUTF16ToUTF32() - Source is illegal, sequence 0x%04x,%04x\n", ch, ch2);
					break;
				}
			} 
			else
			{	
				// we don't have the 16 bits following the high surrogate.
				--source; // return to the high surrogate
				result = false;	// source exhausted
				Core::LogWarning("String::ConvertUTF16ToUTF32() - Source exhausted");
				break;
			}
		} 
		else
		if (strictConversion == true)
		{
			// UTF-16 surrogate values are illegal in UTF-32
			if (ch >= StringIterator::UNICODE_SURLOW_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				--source; // return to the illegal value itself
				result = false;	// source illegal
				Core::LogWarning("String::ConvertUTF16ToUTF32() - Source is illegal, sequence 0x%04x,%04x", ch, ch2);
				break;
			}
		}

		if (target >= targetEnd)
		{
			source = oldSource;		// back up source pointer!
			result = false;
			Core::LogWarning("String::ConvertUTF16ToUTF32() - Target exhausted");
			break;
		}
		*target++ = ch;
	}

	*sourceStart = source;
	*targetStart = target;
	return result;
}


// convert from UTF32 to UTF16
bool StringIterator::ConvertUTF32toUTF16(const uint32** sourceStart, const uint32* sourceEnd, uint16** targetStart, uint16* targetEnd, bool strictConversion)
{
    bool result = true;
    const uint32* source = *sourceStart;
    uint16* target = *targetStart;
    while (source < sourceEnd)
	{
		uint32 ch;
		if (target >= targetEnd)
		{
			result = false;	// target exhausted
			Core::LogWarning("String::ConvertUTF32ToUTF16() - Target exhausted");
			break;
		}

		ch = *source++;
		if (ch <= StringIterator::UNICODE_MAX_BMP)
		{
			// target is a character <= 0xFFFF
			// UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values
			if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				if (strictConversion == true)
				{
					--source; // return to the illegal value itself
					result = false;
					Core::LogWarning("String::ConvertUTF32ToUTF16() - Source is illegal");
					break;
				} 
				else 
					*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
			} 
			else
				*target++ = (uint16)ch; // normal case
		} 
		else 
		if (ch > StringIterator::UNICODE_MAX_LEGAL_UTF32)
		{
			if (strictConversion == true)
			{
				result = false;
				Core::LogWarning("String::ConvertUTF32ToUTF16() - Source is illegal");
			} 
			else 
				*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
		} 
		else
		{
			// target is a character in range 0xFFFF - 0x10FFFF
			if (target + 1 >= targetEnd)
			{
				--source; // backup source pointer
				result = false;	// exhausted
				Core::LogWarning("String::ConvertUTF32ToUTF16() - Target exhausted");
				break;
			}
			ch -= StringIterator::UNICODE_HALFBASE;
			*target++ = (uint16)((ch >> StringIterator::UNICODE_HALFSHIFT) + StringIterator::UNICODE_SURHIGH_START);
			*target++ = (uint16)((ch & StringIterator::UNICODE_HALFMASK) + StringIterator::UNICODE_SURLOW_START);
		}
    }

    *sourceStart = source;
    *targetStart = target;
    return result;
}


// convert UTF16 to UTF8
bool StringIterator::ConvertUTF16toUTF8(const uint16** sourceStart, const uint16* sourceEnd, uint8** targetStart, uint8* targetEnd, bool strictConversion)
{
    bool result = true;
    const uint16* source = *sourceStart;
    uint8* target = *targetStart;
    while (source < sourceEnd)
	{
		uint32 ch;
		uint16 bytesToWrite = 0;
		const uint32 byteMask = 0xBF;
		const uint32 byteMark = 0x80; 
		const uint16* oldSource = source;	// in case we have to back up because of target overflow
		ch = *source++;

		// if we have a surrogate pair, convert to UTF32 first
		if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURHIGH_END)
		{
			// if the 16 bits following the high surrogate are in the source buffer...
			if (source < sourceEnd)
			{
				uint32 ch2 = *source;

				// if it's a low surrogate, convert to UTF32
				if (ch2 >= StringIterator::UNICODE_SURLOW_START && ch2 <= StringIterator::UNICODE_SURLOW_END)
				{
					ch = ((ch - StringIterator::UNICODE_SURHIGH_START) << StringIterator::UNICODE_HALFSHIFT) + (ch2 - StringIterator::UNICODE_SURLOW_START) + StringIterator::UNICODE_HALFBASE;
					++source;
				}
				else
				if (strictConversion == true)
				{
					// it's an unpaired high surrogate
					--source; // return to the illegal value itself
					result = false;	// source illegal
					Core::LogWarning("String::ConvertUTF16ToUTF8() - Source illegal!");
					break;
				}
			}
			else
			{
				// we don't have the 16 bits following the high surrogate
				--source; // return to the high surrogate
				result = false;	// exhausted
				Core::LogWarning("String::ConvertUTF16ToUTF8() - Source exhausted!");
				break;
			}
		} 
		else
		if (strictConversion == true)
		{
			// UTF-16 surrogate values are illegal in UTF-32
			if (ch >= StringIterator::UNICODE_SURLOW_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				--source; // return to the illegal value itself
				result = false;	// source illegal
				Core::LogWarning("String::ConvertUTF16ToUTF8() - Source illegal!");
				break;
			}
		}
	
		// figure out how many bytes the result will require
		if (ch < (uint32)0x80)			bytesToWrite = 1;
		else if (ch < (uint32)0x800)	bytesToWrite = 2;
		else if (ch < (uint32)0x10000)	bytesToWrite = 3;
		else if (ch < (uint32)0x110000)	bytesToWrite = 4;
		else { bytesToWrite = 3; ch = StringIterator::UNICODE_REPLACEMENT_CHAR; }

		target += bytesToWrite;
		if (target > targetEnd)
		{
			source = oldSource; // back up source pointer
			target -= bytesToWrite; 
			result = false;	// target exhausted
			Core::LogWarning("String::ConvertUTF16ToUTF8() - Target exhausted!");
			break;
		}

		switch (bytesToWrite)
		{
			case 4: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 3: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 2: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 1: *--target = (uint8) (ch | StringIterator::firstByteMark[bytesToWrite]);
			default:;
		}

		target += bytesToWrite;
    }

    *sourceStart = source;
    *targetStart = target;
    return result;
}


// convert UTF8 to UTF16
bool StringIterator::ConvertUTF8toUTF16(const uint8** sourceStart, const uint8* sourceEnd, uint16** targetStart, uint16* targetEnd, bool strictConversion)
{
    bool result = true;
    const uint8* source = *sourceStart;
    uint16* target = *targetStart;
    while (source < sourceEnd)
	{
		uint32 ch = 0;
		uint16 extraBytesToRead = StringIterator::trailingBytesForUTF8[*source];
		if (source + extraBytesToRead >= sourceEnd)
		{
			result = false;	// source exhausted
			break;
		}
	
		// do this check whether lenient or strict
		if (strictConversion == true)
		{
			if (!StringIterator::IsLegalUTF8(source, extraBytesToRead+1))
			{
				result = false;	// source illegal
				break;
			}
		}

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

		ch -= StringIterator::offsetsFromUTF8[extraBytesToRead];

		if (target >= targetEnd)
		{
			source -= (extraBytesToRead+1); // backup source pointer
			result = false;	// target exhausted
			break;
		}

		if (ch <= StringIterator::UNICODE_MAX_BMP) 
		{
			// target is a character <= 0xFFFF
		    // UTF-16 surrogate values are illegal in UTF-32
		    if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				if (strictConversion == true)
				{
					source -= (extraBytesToRead+1); // return to the illegal value itself
					result = false;	// source illegal
					break;
				} 
				else
					*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
			} 
			else
				*target++ = (uint16)ch; // normal case
		} 
		else
		if (ch > StringIterator::UNICODE_MAX_UTF16)
		{
		    if (strictConversion == true)
			{
				result = false;	// source illegal
				source -= (extraBytesToRead+1); // return to the start
				break; // bail out, we shouldn't continue
			} 
			else
				*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
		} 
		else
		{
			// target is a character in range 0xFFFF - 0x10FFFF
			if (target + 1 >= targetEnd)
			{
				source -= (extraBytesToRead+1); // backup source pointer!
				result = false;	// target exhausted
				break;
			}

			ch -= StringIterator::UNICODE_HALFBASE;

			*target++ = (uint16)((ch >> StringIterator::UNICODE_HALFSHIFT) + StringIterator::UNICODE_SURHIGH_START);
			*target++ = (uint16)((ch & StringIterator::UNICODE_HALFMASK) + StringIterator::UNICODE_SURLOW_START);
		}
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}


// convert UTF32 to UTF8
bool StringIterator::ConvertUTF32toUTF8(const uint32** sourceStart, const uint32* sourceEnd, uint8** targetStart, uint8* targetEnd, bool strictConversion)
{
    bool result = true;
    const uint32* source = *sourceStart;
    uint8* target = *targetStart;
    while (source < sourceEnd)
	{
		uint32 ch;
		uint16 bytesToWrite = 0;
		const uint32 byteMask = 0xBF;
		const uint32 byteMark = 0x80; 
		ch = *source++;
		if (strictConversion == true)
		{
			// UTF-16 surrogate values are illegal in UTF-32
			if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				--source; // return to the illegal value itself
				result = false;	// source illegal
				break;
			}
		}
		
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
			result = false;	// source illegal
		}
	
		target += bytesToWrite;
		if (target > targetEnd)
		{
			--source; // backup source pointer
			target -= bytesToWrite;
			result = false;	// target exhausted
			break;
		}

		switch (bytesToWrite)
		{
			case 4: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 3: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 2: *--target = (uint8)((ch | byteMark) & byteMask); ch >>= 6;
			case 1: *--target = (uint8) (ch | StringIterator::firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
    }
    
	*sourceStart = source;
    *targetStart = target;
    return result;
}


// convert UTF8 to UTF32
bool StringIterator::ConvertUTF8toUTF32(const uint8** sourceStart, const uint8* sourceEnd, uint32** targetStart, uint32* targetEnd, bool strictConversion)
{
    bool result = true;
    const uint8* source = *sourceStart;
    uint32* target = *targetStart;
    while (source < sourceEnd)
	{
		uint32 ch = 0;
		uint16 extraBytesToRead = StringIterator::trailingBytesForUTF8[*source];
		if (source + extraBytesToRead >= sourceEnd)
		{
			result = false;	// source exhausted
			break;
		}

		// verify if it is legal UTF8
		if (strictConversion == true)
		{
			if (!StringIterator::IsLegalUTF8(source, extraBytesToRead+1))
			{
			    result = false;	// source illegal
				break;
			}
		}

		// the cases all fall through
		switch (extraBytesToRead)
		{
			case 5: ch += *source++; ch <<= 6;
			case 4: ch += *source++; ch <<= 6;
			case 3: ch += *source++; ch <<= 6;
			case 2: ch += *source++; ch <<= 6;
			case 1: ch += *source++; ch <<= 6;
			case 0: ch += *source++;
			default:;
		}

		ch -= StringIterator::offsetsFromUTF8[extraBytesToRead];

		if (target >= targetEnd)
		{
		    source -= (extraBytesToRead+1); // backup the source pointer
			result = false;	// target exhausted
			break;
		}

		if (ch <= StringIterator::UNICODE_MAX_LEGAL_UTF32)
		{
			// UTF-16 surrogate values are illegal in UTF-32, and anything over Plane 17 (> 0x10FFFF) is illegal
			if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				if (strictConversion == true)
				{
					source -= (extraBytesToRead+1); // return to the illegal value itself
					result = false;	// source illegal
					break;
				} 
				else
					*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
			} 
			else
				*target++ = ch;
		} 
		else 
		{
			// for example (ch > UNI_MAX_LEGAL_UTF32)
			result = false;	// source illegal
			*target++ = StringIterator::UNICODE_REPLACEMENT_CHAR;
		}
    }

    *sourceStart = source;
    *targetStart = target;
    return result;
}

/*
// calculate how many code units are needed for this UTF8 string
// does NOT include the null terminator
uint32 StringIterator::CalcNumRequiredCodeUnitsFromUTF8(const uint8* input, bool validate) const
{
    uint32 result = 0;

	const uint8* source = input;
    for (;;)
	{
		uint32 ch = 0;
		uint16 extraBytesToRead = StringIterator::trailingBytesForUTF8[*source];
		if (*source == 0)
			break;

		// verify if it is legal UTF8
		#ifdef CORE_DEBUG
			if (validate == true && !StringIterator::IsLegalUTF8(source, extraBytesToRead+1))
			{
				Core::LogWarning("StringIterator::CalcNumRequiredCodeUnitsFromUTF8() - Input string is not a valid UTF8 string!");
				CORE_ASSERT(1==0);			
			}
		#endif

		// the cases all fall through
		switch (extraBytesToRead)
		{
			case 5: ch += *source++; ch <<= 6;
			case 4: ch += *source++; ch <<= 6;
			case 3: ch += *source++; ch <<= 6;
			case 2: ch += *source++; ch <<= 6;
			case 1: ch += *source++; ch <<= 6;
			case 0: ch += *source++;
			default:;
		}

		ch -= StringIterator::offsetsFromUTF8[extraBytesToRead];

		if (ch <= StringIterator::UNICODE_MAX_LEGAL_UTF32)
		{
			// UTF-16 surrogate values are illegal in UTF-32, and anything over Plane 17 (> 0x10FFFF) is illegal
			if (ch >= StringIterator::UNICODE_SURHIGH_START && ch <= StringIterator::UNICODE_SURLOW_END)
			{
				Core::LogWarning("StringIterator::CalcNumRequiredCodeUnitsFromUTF8() - Input is invalid!");
				source -= (extraBytesToRead+1); // return to the illegal value itself
				CORE_ASSERT(1==0);
			}
			else
			{
				#ifdef CORE_UTF32
					result++;
				#else
					if (ch <= StringIterator::UNICODE_MAX_BMP)
						result++;
					else
						result += 2;
				#endif
			}
		} 
		else 
		{
			if (validate == true)
			{
				Core::LogWarning("StringIterator::CalcNumRequiredCodeUnitsFromUTF8() - Input is invalid!");
				CORE_ASSERT(1==0);
			}
		}
    }

    return result;
}
*/
}	// namespace Core
