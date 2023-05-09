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

#include "Test.h"


Test::Test(const char* name) : mTestName(name)
{
	mNumTestCasesPassed = 0;
	mNumTestCasesFailed = 0;

}


Test::~Test()
{
	const uint32 numTestCases = GetNumTestCases();
	for (uint32 i=0; i<numTestCases; i++)
		delete mTestCases[i];
	
}


bool Test::Run()
{
	Reset();

	bool passed = true;

	const uint32 numTestCases = GetNumTestCases();
	for (uint32 i=0; i<numTestCases; i++)
	{
		std::cout << "[" << i << " / " << numTestCases << "] ";

		bool testPassed = false;
		bool exception = false;

		TestCase* test = mTestCases[i];
		if (test == NULL)
		{
			testPassed = false;
		}
		else
		{
			// test execution 
			try
			{
				testPassed = test->Run();
					
			}			
			catch (...)	// catches all exceptions
			{
				testPassed = test->Run();
				exception = true;

			}	

			// evaluate result
			if (testPassed == true)
			{
				std::cout << " PASSED " << std::endl;
				mNumTestCasesPassed++;
			}
			else
			{
				std::cout << " <<<<< FAILED >>>>>" << ( exception ? "(exception)" : "" ) << std::endl;
				mNumTestCasesFailed++;
					
				passed = false;
			}
		
		}
	}

	return passed;
}
