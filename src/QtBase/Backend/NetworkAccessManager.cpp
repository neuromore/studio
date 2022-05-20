/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required files
#include "NetworkAccessManager.h"
#include "BackendHelpers.h"
#include "../QtBaseManager.h"
#include "LoginRequest.h"
#include "LoginResponse.h"
#include <Core/LogManager.h>
#include <Core/EventManager.h>
#include <QNetworkReply>
#include <QNetworkDiskCache>

using namespace Core;

// constructor
NetworkAccessManager::NetworkAccessManager(QObject* parent) : QObject(parent)
{
	mRequestCounter = 0;
	mLoggingEnabled	= true;

	mPresets.Add(new ServerPreset("neuromore (AWS)", "backend.neuromore.com", "https://backend.neuromore.com/api/", 443, "https://account.neuromore.com"));
	mPresets.Add(new ServerPreset("eego-perform (AWS)", "backend.eego-perform.com", "https://backend.eego-perform.com/api/", 443, "https://account.eego-perform.com"));
	mPresets.Add(new ServerPreset("starrbase (AWS)", "backend.starrbase.myneurva.com", "https://backend.starrbase.myneurva.com/api/", 443, "https://starrbase.myneurva.com"));

	// non-prod server presets (always neuromore)
	#ifndef PRODUCTION_BUILD
		mPresets.Add( new ServerPreset("Swap (AWS)",				"swap-prod-server.neuromore.com",	"https://swap-deployment.neuromore.com/api/",	443,	"https://account.neuromore.com") );
		mPresets.Add( new ServerPreset("Test (AWS)",				"backend-test.neuromore.com",		"https://backend-test.neuromore.com/api/",		443,	"https://account-test.neuromore.com") );
		mPresets.Add( new ServerPreset("localhost",					"localhost",						"http://localhost/neuromore-server/api/",		8080,	"http://localhost") );
	#endif

	// set default one
	if (brandingName == AntBrandingName) {
		mActivePresetIndex = 1;
	} else if (brandingName == StarrbaseBrandingName) {
		mActivePresetIndex = 2;
	} else {
		mActivePresetIndex = 0;
	}

	// create the network access manager
	mNetworkAccessManager = new QNetworkAccessManager(parent);


	// CACHE
    QNetworkDiskCache* diskCache = new QNetworkDiskCache(parent);

	String cacheLocation = GetQtBaseManager()->GetAppDataFolder() + String("Cache/Network/");
	cacheLocation.ConvertToNativePath();
    diskCache->setCacheDirectory( cacheLocation.AsChar() );
    
	diskCache->setMaximumCacheSize( 32 * 1024*1024 );

	mNetworkAccessManager->setCache(diskCache);


	// SSL

	// setup the ssl config
	mSslConfig = QSslConfiguration::defaultConfiguration();

#ifndef NEUROMORE_BACKEND_VERIFY_CERT
   mSslConfig.setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
#endif

    //QList<QSslCertificate> systemCerts = mSslConfig.systemCaCertificates();
    //int numCerts = systemCerts.count();
    //for (int i=0; i<numCerts; i++)
    //{
    //    LogInfo("=========================");
    //    LogSslCertificate( systemCerts[i] );
    //}
    
    //mSslConfig.setCaCertificates(systemCerts);
    
#ifndef NEUROMORE_PLATFORM_WINDOWS
    // somehow OSX doesnt have the GeoTrust root certificate installed (or doesn't trust it)
    // manually add it
	QList<QSslCertificate> certificates = QSslCertificate::fromPath( ":/Assets/Certificates/Equifax_Secure_Certificate_Authority.pem" );
	mSslConfig.setCaCertificates( certificates );
#endif
    //mSslConfig.setProtocol( QSsl::SslV3 );

	// initiates a connection to the host given by hostName at port port, using sslConfiguration
	// this function is useful to complete the TCP and SSL handshake to a host before the HTTPS request is made, resulting in a lower network latency
	mNetworkAccessManager->connectToHostEncrypted( GetActiveServerPreset().mBackendUrl.AsChar(), GetActiveServerPreset().mBackendPort, mSslConfig);

	// connect to the ssl error signal
	connect( mNetworkAccessManager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), this, SLOT(OnSSLError(QNetworkReply*, const QList<QSslError>&)) );
}


// destructor
NetworkAccessManager::~NetworkAccessManager()
{
	// get rid of the presets
	const uint32 numPresets = mPresets.Size();
	for (uint32 i=0; i<numPresets; ++i)
		delete mPresets[i];
	mPresets.Clear();

	delete mNetworkAccessManager;
}


