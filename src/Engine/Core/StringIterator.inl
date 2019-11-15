/*
 * neuromore Engine
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// constructor from string
inline StringIterator::StringIterator(const String& str)
{ 
	Init( str );
}


// constructor from data pointer
inline StringIterator::StringIterator(char* data, uint32 numCodeUnits)
{ 
	Init(data, numCodeUnits);
}

// constructor from data pointer
inline StringIterator::StringIterator(char* data)
{ 
	Init(data, (uint32)strlen(data));
}


// destructor
inline StringIterator::~StringIterator()
{
}


// init from a string
inline void StringIterator::Init(const String& str)
{ 
	Init(str.AsChar(), str.GetLength());
}


// init from data pointer
inline void StringIterator::Init(char* data, uint32 numCodeUnits)
{ 
	mData			= data; 
	mNumCodeUnits	= numCodeUnits;
	mCodeUnitIndex	= 0;
}


// get the number of code units
inline uint32 StringIterator::GetLength() const
{ 
	return mNumCodeUnits;
}


// have we reached the end of the string?
inline bool StringIterator::HasReachedEnd() const
{ 
	return (mCodeUnitIndex >= mNumCodeUnits);
}


// get the current code unit index
inline uint32 StringIterator::GetIndex() const
{ 
	return mCodeUnitIndex;
}
