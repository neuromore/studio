/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_EXPERIENCEASSETCACHE_H
#define __NEUROMORE_EXPERIENCEASSETCACHE_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/Color.h>
#include "Backend/WebDataCache.h"
#include <QObject>


class QTBASE_API ExperienceAssetCache : public QObject
{
	Q_OBJECT
	public:
		ExperienceAssetCache(QObject* parent=NULL);
		~ExperienceAssetCache();

		WebDataCache* GetCache()			{ return mCache; }

	signals:
		void FinishedPreloading();

	public slots:
		void PreloadAssets();

	private slots:
		void OnFinishedPreloading();

	private:
		// web data cache
		WebDataCache* mCache;
};


#endif
