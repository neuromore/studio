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

#ifndef __CORE_ARRAY_H
#define __CORE_ARRAY_H

// include required headers
#include "StandardHeaders.h"
#include "Math.h"


namespace Core
{

template <class T>
class ENGINE_API Array
{
	public:
		enum { MEMORYBLOCK_ID = 2 };

		// main constructors
		Array()																			{ mData=NULL; mSize=0; mMaxSize=0; }
		explicit Array(uint32 size)														{ mSize=size; mMaxSize=size; if (mMaxSize > 0) { mData = (T*)Core::Allocate(mMaxSize * sizeof(T)); for (uint32 i=0; i<mSize; ++i) Construct(i); } else { mData=NULL; } }
		
		// copy and move constructor
		Array(const Array<T>& other)													{ mData=NULL; mSize=0; mMaxSize=0; *this = other; }
		//Array(Array<T>&& other)															{ mData = other.mData; mSize=other.mSize; mMaxSize=other.mMaxSize; other.mData = NULL; other.mSize = 0; other.mMaxSize = 0; }
		
		// single-element constructor
		Array(const T& other) : Array(1)												{ Add(other); }
		
		// destructor
		virtual ~Array()																{ for (uint32 i=0; i<mSize; ++i){ Destruct(i); } if (mData != NULL) Core::Free(mData); }

		// accessors (data)
		T* GetPtr()																		{ return mData; }
		T* GetPtr() const																{ return mData; }
		const T* GetReadPtr() const														{ return mData; }

		// accessors (object)
		T& GetItem(uint32 index)														{ return mData[index]; }
		const T& GetItem(uint32 index) const											{ return mData[index]; }
		T& GetFirst()																	{ return mData[0]; }
		const T& GetFirst() const														{ return mData[0]; }
		T& GetLast()																	{ return mData[mSize-1]; }
		const T& GetLast() const														{ return mData[mSize-1]; }

		// size & index
		bool IsEmpty() const															{ return (mSize == 0); }
		uint32 Size() const																{ return mSize; }
		uint32 GetMaxSize() const														{ return mMaxSize; }
		bool IsValidIndex(uint32 index) const											{ return (index < mSize); }

		// add & insert
		void Add(const T& x)															{ Grow(++mSize); Construct(mSize-1, x); }
		void Add(const Array<T>& other)													{ uint32 l=mSize; Grow(mSize+other.mSize); for (uint32 i=0; i<other.Size(); ++i) Construct(l+i, other[i]); }	// TODO: a.Size() can be precaled before loop?
		T& AddEmpty()																	{ Grow(++mSize); Construct(mSize-1); return mData[mSize-1]; }
		void Insert(uint32 index)														{ Grow(mSize+1); Move(index+1, index, mSize-index-1); Construct(index); }
		void Insert(uint32 index, const T& x)											{ Grow(mSize+1); Move(index+1, index, mSize-index-1); Construct(index, x); }

		// modify
		void Set(uint32 index, const T& value)											{ mData[index] = value; }
		void SetAll(const T& value)														{ for (uint32 i=0; i<mSize; i++) { mData[i] = value; } }
		void Swap(uint32 indexA, uint32 indexB)											{ if (indexA != indexB) Core::Swap(GetItem(indexA), GetItem(indexB)); }
		void Move(uint32 destIndex, uint32 sourceIndex, uint32 numElements)				{ if (numElements > 0) Core::MemMove(mData+destIndex, mData+sourceIndex, numElements * sizeof(T)); }

		// remove
		void Remove(uint32 index)														{ Destruct(index); if (mSize > 1) Move(index, index+1, mSize-index-1); mSize--; }
		void Remove(uint32 index, uint32 num)											{ for (uint32 i=index; i<index+num; ++i) Destruct(i); Move(index, index+num, mSize-index-num); mSize-=num; }
		bool RemoveByValue(const T& item)												{ uint32 index = Find(item); if (index==CORE_INVALIDINDEX32) return false; Remove(index); return true; }
		void RemoveFirst()																{ if (IsEmpty() == false) Remove(0); }
		void RemoveLast()																{ if (IsEmpty() == false) Destruct(--mSize); }
		void Clear(bool clearMem=true)													{ for (uint32 i=0; i<mSize; ++i) Destruct(i); mSize=0; if (clearMem) Free(); }

		// find
		bool Contains(const T& x) const													{ return (Find(x) != CORE_INVALIDINDEX32); }
		uint32 Find(const T& x) const													{ for (uint32 i = 0; i<mSize; ++i) { if (mData[i] == x) return i; } return CORE_INVALIDINDEX32; }

