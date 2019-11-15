/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PAINTERSTATICTEXTCACHE_H
#define __NEUROMORE_PAINTERSTATICTEXTCACHE_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <QPainter>
#include <QStaticText>
#include <unordered_map>
#include <string>


class QTBASE_API PainterStaticTextCache : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		PainterStaticTextCache();
		~PainterStaticTextCache();

		void Init(uint32 numReserveElements=1024);
		void Clear();
		void RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment);
		void RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QPoint& textPos, const QFont& font, const QFontMetrics& fontMetrics);

		void RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment);
		void RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QPoint& textPos, const QFont& font, const QFontMetrics& fontMetrics);

		uint32 GetNumCachedElements() const														{ return (uint32)mHashTable.size(); }

	private:
		struct StaticText
		{
			StaticText(const Core::String& text, const QFont& font, const QFontMetrics& fontMetrics);

			void Render(QPainter& painter, const QColor& textColor, const QRect& rect, Qt::Alignment textAlignment);
			void Render(QPainter& painter, const QColor& textColor, const QPoint& pos);

			Core::String	mText;
			QStaticText		mStaticText;
			QFont			mFont;
			QFontMetrics	mFontMetrics;
		};

		class CacheElement
		{
			public:
				CacheElement();
				~CacheElement();
				StaticText* Find(const Core::String& text, const QFont& font, const QFontMetrics& fontMetrics);

			private:
				QStaticText* CreateStaticText(const Core::String& text, const QFont& font);

				Core::Array<StaticText*>	mStaticTexts;
		};

		CacheElement* FindCacheElement(const Core::String& text);

		std::unordered_map<Core::String, CacheElement*, Core::StringHasher> mHashTable;
		Core::String mTempString;
};


#endif
