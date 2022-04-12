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

#include <Core/StandardHeaders.h>
#include <QApplication>
#include <QMessageBox>
#include <Core/LogManager.h>
#include "Devices/DriverInventory.h"
#include <QtBaseManager.h>
#include <EngineManager.h>
#include "MainWindow.h"
#include <AutoUpdate/AutoUpdate.h>
#include "AppManager.h"
#include "CrashReporter.h"
#include <QtPlugin>

// Library Linking
#if defined(NEUROMORE_PLATFORM_WINDOWS)

   // STATIC QT PLATFORM PLUGIN
   Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);

   // STATIC QT MEDIASERVICE PLUGIN
   //Q_IMPORT_PLUGIN(DSServicePlugin);    // DirectShow
   Q_IMPORT_PLUGIN(WMFServicePlugin); // Windows Media Foundation

   // WINAPI
   #pragma comment(lib, "Version.lib")          // winapi: version
   #pragma comment(lib, "Bthprops.lib")         // winapi: bluetooth
   #pragma comment(lib, "Setupapi.lib")         // winapi: setup
   #pragma comment(lib, "Vfw32.lib")            // winapi: VideoForWindows
   #pragma comment(lib, "Rpcrt4.lib")           // winapi: Rpcrt
   #pragma comment(lib, "Ws2_32.lib")           // winapi: sockets
   #pragma comment(lib, "Iphlpapi.lib")         // winapi: ip helper
   #pragma comment(lib, "Imm32.lib")            // winapi: internationalization
   #pragma comment(lib, "winmm.lib")            // winapi: multimedia
   #pragma comment(lib, "Crypt32.lib")          // winapi: crypto
   #pragma comment(lib, "Netapi32.lib")         // winapi: netapi
   #pragma comment(lib, "Userenv.lib")          // winapi: userenv
   #pragma comment(lib, "Dwmapi.lib")           // winapi: desktop window manager
   #pragma comment(lib, "Wtsapi32.lib")         // winapi: remote desktop services
   #pragma comment(lib, "Gdi32.lib")            // winapi: gdi
   #pragma comment(lib, "WindowsApp.lib")       // winapi: WinRT basics
   #pragma comment(lib, "dxva2.lib")            // winapi: DirectX Video Acceleration
   #pragma comment(lib, "D3d9.lib")             // winapi: Direct3D 9
   #pragma comment(lib, "evr.lib")              // winapi: media foundation
   #pragma comment(lib, "mf.lib")               // winapi: media foundation
   #pragma comment(lib, "mfplat.lib")           // winapi: media foundation
   #pragma comment(lib, "mfplay.lib")           // winapi: media foundation
   #pragma comment(lib, "mfreadwrite.lib")      // winapi: media foundation
   #pragma comment(lib, "mfuuid.lib")           // winapi: media foundation
   #pragma comment(lib, "wmcodecdspuuid.lib")   // winapi: 
   #pragma comment(lib, "strmiids.lib")         // winapi: 
   #pragma comment(lib, "opengl32.lib")         // opengl
   #pragma comment(lib, "glu32.lib")            // opengl

   // PROPRIETARY (TOBIIEYEX)
   #ifdef INCLUDE_DEVICE_TOBIIEYEX
      #pragma comment(lib, "Tobii.EyeX.Client.lib")
   #endif

   // PROPRIETARY (NEUROSKY MINDWAVE)
   #ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE
   #if _M_X64
      #pragma comment(lib, "thinkgear64.lib")
   #else
      #pragma comment(lib, "thinkgear.lib")
   #endif
   #endif

   // PROPRIETARY (ABM)
   #ifdef INCLUDE_DEVICE_ADVANCEDBRAINMONITORING
      #pragma comment (lib, "ABM_Athena.lib")
      #pragma comment (lib, "ABM_CommLib.lib")
      #pragma comment (lib, "ABM_PlayEbs.lib")
      #pragma comment (lib, "ABM_Datastreaming.lib")
      #pragma comment (lib, "ABM_DeviceConfig.lib")
   #endif

   // PROPRIETARY (EMOTIV)
   #ifdef INCLUDE_DEVICE_EMOTIV
      #pragma comment(lib, "edk.lib")
   #endif

   // PROPRIETARY (EEMAGINE)
   #ifdef INCLUDE_DEVICE_EEMAGINE
      #pragma comment(lib, "eego-SDK.lib")
   #endif

   // PROPRIETARY (EMOTIV)
   #ifdef INCLUDE_DEVICE_BRAINQUIRY
      #pragma comment(lib, "BQPetDLL.lib")
   #endif

   // OPENSOURCE / NEUROMORE
   #if _DEBUG
      #pragma comment(lib, "DbgHelp.lib")                // winapi: debug
      #pragma comment(lib, "edflib_d.lib")               // 3rdparty: EDFlib
      #pragma comment(lib, "kissfft_d.lib")              // 3rdparty: KissFFT
      #pragma comment(lib, "libpng_d.lib")               // 3rdparty: LibPNG
      #pragma comment(lib, "libjpeg_d.lib")              // 3rdparty: LibJPEG
      #pragma comment(lib, "oscpack_d.lib")              // 3rdparty: OSC
      #pragma comment(lib, "libsvm_d.lib")               // 3rdparty: libsvm
      #pragma comment(lib, "dspfilters_d.lib")           // 3rdparty: DspFilters
      #pragma comment(lib, "wavelib_d.lib")              // 3rdparty: wavelib
      #pragma comment(lib, "brainflow_d.lib")                 // 3rdparty: brainflow
      #pragma comment(lib, "brainflow-boardcontroller_d.lib") // 3rdparty: brainflow
      #ifdef OPENCV_SUPPORT
         #pragma comment(lib, "opencv-core_d.lib")       // 3rdparty: OpenCV Core
         #pragma comment(lib, "opencv-imgcodecs_d.lib")  // 3rdparty: OpenCV Image Codecs
         #pragma comment(lib, "opencv-imgproc_d.lib")    // 3rdparty: OpenCV Image Processing
         #pragma comment(lib, "opencv-videoio_d.lib")    // 3rdparty: OpenCV VideoIO
      #endif
      #pragma comment(lib, "tinyaes_d.lib")              // 3rdparty: TinyAES
      #pragma comment(lib, "tinyobjloader_d.lib")        // 3rdparty: tinyobjloader
      #pragma comment(lib, "zlib_d.lib")                 // 3rdparty: zlib
      #ifdef USE_CRASHREPORTER
         #pragma comment(lib, "crashrpt_d.lib")          // 3rdparty: crashreport
         #pragma comment(lib, "crashrptprobe_d.lib")     // 3rdparty: crashreport
      #endif
      #pragma comment(lib, "double-conversion_d.lib")    // 3rdparty: double-conversion
      #pragma comment(lib, "libcrypto_d.lib")            // 3rdparty: openssl libcrypto
      #pragma comment(lib, "libssl_d.lib")               // 3rdparty: openssl libssl
      #pragma comment(lib, "pcre2_d.lib")                // 3rdparty: perl c regular expressions
      #pragma comment(lib, "harfbuzz_d.lib")             // 3rdparty: harfbuzz
      #pragma comment(lib, "harfbuzz-ng_d.lib")          // 3rdparty: harfbuzz nextgen
      #pragma comment(lib, "freetype_d.lib")             // 3rdparty: freetype
      #pragma comment(lib, "singleapplication_d.lib")    // 3rdparty: singleapplication
      #pragma comment(lib, "qt-bluetooth_d.lib")         // 3rdparty: qt bluetooth
      #pragma comment(lib, "qt-core_d.lib")              // 3rdparty: qt core
      #pragma comment(lib, "qt-concurrent_d.lib")        // 3rdparty: qt concurrent
      #pragma comment(lib, "qt-eventdispatchers_d.lib")  // 3rdparty: qt eventdispatchers
      #pragma comment(lib, "qt-fontdatabases_d.lib")     // 3rdparty: qt fontdatabases
      #pragma comment(lib, "qt-gamepad_d.lib")           // 3rdparty: qt gamepad
      #pragma comment(lib, "qt-gui_d.lib")               // 3rdparty: qt gui
      #pragma comment(lib, "qt-multimedia_d.lib")        // 3rdparty: qt multimedia
      #pragma comment(lib, "qt-network_d.lib")           // 3rdparty: qt network
      #pragma comment(lib, "qt-multimediaplugins_d.lib") // 3rdparty: qt multimedia plugins
      #pragma comment(lib, "qt-multimediawidgets_d.lib") // 3rdparty: qt multimedia widgets
      #pragma comment(lib, "qt-widgets_d.lib")           // 3rdparty: qt widgets
      #pragma comment(lib, "qt-opengl_d.lib")            // 3rdparty: qt opengl
      #pragma comment(lib, "qt-printsupport_d.lib")      // 3rdparty: qt printsupport
      #pragma comment(lib, "qt-serialport_d.lib")        // 3rdparty: qt serialport
      #pragma comment(lib, "qt-xml_d.lib")               // 3rdparty: qt xml
      #pragma comment(lib, "qt-platform-windows_d.lib")  // 3rdparty: qt xml
      #pragma comment(lib, "Engine_d.lib")               // neuromore engine
      #pragma comment(lib, "QtBase_d.lib")               // neuromore qtbase
   #else
      #pragma comment(lib, "edflib.lib")
      #pragma comment(lib, "kissfft.lib")
      #pragma comment(lib, "libpng.lib")
      #pragma comment(lib, "libjpeg.lib")
      #pragma comment(lib, "oscpack.lib")
      #pragma comment(lib, "libsvm.lib")
      #pragma comment(lib, "dspfilters.lib")
      #pragma comment(lib, "wavelib.lib")
      #pragma comment(lib, "brainflow.lib")
      #pragma comment(lib, "brainflow-boardcontroller.lib")
      #ifdef OPENCV_SUPPORT
         #pragma comment(lib, "opencv-core.lib")
         #pragma comment(lib, "opencv-imgcodecs.lib")
         #pragma comment(lib, "opencv-imgproc.lib")
         #pragma comment(lib, "opencv-videoio.lib")
      #endif
      #pragma comment(lib, "tinyaes.lib")
      #pragma comment(lib, "tinyobjloader.lib")
      #pragma comment(lib, "zlib.lib")
      #ifdef USE_CRASHREPORTER
         #pragma comment(lib, "crashrpt.lib")
         #pragma comment(lib, "crashrptprobe.lib")
      #endif
      #pragma comment(lib, "double-conversion.lib")
      #pragma comment(lib, "libcrypto.lib")
      #pragma comment(lib, "libssl.lib")
      #pragma comment(lib, "pcre2.lib")
      #pragma comment(lib, "harfbuzz.lib")
      #pragma comment(lib, "harfbuzz-ng.lib")
      #pragma comment(lib, "freetype.lib")
      #pragma comment(lib, "singleapplication.lib")
      #pragma comment(lib, "qt-bluetooth.lib")
      #pragma comment(lib, "qt-core.lib")
      #pragma comment(lib, "qt-concurrent.lib")
      #pragma comment(lib, "qt-eventdispatchers.lib")
      #pragma comment(lib, "qt-fontdatabases.lib")
      #pragma comment(lib, "qt-gamepad.lib")
      #pragma comment(lib, "qt-gui.lib")
      #pragma comment(lib, "qt-multimedia.lib")
      #pragma comment(lib, "qt-network.lib")
      #pragma comment(lib, "qt-multimediaplugins.lib")
      #pragma comment(lib, "qt-multimediawidgets.lib")
      #pragma comment(lib, "qt-widgets.lib")
      #pragma comment(lib, "qt-opengl.lib")
      #pragma comment(lib, "qt-printsupport.lib")
      #pragma comment(lib, "qt-serialport.lib")
      #pragma comment(lib, "qt-xml.lib")
      #pragma comment(lib, "qt-platform-windows.lib")
      #pragma comment(lib, "Engine.lib")
      #pragma comment(lib, "QtBase.lib")
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// crash reporting system
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_CRASHREPORTER
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		// first check if it is safe to call functions from CrashRpt.dll
		// OBSOLETE
		/*if (CrashReporterCheckVersion() == false)
		{
			MessageBox(NULL, L"The CrashRpt dll cannot be found or has a wrong version! Please contact the neuromore support team.\n\nneuromore Studio cannot launch." , L"Critical Error", MB_OK|MB_ICONEXCLAMATION);
			return 666;
		}*/
		
		// try to init the crash reporter
		if (CrashReporterInit() == false)
			return 666;
	#endif
