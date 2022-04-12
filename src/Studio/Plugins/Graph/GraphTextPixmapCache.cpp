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

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "GraphTextPixmapCache.h"
#include <Core/Timer.h>


//#define ENABLE_TEXT_DEBUGMODE

using namespace Core;

// constructor
GraphTextPixmapCache::GraphTextPixmapCache(GraphShared* shared) : QObject()
{
	mShared = shared;
	Init();
}


// destructor
GraphTextPixmapCache::~GraphTextPixmapCache()
{
	Clear();
}


// get rid of all cache elements
void GraphTextPixmapCache::Clear()
{
	// clear the hash table
	for (auto itr=mHashTable.begin(); itr!=mHashTable.end(); itr++)
	{
		CacheElement* cacheElement = itr->second;
		delete cacheElement;
	}
	mHashTable.clear();

	// get rid of the uncached text rendering ringbuffer pixmaps
	const uint32 numUncachedTexts = mUncachedPixmaps.Size();
	for (uint32 i=0; i<numUncachedTexts; ++i)
		delete mUncachedPixmaps[i];
	mUncachedPixmaps.Clear();
}


void GraphTextPixmapCache::Init()
{
	mHashTable.reserve( 1024 );

	// support up to 1024 uncached texts per frame
	mUncachedPixmaps.Resize( 1024);

	// allocate memory for uncached text rendering ringbuffer pixmaps
	const uint32 numUncachedTexts = mUncachedPixmaps.Size();
	for (uint32 i=0; i<numUncachedTexts; ++i)
		mUncachedPixmaps[i] = new UncachedTextElement();

	// reset the ringbuffer index
	mUncachedTextIndex = 0;
}


GraphTextPixmapCache::CacheElement* GraphTextPixmapCache::FindCacheElement(const String& text)
{
	// check if the text is empty, if yes return directly
	if (text.IsEmpty() == true)
		return NULL;

	// search for the given text in the hash table
	auto itr = mHashTable.find(text);
	if (itr != mHashTable.end())
	{
		// found
		return itr->second;
	}
	else
	{
		// not found

		// in case it doesn't exist yet, create the element
		CacheElement* cacheElement = new CacheElement(this);

		// add it to the hash map
		mHashTable[text] = cacheElement;

		return cacheElement;
	}
}


void GraphTextPixmapCache::RenderPixmap(QPainter& painter, QPixmap& pixmap, const QRect& rect, Qt::Alignment textAlignment)
{
	//Timer timing;

	// render the pixmap
	//painter.save();

#ifdef ENABLE_TEXT_DEBUGMODE
		painter.setBrush( Qt::white );
		painter.setPen( Qt::black );
		painter.drawRect( rect );
#endif

		const int textWidth			= pixmap.width();
		const int textHeight		= pixmap.height();
		QRect textRect				= QRect( 0, 0, textWidth, textHeight );
			
		switch (textAlignment)
		{
			case Qt::AlignLeft:
			{
				textRect.moveCenter( rect.center() );
				textRect.moveLeft( rect.left() );
				break;
			}

			case Qt::AlignRight:
			{
				textRect.moveCenter( rect.center() );
				textRect.moveRight( rect.right() );
				break;
			}

			default:
			{
				textRect.moveCenter( rect.center() );
			}
		}

		//painter.setPen( Qt::transparent );
		//painter.setBrush( Qt::transparent );
		//painter.drawRect( rect );
		painter.drawPixmap( textRect.topLeft(), pixmap );

	//painter.restore();

	//const float time = timing.GetTime();
	//LogInfo("%s - %.2f ms", text.AsChar(), time*1000);
}


void GraphTextPixmapCache::RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment)
{
	if (text.IsEmpty() == true)
		return;

	// get the cache element (which contains differently styled and rendered pixmaps)
	CacheElement* cacheElement = FindCacheElement(text);
	CORE_ASSERT(cacheElement != NULL);
	if (cacheElement == NULL)
		return;

	// find the correctly styled and rendered pixmap for our case
	TextPixmap* pixmap = cacheElement->FindPixmap( mShared, text, textColor, rect, font, fontMetrics );

	// render it
	RenderPixmap( painter, pixmap->mPixmap, rect, textAlignment );
}


