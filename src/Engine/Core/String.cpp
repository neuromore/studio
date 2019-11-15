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

// include required headers
#include "String.h"
#include "LogManager.h"
#include "Array.h"
#include "Math.h"
#include "LogManager.h"
#include "StringIterator.h"


namespace Core
{

// constructor
String::String()
{
	mData		= NULL;
	mLength		= 0;
	mMaxLength	= 0;
}


// constructor
String::String(const char* other)
{
	mData		= NULL;
	mLength		= 0;
	mMaxLength	= 0;

	if (other == NULL)
		return;
	
	Alloc( (uint32)strlen(other) );
	
	if (mData != NULL)
		MemCopy( mData, other, mLength*sizeof(char) );
}

// constructor
String::String(char c)
{
	mData		= NULL;
	mLength		= 0;
	mMaxLength	= 0;

	Alloc(1);
	
	if (mData != NULL)
		mData[0] = c;
}


// copy constructor
String::String(const String& other)
{
	mData		= NULL;
	mLength		= 0;
	mMaxLength	= 0;

	if (other.mData == NULL)
		return;
	
	Alloc(other.GetLength());
	
	if (mData != NULL)
		Core::MemCopy( mData, other.mData, (other.GetLength()+1)*sizeof(char) );
}


// move constructor
String::String(String&& other)
{
	// move data to this object
	mData		= other.mData;
	mLength		= other.mLength;
	mMaxLength	= other.mMaxLength;

	// release data from source object
	other.mData = NULL;
	other.mLength = 0;
	other.mMaxLength = 0;
}


// destructor
String::~String()
{ 
	if (mData != NULL)
		Core::Free( mData ); 
}

// construct this string from two strings
String::String(const char* sA, uint32 lengthA, const char* sB, uint32 lengthB)
{
	CORE_ASSERT(sA);
	CORE_ASSERT(sB);

	mData		= NULL;
	mMaxLength	= 0;
	mLength		= 0;

	Alloc(lengthA+lengthB, 0);
	Core::MemCopy(mData, sA, lengthA * sizeof(char));
	Core::MemCopy(mData+lengthA, sB, lengthB * sizeof(char));
}


// format a string, returns itself
String& String::Format(const char* text, ...)
{
	if (mMaxLength < 1024)
		Alloc(1024, 0);

	va_list args;
	va_start(args, text);
	
		//const int32 len = vsprintf(mData, text, args);
		const int32 len = vsnprintf( mData, mMaxLength, text, args );
		CORE_ASSERT(len >= 0);

	va_end(args);

	Alloc((uint32)len, 0);
	return *this;
}


// add a formatted string to the current string
String& String::FormatAdd(const char* text, ...)
{
	const uint32 oldLength = mLength;

	if (mMaxLength < 1024)
		Alloc(1024, 0);

	va_list args;
	va_start(args, text);
	
		//const int32 len = vsprintf(mData+oldLength, text, args);
		const int32 availableLength = mMaxLength-oldLength;
		CORE_ASSERT(availableLength >= 0 );
		const int32 len = vsnprintf( mData+oldLength, availableLength, text, args );
		CORE_ASSERT(len >= 0);

	va_end(args);

	Alloc(oldLength + len, 0);
	return *this;
}




// copy some data
String& String::Copy(const char* what, uint32 length)
{
	if (what == NULL)
	{
		if (mData)
		{
			Core::Free(mData);
			mData		= NULL;
			mLength		= 0;
			mMaxLength	= 0;
		}
		return *this;
	}

	// remove existing data
	if (mData)
	{
		//Core::Free(mData);
		mLength		= 0;
		mData[0]	= 0;
	}

	//mData = NULL;
	
	// alloc new
	Alloc(length, 0);

	// copy new data
	if (mData)
		Core::MemCopy(mData, what, length * sizeof(char));

	return *this;
}


// concatenate two strings
String& String::Concat(const char* what, uint32 length)
{
	if (what == NULL || length == 0)
		return *this;
	
	const uint32 oldLength = mLength;

	// realloc new
	if (mMaxLength <= length+mLength)
		Alloc(mLength+length, 0);
	else
		mLength += length;

	Core::MemCopy(mData+oldLength, what, length * sizeof(char));
	mData[mLength] = 0;
	
	return *this;
}


void String::Alloc(uint32 numCodeUnits, uint32 extraUnits)
{
	// if we have a zero length, do nothing if we don't have to
	if (numCodeUnits+extraUnits == 0)
	{
		if (mData != NULL)
			mData[0] = 0;

		mLength = 0;
		return;
	}

	if ((numCodeUnits+extraUnits > mMaxLength) || mData==NULL)
	{
		mLength		= numCodeUnits;
		mMaxLength	= mLength+extraUnits;

		if (mData != NULL)
			mData	= (char*)Core::Realloc(mData, (mMaxLength + 1)*sizeof(char) );
		else
			mData	= (char*)Core::Allocate( (mMaxLength + 1)*sizeof(char) );

		if (mData == NULL)
			return;

		mData[numCodeUnits] = 0;	
	}
	else
	{
		mLength	= numCodeUnits;
		mData[numCodeUnits] = 0;	
	}
}



// convert the string to a boolean. string may be "1", "0", "true", "false", "yes", "no") (non case sensitive)
bool String::ToBool() const
{
	if (mData == NULL || mLength == 0)
		return false;

	// check if it's true, else return false
	return (IsEqualNoCase("1") || IsEqualNoCase("true") || IsEqualNoCase("yes"));
}


// convert the string to an int32
int32 String::ToInt() const
{
	if (mData == NULL || mLength == 0)
		return 0;

	return atoi(mData);
}


// convert the string to a float
float String::ToFloat() const
{
	if (mData == NULL || mLength == 0)
		return 0.0f;

	return atof(mData);
}


// convert the string to a double
double String::ToDouble() const
{
	if (mData == NULL || mLength == 0)
		return 0.0;

	return atof(mData);
}


bool String::IsValidFloat() const
{
	if (mData == NULL || mLength == 0)
		return false;

	const uint32 numValidChars = 15;
	char validChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'e', 'E', '+', '-' };

