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

#ifndef __CORE_STRING_H
#define __CORE_STRING_H

// include required headers
#include "StandardHeaders.h"
#include "Array.h"
#include "Vector.h"
#include "StringCharacter.h"

namespace Core
{

class ENGINE_API String
{
	public:
		enum { MEMORYBLOCK_ID = 1 };

		// constructors
		String();
		String(const char* other);
		String(char c);

		// copy and move constructor
		String(const String& other);
		String(String&& other);

		// destructor
		~String();


		String(const char* sA, uint32 lengthA, const char* sB, uint32 lengthB);
		explicit String(int32 value)												: mData(NULL), mLength(0), mMaxLength(0)	{ FromInt(value); }
		explicit String(uint32 value)												: mData(NULL), mLength(0), mMaxLength(0)	{ FromInt(value); }
		explicit String(float value)												: mData(NULL), mLength(0), mMaxLength(0)	{ FromFloat(value); }
		explicit String(double value)												: mData(NULL), mLength(0), mMaxLength(0)	{ FromDouble(value); }
		

		inline char* AsChar() const												{ return (mData!=NULL) ? mData : (char*)""; }
		inline uint32 GetLength() const											{ return mLength; }
		inline uint32 GetMaxLength() const										{ return mMaxLength; }
		inline bool IsEmpty() const												{ return (mLength == 0); }
		inline void Clear(bool keepMemory=true)									{ if (keepMemory) { mLength=0; if (mData) mData[0]='\0'; } else { if (mData) Core::Free(mData); mData=NULL; mLength=0; mMaxLength=0; } }

		// static version
		static uint32 CalcLength(const char* text)								{ return (uint32)strlen(text); }

		// conversion string -> other
		void FromInt(int32 value);
		void FromFloat(float value);
		void FromDouble(double value);
		void FromBool(bool value);

		// conversion other -> string
		int32 ToInt() const;
		float ToFloat() const;
		double ToDouble() const;
		bool ToBool() const;

		bool IsValidInt() const;
		bool IsValidFloat() const;
		bool IsValidBool() const;

		// DEPRECATED vector operations
		void FromVector4(const Vector4& value);
		Vector4 ToVector4() const;
		bool IsValidVector4() const;

		// Formatting
		String& Format(const char* text, ...);
		String& FormatAdd(const char* text, ...);

		// Split
		Array<String> Split(const StringCharacter& splitChar=StringCharacter::semiColon) const;
		uint32 SplitFast(Array<const char*>& positions, Array<uint32>& lengths, const StringCharacter& splitChar = StringCharacter::semiColon) const;

		// Join
		static String& Join(const Array<String>& array, String separator,  String& result);

		// Comparators
		bool IsEqual(const char* other) const;
		bool IsEqualNoCase(const char* other) const;
		int32 Compare(const char* other) const;
		int32 CompareNoCase(const char* other) const;

		// Count Chars
		uint32 CountNumChars(const StringCharacter& character) const;

		// Find Substrings
		uint32 Find(const char* subString) const;
		uint32 FindRight(const StringCharacter& character) const;

		inline bool Contains(const char* subString) const						{ return (Find(subString) != CORE_INVALIDINDEX32); }

		bool ContainsVisibleCharacter() const;

		// Word operations
		uint32 CalcNumWords() const;
		String ExtractWord(uint32 wordNr) const;

		// Removing Substrings
		bool RemoveFirstPart(const char* part);
		bool RemoveAllParts(const char* part);
		bool RemoveChars(const Array<StringCharacter>& charSet);
		bool RemoveChars(const char* characterSet);

		// File Name and Path stuff
		void RemoveFileExtension();
		String ExtractFilename() const;
		String ExtractPath(bool includeSlashAtEnd=true) const;
		String ExtractFileExtension() const;
		void ConvertToNativePath();
		String ConvertedToNativePath() const;

		// Trim operations
		void RemoveLastCharacterIfEqualTo(const StringCharacter& lastCharacter);
		void RemoveFirstCharacterIfEqualTo(const StringCharacter& firstCharacter);
		void TrimLeft(const StringCharacter& trimChar=StringCharacter::space);
		void TrimRight(const StringCharacter& trimChar=StringCharacter::space);
		void Trim(const StringCharacter& trimChar=StringCharacter::space);

