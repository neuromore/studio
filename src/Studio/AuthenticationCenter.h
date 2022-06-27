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

#ifndef __NEUROMORE_AUTHENTICATIONCENTER_H
#define __NEUROMORE_AUTHENTICATIONCENTER_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Version.h>
#include <User.h>
#include "Config.h"
#include "Windows/LoginWindow.h"
#include <QObject>


class AuthenticationCenter : public QObject
{
	Q_OBJECT
	public:
		AuthenticationCenter();
		virtual ~AuthenticationCenter();

		void Start();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Version check
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	public:
		void CheckVersion();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// User Login
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	public:
		bool LoginUser();
	public slots:
		void OnUserGranted(User user);
		void OnUserDenied();
		void OnRefreshToken();

	public:
		static Core::String GetLicenseFilename();

		inline bool IsInterfaceAllowed() const												{ return (mUserLoggedIn == true); }
		void UpdateInterface();
		bool IsUserInputLogIn() const														{ return mUserInputLogIn; }

		static bool RemoveLoginInfoFromLicense();
		static bool RemoveLicenseFromDisk();

	private:
		Core::String	mLastEnteredUser;
		Core::String	mLastEnteredPassword;
		Core::String	mLastPassword;
		bool			mLastLoginFailed;
		bool			mUserLoggedIn;
		bool			mUserInputLogIn;
};


#endif
