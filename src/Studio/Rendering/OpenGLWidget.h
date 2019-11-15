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

#ifndef __NEUROMORE_OPENGLWIDGET_H
#define __NEUROMORE_OPENGLWIDGET_H

#include "../Config.h"
#include <Core/FpsCounter.h>
#include <Core/Array.h>
#include <DSP/Channel.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "TextureManager.h"
#include "OpenGLWidgetCallback.h"
#include <QPainter>

// font stash
#include <FontStash/fontstash.h>
#include <FontStash/glfontstash.h>


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
	public:
		// constructor and destructor
		OpenGLWidget(QWidget* parent);
		virtual ~OpenGLWidget();

		void initializeGL() override;
		void resizeGL(int width, int height) override;

		bool PreRendering();
		void PostRendering();
		void Render(double border=2.0)								{ RenderSplitViews(1, CORE_INVALIDINDEX32, border); }
		void RenderSplitViews(uint32 numAreas, uint32 numAreasInColumn=CORE_INVALIDINDEX32, double border=2.0);

		int GetHeight() const										{ return mHeight; }

		void ScheduleAsyncReInit()									{ mAsyncNeedsReInit = true; }

		/////////////////////////////////////////
		// text rendering (using font stash and stb)
		/////////////////////////////////////////

		enum TextAlignment
		{
			// horizontal align
			ALIGN_LEFT 		= 1<<0,
			ALIGN_CENTER 	= 1<<1,
			ALIGN_RIGHT 	= 1<<2,

			// vertical align
			ALIGN_TOP 		= 1<<3,
			ALIGN_MIDDLE	= 1<<4,
			ALIGN_BOTTOM	= 1<<5,
			ALIGN_BASELINE	= 1<<6,
		};

		void RenderText(const char* text, float fontSize, const Core::Color& color, float posX, float posY, int alignment = ALIGN_CENTER|ALIGN_MIDDLE);
		void RenderText(const char* text, float fontSize, const Core::Color& color, const QRect& rect, Qt::Alignment textAlignment);
		float GetDefaultFontSize() const;

		void InitFont();
		void DestructFont();

		// texture manager
		TextureManager* GetTextureManager()							{ return mTextureManager; }
		
		void EnableTimeline(double height)							{ mHasTimeline=true; mTimelineHeight=height; }

		void SetCallback(OpenGLWidgetCallback* callback)			{ mCallback = callback; }

		double GetTimeDelta() const									{ return mRenderTimeDelta; }

		void SetPerformanceStatsPos(double x, double y)				{ mPerfStatsX = x; mPerfStatsY = y; }
		void ResetPerformanceStatsPos();

	protected:
		// input handling
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;

	protected:
		void RenderEmpty();
		void RenderBackground(double x, double y, double width, double height);
		void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height);

		bool LoadFontDataFromQtResource(const char* path);

		void RenderFontStashExample();

		Core::String					mTempString;

		// view settings
		bool							mIsViewSplitted;

		bool							mAsyncNeedsReInit;

		// callback
		OpenGLWidgetCallback*			mCallback;
    
        QOpenGLContext*                 mContext;

		// texture manager
		TextureManager*					mTextureManager;

		// text rendering
		FONScontext*					mFontStashContext;

		int								mFontNormal;
		Core::Array<char>				mFontData;

		int								mFontItalic; // both not used atm
		int								mFontBold;

		// performance statistics
		Core::FpsCounter				mFpsCounter;
		double							mPerfStatsX;
		double							mPerfStatsY;

		Core::String					mEmptyText;

		Core::Timer						mRenderTimer;
		double							mRenderTimeDelta;

		// viewport
		int32							mHeight;
		int32							mWidth;
		QRect							mRect;

		// timeline
		bool							mHasTimeline;
		double							mTimelineHeight;

		// background color
		QColor							mBackgroundColor;

		// mouse and keyboard
		int32							mPrevMouseX;
		int32							mPrevMouseY;
		bool							mAltPressed;
};


#endif
