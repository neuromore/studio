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

// include the required headers
#include "OpenGLWidget.h"
#include <MainWindowBase.h>
#include <ColorPalette.h>

// font stash
// the implementation defines enable function declarations inside the header files
// do this only inside one .cpp file else you will get "already defined in" link errors
#define FONTSTASH_IMPLEMENTATION
#include <FontStash/fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <FontStash/glfontstash.h>

using namespace Core;

// constructor
OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	mAsyncNeedsReInit	= false;
	mTextureManager		= NULL;

	mWidth				= 0;
	mHeight				= 0;
	mIsViewSplitted		= true;

	// input helpers
	mPrevMouseX			= 0;
	mPrevMouseY			= 0;
	mAltPressed			= false;

	// performance statistics
	ResetPerformanceStatsPos();

	mRenderTimeDelta	= 0.0f;
	mCallback			= NULL;

	// colors
	mBackgroundColor	= ColorPalette::Shared::GetDarkBackgroundQColor();

	// timeline
	mHasTimeline		= false;
	mTimelineHeight		= 0.0;
    
    mContext            = context();

	// font stash
	mFontStashContext	= NULL;
	mFontNormal			= FONS_INVALID;
	mFontItalic			= FONS_INVALID;
	mFontBold			= FONS_INVALID;

	setAutoFillBackground(false);
	setMouseTracking(true);

	// load the true type font from Qt resource
	String fontFilename = ":/Assets/Fonts/Arial.ttf";
	LoadFontDataFromQtResource( fontFilename.AsChar() );

	// set minimum render widget dimensions
	setMinimumHeight( 50 );
	setMinimumWidth( 50 );
}


// destructor
OpenGLWidget::~OpenGLWidget()
{
	// delete font stash context
	DestructFont();

	// get rid of the texture manager
	delete mTextureManager;

	// unregister OpenGL widget
	GetOpenGLManager()->UnregisterOpenGLWidget(this);
}


// reset performance statistics position
void OpenGLWidget::ResetPerformanceStatsPos()
{
	mPerfStatsX = 3.0;
	mPerfStatsY = 0.0;
}


// load true type font file from Qt resource and add it to font stash
bool OpenGLWidget::LoadFontDataFromQtResource(const char* path)
{
	// load the file
	QFile ttfFile(path);
	if (ttfFile.open(QIODevice::ReadOnly) == false)
	{
		LogError( "OpenGLWidget::LoadFontDataFromQtResource(): Cannot read .TTF file from '%s'.", path );
		return false;
	}

	// read the file size
	int dataSize = ttfFile.size();

	// allocate memory
	mFontData.Resize( dataSize );

	// copy over data
	Core::MemCopy( mFontData.GetPtr(), ttfFile.readAll().data(), dataSize );

	return true;
}


// initialize font stash
void OpenGLWidget::InitFont()
{
	// reinitialize font stash
	mFontStashContext = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (mFontStashContext == NULL)
	{
		LogError("Could not create font stash.");
	}

	// add true type font to font stash
	mFontNormal = fonsAddFontMem( mFontStashContext, "Arial", (unsigned char*)mFontData.GetPtr(), mFontData.Size(), 0 );
	if (mFontNormal == FONS_INVALID)
	{
		LogError("Could not add font normal.");
	}
}


// delete font stash context
void OpenGLWidget::DestructFont()
{
	if (mFontStashContext != NULL)
		glfonsDelete( mFontStashContext );
	mFontStashContext = NULL;
}


// initialize the Qt OpenGL widget (overloaded from the widget base class)
void OpenGLWidget::initializeGL()
{
	// base class initialize
	QOpenGLWidget::initializeGL();

	initializeOpenGLFunctions();

	// reinitialize texture manager
	delete mTextureManager;
	mTextureManager = new TextureManager();

	// re-register OpenGL widget
	GetOpenGLManager()->UnregisterOpenGLWidget(this);
	GetOpenGLManager()->RegisterOpenGLWidget(this, this);

	mAsyncNeedsReInit = true;

	// schedule async reinitialization for all OpenGL widgets
	GetOpenGLManager()->OnScheduleAsyncReInit( this );
}


// resize the Qt OpenGL widget (overloaded from the widget base class)
void OpenGLWidget::resizeGL(int width, int height)
{
	// don't resize in case the render widget is hidden
	if (isHidden() == true)
		return;

	mHeight	= height * devicePixelRatio();
	mWidth	= width * devicePixelRatio();

	mRect = QRect( 0, 0, mWidth, height );

	// base class resize
	QOpenGLWidget::resizeGL(width, height);

	// schedule async reinitialization for all OpenGL widgets
	//GetOpenGLManager()->OnScheduleAsyncReInit( this );
}


