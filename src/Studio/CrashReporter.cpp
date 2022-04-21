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

#include "Config.h"

// only on Windows
#if defined(NEUROMORE_PLATFORM_WINDOWS) && defined(USE_CRASHREPORTER)

// include the required headers
#include "CrashReporter.h"

// include require files
#include <CrashRpt/CrashRpt.h>
#include <Tlhelp32.h>
#include <tchar.h>
#include "AppManager.h"
#include "MainWindow.h"

#ifdef _DEBUG
#define NEUROMORE_CRASHRPT_EXE  L"crashsender_d.exe"
#else
#define NEUROMORE_CRASHRPT_EXE  L"crashsender.exe"
#endif

using namespace Core;

// the following function returns TRUE if the correct CrashRpt DLL was loaded, otherwise it returns FALSE
// DEPRECATED, DON'T CALL
bool CrashReporterCheckVersion()
{
	bool	bExitCode		= false;
	wchar_t	szModuleName[_MAX_PATH] = L"";
	HMODULE	hModule			= NULL;
	DWORD	dwBuffSize		= 0;
	LPBYTE	pBuff			= NULL;
	UINT	uLen			= 0;
	VS_FIXEDFILEINFO* fi	= NULL;

	// Get handle to loaded CrashRpt.dll module
	hModule = GetModuleHandle(L"CrashRpt1403.dll");
	if (hModule==NULL)
	{
		if (pBuff)
		{
			// Free buffer
			GlobalFree((HGLOBAL)pBuff);
			pBuff = NULL;
		}
		return bExitCode;
	}

	// Get module file name
	GetModuleFileName(hModule, szModuleName, _MAX_PATH);

	// Get module version 
	dwBuffSize = GetFileVersionInfoSize(szModuleName, 0);
	if (dwBuffSize==0)
	{
		if (pBuff)
		{
			// Free buffer
			GlobalFree((HGLOBAL)pBuff);
			pBuff = NULL;
		}
		return bExitCode;
	}

	pBuff = (LPBYTE)GlobalAlloc(GPTR, dwBuffSize);
	if (pBuff==NULL)
		return bExitCode;

	if (0 == GetFileVersionInfo(szModuleName, 0, dwBuffSize, pBuff))
	{
		if (pBuff)
		{
			// Free buffer
			GlobalFree((HGLOBAL)pBuff);
			pBuff = NULL;
		}
		return bExitCode;
	}

	VerQueryValue(pBuff, L"\\", (LPVOID*)&fi, &uLen);
	WORD dwVerMajor = HIWORD(fi->dwProductVersionMS);
	WORD dwVerMinor = LOWORD(fi->dwProductVersionMS);  
	WORD dwVerBuild = LOWORD(fi->dwProductVersionLS);
	DWORD dwModuleVersion = dwVerMajor*1000+dwVerMinor*100+dwVerBuild;

	if (CRASHRPT_VER == dwModuleVersion)
		bExitCode = true; // Version match!

	if (pBuff)
	{
		// Free buffer
		GlobalFree((HGLOBAL)pBuff);
		pBuff = NULL;
	}
	return bExitCode;
}


// check if the crash reporter is already running
bool CrashReporterIsRunning() 
{ 
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 ProcessEntry;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return false;

	ProcessEntry.dwSize = sizeof( PROCESSENTRY32 );
	if( !Process32First( hProcessSnap, &ProcessEntry ) )
	{
		CloseHandle( hProcessSnap );  
		return false;
	}

	do
	{
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID );
		if (wcscmp(ProcessEntry.szExeFile, NEUROMORE_CRASHRPT_EXE) == 0)
			return true;
		CloseHandle( hProcess );
	} while( Process32Next( hProcessSnap, &ProcessEntry ) );

	CloseHandle( hProcessSnap );
	return false;
} 


// define the callback function that will be called on crash
BOOL WINAPI CrashCallback(LPVOID lpvState)
{  
	// the application has crashed!

	// if we already have a crash report dialog open, don't do anything
	if (CrashReporterIsRunning() == true)
		return FALSE;

	// Return TRUE to generate error report
	return TRUE;
}


// date define
#define CRASHREPORT_WIDEN2(x) L ## x
#define CRASHREPORT_WIDEN(x) CRASHREPORT_WIDEN2(x)
#define __WDATE__ CRASHREPORT_WIDEN(__DATE__)
#define CRASHREPORT_WDATE __WDATE__
#define CRASHREPORT_DATE __DATE__


