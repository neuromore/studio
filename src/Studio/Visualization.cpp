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

void Visualization::SetExecutableFilename(const char* filename)
{
   mExecutableFilename = filename;
   mExecutableFilename.ConvertToNativePath();
}

bool Visualization::Start()
{
   // do not start if already running
   if (mProcess.isOpen())
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
   String folder = String(filename).ExtractPath();

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
      mExecutableFilename = folder + executableItem.GetString();

   // image
   const Json::Item imageItem = json.Find("image");
   if (imageItem.IsString() == true)
      mImageFilename = folder + imageItem.GetString();

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
