/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_FILEDOWNLOADER
#define __NEUROMORE_FILEDOWNLOADER

// include required headers
#include "../QtBaseConfig.h"
#include <Core/String.h>
#include <QObject>
#include <QUrl>
#include <QFile>
#include <QNetworkAccessManager>


// file downloader
class QTBASE_API FileDownloader : public QObject
{
	Q_OBJECT

	public:
		enum Mode
		{
			MODE_SAVETOFILE,
			MODE_KEEPINMEMORY,
			MODE_INVALID
		};

		// constructor & destructor
		FileDownloader(QUrl downloadFromUrl, const char* saveToFilename, QObject* parent=NULL);
		FileDownloader(QUrl downloadFromUrl, QObject* parent=NULL);
		virtual ~FileDownloader();

		void Start();

		QUrl GetUrl() const													{ return mDownloadFromUrl; }
		uint32 GetNumFails() const											{ return mNumFails; }

		const char* GetFilename()											{ return mSaveToFilename.AsChar(); }
		const QByteArray& GetFileBuffer()									{ return mFileBuffer; }

	signals:
		void FinishedDownload(FileDownloader* downloader);
		void DownloadFailed(FileDownloader* downloader);
		void DownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);

	private slots:
		void OnFileDownloaded(QNetworkReply* reply);
		void OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
		void OnReadyRead();

	private:
		void Init(QUrl downloadFromUrl, const char* saveToFilename, Mode mode);

		Mode					mMode;

		QUrl					mDownloadFromUrl;
		QNetworkReply*			mReply;
		QNetworkAccessManager	mNAM;
		uint32					mNumFails;

		// save to file mode
		Core::String			mSaveToFilename;
		Core::String			mSaveToTempFilename;
		QFile*					mFile;
		uint32					mTempBufferSize;
		QByteArray				mTempBuffer;

		// save to memory mode
		QByteArray				mFileBuffer;
};


#endif
