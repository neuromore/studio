/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_REQUEST
#define __NEUROMORE_REQUEST

// include required headers
#include "../QtBaseConfig.h"
#include <QUrlQuery>
#include <Core/String.h>
#include <Core/Json.h>


// request
class QTBASE_API Request
{
	public:
		enum Method
		{
			REQUESTMETHOD_GET,
			REQUESTMETHOD_POST,
			REQUESTMETHOD_PUT,
			REQUESTMETHOD_DELETE
		};

		enum CacheControl
		{
			CACHE_ALWAYSLOAD,
			CACHE_PREFERCACHE
		};

		enum UiMode
		{
			UIMODE_BLOCKING,
			UIMODE_ASYNC,
			UIMODE_SILENT
		};

		struct UrlParameter
		{
			UrlParameter(const char* name, const char* value);

			Core::String mName;
			Core::String mValue;
		};

		// constructor & destructor
		Request();
		virtual ~Request();

		// url
		void SetUrl(const char* url);
		const char* GetUrl() const;

		// request method
		void SetRequestMethod(Method requestMethod);
		Method GetRequestMethod() const;

		// url parameters
		void AddUrlParameter(const char* name, const char* value);
		void AddUrlParameter(const char* name, uint32 value);
		void AddIntUrlParameter(const char* name, int value);
		void AddBoolUrlParameter(const char* name, bool value);
		void AddDoubleUrlParameter(const char* name, double value);
		bool HasUrlParameters() const;
		QUrlQuery GetUrlQueryObject() const;
		QString ConstructQueryString();

		// body
		const Core::Json& GetBodyJson()	const				{ return mBodyJson; }
		Core::Json::Item GetBodyJsonRoot();
		Core::String GetBodyString(bool pretty) const;

		// display text
		void SetDisplayText(const char* displayText);
		const char* GetDisplayText() const;

	private:
		Core::String		mUrl;
		QUrlQuery			mUrlParameters;
		Method				mRequestMethod;
		Core::String		mDisplayText;
		Core::Json			mBodyJson;
};


#endif
