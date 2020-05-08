

#include <Core/StandardHeaders.h>
#include <QApplication>
#include <QMessageBox>
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <EngineManager.h>
#include <AutoUpdate/AutoUpdate.h>
#include <QtPlugin>

#include <qmediaplayer.h>
#include <qvideowidget.h>

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

using namespace Core;

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);
   QWidget window;
   window.setFixedSize(1280, 720);

   QVideoWidget vwidget(&window);
   vwidget.setFixedSize(1280, 720);

   window.show();

   QMediaPlayer player;
   player.setVideoOutput(&vwidget);

   //player.setMedia(QUrl("http://vpr.streamguys.net/vpr64.mp3"));
   player.setMedia(QUrl("http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4"));
   player.setVolume(100);
   player.play();


   return app.exec();

}