// called when the mouse moved
void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	// calculate the delta mouse movement
	const int32 deltaX = event->globalX() - mPrevMouseX;
	const int32 deltaY = event->globalY() - mPrevMouseY;

	// get the button states
	const bool leftButtonPressed		= event->buttons() & Qt::LeftButton;
	const bool middleButtonPressed		= event->buttons() & Qt::MidButton;
	const bool rightButtonPressed		= event->buttons() & Qt::RightButton;
	//const bool shiftPressed			= event->modifiers() & Qt::ShiftModifier;
	mAltPressed							= event->modifiers() & Qt::AltModifier;

	// store the current value as previous value
	mPrevMouseX = QCursor::pos().x();//event->globalX();
	mPrevMouseY = QCursor::pos().y();//event->globalY();

	QOpenGLWidget::mouseMoveEvent(event);
}


// called when a mouse button got pressed
void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	// calculate the delta mouse movement and set old mouse position
	const int32 deltaX = event->globalX() - mPrevMouseX;
	const int32 deltaY = event->globalY() - mPrevMouseY;
	mPrevMouseX = event->globalX();
	mPrevMouseY = event->globalY();

	// get the button states
	const bool leftButtonPressed		= event->buttons() & Qt::LeftButton;
	const bool middleButtonPressed		= event->buttons() & Qt::MidButton;
	const bool rightButtonPressed		= event->buttons() & Qt::RightButton;
	//const bool shiftPressed			= event->modifiers() & Qt::ShiftModifier;
	mAltPressed							= event->modifiers() & Qt::AltModifier;

	QOpenGLWidget::mousePressEvent(event);
}


// called when a mouse button got pressed
void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// calculate the delta mouse movement and set old mouse position
	const int32 deltaX = event->globalX() - mPrevMouseX;
	const int32 deltaY = event->globalY() - mPrevMouseY;
	mPrevMouseX = event->globalX();
	mPrevMouseY = event->globalY();

	// get the button states
	const bool leftButtonPressed		= event->buttons() & Qt::LeftButton;
	const bool middleButtonPressed		= event->buttons() & Qt::MidButton;
	const bool rightButtonPressed		= event->buttons() & Qt::RightButton;
	//const bool shiftPressed			= event->modifiers() & Qt::ShiftModifier;
	mAltPressed							= event->modifiers() & Qt::AltModifier;

	QOpenGLWidget::mouseReleaseEvent(event);
}


bool OpenGLWidget::PreRendering()
{
	// don't render in case the render widget is hidden
	if (isHidden() == true)
		return false;

	// make sure the window dimensions are set correctly
	if (mWidth < 0 || mHeight < 0)
		return false;

	// async reinitialization
	if (mAsyncNeedsReInit == true)
	{
		//Timer fontReinitTimer;

		// reinitialize font
		DestructFont();
		InitFont();

		//const float fontReinitTiming = fontReinitTimer.GetTime().InMilliseconds();
		//LogInfo( "Reinitializing font took %.3f ms", fontReinitTiming );

		// DO NOT REMOVE COMMENT!!!
		// TODO: HACKFIX: Find nice fix for this later on when there is time, this is still way faster than Qt text rendering
		// and even works on all graphics cards
		// this is a hacky thing to do as we reinit the font each frame now
		// reiniting only does generate a new OpenGL texture though which isn't too bad
		// timings in debug mode on mobile machine turned out that it only needs around 0.05 to 0.15 ms
		// when doing this we work on all graphics cards which is most important atm
		//mAsyncNeedsReInit = false;
	}
    
    //if (mContext != context())
    //{
    //    mContext = context();
    //    initializeGL();
    //}

	GetQtBaseManager()->GetMainWindow()->GetOpenGLFpsCounter().UnPause();
	mFpsCounter.BeginTiming();

	// reset performance statistics
	if (mCallback != NULL)
		mCallback->ResetPerformanceStatistics();

	// this is already done by the QOpenGLWidget
	// results in OpenGL error: "Redundant state change in glBindFramebuffer API call, FBO 1 '', already bound."
	//makeCurrent();

	// reset the render timer
	mRenderTimeDelta = mRenderTimer.GetTimeDelta().InSeconds();

	// begin native painting
	if (mCallback != NULL)
		mCallback->GetPainter()->beginNativePainting();

	// set the viewport
	// NOTE: Retina Support: glViewport expects device pixels, but the various geometry accessors returns values in device-independent pixels.
	glViewport( 0, 0, mWidth, mHeight);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, mWidth, 0.0, mHeight, 0.0, 10.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	// disable the depth buffer and clear the screen and the depth buffer
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glClearColor( mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF(), mBackgroundColor.alphaF() );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}



