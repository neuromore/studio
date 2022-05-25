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
#include <Engine/Precompiled.h>

// include required files
#include "License.h"
#include "Core/LogManager.h"
#include "Core/EncryptedJSONFile.h"


using namespace Core;

#define AGREED_TO_LICENSE "agreedToLicense_20160506"

// constructor
License::License()
{
	mAgreedToLicense	= false;
	mRememberMe			= true;
}


// destructor
License::~License()
{
}


// load the license
bool License::Load(const char* filename)
{
	// load, decrypt and parse the license json file
	Json* json = EncryptedJSONFile::Load( filename );
	if (json == NULL)
		return false;

	// remember me
	Json::Item rememberMeItem = json->Find("rememberMe");
	if (rememberMeItem.IsBool() == true)
		mRememberMe = rememberMeItem.GetBool();

	// user id
	Json::Item userIdItem = json->Find("userid");
	if (userIdItem.IsString() == true)
		mUserId = userIdItem.GetString();

	// password
	Json::Item passwordItem = json->Find("password");
	if (passwordItem.IsString() == true)
		mPassword = passwordItem.GetString();

	// agreed to license
	Json::Item agreedItem = json->Find(AGREED_TO_LICENSE);
	if (agreedItem.IsBool() == true)
		mAgreedToLicense = agreedItem.GetBool();

	// get rid of the json parser object
	delete json;

	return true;
}


// save the license
bool License::Save(const char* filename)
{
	// create a json object
	Json json;
	Json::Item rootItem = json.GetRootItem();
	rootItem.AddBool( "rememberMe", mRememberMe );
	if (mRememberMe == true)
	{
		rootItem.AddString( "userid", mUserId.AsChar() );
		rootItem.AddString( "password", mPassword.AsChar() );
	}
	rootItem.AddBool( AGREED_TO_LICENSE, mAgreedToLicense );

	// encrypt and save the license file
	return EncryptedJSONFile::Save( filename, &json );
}
