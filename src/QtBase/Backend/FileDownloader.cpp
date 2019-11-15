/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "FileDownloader.h"
#include <Core/LogManager.h>
#include <QNetworkRequest>
#include <QNetworkReply>


using namespace Core;

// constructor
FileDownloader::FileDownloader(QUrl downloadFromUrl, const char* saveToFilename, QObject *parent) : QObject(parent)
{
	Init( downloadFromUrl, saveToFilename, MODE_SAVETOFILE );
}


// constructor
FileDownloader::FileDownloader(QUrl downloadFromUrl, QObject *parent) : QObject(parent)
{
	Init( downloadFromUrl, NULL, MODE_KEEPINMEMORY );
}


// destructor
FileDownloader::~FileDownloader()
{
	if (mFile != NULL)
	{
		mFile->close();
		mFile->deleteLater();
	}
}


void FileDownloader::Init(QUrl downloadFromUrl, const char* saveToFilename, Mode mode)
{
	mDownloadFromUrl	= downloadFromUrl;
	mMode				= mode;
	mNumFails			= 0;
	mReply				= NULL;
	mFile				= NULL;
	mTempBufferSize		= 100 * 1024;
	String urlString	= mDownloadFromUrl.url().toLatin1().constData();

	switch (mMode)
	{
		case MODE_SAVETOFILE:
		{
			mSaveToFilename		= FromQtString(saveToFilename);

			mSaveToTempFilename	= mSaveToFilename;
			mSaveToTempFilename.RemoveFileExtension();
			mSaveToTempFilename += ".tmp";
			break;
		}

		case MODE_KEEPINMEMORY:
		{
			mSaveToFilename.Clear();
			mSaveToTempFilename.Clear();
			break;
		}

		default:
		{
			LogError( "FileDownloader(): Cannot download '%s'. Mode invalid.", urlString.AsChar() );
			break;
		}
	}

	// connect to download finished signal
	connect( &mNAM, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnFileDownloaded(QNetworkReply*)) );
}


// start downloading
void FileDownloader::Start()
{
	if (mMode == MODE_SAVETOFILE)
	{
		// remove temporary file before writing to it, so that we don't append
		QFile::remove( mSaveToTempFilename.AsChar() );

		mFile = new QFile(mSaveToTempFilename.AsChar());
		if (mFile->open(QIODevice::WriteOnly) == false)
		{
			LogWarning( "FileDownloader::Start(): Cannot open file '%s' for writing.", mSaveToFilename.AsChar() );
			mFile->close();
			mFile->deleteLater();
			mFile = NULL;
		}
	}

	// create ssl configuration
	QSslConfiguration sslconf = QSslConfiguration::defaultConfiguration();
#ifndef NEUROMORE_BACKEND_VERIFY_CERT
	sslconf.setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
#endif

	// create network request
	QNetworkRequest request(mDownloadFromUrl);
	request.setSslConfiguration(sslconf);

	mReply = mNAM.get(request);

	connect( mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(OnDownloadProgressChanged(qint64, qint64)) );
	connect( mReply, SIGNAL(readyRead()), this, SLOT(OnReadyRead()) );
}


// download progress changed
void FileDownloader::OnDownloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
	emit DownloadProgressChanged(bytesReceived, bytesTotal);
}


// called once every time new data is available for reading
void FileDownloader::OnReadyRead()
{
	if (mReply != NULL)
	{
		mTempBuffer = mReply->read(mTempBufferSize);
		while (mTempBuffer.size() > 0)
		{
			//LogInfo("Reading %i", mTempBuffer.size());

			switch (mMode)
			{
				case MODE_SAVETOFILE:
				{
					if (mFile != NULL)
						mFile->write( mTempBuffer );

					break;
				}

				case MODE_KEEPINMEMORY:
				{
					mFileBuffer.append( mTempBuffer );
					break;
				}

				default:
					break;
			}

			mTempBuffer = mReply->read(mTempBufferSize);
		}
	}
}


// called when file got fully downloaded
void FileDownloader::OnFileDownloaded(QNetworkReply* reply)
{
	if (mFile != NULL)
	{
		mFile->close();
		mFile->deleteLater();
		mFile = NULL;
	}

	// get access to the return code
	if (reply->error() == QNetworkReply::NoError)
	{
		// turn temp file into actual one
		if (mMode == MODE_SAVETOFILE)
			QFile::rename( mSaveToTempFilename.AsChar(), mSaveToFilename.AsChar() );

		emit FinishedDownload(this);
	}
	else
	{
		// remove temp file
		if (mMode == MODE_SAVETOFILE)
			QFile::remove( mSaveToTempFilename.AsChar() );

		String errorMsg;
		errorMsg.Format( "%s (ErrorCode=%i).", reply->errorString().toLatin1().constData(), reply->error() );
		LogError("FileDownloader: %s", errorMsg.AsChar() );

		mNumFails++;
		emit DownloadFailed(this);
	}

	reply->deleteLater();
	mReply = NULL;
}