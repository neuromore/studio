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
#include <Studio/Precompiled.h>

#include "Devices/DriverInventory.h"
#include "MainWindow.h"
#include <AutoUpdate/AutoUpdate.h>

// Library Linking
#if defined(NEUROMORE_PLATFORM_WINDOWS)

   // STATIC QT PLATFORM PLUGIN
   Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);

   // STATIC QT MEDIASERVICE PLUGIN
   //Q_IMPORT_PLUGIN(DSServicePlugin);   // DirectShow
   Q_IMPORT_PLUGIN(WMFServicePlugin);    // Windows Media Foundation
   Q_IMPORT_PLUGIN(QWindowsAudioPlugin); // Windows Audio

   // PROPRIETARY LIBS WITH NO RUNTIME LOADING BUT LIB FOR LINKER ONLY
   #ifdef INCLUDE_DEVICE_TOBIIEYEX
      #pragma comment(lib, "Tobii.EyeX.Client.lib")
   #endif
   #ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
   #if _M_X64
      #pragma comment(lib, "thinkgear64.lib")
   #else
      #pragma comment(lib, "thinkgear.lib")
   #endif
   #endif
   #ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
      #pragma comment (lib, "ABM_Athena.lib")
      #pragma comment (lib, "ABM_CommLib.lib")
      #pragma comment (lib, "ABM_PlayEbs.lib")
      #pragma comment (lib, "ABM_Datastreaming.lib")
      #pragma comment (lib, "ABM_DeviceConfig.lib")
   #endif
   #ifdef INCLUDE_DEVICE_EMOTIV
      #pragma comment(lib, "edk.lib")
   #endif
   #ifdef INCLUDE_DEVICE_BRAINQUIRY
      #pragma comment(lib, "BQPetDLL.lib")
   #endif

#elif defined(NEUROMORE_PLATFORM_LINUX)
   // STATIC QT PLUGIN
   Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
   Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin);
   Q_IMPORT_PLUGIN(QGstreamerAudioDecoderServicePlugin);
   Q_IMPORT_PLUGIN(CameraBinServicePlugin);
   Q_IMPORT_PLUGIN(QGstreamerCaptureServicePlugin);
   Q_IMPORT_PLUGIN(QGstreamerPlayerServicePlugin);
#elif defined(NEUROMORE_PLATFORM_OSX)
   // STATIC QT PLUGIN
   Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
   Q_IMPORT_PLUGIN(AVFServicePlugin);
   Q_IMPORT_PLUGIN(AVFMediaPlayerServicePlugin);
#endif

// STATIC QT PLUGINS FOR ALL PLATFORMS
Q_IMPORT_PLUGIN(QGifPlugin);
Q_IMPORT_PLUGIN(QICOPlugin);
Q_IMPORT_PLUGIN(QJpegPlugin);

using namespace Core;

// Entry Point for Windows Production (No Console, requires /subsystem:WINDOWS)
#if defined(NEUROMORE_PLATFORM_WINDOWS) && defined(PRODUCTION_BUILD)
int APIENTRY WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR     lpCmdLine,
   int       nCmdShow)
{
   int    argc = __argc;
   char** argv = __argv;
#else
// Entry Point for All Others (with Console)
int main(int argc, char *argv[])
{
#endif

	// initialize core helper system
	if (EngineInitializer::Init() == false)
	{
		Core::LogError( "Failed to initialize the neuromore Engine." );
		return -1;
	}

	// init Qt base
	if (QtBaseInitializer::Init("") == false)
	{
		Core::LogCritical("Failed to initialize the Qt base manager.");
		EngineInitializer::Shutdown();			// shutdown neuro system
		return -1;
	}

	// init studio core
	if (AppInitializer::Init(argc, argv) == false)
	{
		Core::LogCritical("Failed to initialize application.");
		QtBaseInitializer::Shutdown();			// shutdown QtBase
		EngineInitializer::Shutdown();			// shutdown Core system
		return -1;
	}

    // show the main window
	int returnCode = GetManager()->ExecuteApp();

	// shutdown the example
	Core::LogInfo( "Shutting down" );

	// shutdown libraries
	AppInitializer::Shutdown();				// shutdown studio core
	QtBaseInitializer::Shutdown();			// shutdown QtBase
	EngineInitializer::Shutdown();			// shutdown Core system

	return returnCode;
}