#endif

	// initialize core helper system
	if (EngineInitializer::Init() == false)
	{
		Core::LogError( "Failed to initialize the neuromore Engine." );
		#ifdef USE_CRASHREPORTER
			CrashReporterShutdown();			// shutdown crash reporter
		#endif
		return -1;
	}

	// init Qt base
	if (QtBaseInitializer::Init("") == false)
	{
		Core::LogCritical("Failed to initialize the Qt base manager.");
		EngineInitializer::Shutdown();			// shutdown neuro system
		#ifdef USE_CRASHREPORTER
			CrashReporterShutdown();			// shutdown crash reporter
		#endif
		return -1;
	}

	// init studio core
	if (AppInitializer::Init(argc, argv) == false)
	{
		Core::LogCritical("Failed to initialize application.");
		QtBaseInitializer::Shutdown();			// shutdown QtBase
		EngineInitializer::Shutdown();			// shutdown Core system
		#ifdef USE_CRASHREPORTER
			CrashReporterShutdown();			// shutdown crash reporter
		#endif
		return -1;
	}
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // auto updater
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_AUTOUPDATE
	// start with ignoreErrors=true so that in case something isn't working with the update tool the user is not bothered
	if (AutoUpdate::IsUpdateAvailable(true) == true)
    {
		//if (QMessageBox::question(NULL, "Update Available", "Would you like to install the available update? Click Yes to install the update or no to skip updating this time.", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
		//if (MessageBox( NULL, L"Would you like to install the available update? Click Yes to install the update or no to skip updating this time.", L"Update Available", MB_YESNO | MB_ICONQUESTION|MB_TOPMOST) == IDYES)
		{
            // start the auto updater and quit directly
            AutoUpdate::StartUpdateTool();
            
			// DON'T destruct memory here, the AutoUpdate automatically closes the application

#ifdef USE_CRASHREPORTER
            CrashReporterShutdown();                // shutdown crash reporter
#endif
            
            return -1;
        }
    }
#endif

    // show the main window
	int returnCode = GetManager()->ExecuteApp();

	// shutdown the example
	Core::LogInfo( "Shutting down" );

	// shutdown libraries
	AppInitializer::Shutdown();				// shutdown studio core
	QtBaseInitializer::Shutdown();			// shutdown QtBase
	EngineInitializer::Shutdown();			// shutdown Core system

	#ifdef USE_CRASHREPORTER
		CrashReporterShutdown();			// shutdown crash reporter
	#endif

	return returnCode;
}