		// misc
		StringCharacter GetLast() const;
		StringCharacter GetFirst() const;

		// replace chars and strings
		void Replace(const char* what, const char* with);
		void Replace(const StringCharacter& c, const StringCharacter& with);

		// Index based operations
		void Remove(uint32 pos, uint32 length);
		void Crop(uint32 pos, uint32 length);
	
		// Memory Operations
		void Resize(uint32 length, char fillChar=' ', bool doFill=true);
		void Reserve(uint32 length);
		String& Align(uint32 newNumCharacters, const StringCharacter& character=StringCharacter::space);
		String& Copy(const char* what, uint32 length);

		// Case Conversion
		String Uppered() const;
		String Lowered() const;
		String& ToUpper();
		String& ToLower();

		// Cast to const char*
		inline operator const char*() const									{ return (mData!=NULL) ? mData : (const char*)""; }

		// Copy and Move Assignment Operator
		inline const String&	operator=(const String& other)				{ if (&other == this) return *this; return Copy(other.AsChar(), other.GetLength()); }
		inline		 String&	operator=(String&& other)					{ if (&other == this) return *this; if (mData) Core::Free(mData); mData = other.mData; mLength = other.mLength; mMaxLength = other.mMaxLength; other.mData = NULL; other.mLength = 0; other.mMaxLength = 0; return *this; }

		// Other Assignment Operator
		inline const String&	operator=(char c)							{ return Copy(&c, 1); }
		inline const String&	operator=(const char* str)					{ if (str!=NULL) return Copy(str, (uint32)strlen(str)); else Clear(); return *this; }

		// Append Operators
		inline String	operator+(const char* str)							{ if (str==NULL) return *this; String s(*this); return s.Concat(str, (uint32)strlen(str)); }
		inline String	operator+(const String& str)						{ String s(*this); return s.Concat(str.AsChar(), str.GetLength()); }
		inline String	operator+(int32 val)								{ String s(*this); String str(val); return s.Concat(str.AsChar(), str.GetLength()); }
		inline String	operator+(float val)								{ String s(*this); String str(val); return s.Concat(str.AsChar(), str.GetLength()); }
		inline String	operator+(double val)								{ String s(*this); String str(val); return s.Concat(str.AsChar(), str.GetLength()); }
		inline String	operator+(char c)									{ String s(*this); return s.Concat(&c, 1); }

		inline const String& operator+=(const char* str)					{ if (str==NULL) return *this; return Concat(str, (uint32)strlen(str)); }
		inline const String& operator+=(const String& str)					{ return Concat(str.AsChar(), str.GetLength()); }
		inline const String& operator+=(char c)								{ return Concat(&c, 1); }
		inline const String& operator+=(const StringCharacter& c)			{ uint32 numBytes; char buffer[8]; c.AsUTF8(buffer, &numBytes, false); return Concat(buffer, numBytes); }
		inline const String& operator+=(int32 val)							{ String str(val); return Concat(str.AsChar(), str.GetLength()); }
		inline const String& operator+=(float val)							{ String str(val); return Concat(str.AsChar(), str.GetLength()); }
		inline const String& operator+=(double val)							{ String str(val); return Concat(str.AsChar(), str.GetLength()); }

		// String Comparison Operators
		inline bool	operator< (const String&	str)						{ return (SafeCompare(mData, str.AsChar())		<  0); }
		inline bool	operator< (const char*		str)						{ return (SafeCompare(mData, str)				<  0); }
		inline bool	operator> (const String&	str)						{ return (SafeCompare(mData, str.AsChar())		>  0); }
		inline bool	operator> (const char*		str)						{ return (SafeCompare(mData, str)				>  0); }
		inline bool	operator<=(const String&	str)						{ return (SafeCompare(mData, str.AsChar())		<= 0); }
		inline bool	operator<=(const char*		str)						{ return (SafeCompare(mData, str)				<= 0); }
		inline bool	operator>=(const String&	str)						{ return (SafeCompare(mData, str.AsChar())		>= 0); }
		inline bool	operator>=(const char*		str)						{ return (SafeCompare(mData, str)				>= 0); }
		inline bool	operator==(const String&	str)						{ return (SafeCompare(mData, str.AsChar())		== 0); }
		inline bool	operator==(const char*		str)						{ return (SafeCompare(mData, str)				== 0); }
		inline bool	operator!=(const String&	str)						{ return (SafeCompare(mData, str.AsChar())		!= 0); }
		inline bool	operator!=(const char*		str)						{ return (SafeCompare(mData, str)				!= 0); }