void NetworkAccessManager::LogStringList(const char* name, const QStringList& stringList)
{
    String text = "    ";
    text += name;
    text += ": ";
    
    int numElements = stringList.count();
    for (int i=0; i<numElements; ++i)
        text += FromQtString( stringList[i] );
    
    LogInfo(text.AsChar());
}


void NetworkAccessManager::LogSslCertificate(QSslCertificate certificate)
{
    LogInfo( "Is Self Signed: %i", certificate.isSelfSigned() );
    
    LogInfo( "Subject Info:" );
    LogStringList( "Organization", certificate.subjectInfo(QSslCertificate::Organization) );
    LogStringList( "Common Name", certificate.subjectInfo(QSslCertificate::CommonName) );
    LogStringList( "Locality", certificate.subjectInfo(QSslCertificate::LocalityName) );
    LogStringList( "Organizational Unit", certificate.subjectInfo(QSslCertificate::OrganizationalUnitName) );
    LogStringList( "Country", certificate.subjectInfo(QSslCertificate::CountryName) );
    LogStringList( "State", certificate.subjectInfo(QSslCertificate::StateOrProvinceName) );
    LogStringList( "Distinguished Name Qualifier", certificate.subjectInfo(QSslCertificate::DistinguishedNameQualifier) );
    LogStringList( "Serial Number", certificate.subjectInfo(QSslCertificate::SerialNumber) );
    LogStringList( "E-Mail Address", certificate.subjectInfo(QSslCertificate::EmailAddress) );
    
    LogInfo( "Issuer Info:" );
    LogStringList( "Organization", certificate.issuerInfo(QSslCertificate::Organization) );
    LogStringList( "Common Name", certificate.issuerInfo(QSslCertificate::CommonName) );
    LogStringList( "Locality", certificate.issuerInfo(QSslCertificate::LocalityName) );
    LogStringList( "Organizational Unit", certificate.issuerInfo(QSslCertificate::OrganizationalUnitName) );
    LogStringList( "Country", certificate.issuerInfo(QSslCertificate::CountryName) );
    LogStringList( "State", certificate.issuerInfo(QSslCertificate::StateOrProvinceName) );
    LogStringList( "Distinguished Name Qualifier", certificate.issuerInfo(QSslCertificate::DistinguishedNameQualifier) );
    LogStringList( "Serial Number", certificate.issuerInfo(QSslCertificate::SerialNumber) );
    LogStringList( "E-Mail Address", certificate.issuerInfo(QSslCertificate::EmailAddress) );
}


// construct a url object
QUrl NetworkAccessManager::ConstructUrl(const String& methodName, const String& parameters)
{
	// construct the url string
	String urlString = GetActiveServerPreset().mBackendUrl;
	urlString += methodName;

	// add the parameters
	if (parameters.IsEmpty() == false)
	{
		urlString += "?";
		urlString += parameters;
	}

	//LogInfo( "URL: %s", urlString.AsChar() );

	// construct the url object
	QUrl url( urlString.AsChar() );
	url.setPort( GetActiveServerPreset().mBackendPort );

	//LogInfo( "QUrl: url='%s'", FromQtString( url.toString(QUrl::None) ).AsChar() );

	return url;
}


// create the network request
QNetworkRequest NetworkAccessManager::ConstructNetworkRequest(const String& methodName, const String& parameters)
{
	// create the url
	QUrl url = ConstructUrl( methodName, parameters );

	// create the network request
	QNetworkRequest request(url);

	// configure ssl
	request.setSslConfiguration( mSslConfig );

	return request;
}


void NetworkAccessManager::PrepareInterface(const char* progressText, Request::UiMode uiMode)
{
	if (uiMode != Request::UIMODE_BLOCKING)
		return;

	// increase the reference counter
	mRequestCounter++;

	if (progressText == NULL)
		return;

	// show the progress window
	CORE_EVENTMANAGER.OnProgressStart( true, false, false, false );
	CORE_EVENTMANAGER.OnProgressText( progressText );

	// lock the interface
	GetQtBaseManager()->UpdateInterfaceLock();
}


