/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "PainterStaticTextCache.h"
#include <Core/Timer.h>
#include <Core/LogManager.h>


//#define ENABLE_TEXT_DEBUGMODE

using namespace Core;

// constructor
PainterStaticTextCache::PainterStaticTextCache() : QObject()
{
	Init();
}


// destructor
PainterStaticTextCache::~PainterStaticTextCache()
{
	Clear();
}


// get rid of all cache elements
void PainterStaticTextCache::Clear()
{
	for (auto itr=mHashTable.begin(); itr!=mHashTable.end(); itr++)
	{
		CacheElement* cacheElement = itr->second;
		delete cacheElement;
	}

	mHashTable.clear();
}


void PainterStaticTextCache::Init(uint32 numReserveElements)
{
	mHashTable.reserve( numReserveElements );
}


PainterStaticTextCache::CacheElement* PainterStaticTextCache::FindCacheElement(const String& text)
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
		CacheElement* cacheElement = new CacheElement();

		// add it to the hash map
		mHashTable[text] = cacheElement;

		return cacheElement;
	}
}


void PainterStaticTextCache::RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment)
{
	if (text.IsEmpty() == true)
		return;

	// get the cache element (which contains differently styled static texts for the same actual text)
	CacheElement* cacheElement = FindCacheElement(text);
	CORE_ASSERT(cacheElement != NULL);
	if (cacheElement == NULL)
		return;
    
    RenderTextUncached(painter, text, textColor, rect, font, fontMetrics, textAlignment);

	// find the corresponding static text for our case
	//StaticText* staticText = cacheElement->Find( text, font, fontMetrics );

	// render it
	//staticText->Render( painter, textColor, rect, textAlignment );
}


void PainterStaticTextCache::RenderText(QPainter& painter, const Core::String& text, const QColor& textColor, const QPoint& textPos, const QFont& font, const QFontMetrics& fontMetrics)
{
	if (text.IsEmpty() == true)
		return;

	// get the cache element (which contains differently styled static texts for the same actual text)
	CacheElement* cacheElement = FindCacheElement(text);
	CORE_ASSERT(cacheElement != NULL);
	if (cacheElement == NULL)
		return;
    
    RenderTextUncached(painter, text, textColor, textPos, font, fontMetrics);
    
	// find the corresponding static text for our case
//	StaticText* staticText = cacheElement->Find( text, font, fontMetrics );

	// render it
//	staticText->Render( painter, textColor, textPos );
}


void PainterStaticTextCache::RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QRect& rect, const QFont& font, const QFontMetrics& fontMetrics, Qt::Alignment textAlignment)
{
	//Timer timing;

    painter.setFont( font );
    painter.setPen( textColor );
    painter.setBrush( Qt::NoBrush );

#ifdef ENABLE_TEXT_DEBUGMODE
	painter.setPen( Qt::black );
	painter.drawRect( rect );
#endif
        
    // static text drawing
    const float textWidth		= fontMetrics.width( text.AsChar() );
    const float halfTextWidth	= textWidth * 0.5 + 0.5;
    const float halfTextHeight	= fontMetrics.height() * 0.5 + 0.5;
    const QPoint rectCenter		= rect.center();
        
    QPoint textPos;
    textPos.setY( rectCenter.y() - halfTextHeight + 1 );
        
    switch (textAlignment)
    {
        case Qt::AlignLeft:
        {
            textPos.setX( rect.left() - 1 );
            break;
        }

        case Qt::AlignRight:
        {
            textPos.setX( rect.right() - textWidth + 2 );
            break;
        }
                
        default:
        {
            textPos.setX( rectCenter.x() - halfTextWidth + 2 );
        }
    }

	QStaticText staticText( text.AsChar() );
	painter.drawStaticText( textPos, staticText );

	//const float time = timing.GetTime();
	//LogInfo("%s - %.2f ms", text.AsChar(), time*1000);

	// this is way slower !!!

/*	Timer timing;

    // static text drawing
    const float textWidth		= fontMetrics.width( text.AsChar() );
    const float halfTextWidth	= textWidth * 0.5 + 0.5;
    const float halfTextHeight	= fontMetrics.height() * 0.5 + 0.5;
    const QPoint rectCenter		= rect.center();
        
    QPoint textPos;
    textPos.setY( rectCenter.y() - halfTextHeight + 1 );
        
    switch (textAlignment)
    {
        case Qt::AlignLeft:
        {
            textPos.setX( rect.left() - 1 );
            break;
        }
                
        case Qt::AlignRight:
        {
            textPos.setX( rect.right() - textWidth + 2 );
            break;
        }
                
        default:
        {
            textPos.setX( rectCenter.x() - halfTextWidth + 2 );
        }
    }

	//QStaticText staticText( text.AsChar() );
	//painter.drawStaticText( textPos, staticText );

	QRect charRect;
	const uint32 numChars = text.Size();
	for (uint32 i=0; i<numChars; ++i)
	{
		mTempString.Format("%c", text[i]);
		 
		RenderText( painter, mTempString, textColor, textPos, font, fontMetrics );

		textPos.setX( textPos.x() + fontMetrics.width(mTempString.AsChar()) );
	}

	const float time = timing.GetTime();
	LogInfo("%s - %.2f ms", text.AsChar(), time*1000);*/
}


