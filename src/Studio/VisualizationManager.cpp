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
#include "VisualizationManager.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <QDir>


using namespace Core;

// constructor
VisualizationManager::VisualizationManager(QObject* parent) : QObject(parent), mVisualizationFolders()
{
	const Core::String appDir = FromQtString(GetQtBaseManager()->GetAppDir());

#ifdef NEUROMORE_PLATFORM_WINDOWS
	mVisualizationFolders.Add(appDir + ".\\Visualizations\\");
	mVisualizationFolders.Add(appDir + "..\\Visualizations\\");
#else
	mVisualizationFolders.Add(appDir + "..\\..\\..\\Visualizations\\");
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

	const uint32_t NUMFOLDERS = mVisualizationFolders.Size();
	for (uint32_t i = 0; i < NUMFOLDERS; i++)
	{
		String vizDir = mVisualizationFolders[i];
		vizDir.ConvertToNativePath();

		// scan the directory for subfolders
		QDir dir(vizDir.AsChar());
		dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		dir.setSorting(QDir::Name);

		// iterate over all files
		String folderName;
		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.size(); ++i)
		{
			Visualization* newViz = new Visualization();

			QFileInfo folderInfo = list.at(i);
			FromQtString(folderInfo.fileName(), &folderName);
			newViz->SetName(folderName.AsChar());

			// scan the subfolder
			QDir subDir(folderInfo.absoluteFilePath());
			subDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
			subDir.setSorting(QDir::Name);

#ifdef NEUROMORE_PLATFORM_WINDOWS
			String currentFilename;
			QFileInfoList subList = subDir.entryInfoList();
			for (int j = 0; j < subList.size(); ++j)
			{
				QFileInfo fileInfo = subList.at(j);
				FromQtString(fileInfo.absoluteFilePath(), &currentFilename);

				// only add files with the .exe extension
				if (currentFilename.ExtractFileExtension().Lowered() == "exe")
					newViz->SetExecutableFilename(currentFilename.AsChar());

				if (currentFilename.ExtractFileExtension().Lowered() == "json")
				{
					newViz->ParseFromJsonFile(currentFilename.AsChar());
				}
			}
#else
			newViz->SetExecutableFilename(FromQtString(folderInfo.absoluteFilePath()).AsChar());
#endif

			mVisualizations.Add(newViz);
		}
	}
}
