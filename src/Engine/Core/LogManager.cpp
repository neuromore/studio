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

// include the Core headers
#include "LogManager.h"
#include "../EngineManager.h"
#include "Time.h"


namespace Core
{

// constructor
LogManager::LogManager()
{

	mLogPresets.Add( LogLevelPreset("None", LOGLEVEL_NONE) );
	mLogPresets.Add( LogLevelPreset("Errors + Warnings", (ELogLevel)(LOGLEVEL_CRITICAL | LOGLEVEL_ERROR | LOGLEVEL_WARNING)) );
	mLogPresets.Add( LogLevelPreset("Info", (ELogLevel)(LOGLEVEL_CRITICAL | LOGLEVEL_ERROR | LOGLEVEL_WARNING | LOGLEVEL_INFO)) );
	mLogPresets.Add( LogLevelPreset("Detailed Info", (ELogLevel)(LOGLEVEL_CRITICAL | LOGLEVEL_ERROR | LOGLEVEL_WARNING | LOGLEVEL_INFO | LOGLEVEL_DETAILEDINFO)) );
	mLogPresets.Add( LogLevelPreset("Debug", (ELogLevel)(LOGLEVEL_CRITICAL | LOGLEVEL_ERROR | LOGLEVEL_WARNING | LOGLEVEL_INFO | LOGLEVEL_DETAILEDINFO | LOGLEVEL_DEBUG)) );
	
	// errors + warnings as default
	mActiveLogPresetIndex = 1;

	// init temporary string to a generous size (32k)
	mLineFormattingBuffer.Reserve(32*1024);
	mVarArgBuffer.Reserve(32*1024);
}


// destructor
LogManager::~LogManager()
{
	// get rid of the callbacks
	ClearLogCallbacks();
}


ELogLevel LogManager::GetLogLevels() const
{
	if (mActiveLogPresetIndex == CORE_INVALIDINDEX32)
		return ELogLevel::LOGLEVEL_NONE;

	return mLogPresets[mActiveLogPresetIndex].GetLogLevel();
}


bool LogManager::CreateLogFile(const char* filename)
{
	// add log file callback to the stack
	AddLogCallback( new LogFileCallback(filename) );
	return true;
}


void LogManager::AddLogCallback(LogCallback* callback)
{
	mLock.Lock();

	if (callback != NULL)
		mLogCallbacks.Add( callback );

	mLock.Unlock();
}


void LogManager::RemoveLogCallback(LogCallback* callback, bool delFromMem)
{
	mLock.Lock();

	const uint32 index = FindLogCallback(callback);
	if (mLogCallbacks.IsValidIndex(index) == false)
		return;

	if (delFromMem == true)
		delete mLogCallbacks[index];

	mLogCallbacks.Remove( index );

	mLock.Unlock();
}


void LogManager::ClearLogCallbacks()
{
	mLock.Lock();

	const uint32 num = mLogCallbacks.Size();
	for (uint32 i=0; i<num; ++i)
		delete mLogCallbacks[i];

	mLogCallbacks.Clear(true);

	mLock.Unlock();
}


// the main logging method
void LogManager::LogMessage(const char* message, ELogLevel logLevel)
{
	// NOTE: thread lock _MUST_ be handled in the calling function.
	
	// append timestamp
	mLineFormattingBuffer.Format("%s: %s", Time::Now().Format("%Y-%m-%d %H:%M:%S.%f").AsChar(), message);
	
	// iterate through all callbacks
	const uint32 num = mLogCallbacks.Size();
	for (uint32 i=0; i<num; ++i)
	{
		LogCallback* callback = mLogCallbacks[i];

		if (callback->IsEnabled() == true)
			callback->Log( mLineFormattingBuffer.AsChar(), logLevel );
	}
}


// find the index of a given callback
uint32 LogManager::FindLogCallback(LogCallback* callback) const
{
	// iterate through all callbacks
	const uint32 num = mLogCallbacks.Size();
	for (uint32 i=0; i<num; ++i)
	{
		if (mLogCallbacks[i] == callback)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// find the preset with the given name
uint32 LogManager::FindLogPresetIndexByName(const char* presetName) const
{
	const uint32 numPresets = mLogPresets.Size();
	for (uint32 i=0; i<numPresets; ++i)
	{
		if (mLogPresets[i].GetNameString().IsEqual(presetName) == true)
			return i;
	}

	return CORE_INVALIDINDEX32;
}


// activate the log preset with the given name
void LogManager::SetActiveLogLevelPreset(const char* presetName)
{
	const uint32 index = FindLogPresetIndexByName(presetName);
	if (index == CORE_INVALIDINDEX32)
	{
		LogError("Cannot find and switch to log level preset with the name '%s'. ", presetName);
		return;
	}

	// switch to the preset
	SetActiveLogLevelPresetIndex(index);
}


// activate the log preset with the given index
void LogManager::SetActiveLogLevelPresetIndex(uint32 index)
{
	if (index >= mLogPresets.Size())
	{
		LogError("Cannot switch to log level preset. Index %i out of range.", index);
		return;
	}

	mActiveLogPresetIndex = index;
}


void LogManager::LogCritical_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_CRITICAL)
	{
		mVarArgBuffer.Clear();

		// construct the final log line
		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_CRITICAL );
	}

	mLock.Unlock();
}


void LogManager::LogError_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_ERROR)
	{
		mVarArgBuffer.Clear();

		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_ERROR );
	}

	mLock.Unlock();
}


void LogManager::LogWarning_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_WARNING)
	{
		mVarArgBuffer.Clear();

		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_WARNING );
	}

	mLock.Unlock();
}


void LogManager::LogInfo_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_INFO)
	{
		mVarArgBuffer.Clear();

		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_INFO );
	}

	mLock.Unlock();
}


void LogManager::LogDetailedInfo_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DETAILEDINFO)
	{
		mVarArgBuffer.Clear();
		
		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_DETAILEDINFO );
	}

	mLock.Unlock();
}


void LogManager::LogDebug_Internal(const char* what, va_list args)
{
	mLock.Lock();

	// skip the va list construction in case that the message won't be logged by any of the callbacks
	if (CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DEBUG)
	{
		mVarArgBuffer.Clear();

		vsprintf(mVarArgBuffer.AsChar(), what, args);
		va_end(args);

		// log the message
		CORE_LOGMANAGER.LogMessage( mVarArgBuffer.AsChar(), LOGLEVEL_DEBUG );
	}

	mLock.Unlock();
}

void ENGINE_API LogCritical(const char* what, ...)						{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogCritical_Internal(what, args);}
void ENGINE_API LogError(const char* what, ...)							{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogError_Internal(what, args);}
void ENGINE_API LogWarning(const char* what, ...)						{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogWarning_Internal(what, args);}
void ENGINE_API LogInfo(const char* what, ...)							{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogInfo_Internal(what, args);}
void ENGINE_API LogDetailedInfo(const char* what, ...)					{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogDetailedInfo_Internal(what, args);}
void ENGINE_API LogDebug(const char* what, ...)							{ va_list args; va_start(args, what); CORE_LOGMANAGER.LogDebug_Internal(what, args);}

} // namespace Core
