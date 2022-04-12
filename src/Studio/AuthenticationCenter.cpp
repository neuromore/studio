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

// include the required headers
#include "AuthenticationCenter.h"
#include <License.h>
#include "MainWindow.h"
#include <Windows/ProgressWindowManager.h>
#include <Backend/BackendInterface.h>
#include <Backend/LoginRequest.h>
#include <Backend/LoginResponse.h>
#include <Backend/SystemsGetRequest.h>
#include <Backend/SystemsGetResponse.h>

using namespace Core;

// constructor
AuthenticationCenter::AuthenticationCenter() : QObject()
{
	LogDetailedInfo("Constructing authentication center ...");

	mUserLoggedIn		= false;
	mLastLoginFailed	= false;
}


// destructor
AuthenticationCenter::~AuthenticationCenter()
{
	LogDetailedInfo("Destructing authentication center ...");
}


Core::String AuthenticationCenter::GetLicenseFilename()
{ 
#ifdef PRODUCTION_BUILD
	return QtBaseManager::GetAppDataFolder() + "License.lic";
#else
	const ServerPreset& preset = GetBackendInterface()->GetNetworkAccessManager()->GetActiveServerPreset();
	String presetName = preset.mName;
	presetName.RemoveAllParts("AWS");
	presetName.RemoveChars(" ()");

	String result = QtBaseManager::GetAppDataFolder();
	result += "LicenseDev_";
	result += presetName;
	result += ".lic";

	return result;
#endif
}


bool AuthenticationCenter::RemoveLoginInfoFromLicense()
{
	License license;
	if (license.Load(GetLicenseFilename().AsChar()) == true)
	{
		license.mUserId		= "";
		license.mPassword	= "";
		license.mRememberMe	= false;

		license.Save( GetLicenseFilename().AsChar() );

		return true;
	}

	return false;
}


bool AuthenticationCenter::RemoveLicenseFromDisk()
{
	LogInfo("Removing license file from hard disk ...");

	String filename = GetLicenseFilename();

	// does the license file exist?
	QFileInfo checkFile(filename.AsChar());
	if (checkFile.exists() == false)
		return true;

	// delete the file
	bool result = QtBaseManager::RemoveFileFromDisk( filename.AsChar() );
	if (result == false)
	{
		QMessageBox::critical( GetQtBaseManager()->GetMainWindow(), "File System Error", "Cannot remove license." );
		LogError( "Cannot remove license from disk." );
	}

	return result;
}


void AuthenticationCenter::UpdateInterface()
{
	LogDebug("Updating authentication center interface ...");
	GetQtBaseManager()->SetInterfaceLocked( IsInterfaceAllowed() == false );
	GetQtBaseManager()->UpdateInterfaceLock();
}