	// check if all characters are valid
	for (uint32 i=0; i<mLength; ++i)
	{
		char c = mData[i];

		// if the + or - is not the first character
		// TODO: doesn't work well for scientific notation
		//if (c == '+' || c =='-')
		//	if (i != 0)
		//		return false;

		// check if the characters are correct values
		bool found = false;
		for (uint32 a=0; a<numValidChars && !found; ++a)
		{
			if (c == validChars[a])
				found = true;
		}

		// if we found a non-valid character, we cannot convert this to a float
		if (!found)
			return false;
	}

	return true;
}


// check if this is a valid int
bool String::IsValidInt() const
{
	if (mData == NULL || mLength == 0)
		return false;

	char validChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-' };

	// check if all characters are valid
	for (uint32 i=0; i<mLength; ++i)
	{
		char c = mData[i];

		// if the + or - is not the first character
		if (c == '+' || c =='-')
			if (i != 0)
				return false;

		// check if the characters are correct values
		bool found = false;
		for (uint32 a=0; a<12 && !found; ++a)
		{
			if (c == validChars[a])
				found = true;
		}

		// if we found a non-valid character, we cannot convert this to an int32
		if (!found)
			return false;
	}

	return true;
}


// check if this is a valid bool
bool String::IsValidBool() const
{
	if (mData == NULL || mLength == 0)
		return false;

	if (IsEqualNoCase("1") || IsEqualNoCase("0") || IsEqualNoCase("false") || IsEqualNoCase("true") || IsEqualNoCase("yes") || IsEqualNoCase("no"))
		return true;

	return false;
}


// init from a boolean
void String::FromBool(bool value)
{
	if (value == true)
		Copy("true", 4);
	else
		Copy("false", 5);
}


// init from an integer
void String::FromInt(int32 value)
{
	Alloc(32);
	sprintf(mData, "%d", value);
	Alloc( (uint32)strlen(mData) );
}


// init from a float
void String::FromFloat(float value)
{
	Alloc(32);

	// determine whether to use normal or scientic view
	const float maxValue = 1000000.0f;
	bool useExponent = false;
	if (value < -maxValue || value > maxValue)
		useExponent = true;


	// convert float to string with fixed number of chars
	if (useExponent == true)
		sprintf(mData, "%.4e", value);
	else
		sprintf(mData, "%.4f", value);
	
	Alloc( (uint32)strlen(mData) );

	// trim all ending zeros
	if (useExponent == false)
	{
		TrimRight( StringCharacter('0') );

		// in case a dot is the last character, add a zero to it to form a valid float
		StringCharacter lastChar = GetLast();
		if (lastChar == StringCharacter::dot)
			*this += "0";
	}
}


// init from a double
void String::FromDouble(double value)
{
	Alloc(32);

	// determine whether to use normal or scientic view
	const double maxValue = 100000000000.0;
	bool useExponent = false;
	if (value < -maxValue || value > maxValue)
		useExponent = true;


	// convert float to string with fixed number of chars
	if (useExponent == true)
		sprintf(mData, "%0.8e", value);
	else
		sprintf(mData, "%.8f", value);
	
	Alloc( (uint32)strlen(mData) );


	// trim all ending zeros
	if (useExponent == false)
	{
		TrimRight( StringCharacter('0') );

		// in case a dot is the last character, add a zero to it to form a valid float
		StringCharacter lastChar = GetLast();
		if (lastChar == StringCharacter::dot)
			*this += "0";
	}
}


