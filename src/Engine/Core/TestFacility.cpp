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

#include "TestFacility.h"


TestFacility::~TestFacility()
{
	const uint32 numTests = GetNumTests();

	for (uint32 i=0; i<numTests; i++)
		delete mTests[i];

}

// run all testsuites within the facility call this directly from main
bool TestFacility::Run(int argc, char** args)
{
	std::cout << "Starting neuromore Tests for '" << GetName() << "' ..." << std::endl;

	std::cout << "Initializing Test Facility ... " << std::endl;
	//Setup();

	// calc total number of testcases
	const uint32 numTests = GetNumTests();
	uint32 numTestCases = 0;
	for (uint32 i=0; i<numTests; i++)
		numTestCases += mTests[i]->GetNumTestCases();
			std::cout << "Initializing ..." << numTests << " Tests ... ";

	// TODO evaluate args hers
	//  -l		   list all tests
	//  -e [a..b]  execute test from index a to b inclusive
	//  -n #	   num threads to use
	
	std::cout << "Starting Tests" << std::endl;

	bool passed = true;
	uint32 numTestsFailed = 0;
	uint32 numTestsPassed = 0;

	// run all tests
	for (uint32 i=0; i<numTests; i++)
	{
		bool testPassed = false;

		Test* test = mTests[i];
		std::cout << "------------------------------- Starting Test '" << test->GetName() << "' (" << i << " / " << numTests << ") ------------------------------" << std::endl;
		std::cout << "Initializing ..." << numTests << " Tests ... ";
		mTests[i]->Setup();
		std::cout << "done." << std::endl;

		testPassed = test->Run();
		
		std::cout << "------------------------------- Test '" << test->GetName() << "' ("<< i << " / " << numTests << ") " << (testPassed ? " PASSED " : " FAILED" ) << "------------------------------" << std::endl;

		if (testPassed == true)
		{
			numTestsPassed++;
		
		
		} 
		else
		{
			numTestsFailed++;
			passed = false;
		}
	}

	// final summary output
	if (passed == true)
	{
		std::cout << "======================================== ALL TESTS PASSED ========================================" << std::endl;
	}
	{
		std::cout << "======================================== " << numTestsFailed  << " TEST" << (numTestsFailed == 1 ? "" : "S") << " FAILED ========================================" << std::endl;
		
		// TODO log fail summary here
		std::cout << "Summary:" << std::endl << "  TODO " << std::endl;
	}

	return passed;
}
