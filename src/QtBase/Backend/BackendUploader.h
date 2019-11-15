/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BACKENDUPLOADER_H
#define __NEUROMORE_BACKENDUPLOADER_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "NetworkAccessManager.h"
#include <QNetworkReply>
#include <QFile>
#include <QBasicTimer>


#define UPLOADER_PERCENTAGE_PROCESSING		0.10f

// PHASE 1: PROCESSING			= Uploading
// PHASE 2: POST PROCESSING		= Check if upload worked and file is on S3 server
// PHASE 3: FINISHING			= uploading JSON
class QTBASE_API BackendUploader : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		BackendUploader(NetworkAccessManager* networkAccessManager, QObject* parent=NULL);
		virtual ~BackendUploader();

		void Start();

		void SetIsEnabled(bool isEnabled)			{ mIsEnabled = isEnabled; }
		bool IsEnabled() const						{ return mIsEnabled; }

		bool IsBusy() const							{ return mIsBusy; }

		class Callback
		{
			public:
				virtual void OnStartUpload() = 0;
				virtual void OnFinishedUpload() = 0;
				virtual void OnUploadProgress(const char* progressText, const char* subProgressText, float progress, float subProgress) = 0;
		};

		void SetCallback(Callback* callback)											{ mCallback = callback; }

	private slots:
		// call this when you want to start physiological data upload
		void StartUpload(const char* token);

	signals:
		// gets emitted after all files got uploaded
		void UploadFinished();

	private slots:
		void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);
		void OnUploadFinished();

	private:
		class QueueEntry
		{
			public:
				QueueEntry(BackendUploader* uploader, const char* fullFilePath, uint32 index);
				virtual ~QueueEntry();

				bool OnStartUpload();
				bool OnUploadFinished(bool error);

				inline bool IsUploading() const									{ return mIsUploading; }
				inline uint32 GetUploadAttemptNr() const						{ return mUploadAttemptNr; }
				inline bool FinishedUpload() const								{ return mFinishedUpload; }

				inline uint32 GetIndex() const									{ return mIndex; }
				inline const char* GetFilename() const							{ return mFilename.AsChar(); }
				inline const Core::String& GetFilenameString() const			{ return mFilename; }
				inline const char* GetAbsoluteFilePath() const					{ return mFullFilePath.AsChar(); }
				inline const Core::String& GetAbsoluteFilePathString() const	{ return mFullFilePath; }
				inline QFile* GetFile() const									{ return mFile; }

			private:
				BackendUploader*	mUploader;
				Core::String		mFullFilePath;
				Core::String		mFilename;
				uint32				mIndex;

				// file upload
				bool				mIsUploading;
				bool				mFinishedUpload;
				uint32				mUploadAttemptNr;

				// data
				QFile*				mFile;
		};

		class Queue
		{
			public:
				Queue(BackendUploader* uploader)										{ mUploader = uploader; mNumEntries=0; mMaxNumAttempts=4; }
				virtual ~Queue()														{ Clear(); }
				void Clear();
				bool IsEmpty() const													{ return mEntries.IsEmpty(); }
				uint32 GetNumEntriesInitTime() const									{ return mNumEntries; }
				uint32 GetNumEntries() const											{ return mEntries.Size(); }
				QueueEntry* GetEntry(uint32 index) const								{ return mEntries[index]; }
				void ReInit(const char* fileExtension);
				void Log();

				// upload queue control
				QueueEntry* FindEntry(const char* filename);
				bool IsInQueue(const char* filename)									{ return (FindEntry(filename) != NULL); }
				bool RemoveEntry(QueueEntry* entry);
				void AddFiles(const char* folderPath, const char* extensionFilter); // example: extensionFilter="*.json"

				QueueEntry* FindUploadingEntry() const;
				QueueEntry* FindNextUploadEntry();

				inline bool IsUploading() const											{ return (FindUploadingEntry() != NULL); }

			private:
				Core::Array<QueueEntry*>	mEntries;
				uint32						mNumEntries;
				BackendUploader*			mUploader;
				uint32						mMaxNumAttempts;
		};

		// progress window
		void UpdateProgressValue(uint32 currentFileIndex, bool isProcessing);
		void UpdateProgressCallback()											{ if (mCallback != NULL) mCallback->OnUploadProgress(mProgressText.AsChar(), mSubProgressText.AsChar(), mProgress, mSubProgress); }

		// upload helpers
		bool UploadEntry(QueueEntry* entry);
		void UploadNextFile();

		// timer event
		void timerEvent(QTimerEvent* event);

		bool RemoveCorrespondingFiles(QueueEntry* entry);

		NetworkAccessManager*		mNetworkAccessManager;
		Core::String				mToken;
		Core::String				mTempString;
		bool						mIsBusy;
		bool						mIsEnabled;

		// queue
		Queue*						mQueue;

		// progress information
		Callback*					mCallback;
		Core::String				mProgressText;
		Core::String				mSubProgressText;
		float						mProgress;
		float						mSubProgress;

		// auto-start timer
		QBasicTimer					mAutoStartTimer;
};


#endif