void NetworkAccessManager::NetworkReplyAftermath(QNetworkReply* reply)
{
	// make sure we delete the object again
	// from documentation: After the request has finished, it is the responsibility of the user to delete the QNetworkReply object at an appropriate time. Do not directly delete it inside the slot connected to finished(). You can use the deleteLater() function.
	reply->deleteLater();

	// was this request a blocking or async background one?
	const int uiModeInt = reply->property("uiMode").toInt();
	if (uiModeInt == Request::UIMODE_BLOCKING)
	{
		// decrease the active http rest request count
		if (mRequestCounter > 0)
			mRequestCounter--;

		if (mRequestCounter <= 0)
		{
			// close the progress window
			CORE_EVENTMANAGER.OnProgressEnd();
		}

		// unlock the interface
		GetQtBaseManager()->UpdateInterfaceLock();
	}
}


QNetworkReply* NetworkAccessManager::post_Deprecated(QNetworkRequest& request, QHttpMultiPart* multiPart, const char* progressText)
{
	Request::UiMode uiMode = Request::UIMODE_BLOCKING;

	PrepareInterface(progressText, uiMode);

	QNetworkReply* reply = mNetworkAccessManager->post( request, multiPart );
	reply->setProperty( "uiMode", uiMode );

	if (mLoggingEnabled == true && CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DETAILEDINFO)
		LogDetailedInfo("REST: post(): url=\"%s\"", FromQtString(request.url().toString()).AsChar());

	return reply;
}


void NetworkAccessManager::OnSSLError(QNetworkReply* networReply, const QList<QSslError>& errors)
{
	String errorString;

	const uint32 numErrors = errors.length();

	if (numErrors == 1)
		errorString = FromQtString( errors[0].errorString() );
	else
	{
		for (uint32 i=0; i<numErrors; ++i)
		{
			errorString.FormatAdd( "Error #%i: ", i );
			errorString += FromQtString( errors[i].errorString() ) + "\n";
		}
	}

	BackendHelpers::ReportError( errorString.AsChar() );
}


void NetworkAccessManager::SetActiveServerPresetIndex(uint32 index)
{
	if (index < mPresets.Size())
		mActivePresetIndex = index;
	else
		mActivePresetIndex = 0;
}


// construct a url object
QUrl NetworkAccessManager::ConstructUrl(const Request& request)
{
	// construct the url string
	String urlString = GetActiveServerPreset().mBackendUrl;
	urlString += request.GetUrl();

	urlString.RemoveAllParts("//api");

	// construct the url object
	QUrl url( urlString.AsChar() );
	url.setPort( GetActiveServerPreset().mBackendPort );

	// add the parameters
	if (request.HasUrlParameters() == true)
		url.setQuery( request.GetUrlQueryObject() );

	//LogInfo( "URL: %s", urlString.AsChar() );
	//LogInfo( "QUrl: url='%s'", FromQtString( url.toString(QUrl::None) ).AsChar() );

	return url;
}