void dash(float dx, float dy)
{
	glBegin(GL_LINES);
	glColor4ub(0,0,0,128);
	glVertex2f(dx-5,dy);
	glVertex2f(dx-10,dy);
	glEnd();
}

void line(float sx, float sy, float ex, float ey)
{
	glBegin(GL_LINES);
	glColor4ub(0,0,0,128);
	glVertex2f(sx,sy);
	glVertex2f(ex,ey);
	glEnd();
}


void OpenGLWidget::RenderFontStashExample()
{
	float sx, sy, dx, dy, lh = 0;

	glViewport(0, 0, mWidth, mHeight);
	glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,mWidth, mHeight,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glColor4ub(255,255,255,255);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	unsigned int white,black,brown,blue;
	white = glfonsRGBA(255,255,255,255);
	brown = glfonsRGBA(192,128,0,128);
	blue = glfonsRGBA(0,192,255,255);
	black = glfonsRGBA(0,0,0,255);

	sx = 50; sy = 50;
		
	dx = sx; dy = sy;

	dash(dx,dy);

	fonsClearState(mFontStashContext);

	fonsSetSize(mFontStashContext, 124.0f);
	fonsSetFont(mFontStashContext, mFontNormal);
	fonsVertMetrics(mFontStashContext, NULL, NULL, &lh);
	dx = sx;
	dy += lh;
	dash(dx,dy);
		
	fonsSetSize(mFontStashContext, 124.0f);
	fonsSetFont(mFontStashContext, mFontNormal);
	fonsSetColor(mFontStashContext, white);
	dx = fonsDrawText(mFontStashContext, dx,dy,"The quick ",NULL);

	fonsSetSize(mFontStashContext, 48.0f);
	fonsSetFont(mFontStashContext, mFontItalic);
	fonsSetColor(mFontStashContext, brown);
	dx = fonsDrawText(mFontStashContext, dx,dy,"brown ",NULL);

	fonsSetSize(mFontStashContext, 24.0f);
	fonsSetFont(mFontStashContext, mFontNormal);
	fonsSetColor(mFontStashContext, white);
	dx = fonsDrawText(mFontStashContext, dx,dy,"fox ",NULL);

	fonsVertMetrics(mFontStashContext, NULL, NULL, &lh);
	dx = sx;
	dy += lh*1.2f;
	dash(dx,dy);
	fonsSetFont(mFontStashContext, mFontItalic);
	dx = fonsDrawText(mFontStashContext, dx,dy,"jumps over ",NULL);
	fonsSetFont(mFontStashContext, mFontBold);
	dx = fonsDrawText(mFontStashContext, dx,dy,"the lazy ",NULL);
	fonsSetFont(mFontStashContext, mFontNormal);
	dx = fonsDrawText(mFontStashContext, dx,dy,"dog.",NULL);

	dx = sx;
	dy += lh*1.2f;
	dash(dx,dy);
	fonsSetSize(mFontStashContext, 12.0f);
	fonsSetFont(mFontStashContext, mFontNormal);
	fonsSetColor(mFontStashContext, blue);
	fonsDrawText(mFontStashContext, dx,dy,"Now is the time for all good men to come to the aid of the party.",NULL);

	fonsVertMetrics(mFontStashContext, NULL,NULL,&lh);
	dx = sx;
	dy += lh*1.2f*2;
	dash(dx,dy);
	fonsSetSize(mFontStashContext, 18.0f);
	fonsSetFont(mFontStashContext, mFontItalic);
	fonsSetColor(mFontStashContext, white);
	fonsDrawText(mFontStashContext, dx,dy,"asdsadsadsad",NULL);

	// Font alignment
	fonsSetSize(mFontStashContext, 18.0f);
	fonsSetFont(mFontStashContext, mFontNormal);
	fonsSetColor(mFontStashContext, white);

	dx = 50; dy = 350;
	line(dx-10,dy,dx+250,dy);
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
	dx = fonsDrawText(mFontStashContext, dx,dy,"Top",NULL);
	dx += 10;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_MIDDLE);
	dx = fonsDrawText(mFontStashContext, dx,dy,"Middle",NULL);
	dx += 10;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
	dx = fonsDrawText(mFontStashContext, dx,dy,"Baseline",NULL);
	dx += 10;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_BOTTOM);
	fonsDrawText(mFontStashContext, dx,dy,"Bottom",NULL);

	dx = 150; dy = 400;
	line(dx,dy-30,dx,dy+80.0f);
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);
	fonsDrawText(mFontStashContext, dx,dy,"Left",NULL);
	dy += 30;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_CENTER | FONS_ALIGN_BASELINE);
	fonsDrawText(mFontStashContext, dx,dy,"Center",NULL);
	dy += 30;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_RIGHT | FONS_ALIGN_BASELINE);
	fonsDrawText(mFontStashContext, dx,dy,"Right",NULL);

	// Blur
	dx = 500; dy = 350;
	fonsSetAlign(mFontStashContext, FONS_ALIGN_LEFT | FONS_ALIGN_BASELINE);

	fonsSetSize(mFontStashContext, 60.0f);
	fonsSetFont(mFontStashContext, mFontItalic);
	fonsSetColor(mFontStashContext, white);
	fonsSetSpacing(mFontStashContext, 5.0f);
	fonsSetBlur(mFontStashContext, 10.0f);
	fonsDrawText(mFontStashContext, dx,dy,"Blurry...",NULL);

	dy += 50.0f;

	fonsSetSize(mFontStashContext, 18.0f);
	fonsSetFont(mFontStashContext, mFontBold);
	fonsSetColor(mFontStashContext, black);
	fonsSetSpacing(mFontStashContext, 0.0f);
	fonsSetBlur(mFontStashContext, 3.0f);
	fonsDrawText(mFontStashContext, dx,dy+2,"DROP THAT SHADOW",NULL);

	fonsSetColor(mFontStashContext, white);
	fonsSetBlur(mFontStashContext, 0);
	fonsDrawText(mFontStashContext, dx,dy,"DROP THAT SHADOW",NULL);

	//if (debug)
		fonsDrawDebug(mFontStashContext, 800.0, 50.0);

	glEnable(GL_DEPTH_TEST);
}


