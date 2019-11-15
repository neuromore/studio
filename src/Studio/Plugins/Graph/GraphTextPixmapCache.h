/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __NEUROMORE_GRAPHTEXTPIXMAPCACHE_H
#define __NEUROMORE_GRAPHTEXTPIXMAPCACHE_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include "GraphShared.h"
#include <QPainter>
#include <QPixmap>
#include <unordered_map>


class GraphTextPixmapCache : public QObject
{
	Q_OBJECT
	public:
		GraphTextPixmapCache(GraphShared* shared);
		virtual ~GraphTextPixmapCache();

		void Init();
		void Clear();
		void RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment);
		void RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment);

		void RenderPixmap(QPainter& painter, QPixmap& pixmap, const QRect& rect, Qt::Alignment textAlignment);
		void FillPixmap(QPainter* painter, QPixmap* pixmap, GraphShared* shared, const Core::String& text, const QColor& textColor, const QFont& font, const QFontMetrics& fontMetrics);

		uint32 GetNumCachedElements() const														{ return (uint32)mHashTable.size(); }

	private:
		struct TextPixmap
		{
			QRect			mRect;
			int				mWidth;		// pixmap width
			int				mHeight;	// pixmap height
			QColor			mColor;
			QPixmap			mPixmap;
		};

		class CacheElement
		{
			public:
				CacheElement(GraphTextPixmapCache* pixmapCache);
				virtual ~CacheElement();
				TextPixmap* FindPixmap(GraphShared* shared, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics);

			private:
				TextPixmap* CreatePixmap(GraphShared* shared, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics);
				
				GraphTextPixmapCache*		mPixmapCache;
				Core::Array<TextPixmap*>	mPixmaps;
				QPainter					mPainter;
				QPixmap						mPixmap;
				QString						mTempString;
		};

		CacheElement* FindCacheElement(const Core::String& text);

		std::unordered_map<Core::String, CacheElement*, Core::StringHasher> mHashTable;
		GraphShared*									mShared;
		QString											mTempString;

		class UncachedTextElement
		{
			public:
				QPainter					mPainter;
				QPixmap						mPixmap;
		};

		Core::Array<UncachedTextElement*>							mUncachedPixmaps;
		int												mUncachedTextIndex;
};


#endif