		inline static int32 SafeCompare(const char* strA, const char* strB)
		{
			// if both strings exist
			if (strA != NULL && strB != NULL)
				return strcmp(strA, strB);

			// if one of them is NULL
			if (strA == NULL)
			{
				if (strB != NULL)
				{
					if (strB[0] == 0)
						return 0;
					else
						return 1; 
				}
				else // both strA and strB are NULL
					return 0;
			}
			else	// strA not NULL, strB must be NULL
			{
				if (strA[0] == 0)
					return 0;
				else
					return -1;
			}
		}


	private:
		char*		mData;
		uint32		mLength;		// num code units
		uint32		mMaxLength;

		String& Concat(const char* what, uint32 length);			// append two strings
		void Alloc(uint32 numCodeUnits, uint32 extraUnits=0);
};

// external operators
inline String operator+ (const String&	strA, const String&	strB)			{ return String(strA.AsChar(), strA.GetLength(), strB.AsChar(), strB.GetLength()); }
inline String operator+ (const String&	strA, char			strB)			{ return String(strA.AsChar(), strA.GetLength(), &strB, 1); }
inline String operator+ (const char		strA, const String&	strB)			{ return String(&strA, 1, strB.AsChar(), strB.GetLength()); }
inline String operator+ (const String&	strA, const char*	strB)			{ if (strB != NULL) return String(strA.AsChar(), strA.GetLength(), strB, (uint32)strlen(strB)); else return strA; }
inline String operator+ (const char*	strA, const String&	strB)			{ if (strA != NULL) return String(strA, (uint32)strlen(strA), strB.AsChar(), strB.GetLength()); else return strB; }

inline bool	operator< (const String&	strA, const String&	strB)			{ return (String::SafeCompare(strA.AsChar(),strB.AsChar())	<0); }
inline bool	operator< (const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			<0); }
inline bool	operator< (const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	<0); }
inline bool	operator> (const String&	strA, const String&	strB)			{ return (String::SafeCompare(strA.AsChar(),strB.AsChar())	>0); }
inline bool	operator> (const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			>0); }
inline bool	operator> (const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	>0); }
inline bool	operator<=(const String&	strA, const String&	strB)			{ return (String::SafeCompare(strA.AsChar(),strB.AsChar())	<=0); }
inline bool	operator<=(const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			<=0); }
inline bool	operator<=(const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	<=0); }
inline bool	operator>=(const String&	strA, const String&	strB)			{ return (String::SafeCompare(strA.AsChar(),strB.AsChar())	>=0); }
inline bool	operator>=(const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			>=0); }
inline bool	operator>=(const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	>=0); }
inline bool	operator==(const String&	strA, const String&	strB)			{ return (String::SafeCompare(strA.AsChar(),strB.AsChar())	==0); }
inline bool	operator==(const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			==0); }
inline bool	operator==(const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	==0); }
inline bool	operator!=(const String&	strA, const String&	strB)			{ return ((strA.GetLength() != strB.GetLength()) || (String::SafeCompare(strA.AsChar(), strB.AsChar())!=0)); }
inline bool	operator!=(const String&	strA, const char*	strB)			{ return (String::SafeCompare(strA.AsChar(),strB)			!=0); }
inline bool	operator!=(const char*		strA, const String&	strB)			{ return (String::SafeCompare(strA,			strB.AsChar())	!=0); }

// some shortcut
typedef String String;		/**< The string type, which is a String. */


struct StringHasher
{
    size_t operator()(const String& key) const
	{
		uint32 result = 0;
		const uint32 length = key.GetLength();
		for (uint32 i=0; i<length; ++i)
		{
			result = (result << 4) + key[i];
			const uint32 g = result & 0xf0000000L;
			if (g != 0)
				result ^= g >> 24;
			result &= ~g;
		}

		return result;
    }
};

} // namespace Core

#endif