QNetworkReply* NetworkAccessManager::ProcessRequest(const Request& request, Request::UiMode uiMode, bool disableLogging, const QVariant& cacheMode)
{
	mTimer.GetTimeDelta();
	QNetworkReply* reply = NULL;

	/*if ()
	{
		// 1. construct /users/get request
		LoginRequest request(  );

		// 2. process request and connect to the reply
		QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
		connect(reply, &QNetworkReply::finished, this, [reply, this, request, uiMode, disableLogging]()
		{
			QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

			// 3. construct and parse response
			LoginResponse response(networkReply);

			// 4. handle response
			if (response.HasError() == false)
				ProcessRequest(request, uiMode, disableLogging);
		});

		return;
	}*/

	PrepareInterface( request.GetDisplayText(), uiMode );

	const bool logging = (disableLogging == false && mLoggingEnabled == true && CORE_LOGMANAGER.GetLogLevels() & LOGLEVEL_DETAILEDINFO);

	String bodyString = request.GetBodyString(false);

	Request::Method requestMethod = request.GetRequestMethod();
	switch (requestMethod)
	{
		// HTTP GET REQUEST
		case Request::REQUESTMETHOD_GET:
		{
			// create the url
			QUrl url = ConstructUrl( request );

			// create the network request
			QNetworkRequest networkRequest(url);

			// cache control
			networkRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, cacheMode);

			// configure ssl
			networkRequest.setSslConfiguration( mSslConfig );

			reply = mNetworkAccessManager->get( networkRequest );
			reply->setProperty( "uiMode", uiMode );

			if (logging == true)
			{
				LogDetailedInfo("REST get(): url=\"%s\"", FromQtString(BackendHelpers::ConvertToSecureUrl(networkRequest.url()).toString()).AsChar());

				// response timing
				connect(reply, &QNetworkReply::finished, this, [reply, this](){ LogDetailedInfo( "Response timing: %.2f ms (Url=%s)", mTimer.GetTimeDelta().InMilliseconds(), FromQtString(BackendHelpers::ConvertToSecureUrl(reply->url()).toString()).AsChar() ); });
			}

			break;
		}

		// HTTP POST REQUEST
		case Request::REQUESTMETHOD_POST:
		{
			// create the url
			QUrl url = ConstructUrl( request );

			// create the network request
			QNetworkRequest networkRequest(url);

			// configure ssl
			networkRequest.setSslConfiguration( mSslConfig );

			// header & body
			if (bodyString.IsEmpty() == false)
				networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

			reply = mNetworkAccessManager->post( networkRequest, bodyString.AsChar() );
			reply->setProperty( "uiMode", uiMode );

			if (logging == true)
			{
				LogDetailedInfo("REST post(): url=\"%s\"", FromQtString(BackendHelpers::ConvertToSecureUrl(networkRequest.url()).toString()).AsChar());

				// response timing
				connect(reply, &QNetworkReply::finished, this, [reply, this](){ LogDetailedInfo( "Response timing: %.2f ms (Url=%s)", mTimer.GetTimeDelta().InMilliseconds(), FromQtString(BackendHelpers::ConvertToSecureUrl(reply->url()).toString()).AsChar() ); });
			}

			break;
		}

		// HTTP PUT REQUEST
		case Request::REQUESTMETHOD_PUT:
		{
			// create the url
			QUrl url = ConstructUrl( request );

			// create the network request
			QNetworkRequest networkRequest(url);

			// configure ssl
			networkRequest.setSslConfiguration( mSslConfig );

			// header & body
			if (bodyString.IsEmpty() == false)
				networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

			reply = mNetworkAccessManager->put( networkRequest, bodyString.AsChar() );
			reply->setProperty( "uiMode", uiMode );

			if (logging == true)
			{
				LogDetailedInfo("REST put(): url=\"%s\"", FromQtString(BackendHelpers::ConvertToSecureUrl(networkRequest.url()).toString()).AsChar());

				// response timing
				connect(reply, &QNetworkReply::finished, this, [reply, this](){ LogDetailedInfo( "Response timing: %.2f ms (Url=%s)", mTimer.GetTimeDelta().InMilliseconds(), FromQtString(BackendHelpers::ConvertToSecureUrl(reply->url()).toString()).AsChar() ); });
			}

			break;
		}

        // HTTP DELETE REQUEST
        case Request::REQUESTMETHOD_DELETE:
        {
			// create the url
			QUrl url = ConstructUrl( request );

			// create the network request
			QNetworkRequest networkRequest(url);

			// configure ssl
			networkRequest.setSslConfiguration( mSslConfig );

			reply = mNetworkAccessManager->deleteResource( networkRequest );
			reply->setProperty( "uiMode", uiMode );

			if (logging == true)
			{
				LogDetailedInfo("REST delete(): url=\"%s\"", FromQtString(BackendHelpers::ConvertToSecureUrl(networkRequest.url()).toString()).AsChar());

				// response timing
				connect(reply, &QNetworkReply::finished, this, [reply, this](){ LogDetailedInfo( "Response timing: %.2f ms (Url=%s)", mTimer.GetTimeDelta().InMilliseconds(), FromQtString(BackendHelpers::ConvertToSecureUrl(reply->url()).toString()).AsChar() ); });
			}

            break;
        }
	}

	if (logging == true)
	{
		if (bodyString.Contains("password") == false)
			LogDetailedInfo( bodyString.AsChar() );
	}

	return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
ServerPreset::ServerPreset() : QObject(NULL)																							
{ 
	mBackendPort		= 0; 
	mLookupSuccessful	= false; 
}


// constructor
ServerPreset::ServerPreset(const char* name, const char* lookupUrl, const char* backendUrl, int backendPort, const char* webserverUrl) : QObject(NULL)
{ 
	mName				= name; 
	mBackendUrl			= backendUrl; 
	mWebserverUrl		= webserverUrl; 
	mBackendPort		= backendPort; 
	mLookupSuccessful	= false;

	//QHostInfo::lookupHost( lookupUrl, this, SLOT(OnLookedUp(QHostInfo)));
}


// called from the host lookup
void ServerPreset::OnLookedUp(const QHostInfo &host)
{
	if (host.error() != QHostInfo::NoError)
	{
		mLookupSuccessful = false;
		return;
	}

	mLookupSuccessful = true;
}