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

#ifndef __CORE_LOGMANAGER_H
#define __CORE_LOGMANAGER_H

// include the required headers
#include "StandardHeaders.h"
#include "Array.h"
#include "String.h"
#include "Mutex.h"
#include "LogCallbacks.h"


namespace Core
{

class ENGINE_API LogManager
{
	public:
		LogManager();
		~LogManager();

		uint32 GetNumLogPresets() const																{ return mLogPresets.Size(); }
		const LogLevelPreset* GetLogPreset(uint32 index) const										{ return &mLogPresets[index]; }
		uint32 GetActiveLogPresetIndex() const														{ return mActiveLogPresetIndex; }
		const LogLevelPreset* GetActiveLogLevelPreset() const										{ if (mActiveLogPresetIndex == CORE_INVALIDINDEX32) return NULL; return &mLogPresets[mActiveLogPresetIndex]; }
		ELogLevel GetLogLevels() const;

		void SetActiveLogLevelPreset(const char* presetName);
		void SetActiveLogLevelPresetIndex(uint32 index);
		uint32 FindLogPresetIndexByName(const char* presetName) const;

		bool CreateLogFile(const char* filename);
		void AddLogCallback(LogCallback* callback);
		void RemoveLogCallback(LogCallback* callback, bool delFromMem=true);
		void ClearLogCallbacks();

		LogCallback* GetLogCallback(const uint32 index)												{ return mLogCallbacks[index]; }
		uint32 GetNumLogCallbacks() const															{ return mLogCallbacks.Size(); }
		uint32 FindLogCallback(LogCallback* callback) const;

		void LogMessage(const char* message, ELogLevel logLevel=LOGLEVEL_INFO);

		void LogCritical_Internal(const char* what, va_list args);
		void LogError_Internal(const char* what, va_list args);
		void LogWarning_Internal(const char* what, va_list args);
		void LogInfo_Internal(const char* what, va_list args);
		void LogDetailedInfo_Internal(const char* what, va_list args);
		void LogDebug_Internal(const char* what, va_list args);

	private:
		String					mLineFormattingBuffer;
		String					mVarArgBuffer;

		Array<LogCallback*>		mLogCallbacks;
		Array<LogLevelPreset>	mLogPresets;
		uint32					mActiveLogPresetIndex;
		Mutex					mLock;
};


// macros for simple multi level logtracing: use one of the following  CORE_LOGTRACE to add a method LogTrace(const char* methodName) to a class

// logtracing of non-realtime methods
#define CORE_LOGTRACE_ENABLE(CLASSNAME, OBJECTNAME, POINTER)												\
protected:																									\
 void LogTrace(const char* methodName, uint32 level = 1) {													\
   if (level <= 1) Core::LogDebug("0x%x  %s::%s() \t(%s)", POINTER, CLASSNAME, methodName, OBJECTNAME); }	\
 void LogTraceRT(const char* methodName) const {}															\

// enable trace logging in realtime codepaths
#define CORE_LOGTRACE_REALTIME(CLASSNAME, OBJECTNAME, POINTER)											\
protected:																								\
 void LogTrace(const char* methodName, uint32 level = 1) {												\
   if (level <= 2) Core::LogDebug("0x%x  %s::%s() \t(%s)", POINTER, CLASSNAME, methodName, OBJECTNAME); }	\
 void LogTraceRT(const char* methodName) const { LogTrace(methodName, 2); }								\

// disable the trace logging
#define CORE_LOGTRACE_DISABLE(CLASSNAME, OBJECTNAME, POINTER)			\
protected:																\
  void LogTrace(const char* methodName, uint32 level = 0)	const {}	\
  void LogTraceRT(const char* methodName, uint32 level = 0) const {}	\


// helper macro to define the logging body for LogDebug and LogDebugRT
//  the body creates a log line using the member mTempLogString and the three macro parameters, and finally appends the va args
#define CORE_LOGDEBUG_DEFINE_BODY(CLASSNAME, OBJECTNAME, POINTER)						\
	mTempLogString.Resize(1024);														\
	mTempLogString.Format("0x%x  %s (%s): ", POINTER, CLASSNAME, OBJECTNAME);			\
	va_list args;																		\
	va_start(args, what);																\
	char* start = mTempLogString.GetPtr() + mTempLogString.GetLength() * sizeof(char);	\
	vsprintf(start, what, args);														\
	va_end(args);																		\
	Core::LogDebug( mTempLogString.AsChar());

// macros for extended debug logging (select one of the three in classes that use it)
// note that CORE_LOGDEBUG_DISABLE creates empty methods and thus will be removed by the compiler
#define CORE_LOGDEBUG_ENABLE(CLASSNAME, OBJECTNAME, POINTER)													\
private:																										\
  Core::String	mTempLogString;																					\
public:																											\
  void LogDebug(const char* what, ...)   const { CORE_LOGDEBUG_DEFINE_BODY(CLASSNAME, OBJECTNAME, POINTER) }	\
  void LogDebugRT(const char* what, ...) const { }

// macros for extended debug logging
#define CORE_LOGDEBUG_REALTIME(CLASSNAME, OBJECTNAME, POINTER)													\
private:																										\
  Core::String	mTempLogString;																					\
public:																											\
  void LogDebug(const char* what, ...)   const { CORE_LOGDEBUG_DEFINE_BODY(CLASSNAME, OBJECTNAME, POINTER) }	\
  void LogDebugRT(const char* what, ...) const { CORE_LOGDEBUG_DEFINE_BODY(CLASSNAME, OBJECTNAME, POINTER) }

#define CORE_LOGDEBUG_DISABLE(CLASSNAME, OBJECTNAME, POINTER)	\
public:															\
  void LogDebug(const char* what, ...)   const { }					\
  void LogDebugRT(const char* what, ...) const { }

//----------------------------------------------------------------------------

void ENGINE_API LogCritical(const char* what, ...);
void ENGINE_API LogError(const char* what, ...);
void ENGINE_API LogWarning(const char* what, ...);
void ENGINE_API LogInfo(const char* what, ...);
void ENGINE_API LogDetailedInfo(const char* what, ...);
void ENGINE_API LogDebug(const char* what, ...);


} // namespace Core




#endif