void AuthenticationCenter::Start()
{
	CheckVersion();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Version check
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AuthenticationCenter::CheckVersion()
{
	// set splash screen message
	GetManager()->SetSplashScreenMessage("Checking version ...");


	// 1. construct /systems/${systemName}/get request
	SystemsGetRequest request( GetManager()->GetBackendSystemName() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		SystemsGetResponse response(networkReply);

		// don't return in case of an error, else we'll just close the studio and the user will be confused
		//if (response.HasError() == true)
		//	return;

		// 4. handle response

		// check versions
		if (GetManager()->GetVersion() < response.GetVersion())
		{
			// close the splash screen first so that users can see the message box
			GetManager()->CloseSplashScreen( GetMainWindow() );

			// NOTE: set NULL as parent here so that the window gets its own item in the task bar
			String text;
			text.Format( "Your %s is out of date (%s). There is a new version available (%s).\n\nPlease go to the User Portal (https://account.neuromore.com) and download the latest version.", GetManager()->GetAppName().AsChar(), GetManager()->GetVersion().AsString().AsChar(), response.GetVersion().AsString().AsChar());
			QMessageBox::information( NULL, "OUT OF DATE", text.AsChar(), QMessageBox::Ok );
			GetQtBaseManager()->ForceTerminateApplication();
			return;
		}

		// authenticate the user
		LoginUser();
	});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// User Login
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AuthenticationCenter::LoginUser()
{
	LogDetailedInfo("User login ...");

	License license;
	// show the user login window
	if (mLastLoginFailed == true || license.Load(GetLicenseFilename().AsChar()) == false || license.mRememberMe == false || license.mUserId.IsEmpty() == true || license.mPassword.IsEmpty() == true)
	{
		GetMainWindow()->CloseSplashScreen();
		LogDebug("Showing main window ...");
		GetMainWindow()->showMaximized();
		LogDebug("Processing Qt events ...");
		qApp->processEvents();

		LogDebug("Opening login window ...");
		LoginWindow loginWindow( GetMainWindow(), "Log in", "User ID or Email", mLastEnteredUser.AsChar(), "Password", mLastEnteredPassword.AsChar(), "Log in" );
		if (loginWindow.exec() == QDialog::Rejected)
		{
			qApp->exit();
			GetQtBaseManager()->ForceTerminateApplication();
			return false;
		}

		// gather the new information from the interface
		license.mUserId		= FromQtString( loginWindow.GetUserId() );
		license.mPassword	= FromQtString( loginWindow.GetPassword() );
		license.mRememberMe	= loginWindow.GetRememberMe();

		mLastEnteredUser	= license.mUserId;
		mLastEnteredPassword= license.mPassword;

		// in case the user doesn't want to enter his information again, remember the settings
		if (loginWindow.RememberMe() == true)
		{
			LogDebug("Saving login info ...");
			license.Save( GetLicenseFilename().AsChar() );
		}
	}

	// user login
	GetManager()->SetSplashScreenMessage("Signing in ...");
	mLastPassword = license.mPassword;

	// 1. construct login request
	LoginRequest request( GetManager()->GetBackendSystemName(), GetManager()->GetVersion().AsString().AsChar(), license.mUserId.AsChar(), license.mPassword.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		LoginResponse response(networkReply);

		// 4. handle response
		if (response.HasError() == true)
			OnUserDenied();
		else
		{
			OnUserGranted( response.GetUser() );

			// refresh token every 8 hours
			QTimer::singleShot(8*3600000, this, SLOT(OnRefreshToken()));
		}
	});

	return true;
}


void AuthenticationCenter::OnRefreshToken()
{
	LogDetailedInfo("Refreshing token ...");

	// 1. construct login request
	LoginRequest request( GetManager()->GetBackendSystemName(), GetManager()->GetVersion().AsString().AsChar(), GetUser()->GetId(), mLastPassword.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		LoginResponse response(networkReply);

		// refresh token every 8 hours
		QTimer::singleShot(8*3600000, this, SLOT(OnRefreshToken()));
	});
}


void AuthenticationCenter::OnUserGranted(User user)
{
	LogInfo("Login succeeded ...");

	GetEngine()->SetUser(user);

	// update the window title
	GetMainWindow()->UpdateTitle( user.GetDisplayName() );

	// main window post authentication init
	GetMainWindow()->OnPostAuthenticationInit();

	// update the flag and the interface
	mUserLoggedIn = true;
	mLastLoginFailed = false;
	UpdateInterface();

	// call post-authentication init for all plugins
	GetPluginManager()->OnPostAuthenticationInit();
}


void AuthenticationCenter::OnUserDenied()
{
	LogInfo("Login failed ...");

	// remove the user info and password from the license
	RemoveLoginInfoFromLicense();

	// reset the user
	GetEngine()->SetUser( User() );

	// update the window title
	GetMainWindow()->UpdateTitle( "" );

	// prevent brute force hacking
	Thread::Sleep( 1000 );

	// update the flag and the interface
	mUserLoggedIn = false;
	mLastLoginFailed = true;
	UpdateInterface();

	// retry user login
	LoginUser();
}
