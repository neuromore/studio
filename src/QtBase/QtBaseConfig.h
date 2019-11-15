/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_CONFIG_H
#define __QTBASE_CONFIG_H

#include <QString>
#include <QColor>
#include <QDateTime>
#include <Core/String.h>
#include <Core/Color.h>
#include <Core/Time.h>


// some qtbase related config options that can't be placed in Studio/Config.h

//#define HIDE_AUTOUPDATE_ERROR

//#define CUSTOM_DEFAULT_LAYOUT ":/Layouts/Neurofeedback.layout"

#define DEFAULT_ICONSIZE 20


// memory categories
enum
{
	MEMCATEGORY_QTBASE						= 1461,
	MEMCATEGORY_QTBASE_CUSTOMWIDGETS		= 1462,
	MEMCATEGORY_QTBASE_CUSTOMWINDOWS		= 1463,
	MEMCATEGORY_QTBASE_ATTRIBUTEWIDGETS		= 1464,
	MEMCATEGORY_QTBASE_DIRTYFILES			= 1465,
	MEMCATEGORY_QTBASE_NETWORK				= 1466,
	MEMCATEGORY_QTBASE_NETWORK_SERVER		= 1467,
	MEMCATEGORY_QTBASE_NETWORK_BACKEND		= 1468,
	MEMCATEGORY_QTBASE_PROGRESSWINDOW		= 1469,
	MEMCATEGORY_QTBASE_PLUGINS				= 1470,
	MEMCATEGORY_QTBASE_WIDGETS				= 1471,
	MEMCATEGORY_QTBASE_LAYOUT				= 1472,
	MEMCATEGORY_QTBASE_OSCLISTENER			= 1473,
	MEMCATEGORY_QTBASE_KEYBOARDSHORTCUTS	= 1474,
	MEMCATEGORY_QTBASE_SERIALPORT			= 1475,
	MEMCATEGORY_QTBASE_OPENGLMANAGER		= 1476,
	MEMCATEGORY_QTBASE_STATICTEXTCACHE		= 1477,
	MEMCATEGORY_QTBASE_FILEDOWNLOADER		= 1478,
	MEMCATEGORY_QTBASE_WEBDATACACHE			= 1479,
	MEMCATEGORY_QTBASE_AUDIO				= 1480,
	MEMCATEGORY_QTBASE_BACKEND_DATA			= 1481,
	MEMCATEGORY_QTBASE_REQUEST				= 1482,
	MEMCATEGORY_QTBASE_RESPONSE				= 1483
};


// convert from a QString into an Core::String
inline Core::String FromQtString(const QString& s)
{
	return s.toUtf8().data();
}


// convert from a QString into an Core::String
inline void FromQtString(const QString& s, Core::String* result)
{
	(*result) = s.toUtf8().data();
}


// convert from our to a Qt color
inline QColor ToQColor(const Core::Color& color)
{
	return QColor( color.r*255.0, color.g*255.0, color.b*255.0, color.a*255.0 );
}

// convert from our to a Qt color
inline Core::Color FromQtColor(const QColor& color)
{
	return Core::Color( (float)color.red()/255.0f, (float)color.green()/255.0f, (float)color.blue()/255.0f, (float)color.alpha()/255.0f );
}



inline Core::Time FromQtTime(const QDateTime& dateTime)
{
	qint64 milliSeconds	= dateTime.toMSecsSinceEpoch();
	uint64 seconds		= milliSeconds / 1000;
	
	// original:
	// uint32 nanoseconds = (milliSeconds * 1000000 - (seconds * 1000000000);
	
	// optimized:
	uint32 nanoseconds = (milliSeconds - (seconds * 1000)) * 1000000;

	return Core::Time(seconds, nanoseconds);
}


inline QDateTime ToQtTime(const Core::Time& time)
{
	qint64 msecs = (time.mSeconds * 1000) + (time.mNanoSeconds / 1000000);

	QDateTime result;
	result.setMSecsSinceEpoch( msecs );
	return result;
}


// DLL import & export
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#ifdef QTBASE_DLL_IMPORT
		#define QTBASE_API	__declspec(dllimport)
	#elif QTBASE_DLL_EXPORT
		#define QTBASE_API	__declspec(dllexport)
	#else
		#define QTBASE_API
	#endif
#else
	#define QTBASE_API
#endif


#endif