void OpenGLWidget::RenderText(const char* text, float fontSize, const Color& color, const QRect& rect, Qt::Alignment textAlignment)
{
	// get the text height
	//float textHeight;
	//fonsVertMetrics(mFontStashContext, NULL, NULL, &textHeight);

    QPoint textPos;
    
	int align=0;
    switch (textAlignment)
    {
        case Qt::AlignLeft:
        {
			align = ALIGN_LEFT | ALIGN_MIDDLE;
			textPos = QPoint( rect.left(), rect.center().y() );
            break;
        }

        case Qt::AlignRight:
        {
			align = ALIGN_RIGHT | ALIGN_MIDDLE;
			textPos = QPoint( rect.right(), rect.center().y() );
            break;
        }
                
        default:
        {
            align = ALIGN_CENTER | ALIGN_MIDDLE;
			textPos = rect.center();
        }
    }

	RenderText( text, fontSize, color, textPos.x(), textPos.y() );
}


float OpenGLWidget::GetDefaultFontSize() const
{
	return GetQtBaseManager()->GetMainWindow()->GetDefaultFontSize() + 2;
}


// render text using font stash
void OpenGLWidget::RenderText(const char* text, float fontSize, const Color& color, float posX, float posY, int alignment)
{
	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0f, mWidth, mHeight, 0.0f, -1.0f, 1.0f );

	// set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// remember OpenGL render states
	bool texturing		= glIsEnabled(GL_TEXTURE_2D);
	bool lighting		= glIsEnabled(GL_LIGHTING);
	bool depthTesting	= glIsEnabled(GL_DEPTH_TEST);
	bool blending		= glIsEnabled(GL_BLEND);
	bool culling		= glIsEnabled(GL_CULL_FACE);

	// prepare OpenGL render states
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_CULL_FACE );


	// reset the font state (color, font, size, spacing, ...)
	fonsClearState(mFontStashContext);

	// alignment
	fonsSetAlign( mFontStashContext, alignment );

	// font size
	fonsSetSize( mFontStashContext, fontSize );

	// TrueType font
	fonsSetFont( mFontStashContext, mFontNormal );

	// font spacing
	//fonsSetSpacing(mFontStashContext, 5.0f);
	
	// blur
	//fonsSetBlur(mFontStashContext, 10.0f);

	// font color
	unsigned int fontStashColor = glfonsRGBA( color.r*255.0f, color.g*255.0f, color.b*255.0f, color.a*255.0f );
	fonsSetColor( mFontStashContext, fontStashColor );

	// render text
	/*const float dx = */fonsDrawText( mFontStashContext, posX, posY, text, NULL );


	// set OpenGL render states back to their old values
	if (texturing == true)		glEnable(GL_TEXTURE_2D);	else glDisable(GL_TEXTURE_2D);
	if (lighting == true)		glEnable(GL_LIGHTING);		else glDisable(GL_LIGHTING);
	if (depthTesting == true)	glEnable(GL_DEPTH_TEST);	else glDisable(GL_DEPTH_TEST);
	if (blending == true)		glEnable(GL_BLEND);			else glDisable(GL_BLEND);
	if (culling == true)		glEnable(GL_CULL_FACE);		else glDisable(GL_CULL_FACE);
}


