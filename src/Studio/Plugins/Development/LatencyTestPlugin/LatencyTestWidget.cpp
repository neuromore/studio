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
#include "LatencyTestWidget.h"
#include "LatencyTestPlugin.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include "../../../Rendering/OpenGLWidget2DHelpers.h"
#include <QPainter>


using namespace Core;

// constructor
LatencyTestWidget::LatencyTestWidget(LatencyTestPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	// create the render callback
	mRenderCallback = new RenderCallback(this);
	SetCallback( mRenderCallback );

	mPlugin = plugin;
}


// destructor
LatencyTestWidget::~LatencyTestWidget()
{
	// destroy the render callback
	delete mRenderCallback;
}


// render frame
void LatencyTestWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	Classifier* classifier = GetEngine()->GetActiveClassifier();

	// multi view rendering
	if (classifier != NULL)
	{
		const double numFeedbackNodes = classifier->GetNumCustomFeedbackNodes();
		RenderSplitViews( numFeedbackNodes, Math::Sqrt(numFeedbackNodes) );
	}

	// post rendering
	PostRendering();
}


// render callback
void LatencyTestWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// get the time delta since the last render call
	//const double timeDelta = mParent->GetTimeDelta();

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	CustomFeedbackNode* feedbackNode = classifier->GetCustomFeedbackNode(index);
	const double feedbackValue = feedbackNode->GetCurrentValue();

	// draw background rect
	if (feedbackValue > mParent->mPlugin->GetThreshold())
		AddRect( 0, 0, width, height, Color(1.0,1.0,1.0) );
	else
		AddRect( 0, 0, width, height, Color(.0,0.0,0.0) );
	RenderRects();

	// render text
	mTempString.Format("%s", feedbackNode->GetName());
	RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), feedbackNode->GetColor(), 10, 10, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_RIGHT );
}
