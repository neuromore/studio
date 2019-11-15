/************************************************************************************* 
This file is a part of CrashRpt library.
Copyright (c) 2003-2013 The CrashRpt project authors. All Rights Reserved.

Use of this source code is governed by a BSD-style license
that can be found in the License.txt file in the root of the source
tree. All contributing project authors may
be found in the Authors.txt file in the root of the source tree.
***************************************************************************************/

// File: CrashSender.cpp
// Description: Entry point to the application. 
// Authors: zexspectrum
// Date: 2010

#include "stdafx.h"
#include "resource.h"
#include "ErrorReportDlg.h"
#include "ResendDlg.h"
#include "CrashInfoReader.h"
#include "strconv.h"
#include "Utility.h"

#if _DEBUG
#pragma comment(lib, "Version.lib")            // winapi: version
#pragma comment(lib, "ws2_32.lib")             // winapi: Winsock
#pragma comment(lib, "Wininet.lib")            // winapi: WinInet
#pragma comment(lib, "Psapi.lib")              // winapi: PsApi
#pragma comment(lib, "Rpcrt4.lib")             // winapi: Rpcrt
#pragma comment(lib, "Dnsapi.lib")             // winapi: DnsApi
#pragma comment(lib, "crashrpt_d.lib")         // ownlib: crashrpt
#pragma comment(lib, "libogg_d.lib")           // 3rdparty: LibOGG
#pragma comment(lib, "libpng_d.lib")           // 3rdparty: LibPNG
#pragma comment(lib, "libjpeg_d.lib")          // 3rdparty: LibJPEG
#pragma comment(lib, "libtheora_d.lib")        // 3rdparty: LibTHEORA
#pragma comment(lib, "tinyxml_d.lib")          // 3rdparty: tinyxml
#pragma comment(lib, "minizip_d.lib")          // 3rdparty: minizip
#pragma comment(lib, "zlib_d.lib")             // 3rdparty: zlib
#else
#pragma comment(lib, "Version.lib")            // winapi: version
#pragma comment(lib, "ws2_32.lib")             // winapi: Winsock
#pragma comment(lib, "Wininet.lib")            // winapi: WinInet
#pragma comment(lib, "Psapi.lib")              // winapi: PsApi
#pragma comment(lib, "Rpcrt4.lib")             // winapi: Rpcrt
#pragma comment(lib, "Dnsapi.lib")             // winapi: DnsApi
#pragma comment(lib, "crashrpt.lib")           // ownlib: crashrpt
#pragma comment(lib, "libogg.lib")             // 3rdparty: LibOGG
#pragma comment(lib, "libpng.lib")             // 3rdparty: LibPNG
#pragma comment(lib, "libjpeg.lib")            // 3rdparty: LibJPEG
#pragma comment(lib, "libtheora.lib")          // 3rdparty: LibTHEORA
#pragma comment(lib, "tinyxml.lib")            // 3rdparty: tinyxml
#pragma comment(lib, "minizip.lib")            // 3rdparty: minizip
#pragma comment(lib, "zlib.lib")               // 3rdparty: zlib
#endif

CAppModule _Module;             // WTL's application module.

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int /*nCmdShow*/ = SW_SHOWDEFAULT)
{ 
	int nRet = 0; // Return code
	CErrorReportDlg dlgErrorReport; // Error Report dialog
	CResendDlg dlgResend; // Resend dialog

	// Get command line parameters.
	LPCWSTR szCommandLine = GetCommandLineW();

    // Split command line.
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(szCommandLine, &argc);

    // Check parameter count.
    if(argc!=2)
        return 1; // No arguments passed, exit.

	if(_tcscmp(argv[1], _T("/terminate"))==0)
	{
		// User wants us to find and terminate all instances of CrashSender.exe
		return CErrorReportSender::TerminateAllCrashSenderProcesses();
	}

	// Extract file mapping name from command line arg.    
    CString sFileMappingName = CString(argv[1]);
		
	// Create the sender model that will collect crash report data 
	// and send error report(s).
	CErrorReportSender* pSender = CErrorReportSender::GetInstance();

	// Init the sender object
	BOOL bInit = pSender->Init(sFileMappingName.GetBuffer(0));
	if(!bInit)
    {
		// Failed to init 
		delete pSender;
        return 0;
    }      

	// Determine what to do next 
	// (either run in GUI more or run in silent mode).
	if(!pSender->GetCrashInfo()->m_bSilentMode)
	{
		// GUI mode.
		// Create message loop.
		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		if(!pSender->GetCrashInfo()->m_bSendRecentReports)
		{
			// Create "Error Report" dialog			
			if(dlgErrorReport.Create(NULL) == NULL)
			{
				ATLTRACE(_T("Error report dialog creation failed!\n"));
				delete pSender;
				return 1;
			}			
		}
		else
		{        
			// Create "Send Error Reports" dialog.					
			if(dlgResend.Create(NULL) == NULL)
			{
				ATLTRACE(_T("Resend dialog creation failed!\n"));
				delete pSender;
				return 1;
			}			
		}

		// Process window messages.
		nRet = theLoop.Run();	    
		_Module.RemoveMessageLoop();
	}
	else
	{
		// Silent (non-GUI mode).
		// Run the sender and wait until it exits.
		pSender->Run();
		pSender->WaitForCompletion();
		// Get return status
		nRet = pSender->GetStatus();
	}
    
	// Delete sender object.
	delete pSender;

	// Exit.
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{  
    HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = Run(lpstrCmdLine, nCmdShow);

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}