void PainterStaticTextCache::RenderTextUncached(QPainter& painter, const Core::String& text, const QColor& textColor, const QPoint& textPos, const QFont& font, const QFontMetrics& fontMetrics)
{
   painter.setFont( font );
    painter.setPen( textColor );
    painter.setBrush( Qt::NoBrush );

#ifdef ENABLE_TEXT_DEBUGMODE
	painter.setPen( Qt::black );
	painter.drawRect( rect );
#endif
        
	QStaticText staticText( text.AsChar() );
	painter.drawStaticText( textPos, staticText );
}


// constructor
PainterStaticTextCache::CacheElement::CacheElement()
{
}


// destructor
PainterStaticTextCache::CacheElement::~CacheElement()
{
	// get the number of static texts and iterate through them
	const uint32 numStaticTexts = mStaticTexts.Size();
	for (uint32 i=0; i<numStaticTexts; ++i)
		delete mStaticTexts[i];

	mStaticTexts.Clear();
}


PainterStaticTextCache::StaticText* PainterStaticTextCache::CacheElement::Find(const String& text, const QFont& font, const QFontMetrics& fontMetrics)
{
	// get the number of static texts and iterate through them
	const uint32 numStaticTexts = mStaticTexts.Size();
	for (uint32 i=0; i<numStaticTexts; ++i)
	{
		StaticText* staticText = mStaticTexts[i];

		if (staticText->mFont.bold() == font.bold() &&
			staticText->mFont.weight() == font.weight() &&
			staticText->mFont.pointSizeF() == font.pointSizeF() &&
			staticText->mFont.pixelSize() == font.pixelSize())
			return staticText;
	}

	// no static text found, create and return a new one
	StaticText* newStaticText = new StaticText(text, font, fontMetrics);
	mStaticTexts.Add( newStaticText );
	return newStaticText;
}


// update the transparent pixmap that contains all text for the node
PainterStaticTextCache::StaticText::StaticText(const String& text, const QFont& font, const QFontMetrics& fontMetrics) : mFontMetrics(fontMetrics)
{
    mFont = font;
    
	mStaticText.setTextFormat( Qt::PlainText );
	//mStaticText.setTextOption( textOption ); // TODO: NOTE: Not used yet
	mStaticText.setPerformanceHint( QStaticText::AggressiveCaching );
	mStaticText.setText( text.AsChar() );

	mText = text;

	//LogDebug( "Created static text cache for '%s'.", text.AsChar() );
}


void PainterStaticTextCache::StaticText::Render(QPainter& painter, const QColor& textColor, const QPoint& pos)
{
 /*   painter.setFont( mFont );
    painter.setPen( textColor );
    painter.setBrush( Qt::NoBrush );

	painter.drawStaticText( pos, mStaticText );*/
}


void PainterStaticTextCache::StaticText::Render(QPainter& painter, const QColor& textColor, const QRect& rect, Qt::Alignment textAlignment)
{
	//Timer timing;
/*
#ifdef ENABLE_TEXT_DEBUGMODE
	painter.setPen( Qt::black );
	painter.drawRect( rect );
#endif

    // static text drawing
    const float textWidth		= mFontMetrics.width( mText.AsChar() );
    const float halfTextWidth	= textWidth * 0.5 + 0.5;
    const float halfTextHeight	= mFontMetrics.height() * 0.5 + 0.5;
    const QPoint rectCenter		= rect.center();
        
    QPoint textPos;
    textPos.setY( rectCenter.y() - halfTextHeight + 1 );
        
    switch (textAlignment)
    {
        case Qt::AlignLeft:
        {
            textPos.setX( rect.left() - 1 );
            break;
        }
                
        case Qt::AlignRight:
        {
            textPos.setX( rect.right() - textWidth + 2 );
            break;
        }
                
        default:
        {
            textPos.setX( rectCenter.x() - halfTextWidth + 2 );
        }
    }

    Render( painter, textColor, textPos );

	//const float time = timing.GetTime();
	//LogInfo("%s - %.2f ms", text.AsChar(), time*1000);*/
}