void OpenGLWidget::PostRendering()
{
	// make sure all lines and rects are drawn
	mCallback->RenderRects();

	// render font stash example
	//RenderFontStashExample();

	mFpsCounter.StopTiming();

	// render FPS counter
	if (GetQtBaseManager()->GetMainWindow()->GetShowFPS() == true || GetQtBaseManager()->GetMainWindow()->GetShowPerformanceInfo() == true)
	{
		QPainter* painter = mCallback->GetPainter();
		if (painter != NULL)
		{
			//const QFont& font = mCallback->GetFont();
			//const QFontMetrics& fontMetrics = mCallback->GetFontMetrics();
			const Color textColor(1.0f, 1.0f, 1.0f, 1.0f);
			const double textHeight = mCallback->GetTextHeight();
			const double textDeltaY = textHeight * 1.1;

			const double fps = mFpsCounter.GetFps();

			// NOTE: use the version from the widget and not the callback as we don't want the offset to be in!
			RenderText( mFpsCounter.GetTextString().AsChar(), GetDefaultFontSize(), textColor, mPerfStatsX, mPerfStatsY, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );

			if (GetQtBaseManager()->GetMainWindow()->GetShowPerformanceInfo() == true)
			{
				// draw calls
				const double numDrawCalls =  mCallback->GetNumDrawCalls();
				mTempString.Format( "%.0f drawcalls/frame - %.0f drawcalls/sec", numDrawCalls, fps * numDrawCalls );
				RenderText(mTempString.AsChar(), GetDefaultFontSize(), textColor, mPerfStatsX, mPerfStatsY + textDeltaY * 1, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );

				// line rendering stats
				const double numDrawnLines = mCallback->GetNumDrawn2DLines();
				mTempString.Format( "%.0f lines/frame - %.0f lines/sec", numDrawnLines, fps * numDrawnLines );
				RenderText(mTempString.AsChar(), GetDefaultFontSize(), textColor, mPerfStatsX, mPerfStatsY + textDeltaY * 2, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );

				// rect rendering stats
				const double numDrawnRects = mCallback->GetNumDrawn2DRects();
				mTempString.Format( "%.0f rects/frame - %.0f rects/sec", numDrawnRects, fps * numDrawnRects );
				RenderText(mTempString.AsChar(), GetDefaultFontSize(), textColor, mPerfStatsX, mPerfStatsY + textDeltaY * 3, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );

				// dynamic text rendering stats
				const double numDrawnTexts = mCallback->GetNumDrawnTexts();
				mTempString.Format( "%.0f texts/frame - %.0f texts/sec", numDrawnTexts, fps * numDrawnTexts );
				RenderText(mTempString.AsChar(), GetDefaultFontSize(), textColor, mPerfStatsX, mPerfStatsY + textDeltaY * 4, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
			}
		}
	}

	// end native painting
	if (mCallback != NULL)
		mCallback->GetPainter()->endNativePainting();

	GetQtBaseManager()->GetMainWindow()->GetOpenGLFpsCounter().Pause();
}


// render each sensor in an individual view
void OpenGLWidget::RenderSplitViews(uint32 numAreas, uint32 numAreasInColumn, double border)
{
	// get the current local cursor pos
	QPoint localCursorPos = mapFromGlobal( QCursor::pos() );

	double viewportHeight = mHeight;
	if (mHasTimeline == true)
		viewportHeight -= mTimelineHeight;

	// iterate through the areas
	uint32 numActuallyRendered = 0;
	for (uint32 i=0; i<numAreas; ++i)
	{
		// check if we need to render the signal for the given sensor
		//if (mCheckboxWidget->IsChannelSelected(sensor->GetChannel()) == false)
		//	continue;

		// the number of sensors to be shown within one column (vertically) before we start a new column and split the screen to fit more channels
		bool useSeveralColumns = true;
		if (numAreasInColumn == CORE_INVALIDINDEX32)
			useSeveralColumns = false;

		double xStart, yStart, xEnd, yEnd, cellWidth, cellHeight;
		if (useSeveralColumns == true && numAreas > numAreasInColumn)
		{
			// split the viewport vertically and if there are too many items, also split up into columns
			const uint32 numColumns	= ceil( (double)numAreas / (double)numAreasInColumn );
			const double columnWidth= (double)mWidth / (double)numColumns;
			const uint32 columnNr	= numActuallyRendered / numAreasInColumn;

			xStart		= columnNr * columnWidth;
			xEnd		= xStart + columnWidth;
			cellHeight	= (double)viewportHeight / (double)numAreasInColumn;
			cellWidth	= columnWidth;
			yStart		= (numActuallyRendered % numAreasInColumn) * cellHeight;
			yEnd		= yStart + cellHeight;
		}
		else
		{
			// split the viewport vertically (only!) and resize the the items based on the number of sensors to be rendered
			xStart		= 0.0;
			xEnd		= mWidth;
			cellHeight	= (double)viewportHeight / (double)numAreas;
			cellWidth	= mWidth;
			yStart		= numActuallyRendered * cellHeight;
			yEnd		= yStart + cellHeight;
		}

		xStart		= (int32)xStart + 0.5;
		xEnd		= (int32)xEnd + 0.5;
		cellHeight	= (int32)cellHeight + 0.5;
		cellWidth	= (int32)cellWidth + 0.5;
		yStart		= (int32)yStart + 0.5;
		yEnd		= (int32)yEnd + 0.5;

		// highlight it in case:
		// 1. the checkbox is hovered
		// 2. the mouse is inside the corresponding cell
		bool isHighlighted = false;
		if (//highlightedIndex == i || 
			(localCursorPos.x() >= xStart && localCursorPos.x() <= xEnd && localCursorPos.y() <= yEnd && localCursorPos.y() >= yStart))
			isHighlighted = true;



		// render callback per sensor
		Render(i, isHighlighted, xStart+border, yStart+border, cellWidth-2.0*border, cellHeight-2.0*border);

		// increase the number of actually rendered sensors
		numActuallyRendered++;
	}

	if (numActuallyRendered == 0)
	{
		RenderEmpty();
		return;
	}

	if (mCallback != NULL && mHasTimeline == true)
	{
		// fill background
		//RenderBackground( x, y, width, height );
		mCallback->RenderTimeline(border, viewportHeight+border, mWidth-(2.0*border), mHeight-(viewportHeight+2.0*border));
	}
}


void OpenGLWidget::RenderEmpty()
{
	if (mCallback == NULL)
		return;

	mCallback->SetOffset( 0.0, 0.0 );

	// draw background rect
	mCallback->AddRect( 0, 0, mWidth, mHeight, ColorPalette::Shared::GetDarkBackgroundColor() );
	mCallback->RenderRects();

	mCallback->RenderText( mEmptyText.AsChar(), GetDefaultFontSize(), ColorPalette::Shared::GetTextQColor(), (float)mWidth*0.5f, (float)mHeight*0.5f, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_CENTER );
}


void OpenGLWidget::RenderBackground(double x, double y, double width, double height)
{
	if (mCallback != NULL)
	{
		mCallback->AddRect( x, y, width, height, FromQtColor(mBackgroundColor) );
		mCallback->RenderRects();
	}
}


void OpenGLWidget::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// fill background
	//RenderBackground( x, y, width, height );

	// render background
	/*if (isHighlighted == true)
	{
		// get the sensor color and use it as background color
		Color backgroundColor = Color(1,0,0);//sensor->GetChannel()->GetColor();
		backgroundColor.Exposure(0.2f);
		//glColor3f( backgroundColor.r, backgroundColor.g, backgroundColor.b );

		//glBegin(GL_QUADS);
		//	glVertex2d(0.0,			0.0);
		//	glVertex2d(cellWidth,	0.0);
		//	glVertex2d(cellWidth,	cellHeight);
		//	glVertex2d(0.0,			cellHeight);
		//glEnd();
	}*/

	if (mCallback != NULL)
		mCallback->Render( index, isHighlighted, x, y, width, height );
}
