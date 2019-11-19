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

#ifndef __NEUROMORE_CONFIG_H
#define __NEUROMORE_CONFIG_H

#include <QString>
#include <Core/StandardHeaders.h>
#include <Core/String.h>

// developer mode
// feel free to enable or disable defines
#ifndef PRODUCTION_BUILD
	
	// misc dev stuff
	//#define FORCE_LOCALHOST					// uncomment this to limit the server announce to the local machine
	//#define SCREENSHOT_MODE					// used to make screenshots of the graph (white background, no error state, no connection values etc.)
	//#define FULLSCREEN_MODE					// remove window decoration so the window can occupy the whole screen (for windows screenshots)
	//#define FORCE_DEBUGLOGGING				// enable debug log even if settings are disabled
	
	// backend
	#define BACKEND_LOGGING

	// updates
	//#define USE_AUTOUPDATE					// comment this if we don't want to use myBuild's auto updater anymore

	// disable memory manager in debug
	//#define CORE_DISABLE_MEMORYINFO

	// Qt Data Visualization

    //#ifdef NEUROMORE_PLATFORM_WINDOWS
    //    #define USE_QTDATAVISUALIZATION
    //#endif

	// OpenCV support
    #ifdef NEUROMORE_PLATFORM_WINDOWS
        //#define OPENCV_SUPPORT //TODO: RE-enable once integrated in clang+make
    #endif


	// crash reporting system
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		//#define USE_CRASHREPORTER				// comment this if we don't want to use the CrashRpt system
	#endif

	// loreta debug mode
	#define LORETA_DEBUG
#endif


// IMPORTANT NOTE:
// DO NOT EDIT THIS BLOCK WHILE DEVELOPING !!!
// HAT OF SHAME IS WAITING FOR YOU !!! ;)
#ifdef PRODUCTION_BUILD
	// back-end
	#define BACKEND_LOGGING

    // Qt Data Visualization

    //#ifdef NEUROMORE_PLATFORM_WINDOWS
    //    #define USE_QTDATAVISUALIZATION
    //#endif

	// OpenCV support
    #ifdef NEUROMORE_PLATFORM_WINDOWS
        #define OPENCV_SUPPORT
    #endif


	// updates
	#define USE_AUTOUPDATE

	// crash reporting system
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		//#define USE_CRASHREPORTER
	#endif
#endif


// memory categories
enum
{
	MEMCATEGORY_BASEAPP							= 5991,
	MEMCATEGORY_PLUGINS							= 5992,
	MEMCATEGORY_WIDGETS							= 5993,
	MEMCATEGORY_WINDOWS							= 5994,
	MEMCATEGORY_GRAPH_INTERFACE					= 5995,
	MEMCATEGORY_OPENCV_VIDEOPLAYER				= 5996,

	// rendering
	MEMCATEGORY_RENDERING						= 5801,
	MEMCATEGORY_RENDERING_MESH					= 5804,
	MEMCATEGORY_RENDERING_OPENGLMANAGER			= 5805,
	MEMCATEGORY_RENDERING_OPENGLWIDGET			= 5806,
	MEMCATEGORY_RENDERING_TEXTURES				= 5807
};


// Studio Networking Config default values

// default server TCP and UDP ports
#define STUDIO_NETWORKCLIENT_UDP_PORT	45554			// port of client where the data is send to
#define STUDIO_NETWORKSERVER_UDP_PORT	45555			// local udp port for outgoing studio->client udp data transfer
#define STUDIO_NETWORKSERVER_TCP_PORT	45456			// local tcp port where we listen to incoming connections from the clients

// default osc listener port
#define STUDIO_OSCLISTENER_UDP_PORT		4545			// port for OSC input nodes
#define STUDIO_OSCREMOTE_UDP_PORT		4546			// port for OSC output nodes

// client timeout duration in seconds
#define STUDIO_NETWORKSERVER_CLIENT_TIMEOUT				5	

// server announce frequency in Hertz
#define STUDIO_NETWORKSERVER_ANNOUNCE_INTERVAL			2

// realtime update frequency in Hertz (rate of UDP packages)
#define STUDIO_NETWORKSERVER_REALTIME_UPDATE_FREQUENCY	60

#endif