// convert the string into a Vector4
Vector4 String::ToVector4() const
{
	// split the string into different floats
	Array<String> parts = Split( StringCharacter::comma );
	if (parts.Size() != 4)
		return Vector4(0.0f, 0.0f, 0.0f, 0.0f);

	// remove spaces
	for (uint32 i=0; i<4; ++i)
		parts[i].Trim();

	return Vector4( parts[0].ToFloat(), parts[1].ToFloat(), parts[2].ToFloat(), parts[3].ToFloat() );
}


// init from a Vector4
void String::FromVector4(const Vector4& value)
{ 
	Format("%.8f,%.8f,%.8f,%.8f", value.x, value.y, value.z, value.w);
}


// check if this is a valid Vector4
bool String::IsValidVector4() const
{
	if (mData == NULL || mLength == 0)
		return false;

	// split the string into different floats
	Array<String> parts = Split( StringCharacter::comma );
	if (parts.Size() != 4)
		return false;

	// remove spaces
	for (uint32 i=0; i<4; ++i)
		parts[i].Trim();

	return (parts[0].IsValidFloat() && parts[1].IsValidFloat() && parts[2].IsValidFloat() && parts[3].IsValidFloat());
}

//---------------------------

// return the last char of the string
StringCharacter String::GetLast() const
{
	// return invalid character
	if (IsEmpty() == true)
		return StringCharacter();

	// return the last char
	return StringCharacter( mData[GetLength()-1] );
}


// return the first char of the string
StringCharacter String::GetFirst() const
{
	// return invalid character
	if (IsEmpty() == true)
		return StringCharacter();

	// return the first char
	return StringCharacter( mData[0] );
}


//---------------------------

// count the number of characters in a string
uint32 String::CountNumChars(const StringCharacter& character) const
{
	if (mData == NULL || mLength == 0)
		return 0;
	 
	uint32 count = 0;
	StringIterator iterator( *this );
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();
		if (c == character)
			count++;
	}

	return count;
}


// find a given substring, returns the position, or CORE_INVALIDINDEX32 when not found
uint32 String::Find(const char* subString) const
{
	if (mData == NULL || mLength==0 || subString == NULL || subString[0] == 0)
		return CORE_INVALIDINDEX32;

	// try to locate the string
	const char* strResult = strstr(mData, subString);
	
	// if the string has not been found
	if (strResult == NULL) 
		return CORE_INVALIDINDEX32;
	
	// else return the position
	return (strResult - mData);
}


// search for a given character, starting from the end of the string to the start, and return the code unit index, or CORE_INVALIDINDEX32 when not found
uint32 String::FindRight(const StringCharacter& character) const
{
	if (mData == NULL || mLength == 0)
		return CORE_INVALIDINDEX32;

	// search the string from end to start
	StringIterator iterator( *this );
	iterator.SetIndexToEnd();
	for (uint32 i=0; i<mLength; ++i)
	{
		StringCharacter c = iterator.GetPreviousCharacter();
		if (c == character)
			return iterator.GetIndex();
	}

	// character hasn't been found
	return CORE_INVALIDINDEX32;
}


// split the string into substrings. "a;b;c" with splitchar ';' would result in an array of 3 strings ("a", "b", "c")
Array<String> String::Split(const StringCharacter& splitChar) const
{
	Array<String> result;
	if (mData == NULL || mLength == 0)
		return result;

	// count the number of split characters
	const uint32 numSplitChars = CountNumChars( splitChar );

	// reserve array memory for our strings, to prevent reallocs
	result.Reserve( numSplitChars + 1 );

	const uint32 numSplitCharBytes = splitChar.CalcNumRequiredUTF8Bytes();

	uint32 startOffset = 0;
	StringIterator iterator( *this );
	while (iterator.HasReachedEnd() == false)
	{
		const StringCharacter curChar = iterator.GetNextCharacter();
		
		// if we reached the end of the string
		if (iterator.HasReachedEnd() == true)
		{
			//if (startOffset - iterator.GetIndex() > 0)
			if (startOffset != iterator.GetIndex())
			{
				result.AddEmpty();
				if (curChar != splitChar)
					result.GetLast().Copy( &mData[startOffset], iterator.GetIndex() - startOffset );
				else
					result.GetLast().Copy( &mData[startOffset], iterator.GetIndex() - startOffset - numSplitCharBytes );
			}
			break;
		}

		// if the current character is the split character
		if (curChar == splitChar)
		{
			result.AddEmpty();
			result.GetLast().Copy( &mData[startOffset], iterator.GetIndex() - startOffset - numSplitCharBytes );
			startOffset = iterator.GetIndex();
		}
	}

	// if there are no substrings, return itself
	if (result.Size() == 0)
		if (FindRight( splitChar ) != CORE_INVALIDINDEX32)
			result.Add( *this );

	return result;
}


