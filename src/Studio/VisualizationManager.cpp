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
#include "VisualizationManager.h"

using namespace Core;

// constructor
VisualizationManager::VisualizationManager(QObject* parent) : QObject(parent), mVisualizationFolders()
{
   const Core::String appDir = FromQtString(GetQtBaseManager()->GetAppDir());

   mVisualizationFolders.Add(appDir + "visualizations");
   mVisualizationFolders.Add(appDir + "../visualizations");
   mVisualizationFolders.Add(appDir + "../../visualizations");
   mVisualizationFolders.Add(appDir + "../../../visualizations");
   mVisualizationFolders.Add(appDir + "../../../../visualizations");

#ifndef NEUROMORE_PLATFORM_WINDOWS
   mVisualizationFolders.Add(appDir + "Visualizations");
   mVisualizationFolders.Add(appDir + "../Visualizations");
   mVisualizationFolders.Add(appDir + "../../Visualizations");
   mVisualizationFolders.Add(appDir + "../../../Visualizations");
   mVisualizationFolders.Add(appDir + "../../../../Visualizations");
#endif

#ifdef NEUROMORE_PLATFORM_LINUX
   mVisualizationFolders.Add("/usr/share/neuromore/visualizations"); // linux distribution packages
#endif
}


// destructor
VisualizationManager::~VisualizationManager()
{
   Clear();
}


// remove all visualizations
void VisualizationManager::Clear()
{
   const uint32 numVisualizations = mVisualizations.Size();
   for (uint32 i=0; i<numVisualizations; ++i)
      delete mVisualizations[i];

   mVisualizations.Clear();
}


// automatic folder browsing for visualization executables
void VisualizationManager::ReInit()
{
   // remove all visualizations
   Clear();

   // iterate the configured visualization folders
   const uint32_t NUMFOLDERS = mVisualizationFolders.Size();
   for (uint32_t i = 0; i < NUMFOLDERS; i++)
   {
      String vizDir = mVisualizationFolders[i];
      vizDir.ConvertToNativePath();

      // scan the directory for subfolders
      QDir dir(vizDir.AsChar());
      dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
      dir.setSorting(QDir::Name);
      if (!dir.exists())
         continue;

      // iterate over all subfolders
      String folderName;
      QFileInfoList list = dir.entryInfoList();
      for (int i = 0; i < list.size(); ++i)
      {
         QFileInfo folderInfo = list.at(i);

         // scan the subfolder
         QDir subDir(folderInfo.absoluteFilePath());
         subDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
         subDir.setSorting(QDir::Name);

         QFileInfoList subList = subDir.entryInfoList();
         for (int j = 0; j < subList.size(); ++j)
         {
            QFileInfo fileInfo = subList.at(j);
            if (fileInfo.fileName() == "Info.json")
            {
               Visualization* newViz = new Visualization();
               if (newViz->ParseFromJsonFile(fileInfo.absoluteFilePath().toUtf8().data()))
                  mVisualizations.Add(newViz);
               else
                  delete newViz;

               break;
            }
         }
      }
   }
}
