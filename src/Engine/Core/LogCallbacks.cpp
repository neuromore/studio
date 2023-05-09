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

// include the required headers
#include "LogCallbacks.h"
#include "LogManager.h"


namespace Core
{

// constructor
LogCallback::LogCallback()
{
	mIsEnabled = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// constructor
LogFileCallback::LogFileCallback(const char* filename) : LogCallback()
{
	mLogFile = fopen(filename, "wt\0");
}


// destructor
LogFileCallback::~LogFileCallback()
{
	fclose(mLogFile);
}


// log callback function
void LogFileCallback::Log(const char* text, const ELogLevel logLevel)
{
	// output to the Visual Studio debug window
#ifdef NEUROMORE_PLATFORM_WINDOWS
	OutputDebugStringA( text );
	OutputDebugStringA( "\n" );
#else
	printf( "%s\n", text );
#endif
    
	// write the log line
	if (mLogFile != NULL)
	{
		fputs( text, mLogFile );
		fputs( "\n", mLogFile );
		fflush(mLogFile);
	}
}

} // namespace Core