// fast splitstring; fills one array with pointers to the first char of each splitstring and fills another array with all the string lengths
uint32 String::SplitFast(Array<const char*>& positions, Array<uint32>& lengths, const StringCharacter& splitChar) const
{
	// always clear arrays first (no dealloc)
	positions.Clear();
	lengths.Clear();

	if (mData == NULL || mLength == 0)
		return 0;

	// count number of delimiters and reserver space in arrays (allocs if arrays are too small)
	const uint32 numSplitChars = CountNumChars(splitChar);
	positions.Reserve(numSplitChars+1);
	lengths.Reserve(numSplitChars+1);

	// step from delimiter to delimiter and fill pointer/lenghts arrays
	const uint32 numSplitCharBytes = splitChar.CalcNumRequiredUTF8Bytes();
	
	// Node: modified version of String::Split
	uint32 startOffset = 0;
	StringIterator iterator(*this);
	while (iterator.HasReachedEnd() == false)
	{
		const StringCharacter curChar = iterator.GetNextCharacter();

		// if we reached the end of the string
		if (iterator.HasReachedEnd() == true)
		{
			if (startOffset != iterator.GetIndex())
			{
				positions.Add(&mData[startOffset]);
				if (curChar != splitChar)
					lengths.Add(iterator.GetIndex() - startOffset);
				else
					lengths.Add(iterator.GetIndex() - startOffset - numSplitCharBytes);
			}
			break;
		}

		// if the current character is the split character
		if (curChar == splitChar)
		{
			positions.Add(&mData[startOffset]);
			lengths.Add(iterator.GetIndex() - startOffset - numSplitCharBytes);
			startOffset = iterator.GetIndex();
		}
	}

	// if there are no substrings, return itself
	if (positions.Size() == 0)
	{
		positions.Add(mData);
		lengths.Add(mLength);
	}

	return positions.Size();
}


// join an array of strings with a separator string in between
String& String::Join(const Array<String>& array, String separator,  String& result)
{
	result.Clear();

	// calculate required length first
	//const uint32 separatorSize = separator.GetLength();
	const uint32 numStrings = array.Size();
	uint32 totalSize = (numStrings > 0 ? numStrings - 1 : 0); // size of all separators
	for (uint32 i=0; i<numStrings; ++i)
		totalSize += array[i].GetLength();

	result.Reserve(totalSize);

	// construct joined string
	for (uint32 i=0; i<numStrings; ++i)
	{
		// add separator between strings
		if (i > 0) 
			result += separator;

		// add the string
		result += array[i];
	}

	return result;
}


// returns true if this string is equal to the given other string (case sensitive)
bool String::IsEqual(const char* other) const
{
	// compare
	return (SafeCompare(mData, other) == 0);
}


// returns true if this string is equal to the given other string (not case sensitive)
bool String::IsEqualNoCase(const char* other) const
{
	// compare
	return (CompareNoCase(other) == 0);
}



// compares two strings (case sensitive) returns 0 when equal, -1 when this string is bigger and 1 when other is bigger
int32 String::Compare(const char* other) const
{
	return SafeCompare(mData, other);
}


// compares two strings (non case sensitive) returns 0 when equal, -1 when this string is bigger and 1 when other is bigger
int32 String::CompareNoCase(const char* other) const
{
	const char* with = AsChar();
	const char* what = (other != NULL) ? other : "";

	#ifdef _WIN32
		return stricmp(with, what);
	#else
		return strcasecmp(with, what);
	#endif
}


// returns true if the string contains visible characters, false if it contains only whitespace
bool String::ContainsVisibleCharacter() const
{
	if (mData == NULL || mLength == 0)
		return false;

	// iterate over string and check for ascii characters that are usually printed
	StringIterator iterator(*this);
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();

		// all chars between char(33) and char(126) inclusively are printable
		if (c >= StringCharacter((uint32)33) && c <= StringCharacter((uint32)126))
			return true;
	}

	return false;
}


// returns the number of words inside this string
uint32 String::CalcNumWords() const
{
	if (mData == NULL || mLength == 0)
		return 0;

	uint32 numWords = 0;

	// read away all spaces in the beginning
	StringIterator iterator( *this );
	while (iterator.HasReachedEnd() == false)
	{
		const uint32 curPos = iterator.GetIndex();
		StringCharacter c = iterator.GetNextCharacter();
		if (c != StringCharacter::space && c != StringCharacter::endLine && c != StringCharacter::tab)
		{
			iterator.SetIndex( curPos );
			break;
		}
	}

	// count the number of words
	while (iterator.HasReachedEnd() == false)
	{
		// move cursor to next space
		while (iterator.HasReachedEnd() == false)
		{
			StringCharacter c = iterator.GetNextCharacter();
			if (c == StringCharacter::space || c == StringCharacter::endLine || c == StringCharacter::tab)
				break;
		}


		numWords++;

		// move cursor to next word
		bool lastIsSpace = true;
		while (iterator.HasReachedEnd() == false)
		{
			StringCharacter c = iterator.GetNextCharacter();
			if (c != StringCharacter::space && c != StringCharacter::endLine && c != StringCharacter::tab)
			{
				lastIsSpace = false;
				break;
			}
		}

		if (iterator.HasReachedEnd() == true && lastIsSpace == false)
			numWords++;
	}

	return numWords;
}


