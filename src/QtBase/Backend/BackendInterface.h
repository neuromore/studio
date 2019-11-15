/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BACKENDINTERFACE_H
#define __NEUROMORE_BACKENDINTERFACE_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/LogManager.h>
#include "NetworkAccessManager.h"
#include "BackendUploader.h"
#include "BackendFileSystem.h"
#include "BackendParameters.h"
#include <QNetworkReply>


class QTBASE_API BackendLogCallback : public QObject, public Core::LogCallback
{
	Q_OBJECT

	public:
		enum { TYPE_ID = 0x023007 };
		BackendLogCallback(NetworkAccessManager* nam)	: LogCallback()				{ mNetworkAccessManager = nam; }
		virtual ~BackendLogCallback()												{}
		uint32 GetType() const override												{ return TYPE_ID; }

		void Log(const char* text, Core::ELogLevel logLevel) override final;

		void ForceLog(const char* text, Core::ELogLevel logLevel);

	private:
		NetworkAccessManager*	mNetworkAccessManager;
};


//
class BackendInterface : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		BackendInterface(QObject* parent=NULL);
		virtual ~BackendInterface();

		inline NetworkAccessManager*	GetNetworkAccessManager() const						{ return mNetworkAccessManager; }
		inline BackendLogCallback*		GetLogCallback() const								{ return mLogCallback; }
		inline BackendUploader*			GetUploader() const									{ return mUploader; }
		inline BackendFileSystem*		GetFileSystem() const								{ return mFileSystem; }
		inline BackendParameters*		GetParameters() const								{ return mParameters; }
		
		void SetIsLogEnabled(bool isLogEnabled)												{ mLogCallback->SetIsEnabled(isLogEnabled); }

	private:
		NetworkAccessManager*		mNetworkAccessManager;
		BackendLogCallback*			mLogCallback;
		BackendUploader*			mUploader;
		BackendFileSystem*			mFileSystem;
		BackendParameters*			mParameters;
};


#endif
