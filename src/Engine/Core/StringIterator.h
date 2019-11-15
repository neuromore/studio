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

#ifndef __CORE_STRINGITERATOR_H
#define __CORE_STRINGITERATOR_H

// include the required headers
#include "StandardHeaders.h"
#include "StringCharacter.h"
#include "String.h"

namespace Core
{


class StringIterator
{
	friend class String;
	friend class StringCharacter;
	public:
		enum
		{
			UNICODE_SURHIGH_START		= (uint32)0xD800,
			UNICODE_SURHIGH_END			= (uint32)0xDBFF,
			UNICODE_SURLOW_START		= (uint32)0xDC00,
			UNICODE_SURLOW_END			= (uint32)0xDFFF,
			UNICODE_REPLACEMENT_CHAR	= (uint32)0x0000FFFD,
			UNICODE_MAX_BMP				= (uint32)0x0000FFFF,
			UNICODE_MAX_UTF16			= (uint32)0x0010FFFF,
			UNICODE_MAX_UTF32			= (uint32)0x7FFFFFFF,
			UNICODE_MAX_LEGAL_UTF32		= (uint32)0x0010FFFF,
			UNICODE_HALFSHIFT			= 10,
			UNICODE_HALFBASE			= 0x0010000UL,
			UNICODE_HALFMASK			= 0x3FFUL
		};

		inline StringIterator(const String& str);
		inline StringIterator(char* data, uint32 numCodeUnits);
		inline StringIterator(char* data);
		inline ~StringIterator();

		inline void Init(const String& str);
		inline void Init(char* data, uint32 numCodeUnits);

		inline uint32 GetIndex() const;
		inline uint32 GetLength() const;
		inline bool HasReachedEnd() const;

		StringCharacter GetNextCharacter();
		void GetNextCharacter(StringCharacter* outCharacter);

		StringCharacter GetPreviousCharacter();
		StringCharacter GetCurrentCharacter() const;
		StringCharacter GetCharacter(uint32 characterIndex) const;

		uint32 FindCharacterCodeUnitIndex(uint32 characterIndex) const;
		uint32 CalcNumCharacters() const;

		void ReverseToPreviousCharacter();
		void ForwardToNextCharacter();
		void SetIndexToEnd();
		void Rewind();
		void SetIndexToCharacter(uint32 characterIndex);
		void SetIndex(uint32 codeUnitIndex);

	private:
		char*		mData;
		uint32		mCodeUnitIndex;
		uint32		mNumCodeUnits;

		static uint32 CalcNumCharacters(const char* utf8String, uint32 numBytes);
		static uint32 CalcNumCharacters(const char* utf8String);

		static const char trailingBytesForUTF8[256];
		static const uint32 offsetsFromUTF8[6];
		static const uint8 firstByteMark[7];

		static bool IsLegalUTF8Sequence(const uint8* source, const uint8* sourceEnd);
		static bool IsLegalUTF8(const uint8* source, uint32 length);
		static bool ConvertUTF16toUTF32(const uint16** sourceStart, const uint16* sourceEnd, uint32** targetStart, uint32* targetEnd, bool strictConversion);
		static bool ConvertUTF32toUTF16(const uint32** sourceStart, const uint32* sourceEnd, uint16** targetStart, uint16* targetEnd, bool strictConversion);
		static bool ConvertUTF16toUTF8(const uint16** sourceStart, const uint16* sourceEnd, uint8** targetStart, uint8* targetEnd, bool strictConversion);
		static bool ConvertUTF8toUTF16 (const uint8** sourceStart, const uint8* sourceEnd, uint16** targetStart, uint16* targetEnd, bool strictConversion);
		static bool ConvertUTF32toUTF8(const uint32** sourceStart, const uint32* sourceEnd, uint8** targetStart, uint8* targetEnd, bool strictConversion);
		static bool ConvertUTF8toUTF32(const uint8** sourceStart, const uint8* sourceEnd, uint32** targetStart, uint32* targetEnd, bool strictConversion);
};


// add the inline code
#include "StringIterator.inl"

}	// namespace Core

#endif