// returns word number <wordNr>
String String::ExtractWord(uint32 wordNr) const
{
	CORE_ASSERT(wordNr < CalcNumWords());	// slow in debugmode, but more safe

	if (mData == NULL || mLength == 0)
		return String();

	String result;
	uint32 numWords = 0;

	// read away all spaces in the beginning
	StringIterator iterator( *this );
	while (iterator.HasReachedEnd() == false)
	{
		const uint32 curPos = iterator.GetIndex();
		StringCharacter c = iterator.GetNextCharacter();
		if (c != StringCharacter::space && c != StringCharacter::endLine && c != StringCharacter::tab)
		{
			iterator.SetIndex( curPos );
			break;
		}
	}

	// count the number of words
	uint32 curPos = iterator.GetIndex();
	while (iterator.HasReachedEnd() == false)
	{
		const uint32 startOffset = curPos;

		// move cursor to next space
		while (iterator.HasReachedEnd() == false)
		{
			curPos = iterator.GetIndex();
			StringCharacter c = iterator.GetNextCharacter();
			if (c == StringCharacter::space || c == StringCharacter::endLine || c == StringCharacter::tab)
				break;
		}


		// if this is the word number we are interested in
		if (numWords == wordNr)
		{
			if (iterator.HasReachedEnd() == false)
				result.Copy( &mData[startOffset], curPos - startOffset );
			else
				result.Copy( &mData[startOffset], curPos - startOffset + 1 );

			return result;
		}

		numWords++;
		
		// move cursor to next word
		bool lastIsSpace = true;
		while (iterator.HasReachedEnd() == false)
		{
			curPos = iterator.GetIndex();
			StringCharacter c = iterator.GetNextCharacter();
			if (c != StringCharacter::space && c != StringCharacter::endLine && c != StringCharacter::tab)
			{
				lastIsSpace = false;
				break;
			}
		}

		if (iterator.HasReachedEnd() == true && lastIsSpace == false)
		{
			if (numWords == wordNr)
			{
				result.Copy( &mData[curPos], mLength - curPos );
				return result;
			}

			numWords++;
		}
	}
	
	return result;
}


// removes a given part from the string (the first found one)
bool String::RemoveFirstPart(const char* part)
{
	const uint32 pos = Find(part);

	// substring not found, so exit
	if (pos == CORE_INVALIDINDEX32)
		return false;

	const uint32 partLength = (uint32)strlen(part);

	// remove the part
	Core::MemMove(mData+pos, mData+pos+partLength, (mLength-(pos+partLength)) * sizeof(char) );

	// resize the memory amount
	Alloc(mLength - partLength, 0);

	return true;
}


// removes all given parts from a string (all occurences)
bool String::RemoveAllParts(const char* part)
{
	if (mData == NULL || mLength == 0)
		return false;

	bool result = false;
	while (RemoveFirstPart(part)) 
		result = true;

	return result;
}


// removes a given set of characters from the string
bool String::RemoveChars(const Array<StringCharacter>& charSet)
{
	if (mData == NULL || mLength == 0)
		return false;

	bool result = false;

	// for all characters in the set
	char partToRemove[8];
	const uint32 numChars = charSet.Size();
	for (uint32 i=0; i<numChars; ++i)
	{
		uint32 numBytes = 0;
		charSet[i].AsUTF8(partToRemove, &numBytes, true);
		CORE_ASSERT(numBytes < 8);
		if (RemoveAllParts( partToRemove ))
			result = true;
	}

	return result;
}


// removes a given set of characters from the string
bool String::RemoveChars(const char* characterSet)
{
	if (mData == NULL || mLength == 0)
		return false;

	bool result = false;

	String s = characterSet;

	// for all characters in the set
	char partToRemove[8];
//	StringIterator iterator(characterSet);
	StringIterator iterator( s );
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();
		
		uint32 numBytes = 0;
		c.AsUTF8(partToRemove, &numBytes, true);
		CORE_ASSERT(numBytes < 8);
		if (RemoveAllParts( partToRemove ))
			result = true;
	}

	return result;
}


