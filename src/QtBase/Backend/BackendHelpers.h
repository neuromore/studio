/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_BACKENDHELPERS_H
#define __NEUROMORE_BACKENDHELPERS_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <QUrl>
#include <QNetworkReply>

#define BACKEND_RETURNCODE_OK					200
#define BACKEND_RETURNCODE_UPLOADRECEIVED		202


class QTBASE_API BackendHelpers
{
	public:
		static void LogHeader(QNetworkReply* reply);
		static void ReportError(const char* errorMessage, bool allowMessageBox=true);
		static bool HasError(QNetworkReply* networkReply, const char* replyDataString, int expectedReturnCode=BACKEND_RETURNCODE_OK, bool allowMessageBox=true);
		static Core::String EncodeUrlParameter(const char* parameterValue);
		static QUrl ConvertToSecureUrl(QUrl url);
};


#endif
