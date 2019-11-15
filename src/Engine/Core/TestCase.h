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

#ifndef __NEUROMORE_TESTCASE_H
#define __NEUROMORE_TESTCASE_H

#include "StandardHeaders.h"
#include "Array.h"
#include <iostream>


// base class for a single test case (the smallest testable unit)
class TestCase
{
	public:
		TestCase(const char* testName) : mTestName(testName), mPassed(false) {}
		virtual ~TestCase() {}

		virtual bool Run() = 0;
		bool Passed() const { return mPassed; }
		virtual const char* GetErrorDescription() const { return ""; }

		const char* GetName()		{ return mTestName; }

	private:
		const char* mTestName;

	protected:
		bool mPassed;
};

// simple statement test case, use this for writing tests if the unit test can be expressed in a single line
class AssertTestCase : public TestCase
{
	public:
		AssertTestCase(bool statement, const char* statementString) : TestCase("Bool Assertion"), mStatementString(statementString)		{ mPassed = statement; }
		virtual ~AssertTestCase() {}

		virtual bool Run() override	{ return mPassed; }
		const char* GetErrorDescription() const override	{ return mStatementString; }

	private:
		const char* mStatementString;
};


// the main assert function - use this to check statements inside Test implementations
#define Assert (STATEMENT)	{  new AssertTestCase(STATEMENT, "(STATEMENT) is not true") ); }


#endif