// remove the extension from a given string containing a filename
// this removes everything after the last encountered dot, so "filename.bla" would result in "filename"
void String::RemoveFileExtension()
{
	if (mData == NULL || mLength == 0)
		return;

	// search for the dot, starting from the end of the string, searching towards the first character
	const uint32 dotPos = FindRight( StringCharacter::dot );

	// search for any slash, starting from the end of the string, searching towards the first character
	uint32 slashPos = CORE_INVALIDINDEX32;
	StringIterator iterator( *this );
	iterator.SetIndexToEnd();
	for (uint32 i=0; i<mLength; ++i)
	{
		StringCharacter c = iterator.GetPreviousCharacter();
		if (c == StringCharacter::backSlash || c == StringCharacter::forwardSlash)
		{
			slashPos = iterator.GetIndex();
			break;
		}
	}

	// if the dot has not been found, there is nothing to remove, since the string contains no extension
	if (dotPos == CORE_INVALIDINDEX32)
		return;

	// if there is a slash present
	if (slashPos != CORE_INVALIDINDEX32)
	{
		// if the slash comes after the dot, like in "../SomeFilenameWithoutExtension", then do nothing
		if (slashPos > dotPos)
			return;
	}

	// if the string ends with a dot, then that's no extension
	if (dotPos == mLength-1)
		return;

	// simply terminate the string at the dot location
	mLength = dotPos;
	mData[dotPos] = 0;
}


// extract the file name
String String::ExtractFilename() const
{
	if (mData == NULL || mLength == 0)
		return String();

	// find the forward slash and/or back slash
	const uint32 forwardSlashIndex = FindRight( StringCharacter::forwardSlash );
	const uint32 backSlashIndex    = FindRight( StringCharacter::backSlash );

	// if no slash or backslash found, simply return itself
	if (forwardSlashIndex == CORE_INVALIDINDEX32 && backSlashIndex == CORE_INVALIDINDEX32)
		return *this;

	// pick the one at the end
	uint32 index = 0;
	if (forwardSlashIndex != CORE_INVALIDINDEX32)
		index = forwardSlashIndex;
	
	if (backSlashIndex != CORE_INVALIDINDEX32)
		index = Max<uint32>(index, backSlashIndex);

	// copy the filename into the result string
	String result;
	result.Copy(mData + index + 1, mLength - index - 1);

	// if there is no dot in the filename, then there is no filename, so return an empty string
	if (result.FindRight( StringCharacter::dot ) == CORE_INVALIDINDEX32)
		return *this;

	// return the result
	return result;
}


// extract the path
String String::ExtractPath(bool includeSlashAtEnd) const
{
	if (mData == NULL || mLength == 0)
		return String();

	// first extract the file name
	String filename = ExtractFilename();

	// since the filename is stored at the end, everything in front of the filename is considered as path
	String result;
	result.Copy(mData, mLength - filename.GetLength());

	// find the forward slash and/or back slash
	const uint32 forwardSlashIndex = result.FindRight( StringCharacter::forwardSlash );
	const uint32 backSlashIndex    = result.FindRight( StringCharacter::backSlash );

	// if no slash or backslash found, simply return the same string
	if (forwardSlashIndex == CORE_INVALIDINDEX32 && backSlashIndex == CORE_INVALIDINDEX32)
		return *this;

	// decide to use a forward or backslash when we have to end with a slash
	bool useForward = true;
	if (forwardSlashIndex != CORE_INVALIDINDEX32)
		useForward = true;
	
	if (backSlashIndex != CORE_INVALIDINDEX32)
		if (forwardSlashIndex < backSlashIndex)
			useForward = false;

	// make sure there are no slashes at the end when we don't want it
	if (!includeSlashAtEnd)
	{
		result.TrimRight( StringCharacter::backSlash );
		result.TrimRight( StringCharacter::forwardSlash );
	}
	else	// when we do want a slash at the end, make sure it's there
	{
		// when the last character isn't a forward slash or backslash, add it
		StringIterator iterator(result);
		iterator.SetIndexToEnd();
		StringCharacter lastChar = iterator.GetPreviousCharacter();
		if (lastChar != StringCharacter::forwardSlash && lastChar != StringCharacter::backSlash)
		{
			// check if we need to add a forward slash or backslash
			if (useForward)
				result += '/';
			else
				result += '\\';
		}
	}

	// return the result
	return result;
}


// remove all given trimChars on the left of the string
void String::TrimLeft(const StringCharacter& trimChar)
{
	if (mData == NULL || mLength == 0)
		return;

	// for all characters in the string
	StringIterator iterator( *this );
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();

		// return when another character found
		uint32 index = c.GetIndex();
		if (c != trimChar)
		{
			if (index == 0)
				return;

			const uint32 numUnits = (mLength - index) + 1;	// +1 because of the '\0'
			Core::MemMove(mData, mData + index, numUnits * sizeof(char));
			mLength -= index;
			return;
		}
	}

	// in case we have for example a string of only spaces
	Clear(true);
}



// remove all given trimChars on the right of the string
void String::TrimRight(const StringCharacter& trimChar)
{
	if (mData == NULL || mLength == 0)
		return;
	
	StringIterator iterator( *this );
	iterator.SetIndexToEnd();
	for (uint32 i=0; i<mLength; ++i)
	{
		StringCharacter c = iterator.GetPreviousCharacter();
		if (c != trimChar)
		{
			uint32 index = c.GetIndex();

			// remove the last chars
			if ((int32)index != (int32)(mLength-1))
			{
				mLength = index + 1;
				mData[index+1] = 0;
			}

			return;
		}
	}

	// seems like we have a string of only spaces for example
	Clear(true);
}



