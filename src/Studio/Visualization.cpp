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

// include the required headers
#include "Visualization.h"
#include <Core/LogManager.h>
#include <Core/Json.h>
#include "AppManager.h"
#include "MainWindow.h"
#include <QProcess>
#include <QMessageBox>


using namespace Core;

// constructor
Visualization::Visualization()
{
}


// destructor
Visualization::~Visualization()
{
}


void Visualization::SetExecutableFilename(const char* filename)
{
	mExecutableFilename = filename;
	mExecutableFilename.ConvertToNativePath();
}


void Visualization::Start()
{
	String arguments = "-single-instance ";
	String quotedFilename;
	String temp;

	bool showScreenSelector		= true;
	bool showFullscreen			= true;
	bool popupWindow			= false;				// The window will be created as a a pop-up window (without a frame).
	int32 screenWidth			= 640;
	int32 screenHeight			= 480;

	/*QStringList args;

	// single instance
	args.append("-single-instance");

	// dimensions
	temp.Format( "-screen-width %i", screenWidth );
	args.append( temp.AsChar() );
	temp.Format( "-screen-height %i", screenHeight );
	args.append( temp.AsChar() );

	// fullscreen
	temp.Format( "-screen-fullscreen %i", showFullscreen );
	args.append( temp.AsChar() );

	// popup window
	if (popupWindow == true)
		args.append("-popupwindow");

	// TODO: not working yet
	// show screen selector
	if (showScreenSelector == true)
		args.append("-show-screen-selector");

	// screen
	args.append( "-adapter 2" );

	arguments = args.join(' ').toUtf8().data();*/

#ifdef NEUROMORE_PLATFORM_WINDOWS
	quotedFilename.Format("\"%s\" %s", mExecutableFilename.AsChar(), arguments.AsChar());
#elif NEUROMORE_PLATFORM_OSX
	// TODO implement arguments on osx
    quotedFilename.Format("open \"%s\"", mExecutableFilename.AsChar());
#else
	quotedFilename.Format("%s", mExecutableFilename.AsChar());
#endif

	QProcess process;
	if (QProcess::startDetached(quotedFilename.AsChar()) == false)
	{
		String message;
		message.Format("Can't start the '%s' visualization.\n\nPlease contact the support team.", mExecutableFilename.ExtractFilename().AsChar());
		QMessageBox::critical( GetMainWindow(), "Can't start visualization", message.AsChar(), QMessageBox::Ok);
	}
}


bool Visualization::ParseFromJsonFile(const char* filename)
{
	String folder = String(filename).ExtractPath();

	Json json;
	if (json.ParseFile(filename) == false)
		return false;

	Json::Item rootItem = json.GetRootItem();
	
	// name
	Json::Item nameItem = json.Find("name");
	if (nameItem.IsString() == true)
		mName = nameItem.GetString();

	// description
	Json::Item descItem = json.Find("description");
	if (descItem.IsString() == true)
		mDescription = descItem.GetString();

	// executable
	Json::Item executableItem = json.Find("executable");
	if (executableItem.IsString() == true)
		mExecutableFilename = folder + executableItem.GetString();

	// image
	Json::Item imageItem = json.Find("image");
	if (imageItem.IsString() == true)
		mImageFilename = folder + imageItem.GetString();

	return true;
}
