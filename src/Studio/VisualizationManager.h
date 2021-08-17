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

#ifndef __NEUROMORE_VISUALIZATIONMANAGER_H
#define __NEUROMORE_VISUALIZATIONMANAGER_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/Array.h>
#include "Visualization.h"
#include "Config.h"
#include <QObject>


class VisualizationManager : public QObject
{
	Q_OBJECT
	public:
		VisualizationManager(QObject* parent=NULL);
		virtual ~VisualizationManager();

		void ReInit();

		inline Visualization* GetVisualization(uint32 index) const	{ return mVisualizations[index]; }
		inline uint32 GetNumVisualizations() const							{ return mVisualizations.Size(); }
	
		inline bool IsRunning() const
		{
			const uint32_t numVis = mVisualizations.Size();
			for (uint32 i = 0; i < numVis; i++)
				if (mVisualizations[i]->IsRunnning())
					return true;
			return false;
		}

		inline uint32 GetVisualizationIndex(const Visualization* v)
		{
			if (v)
			{
				const uint32_t numVis = mVisualizations.Size();
				for (uint32 i = 0; i < numVis; i++)
					if (mVisualizations[i] == v)
						return i;
			}

			return CORE_INVALIDINDEX32;
		}

		inline bool Start(uint32_t index) const
		{
			const uint32_t numVis = mVisualizations.Size();

			// invalid index or already one running
			if (index >= numVis || IsRunning())
				return false;

			// try start it
			return mVisualizations[index]->Start();
		}

		inline bool Start(const Visualization* v)
		{
			const uint32 idx = GetVisualizationIndex(v);
			return (idx != CORE_INVALIDINDEX32) ? Start(idx) : false;
		}

	private:
		void Clear();
		
		Core::Array<Core::String>   mVisualizationFolders;
		Core::Array<Visualization*> mVisualizations;
};

#endif