// remove all given trimChars on both left and right side of the string
void String::Trim(const StringCharacter& trimChar)
{
	TrimLeft(trimChar);
	TrimRight(trimChar);
}


// remove the last character if it is equal to a given character
void String::RemoveLastCharacterIfEqualTo(const StringCharacter& lastCharacter)
{
	if (mData == NULL || mLength == 0)
		return;
	
	StringIterator iterator( *this );
	iterator.SetIndexToEnd();
	StringCharacter c = iterator.GetPreviousCharacter();
	if (c == lastCharacter)
	{
		// remove the last char
		const uint32 index = c.GetIndex();
		mLength = index;
		mData[index] = 0;
	}
}


// remove the first character if it is equal to a given character
void String::RemoveFirstCharacterIfEqualTo(const StringCharacter& firstCharacter)
{
	if (mData == NULL || mLength == 0)
		return;
	
	StringIterator iterator( *this );
	StringCharacter c = iterator.GetNextCharacter();
	if (c == firstCharacter)
	{
		MemMove((uint8*)mData, (uint8*)mData+sizeof(char), (mLength-1)*sizeof(char));
		mLength--;
		mData[mLength] = 0;
	}
}


// extract the file extension
String String::ExtractFileExtension() const
{
	if (mData == NULL || mLength == 0)
		return String();

	// search for the dot, starting from the end of the string, searching towards the first character
	const uint32 dotPos = FindRight( StringCharacter::dot );

	// if the dot has not been found, there is nothing to remove, since the string contains no extension
	if (dotPos == CORE_INVALIDINDEX32)
		return String();	// an empty string

	// search for any slash, starting from the end of the string, searching towards the first character
	uint32 slashPos = CORE_INVALIDINDEX32;
	StringIterator iterator( *this );
	iterator.SetIndexToEnd();
	for (uint32 i=0; i<mLength; ++i)
	{
		StringCharacter c = iterator.GetPreviousCharacter();
		if (c == StringCharacter::backSlash || c == StringCharacter::forwardSlash)
		{
			slashPos = iterator.GetIndex();
			break;
		}
	}

	// if there is a slash present
	if (slashPos != CORE_INVALIDINDEX32)
	{
		// if the slash comes after the dot, like in "../SomeFilenameWithoutExtension", then do nothing
		if (slashPos > dotPos)
			return String();
	}

	// if the string ends with a dot, then that's no extension
	if (dotPos == mLength-1)
		return String();

	// return the extension
	return String( (const char*)(&mData[dotPos+1]) );
}


// convert slashes of a file path to the natively used ones on the used platform
void String::ConvertToNativePath()
{
	Replace( StringCharacter::forwardSlash,		StringCharacter((char)CORE_FOLDERSEPARATORCHAR) );
	Replace( StringCharacter::backSlash,		StringCharacter((char)CORE_FOLDERSEPARATORCHAR) );
}


// convert slashes of a copied file path to the natively used ones on the used platform and return the result
String String::ConvertedToNativePath() const
{
	String result( *this );
	result.ConvertToNativePath();
	return result;
}


// replace a given string with another one
// for example replace all %NAME% parts of the string with a given name
void String::Replace(const char* what, const char* with)
{
	if (mData == NULL || mLength == 0)
		return;

	const uint32 withLen = (uint32)strlen(with);
	const uint32 whatLen = (uint32)strlen(what);

	if (whatLen == 0)
		return;

	uint32 location = Find(what);
	String temp;
	while (location != CORE_INVALIDINDEX32)
	{
		//numPasses++;
		temp.Alloc( mLength + (withLen - whatLen), 0 );
		Core::MemCopy(temp.mData, mData, location * sizeof(char));
		Core::MemCopy(temp.mData + location, with, withLen * sizeof(char));
		Core::MemCopy(temp.mData + location + withLen, mData + location + whatLen, (mLength - location - whatLen) * sizeof(char));
		*this = temp;

		// try to locate the string again
		const char* strResult = strstr(mData + location + withLen, what);
		if (strResult == NULL) 
			location = CORE_INVALIDINDEX32; // if the string has not been found break the loop
		else
			location = (strResult - mData); // in case the string has been found go again
	}
}


