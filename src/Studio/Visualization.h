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

#ifndef __NEUROMORE_VISUALIZATION_H
#define __NEUROMORE_VISUALIZATION_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "Config.h"
#include <QProcess>

class Visualization : public QObject
{
public:
   Visualization();
   virtual ~Visualization();

   // true if this visualization process is running
   inline bool IsRunnning() const { return mProcess.isOpen(); }

   // start the visualization
   bool Start();

   // parse visualization configuration
   bool ParseFromJsonFile(const char* filename);

   // name
   inline void SetName(const char* name)              { mName = name; }
   inline const char* GetName() const                 { return mName; }

   // description
   inline void SetDescription(const char* desc)       { mDescription = desc; }
   inline const char* GetDescription() const          { return mDescription; }

   // executable filename
   void SetExecutableFilename(const char* filename);
   inline const char* GetExecutableFilename() const   { return mExecutableFilename; }

   // name
   inline void SetImageFilename(const char* filename) { mImageFilename = filename; }
   inline const char* GetImageFilename() const        { return mImageFilename; }

   // qt signal handlers
   void OnProcessStarted();
   void OnProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
   void OnProcessStateChanged(const QProcess::ProcessState& newState);

protected:
   QProcess     mProcess;
   QStringList  mArguments;
   Core::String mName;
   Core::String mDescription;
   Core::String mExecutableFilename;
   Core::String mImageFilename;
};

#endif
