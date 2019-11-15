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

#ifndef __NEUROMORE_TEST_H
#define __NEUROMORE_TEST_H

#include "TestCase.h"
#include "StandardHeaders.h"
#include "Array.h"
#include <iostream>

// represents one Test (one implementation should contain all the unit tests required to test a single class)
class Test
{
	public:
		Test(const char* name);
		virtual ~Test();

		// test name (usually the name of the class that is testet)
		const char* GetName() const				{ return mTestName;	}

		// init all testcases here
		virtual void Setup() = 0;
		
		// runs all testcases
		bool Run();

		uint32 GetNumTestCases() const				{ return mTestCases.Size(); }
		
		uint32 GetNumTestCasesPassed() const		{ return mNumTestCasesPassed; }
		uint32 GetNumTestCasesFailed() const		{ return mNumTestCasesFailed; }

	protected:
		// use this from Setup() to add all testcases
		void AddTest(const TestCase&);
		
	private:
		const char* mTestName;
		
		void RunTest(const TestCase&);
		void Reset()
		{
			mNumTestCasesPassed = 0;
			mNumTestCasesFailed = 0;
		}

		const Core::Array<TestCase*>& GetTestCases() const		{ return mTestCases;}

		Core::Array<TestCase*>  mTestCases;

		uint32 mNumTestCasesPassed;
		uint32 mNumTestCasesFailed;

	
	public:

		//
		// test helpers
		//

		// numbers
		static bool IsValid(float number) 							{ return ( std::isinf(number) || std::isnan(number) ) == false; }
		static bool IsValid(double number) 							{ return ( std::isinf(number) || std::isnan(number) ) == false; }
		
		// indices
		static bool IsValidIndex(uint16 index)						{ return index != CORE_INVALIDINDEX16; }
		static bool IsValidIndex(int16 index)						{ return index > 0; }
		static bool IsValidIndex(uint32 index)						{ return index != CORE_INVALIDINDEX32; }
		static bool IsValidIndex(int32 index)						{ return index > 0; }

		// strings
		static bool IsValid(const char* string)						{ return string != NULL && strlen(string) > 0; }
		static bool IsEmpty(const char* string)						{ return strlen(string) > 0; }
		static bool IsEqual(const char* a, const char* b)			{ return (strcmp(a, b) == 0); }

		// pointer 
		static bool IsValid(void * ptr)								{ return ptr != NULL; }

};

// use this to implement unit tests that can be expressed in a single statement
#define AssertTest (STATEMENT)	{ AddTest( new AssertTestCase(STATEMENT, "(STATEMENT) is not true") ); }

#endif