// replace a given character with another one
void String::Replace(const StringCharacter& c, const StringCharacter& with)
{
	// if no data has been allocated yet skip directly
	if (c == with || mData == NULL || mLength == 0)
		return;

	const uint32 numCBytes = c.CalcNumRequiredUTF8Bytes();

	// iterate through all characters and replace them if needed
	StringIterator iterator(*this);
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter character = iterator.GetNextCharacter();

		// check if the current character is the one we want to replace
		if (character == c)
		{
			uint32 numWithBytes = with.CalcNumRequiredUTF8Bytes();
			if (numCBytes == numWithBytes)
				with.AsUTF8( &mData[character.GetIndex()], &numWithBytes, false );
			else
			{
				const int32 numExtraChars = numWithBytes - numCBytes;
				if (numExtraChars > 0)
					Resize( mLength + numExtraChars, ' ', false );

				const uint32 numBytesToMove = mLength - character.GetIndex() - numExtraChars - 1;
				MemMove( &mData[character.GetIndex()+numWithBytes], &mData[iterator.GetIndex()], numBytesToMove);
				with.AsUTF8( &mData[character.GetIndex()], &numWithBytes, false );

				if (numExtraChars < 0)
					Resize( (uint32)strlen(mData) );	// adjust the length if we did shrink down

				iterator.Init( *this );
				iterator.SetIndex(character.GetIndex()+numWithBytes);
			}
		}
	}
}


// deletes a substring from the string (inclusive indices)
void String::Remove(uint32 pos, uint32 length)
{
	// if no data has been allocated yet skip directly
	if (mData == NULL || mLength == 0)
		return;

	// invalid range -> result is an unchanged string
	if (pos > mLength)
		return;

	// adjust length if it is longer if we reach the end of the string
	if (pos + length > mLength)
		length = mLength - pos;

	// move second part of the string up 
	MemMove(&mData[pos], &mData[pos + length], 1 + mLength - pos - length);

	// set new string length
	Resize(mLength - length);
}


// crops the string to the specified range (inclusive indices)
void String::Crop(uint32 pos, uint32 length)
{
	// if no data has been allocated yet skip directly
	if (mData == NULL || mLength == 0)
		return;

	// invalid range -> result is an unchanged string
	if (pos > mLength)
		return;

	// adjust length if it is longer if we reach the end of the string
	if (pos + length > mLength)
		length = mLength - pos;

	// move selection to the beginning
	MemMove(&mData, &mData[pos], length);

	// set new string length
	Resize(length);
}


// resize the string
void String::Resize(uint32 length, char fillChar, bool doFill)
{
	if (length == mLength)
		return;

	// clear the string
	if (length == 0)
	{
		Clear(true);
		return;
	}

	// make the string smaller
	if (length < mLength)
	{
		mData[length] = 0;
		mLength = length;
		return;
	}

	// make it bigger
	const uint32 oldLength = mLength;
	Alloc(length, 0);
	if (doFill == true)
		for (uint32 i=oldLength; i<length; ++i)
			mData[i] = fillChar;
}


// pre-alloc space
void String::Reserve(uint32 length)
{
	if (length == 0)
		return;

	if ((length > mMaxLength) || mData==NULL)
	{
		mMaxLength	= length;

		if (mData != NULL)
			mData	= (char*)Core::Realloc(mData, (mMaxLength+1)*sizeof(char) );
		else
		{
			mData	= (char*)Core::Allocate( (mMaxLength+1)*sizeof(char) );
			mData[0] = 0;
		}
	}
}


// returns an uppercase version of this string
String String::Uppered() const
{
	String newString( *this );
	newString.ToUpper();
	return newString;
}


// returns a lowercase version of this string
String String::Lowered() const
{
	String newString( *this );
	newString.ToLower();
	return newString;
}


// uppercase this string
String& String::ToUpper()
{
	if (mData == NULL || mLength == 0)
		return *this;

	StringIterator iterator(*this);
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();
		if (c.CalcNumRequiredUTF8Bytes() == 1)
			mData[c.GetIndex()] = toupper( mData[c.GetIndex()] );
	}

	return *this;
}


// lowercase this string
String& String::ToLower()
{
	if (mData == NULL || mLength == 0)
		return *this;

	StringIterator iterator(*this);
	while (iterator.HasReachedEnd() == false)
	{
		StringCharacter c = iterator.GetNextCharacter();
		if (c.CalcNumRequiredUTF8Bytes() == 1)
			mData[c.GetIndex()] = tolower( mData[c.GetIndex()] );
	}

	return *this;
}


// align a string to a certain length
String& String::Align(uint32 newNumCharacters, const StringCharacter& character)
{
	StringIterator iterator(*this);
	const uint32 numCharacters = iterator.CalcNumCharacters( mData, mLength );

	// if the string is already long enough, there is nothing to do
	if (numCharacters >= newNumCharacters)
		return *this;

	//
	const uint32 numExtraCharacters = newNumCharacters - numCharacters;
	const uint32 numExtraBytes = character.CalcNumRequiredUTF8Bytes() * numExtraCharacters;

	// allocate space
	const uint32 oldLength = mLength;
	Alloc(mLength + numExtraBytes, 0);

	// add the extra characters
	uint32 numCharBytes;
	uint32 offset = oldLength;
	for (uint32 i=0; i<numExtraCharacters; ++i)
	{
		character.AsUTF8( &mData[offset], &numCharBytes, false );
		offset += numCharBytes;
	}

	return *this;
}

} // namespace Core
