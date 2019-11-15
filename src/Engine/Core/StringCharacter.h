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

#ifndef __CORE_STRINGCHARACTER_H
#define __CORE_STRINGCHARACTER_H

// include the required headers
#include "StandardHeaders.h"

namespace Core
{

/**
 * A Unicode character.
 * This class internally represents a character in UTF32 format and allows conversion from and to UTF8 and UTF16.
 */
class ENGINE_API StringCharacter
{
	public:
		inline StringCharacter()																			{ mCodeUnit=0; mIndex=CORE_INVALIDINDEX32; }
		inline explicit StringCharacter(char character, uint32 index=CORE_INVALIDINDEX32)					{ InitFromUTF8((char*)&character, index); }
		inline explicit StringCharacter(uint16 firstUTF16, uint16 secondUTF16=0, uint32 index=CORE_INVALIDINDEX32)	{ InitFromUTF16(firstUTF16, secondUTF16, index); }
		inline explicit StringCharacter(const char* utf8Data, uint32 index=CORE_INVALIDINDEX32)				{ InitFromUTF8( utf8Data, index );}
		inline explicit StringCharacter(uint32 utf32Value, uint32 index=CORE_INVALIDINDEX32)				{ InitFromUTF32( utf32Value, index ); }
		inline StringCharacter(const StringCharacter& other)												{ mCodeUnit=other.mCodeUnit; mIndex=other.mIndex; }
		inline ~StringCharacter() {}

		inline void SetIndex(uint32 index)							{ mIndex = index; }
		inline uint32 GetIndex() const								{ return mIndex; }

		bool IsValid() const																				{ if (mIndex == CORE_INVALIDINDEX32) return false; return true; }

		uint32 CalcNumRequiredUTF8Bytes() const;

		bool InitFromUTF8(const char* utf8Data, uint32 index=CORE_INVALIDINDEX32);
		void InitFromUTF16(uint16 first, uint16 second=0, uint32 index=CORE_INVALIDINDEX32);
		inline void InitFromUTF32(uint32 utf32Value, uint32 index=CORE_INVALIDINDEX32)						{ mCodeUnit = utf32Value; mIndex = index; }

		char* AsUTF8(char* output, uint32* outNumBytes, bool addNullTerminator=false) const;	// outNumBytes does NOT include the null terminator
		uint16* AsUTF16(uint16* output) const;
		inline uint32 AsUTF32() const									{ return mCodeUnit; }

		inline static StringCharacter FromUTF16(uint16 first, uint16 second=0, uint32 index=CORE_INVALIDINDEX32)		{ return StringCharacter(first, second, index); }
		inline static StringCharacter FromUTF32(uint32 utf32Value, uint32 index=CORE_INVALIDINDEX32)					{ return StringCharacter((uint32)utf32Value, index); }
		inline static StringCharacter FromUTF8(const char* characterData, uint32 index=CORE_INVALIDINDEX32)				{ return StringCharacter(characterData, index); }

		inline bool operator==(const StringCharacter& other)			{ return (mCodeUnit == other.mCodeUnit); }
		inline bool operator!=(const StringCharacter& other)			{ return (mCodeUnit != other.mCodeUnit); }
		inline bool operator< (const StringCharacter& other)			{ return (mCodeUnit <  other.mCodeUnit); }
		inline bool operator> (const StringCharacter& other)			{ return (mCodeUnit >  other.mCodeUnit); }
		inline bool operator<=(const StringCharacter& other)			{ return (mCodeUnit <= other.mCodeUnit); }
		inline bool operator>=(const StringCharacter& other)			{ return (mCodeUnit >= other.mCodeUnit); }

		static const StringCharacter space;
		static const StringCharacter tab;
		static const StringCharacter endLine;
		static const StringCharacter comma;
		static const StringCharacter dot;
		static const StringCharacter backSlash;
		static const StringCharacter forwardSlash;
		static const StringCharacter semiColon;
		static const StringCharacter colon;
		static const StringCharacter doubleQuote;
		static const StringCharacter dash;
		static const StringCharacter asterisk;

	private:
		uint32	mCodeUnit;	/**< UTF32 code unit. */
		uint32	mIndex;		/**< The start index inside the string buffer. */
};

inline bool operator==(const StringCharacter& charA, const StringCharacter& charB)	{ return (charA.AsUTF32() == charB.AsUTF32()); }
inline bool operator!=(const StringCharacter& charA, const StringCharacter& charB)	{ return (charA.AsUTF32() != charB.AsUTF32()); }

}	// namespace Core

#endif
