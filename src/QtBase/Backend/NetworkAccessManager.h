/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKACCESSMANAGER_H
#define __NEUROMORE_NETWORKACCESSMANAGER_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/Timer.h>
#include <QObject>
#include <QNetworkAccessManager>
#include <QHostInfo>
#include "Request.h"


class QTBASE_API ServerPreset : public QObject
{
	Q_OBJECT
	
	public:
		ServerPreset();
		ServerPreset(const char* name, const char* lookupUrl, const char* backendUrl, int backendPort, const char* webserverUrl);
		virtual ~ServerPreset()							{}

		Core::String	mName;
		Core::String	mBackendUrl;
		Core::String	mWebserverUrl;
		int				mBackendPort;
		bool			mLookupSuccessful;

	private slots:
		void OnLookedUp(const QHostInfo &host);
};

//
class QTBASE_API NetworkAccessManager : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		NetworkAccessManager(QObject* parent=NULL);
		virtual ~NetworkAccessManager();

		void NetworkReplyAftermath(QNetworkReply* reply);

		QNetworkReply* post_Deprecated(QNetworkRequest& request, QHttpMultiPart* multiPart, const char* progressText);

		QNetworkReply* ProcessRequest(const Request& request, Request::UiMode uiMode, bool disableLogging=false, const QVariant& cacheMode = QNetworkRequest::PreferCache);
		QNetworkReply* ProcessRequest(const Request& request)						{ return ProcessRequest(request, Request::UIMODE_BLOCKING); }
		QNetworkReply* ProcessRequestAsync(const Request& request)					{ return ProcessRequest(request, Request::UIMODE_ASYNC); }

		QNetworkRequest ConstructNetworkRequest(const Core::String& methodName, const Core::String& parameters);
    
        void LogSslCertificate(QSslCertificate certificate);

		void SetActiveServerPresetIndex(uint32 index);
		uint32 GetActiveServerPresetIndex() const				{ return mActivePresetIndex; }
		const ServerPreset& GetActiveServerPreset()				{ return *mPresets[mActivePresetIndex]; }
		uint32 GetNumPresets() const							{ return mPresets.Size(); }
		const ServerPreset& GetPreset(uint32 index)				{ return *mPresets[index]; }

		bool IsLogEnabled() const								{ return mLoggingEnabled; }
		void SetLoggingEnabled(bool isEnabled)					{ mLoggingEnabled = isEnabled; }

		QNetworkAccessManager* GetQNetworkAccessManager()		{ return mNetworkAccessManager; }

	signals:
		void finished(QNetworkReply* reply);

	private slots:
		void OnSSLError(QNetworkReply* networReply, const QList<QSslError>& errors);

	private:
		void PrepareInterface(const char* progressText, Request::UiMode uiMode);
        void LogStringList(const char* name, const QStringList& stringList);

		QUrl ConstructUrl(const Core::String& methodName, const Core::String& parameters="");
		QUrl ConstructUrl(const Request& request);

		QNetworkAccessManager*		mNetworkAccessManager;

		// ssl
		QSslConfiguration			mSslConfig;

		uint32						mRequestCounter;
		bool						mLoggingEnabled;

		// server information
		Core::Array<ServerPreset*>	mPresets;
		uint32						mActivePresetIndex;

		// global response timer (only works if not multiple requests are sent simultaneously)
		Core::Timer					mTimer;
};


#endif
