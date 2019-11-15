/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_RESPONSE
#define __NEUROMORE_RESPONSE

// include required headers
#include "../QtBaseConfig.h"
#include <Core/Json.h>
#include <QNetworkReply>


// response
class QTBASE_API Response
{
	public:
		// constructor & destructor
		Response(QNetworkReply* reply, bool handleCustomErrorCodes=false);
		virtual ~Response();

		bool HasError() const;
		bool HasErrorOfType(const char* errorType);

		const char* GetErrorType();
		const char* GetErrorMessage();

		void ShowErrorDialog(QWidget* parent);

	protected:
		void ShowError(QWidget* parent, const char* replyData, const char* title, const char* errorMessage);

		QNetworkReply*	mReply;
		int32			mExpectedReturnCode;
		bool			mHasError;
		Core::Json		mJson;
};


#endif
