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
#include "Visualization.h"
#include "MainWindow.h"

using namespace Core;

// constructor
Visualization::Visualization() : mIsSupported(false)
{
   // setup process arguments
   mArguments.append("-single-instance");

   // others that might work
   //mArguments.append("-screen-width 800");
   //mArguments.append("-screen-height 640");
   //mArguments.append("-screen-fullscreen 1");
   //mArguments.append("-popupwindow");
   //mArguments.append("-show-screen-selector");
   //mArguments.append("-adapter 2");

   // setup process
   mProcess.setArguments(mArguments);
   mProcess.setProcessChannelMode(QProcess::ProcessChannelMode::ForwardedChannels);

   // connect signals
   connect(&mProcess, &QProcess::stateChanged, this, &Visualization::OnProcessStateChanged);
   connect(&mProcess, &QProcess::started, this, &Visualization::OnProcessStarted);
   connect(&mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Visualization::OnProcessFinished);
}

// destructor
Visualization::~Visualization()
{
}

void Visualization::SetFolder(const char* folder)
{
   mFolder = folder;
   mFolder.ConvertToNativePath();
}

void Visualization::SetExecutableFilename(const char* filename)
{
   mExecutableFilename = filename;
   mExecutableFilename.ConvertToNativePath();
}

bool Visualization::Start()
{
   // do not start if already running or not supported
   if (mProcess.isOpen() || !mIsSupported)
      return false;

   // set executable and arguments
   mProcess.setProgram(mExecutableFilename.AsChar());
   mProcess.setArguments(mArguments);
   mProcess.start();

   // wait for it to start
   return mProcess.waitForStarted(1000);

}

void Visualization::Stop()
{
   if (mProcess.isOpen())
      mProcess.terminate();
}

bool Visualization::ParseFromJsonFile(const char* filename)
{
   // save folder
   mFolder = String(filename).ExtractPath(false);
   mIsSupported = false;

   // parse json
   Json json;
   if (json.ParseFile(filename) == false)
      return false;

   // rootitem
   const Json::Item rootItem = json.GetRootItem();

   // name
   const Json::Item nameItem = json.Find("name");
   if (nameItem.IsString() == true)
      mName = nameItem.GetString();

   // description
   const Json::Item descItem = json.Find("description");
   if (descItem.IsString() == true)
      mDescription = descItem.GetString();

   // executable
   const Json::Item executableItem = json.Find("executable");
   if (executableItem.IsString() == true)
   {
      // build full executable based on platform and cpu
      mExecutableFilename = mFolder + "/" + 
         NEUROMORE_PLATFORM_STRING + "-" + NEUROMORE_CPU_STRING + "/" + 
         executableItem.GetString();

      // dynamically add extension for platforms
   #if defined(NEUROMORE_PLATFORM_WINDOWS)
      mExecutableFilename += ".exe";
   #elif defined(NEUROMORE_PLATFORM_OSX)
      mExecutableFilename += ".app";
   #endif

      // check if binary exists
      QFileInfo inf(mExecutableFilename.AsChar());
      mIsSupported = inf.exists();
   }

   // image
   const Json::Item imageItem = json.Find("image");
   if (imageItem.IsString() == true)
      mImageFilename = mFolder + "/" + imageItem.GetString();

   return true;
}

// SIGNALS

void Visualization::OnProcessStarted()
{
   // not used
}

void Visualization::OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
   // close the process
   if (mProcess.isOpen())
      mProcess.close();
}

void Visualization::OnProcessStateChanged(const QProcess::ProcessState& newState)
{
   // not used
}