		// sort
		typedef int32 (CORE_CDECL *CmpFunc)(const T& itemA, const T& itemB);
		static int32 CORE_CDECL StdCmp(const T& itemA, const T& itemB)					{ if ( itemA <  itemB) return -1; else if ( itemA ==  itemB) return 0; else return 1; }

		void Sort(CmpFunc cmp)															{ InnerSort(0, mSize - 1, cmp); }
		void Sort(uint32 first=0, uint32 last=CORE_INVALIDINDEX32, CmpFunc cmp=StdCmp) 	{ if (last==CORE_INVALIDINDEX32) last=mSize-1; InnerSort(first, last, cmp); }
		void InnerSort(int32 first, int32 last, CmpFunc cmp)							{ if (first >= last) return; int32 split=Partition(first, last, cmp); InnerSort(first, split-1, cmp); InnerSort(split+1, last, cmp); }

		// memory management
		void Reserve(uint32 num)														{ if (mMaxSize < num) Realloc(num); }
		void Shrink()																	{ if (mSize == mMaxSize) return; CORE_ASSERT(mMaxSize >= mSize); Realloc(mSize); }
		void Resize(uint32 newSize)														{ if (mSize == newSize) return; if (newSize > mSize) { const uint32 oldSize = mSize; Grow(newSize); for (uint32 i=oldSize; i<newSize; ++i) Construct(i); } else { for (uint32 i=newSize; i<mSize; ++i) Destruct(i); mSize = newSize; } }

		// access operators
		T& operator[](uint32 index)														{ return mData[index]; }
		const T& operator[](uint32 index) const											{ return mData[index]; }

		// concatenate operators
		Array<T>& operator+=(const T& other)											{ Add(other); return *this; }
		Array<T>& operator+=(const Array<T>& other)										{ Add(other); return *this; }

		// compare based on data pointers
		bool operator==(const Array<T>& other) const									{ if (mSize != other.mSize) return false; for (uint32 i=0; i<mSize; ++i) if (mData[i] != other.mData[i]) return false; return true; }

		// copy and move assignment operator
		Array<T>& operator=(const Array<T>& other)										{ if (&other != this) { Clear(false); Grow(other.mSize); for (uint32 i=0; i<mSize; ++i) Construct(i, other.mData[i]); } return *this; }
		Array<T>& operator=(Array<T>&& other)											{ Clear(); Free(); mData = other.mData; mSize = other.mSize; mMaxSize = other.mMaxSize; other.mData = NULL; other.mSize = 0; other.mMaxSize = 0; return *this; }

	protected:
		T*		mData;
		uint32	mSize;
		uint32	mMaxSize;

		// allocate
		void Grow(uint32 newSize)														{ mSize = newSize; if (mMaxSize < newSize) Realloc(newSize); }
		void Alloc(uint32 num)															{ mData = (T*)Core::Allocate(num * sizeof(T)); }
		void Realloc(uint32 newSize)													{ if (newSize == 0) { this->Free(); return; } if (mData) mData = (T*)Core::Realloc(mData, newSize * sizeof(T)); else mData = (T*)Core::Allocate(newSize * sizeof(T)); mMaxSize = newSize; }

		// free
		void Free()																		{ mSize=0; mMaxSize=0; if (mData) { Core::Free(mData); mData=NULL; } }

		// object construction & destruction
		void Construct(uint32 index, const T& original)									{ ::new(mData+index) T(original); }
		void Construct(uint32 index)													{ ::new(mData+index) T; }
		void Destruct(uint32 index)														{ (mData+index)->~T(); }

		// partition part of array (for sorting)
		int32 Partition(int32 left, int32 right, CmpFunc cmp)							{ ::Core::Swap(mData[left], mData[ (left+right)>>1 ]); T& target = mData[right]; int32 i = left-1; int32 j = right; bool neverQuit = true; while (neverQuit) { while (i < j) { if (cmp(mData[++i], target) >= 0) break; } while (j > i) { if (cmp(mData[--j], target) <= 0) break; } if (i >= j) break; ::Core::Swap(mData[i], mData[j]); } ::Core::Swap(mData[i], mData[right]); return i; }
};


/**
 * If T is a pointer: delete all elements and also deallocate all the objects that are referenced by the pointers.
 * Size() will return 0 after performing this method.
 * @param clearMem If set to true (default) the allocated array memory will also be released. If set to false, GetMaxSize() will still return the number of elements
 * which the array contained before calling the Clear() method.
 */
template <class T>
void DestructArray(Array<T>& array, bool clearMem=true)
{
	const uint32 numObjects = array.Size();
	for (uint32 i=0; i<numObjects; ++i)
	{
		delete array[i];
	}

	if (clearMem)
		array.Clear();
}

} // namespace Core

#endif
