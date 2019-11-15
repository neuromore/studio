/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_WEBDATACACHE
#define __NEUROMORE_WEBDATACACHE

// include required headers
#include "../QtBaseConfig.h"
#include <Core/String.h>
#include <Core/Array.h>
#include <QObject>
#include "../Windows/StatusPopupWindow.h"
#include "FileDownloader.h"


// web data cache
class QTBASE_API WebDataCache : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		WebDataCache(const char* subfolderPath, QObject* parent=NULL);
		virtual ~WebDataCache();

		void RestrictCacheSize(uint64 maxCacheSizeInMb)				{ mMaxCacheRestrictionEnabled = true; mMaxCacheSizeInMb = maxCacheSizeInMb; }
		void RestrictAliveTime(uint32 maxAliveTimeInDays)			{ mMaxAliveTimeRestrictionEnabled = true; mMaxAliveTimeInDays = maxAliveTimeInDays; }

		bool Download(const Core::Array<Core::String>& urls);

		// restrict cache to max size and alive time
		bool CleanCache();

		void Log();

		// gather the total size in bytes from all files that are currently cached
		uint64 CalcUsedCacheSizeInBytes();

		bool IsDownloading() const						{ return mDownloadQueue.IsEmpty() == false; }

		bool FileExists(const char* url);

		Core::String GetCacheFilenameForUrl(const char* url);
		bool IsCached(const char* url);

		Core::String UrlToFilename(const char* url);
		Core::String FilenameToUrl(const char* filename);

	signals:
		void FinishedDownload();

	private slots:
		void OnFileDownloaded(FileDownloader* downloader);
		void OnFileFailed(FileDownloader* downloader);
		void OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

	private:
		void Start();
		void DownloadNext();
		void RemoveFromQueue(FileDownloader* downloader);
		void UpdateProgressBar();

		Core::Array<FileDownloader*>	mDownloadQueue;
		Core::String					mFolder;
		Core::String					mTempString;
		uint32							mMaxNumFails;

		bool							mMaxCacheRestrictionEnabled;
		uint64							mMaxCacheSizeInMb;

		bool							mMaxAliveTimeRestrictionEnabled;
		uint32							mMaxAliveTimeInDays;

		// current download status
		StatusPopupWindow*				mStatusWindow;
		uint32							mNumDownloadItems;
		uint32							mCurrentDownload;
		Core::String					mTempStatusString;
		Core::String					mCurrentUrl;
		uint32							mCurrentBytesReceived;
		uint32							mCurrentBytesTotal;
};


#endif