// try to init the crash reporter
bool CrashReporterInit()
{
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));

	//wchar_t date[128];
	wchar_t dateString[256];
	//MultiByteToWideChar(CP_ACP, 0, __DATE__, strlen(__DATE__)+1, date, strlen(__DATE__)+1);
	wsprintf(dateString, L"%ls", CRASHREPORT_WDATE);

	info.cb						= sizeof(CR_INSTALL_INFO);
	info.pszAppName				= L"neuromore Studio";
	info.pszAppVersion			= dateString;
	info.pszEmailSubject		= L"neuromore Studio Crash Report";
	info.pszEmailTo				= L"crashreport@neuromore.com";
	info.pszEmailText			= L"Hi devteam,\n\nhere is a crash report from neuromore Studio.\n\nThis is an automatically generated message from the crash reporting system.\n\nSee the attached package for further information.\n\nKind Regards,\n\nYour crash reporter";
	info.pszUrl					= NULL; // don't use
	info.pszSmtpLogin			= L"wp11035298-crashreports";
	info.pszSmtpPassword		= L"KrBbrEbg2p4v0EXn";
	info.pszSmtpProxy			= L"wp280.webpack.hosteurope.de:25";
	//info.pszErrorReportSaveDir	= gCrashReportFolder;
	info.pfnCrashCallback		= CrashCallback;

	// the greater positive number defines the greater priority
	info.uPriorities[CR_HTTP]	= CR_NEGATIVE_PRIORITY;  // First try send report over HTTP
	info.uPriorities[CR_SMAPI]	= CR_NEGATIVE_PRIORITY;  // Third try send report over Simple MAPI
	info.uPriorities[CR_SMTP]	= 3;  // Second try send report over SMTP
//	info.uMiniDumpType			= MiniDumpWithDataSegs;
	info.uMiniDumpType			= MiniDumpNormal;

	// Install all available exception handlers, use HTTP binary transfer encoding (recommended).
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING; 
	info.dwFlags |= CR_INST_SHOW_ADDITIONAL_INFO_FIELDS;
	//info.dwFlags |= CR_INST_DONT_SEND_REPORT; // NOTE: remove this if you want to automatically send reports
	//info.dwFlags |= CR_INST_APP_RESTART;
	//info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS; 
	//info.pszRestartCmdLine = L"/restart";

	// Define the Privacy Policy URL 
	info.pszPrivacyPolicyURL = NULL;//L"http://www.neuromore.com/privacy/"; 

	// Install exception handlers
	int nResult = crInstall(&info);    
	if (nResult != 0)  
	{    
		// Something goes wrong. Get error message.
		wchar_t szErrorMsg[512] = L"";
		wchar_t message[1024] = L"";
		crGetLastErrorMsg(szErrorMsg, 512);    
		wsprintf(message, L"The crash reporter failed to initialize, because:\n\n%ls\n\nneuromore Studio cannot continue, please contact support@neuromore.com.", szErrorMsg);
		MessageBox(NULL, message, L"Critical Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	// CR_AS_VIRTUAL_SCREEN Use this to take a screenshot of the whole desktop (virtual screen).
    // CR_AS_MAIN_WINDOW Use this to take a screenshot of the application's main window.
    // CR_AS_PROCESS_WINDOWS Use this to take a screenshot of all visible windows that belong to the process.
	crAddScreenshot2( CR_AS_PROCESS_WINDOWS, 100 );

	// add a video file
//	//SIZE vidSize = { 720, 0 };
//	int vidFlags = CR_AV_PROCESS_WINDOWS | CR_AV_QUALITY_GOOD | CR_AV_ALLOW_DELETE;
//	if (crAddVideo(	vidFlags, 1000*10, 1000/15, NULL, NULL ) != 0)
//	{
//		wchar_t errorBuffer[2048];
//		crGetLastErrorMsgW(errorBuffer, 2048);
//		MessageBox(NULL, L"Video Error", errorBuffer, MB_OK|MB_ICONEXCLAMATION);
//	}

	return true;
}


// add a given file to the crash report package
void CrashReporterAddFile(const char* filename, const char* name)
{
	// attach the given file
	crAddFile2A( filename, NULL, name, CR_AF_TAKE_ORIGINAL_FILE | CR_AF_MISSING_FILE_OK );
	LogInfo( "CrashReporter: Adding file '%s' ...", filename );
}


// shutdown the crash reporter
void CrashReporterShutdown()
{
	crUninstall();
}


#endif // #ifdef NEUROMORE_PLATFORM_WINDOWS

