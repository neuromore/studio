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

// include required headers
#include "ExampleOpenGLWidget.h"
#include "ExampleOpenGLPlugin.h"
#include <Core/LogManager.h>
#include "../../../Rendering/OpenGLWidget2DHelpers.h"
#include <QPainter>


using namespace Core;

// constructor
ExampleOpenGLWidget::ExampleOpenGLWidget(ExampleOpenGLPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	// create the render callback
	mRenderCallback = new RenderCallback(this);
	SetCallback( mRenderCallback );

	mPlugin		= plugin;
	mEmptyText	= "This is the text shown in case no split view is rendered (e.g. in case no classifier or device is active)";
}


// destructor
ExampleOpenGLWidget::~ExampleOpenGLWidget()
{
	// destroy the render callback
	delete mRenderCallback;
}


// render frame
void ExampleOpenGLWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	// multi view rendering
	const bool multiViewEnabled = mPlugin->GetMultiView();
	const uint32 numViews = 9;
	const uint32 numViewsPerColumn = 3;

	// render single or multi view
	if (multiViewEnabled == false)
		Render();
	else
		RenderSplitViews( numViews, numViewsPerColumn );

	// post rendering
	PostRendering();
}


// render callback
void ExampleOpenGLWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// get the time delta since the last render call
	const double timeDelta = mParent->GetTimeDelta();

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// draw background rect
	//const double greyValue = 0.3 + index * 0.1;
	//AddRect( 0, 0, width, height, Color(greyValue,greyValue,greyValue) );
	//RenderRects();

		// set the viewport to the multi view area
		// NOTE: Retina Support: glViewport expects device pixels, but the various geometry accessors returns values in device-independent pixels
		const double devicePixelRatio = mParent->devicePixelRatio();
		mParent->glViewport( mOffsetX * devicePixelRatio, mOffsetY * devicePixelRatio, width * devicePixelRatio, height * devicePixelRatio );

		// enable depth testing
		mParent->glEnable( GL_DEPTH_TEST );

		// set the projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// TODO replace with QMatrix4x4
		/*Matrix projMat;
		const float aspect = height/width;
		projMat.Projection( 45.0f, aspect, 0.01f, 100.0f );
		glLoadMatrixf( projMat.m16 );*/

		// set the camera matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// cube rotation animation
		const double rotSpeed = 35.0;
		if (index == 0)
			mCubeRotation += timeDelta * rotSpeed;

		///////////////////////////////////////////////
		// code copy & pasted from NeHe Tutorials
		glLoadIdentity();									// Reset The Current Modelview Matrix
		glTranslatef(4.0f,0.0f,7.0f);						// Move Right 1.5 Units And Into The Screen 7.0
		glRotatef(mCubeRotation,1.0f,1.0f,0.0f);			// Rotate The Quad On The X axis ( NEW )
		glBegin(GL_QUADS);									// Draw A Quad
			glColor3f(0.0f,1.0f,0.0f);						// Set The Color To Green
			glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Top)
			glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Top)
			glVertex3f(-1.0f, 1.0f, 1.0f);					// Bottom Left Of The Quad (Top)
			glVertex3f( 1.0f, 1.0f, 1.0f);					// Bottom Right Of The Quad (Top)
			glColor3f(1.0f,0.5f,0.0f);						// Set The Color To Orange
			glVertex3f( 1.0f,-1.0f, 1.0f);					// Top Right Of The Quad (Bottom)
			glVertex3f(-1.0f,-1.0f, 1.0f);					// Top Left Of The Quad (Bottom)
			glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Bottom)
			glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Bottom)
			glColor3f(1.0f,0.0f,0.0f);						// Set The Color To Red
			glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Front)
			glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Front)
			glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Front)
			glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Front)
			glColor3f(1.0f,1.0f,0.0f);						// Set The Color To Yellow
			glVertex3f( 1.0f,-1.0f,-1.0f);					// Top Right Of The Quad (Back)
			glVertex3f(-1.0f,-1.0f,-1.0f);					// Top Left Of The Quad (Back)
			glVertex3f(-1.0f, 1.0f,-1.0f);					// Bottom Left Of The Quad (Back)
			glVertex3f( 1.0f, 1.0f,-1.0f);					// Bottom Right Of The Quad (Back)
			glColor3f(0.0f,0.0f,1.0f);						// Set The Color To Blue
			glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Left)
			glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Left)
			glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Left)
			glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Left)
			glColor3f(1.0f,0.0f,1.0f);						// Set The Color To Violet
			glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Right)
			glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Right)
			glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Right)
			glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Right)
		glEnd();											// Done Drawing The Quad
		// end copy paste
		///////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////////////////////////

	// render some test lines
	AddLine( 90, 60, Color(1,0,0), 90, 130, Color(0,1,0) );
	AddLine( 60, 90, Color(0,0,1), 130, 90, Color(0,1,1) );
	RenderLines();

	// render some test rects
	AddRect( 55, 55, 30, 30, Color(0.5,0.5,0.5) );
	AddRect( 95, 95, 30, 30, Color(0.5,0.5,0.5) );
	RenderRects();

	// render text
	// NOTE: render text at the very end
	mTempString.Format("View %i", index);
	RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 0.0f, 0.0f), 50, 50, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_CENTER );

	mTempString.Format("View %i", index);
	RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 0.0f), 5, 5, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
}