void GraphTextPixmapCache::RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment)
{
	if (text.IsEmpty() == true)
		return;

	UncachedTextElement* element = mUncachedPixmaps[mUncachedTextIndex];

	// pre-render to pixmap
	FillPixmap( &element->mPainter, &element->mPixmap, mShared, text, textColor, font, fontMetrics );

	// render it
	RenderPixmap( painter, element->mPixmap, rect, textAlignment );

	// increase the ring buffer index and wrap it in case it reaches the end
	mUncachedTextIndex = (mUncachedTextIndex+1) % mUncachedPixmaps.Size();
}


// constructor
GraphTextPixmapCache::CacheElement::CacheElement(GraphTextPixmapCache* pixmapCache)
{
	mPixmapCache = pixmapCache;
}


// destructor
GraphTextPixmapCache::CacheElement::~CacheElement()
{
	// get the number of pixmaps and iterate through them
	const uint32 numPixmaps = mPixmaps.Size();
	for (uint32 i=0; i<numPixmaps; ++i)
		delete mPixmaps[i];

	mPixmaps.Clear();
}


GraphTextPixmapCache::TextPixmap* GraphTextPixmapCache::CacheElement::FindPixmap(GraphShared* shared, const String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics)
{
	// get the number of pixmaps and iterate through them
	const uint32 numPixmaps = mPixmaps.Size();
	for (uint32 i=0; i<numPixmaps; ++i)
	{
		TextPixmap* pixmap = mPixmaps[i];

		if (pixmap->mRect == rect &&
			pixmap->mColor == textColor)
			return pixmap;
	}

	// the pixmap has not been found, create and return it
	return CreatePixmap(shared, text, textColor, rect, font, fontMetrics);
}


void GraphTextPixmapCache::FillPixmap(QPainter* painter, QPixmap* pixmap, GraphShared* shared, const String& text, const QColor& textColor, const QFont& font, const QFontMetrics& fontMetrics)
{
	shared->GetTextOption().setAlignment( Qt::AlignCenter );

	// specify the size of the pixmap based on the text
    int width	= fontMetrics.width(text.AsChar());
#ifndef NEUROMORE_PLATFORM_WINDOWS
    width += 1;
#endif
	const int height= fontMetrics.height();
	*pixmap = QPixmap(width, height);

	// fill the pixmap fully transparent (do before begin)
#ifdef ENABLE_TEXT_DEBUGMODE
	pixmap->fill(Qt::red);
#else
	pixmap->fill(Qt::transparent);
#endif

	// begin painting onto the pixmap
	painter->begin( pixmap );

		painter->setRenderHint( QPainter::Antialiasing );
		painter->setRenderHint( QPainter::HighQualityAntialiasing );
		painter->setRenderHint( QPainter::TextAntialiasing );
		painter->setBackgroundMode( Qt::TransparentMode );

		painter->setFont( font );
		painter->setPen( textColor );
		painter->setBrush( Qt::NoBrush );

		QRect textRect( 0, 0, width, height );
		mTempString = text.AsChar();
		painter->drawText( textRect, mTempString, shared->GetTextOption() );

	painter->end();
}


// update the transparent pixmap that contains all text for the node
GraphTextPixmapCache::TextPixmap* GraphTextPixmapCache::CacheElement::CreatePixmap(GraphShared* shared, const String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics)
{
	mPixmapCache->FillPixmap( &mPainter, &mPixmap, shared, text, textColor, font, fontMetrics );

	TextPixmap* textPixmap	= new TextPixmap();
	textPixmap->mPixmap		= mPixmap;
	textPixmap->mWidth		= mPixmap.width();
	textPixmap->mHeight		= mPixmap.height();
	textPixmap->mRect		= rect;
	textPixmap->mColor		= textColor;
	textPixmap->mPixmap		= mPixmap;

	mPixmaps.Add(textPixmap);

	return textPixmap;
}
