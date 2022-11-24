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
#include "ViewWidget.h"
#include "ViewPlugin.h"

using namespace Core;

// constructor
ViewWidget::ViewWidget(ViewPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	mRenderCallback = new RenderCallback(this, this);
	SetCallback( mRenderCallback );

	mLeftTextWidth		= 0.0;
	mPlugin				= plugin;
	mEmptyText			= "No signals";
}


// destructor
ViewWidget::~ViewWidget()
{
	delete mRenderCallback;
}


Classifier* ViewWidget::GetClassifier() const
{
	return GetEngine()->GetActiveClassifier();
}


// render frame
void ViewWidget::paintGL()
{
	uint32 numMultiChannels = 0;
	Classifier* classifier = GetClassifier();
	if (classifier != NULL)
	{
		double maxTextWidth = 0.0;

		// get the channels to render
		numMultiChannels = mPlugin->GetNumMultiChannels();
		for (uint32 i = 0; i<numMultiChannels; ++i)
		{
			const MultiChannel& channels = mPlugin->GetMultiChannel(i);

			// calc range min text width
			mTempString.Format( "%.2f", channels.GetMinValue() );
			maxTextWidth = Max<double>( maxTextWidth, mRenderCallback->CalcTextWidth(mTempString.AsChar()) );

			// calc range max text width
			mTempString.Format("%.2f", channels.GetMaxValue() );
			maxTextWidth = Max<double>( maxTextWidth, mRenderCallback->CalcTextWidth(mTempString.AsChar()) );
		}

		mLeftTextWidth = maxTextWidth + 5; // +5 for spacing

		// WHAT is this ??:
		// align the fps stats
		if (numMultiChannels > 0)
			SetPerformanceStatsPos( mLeftTextWidth+5, 16 );
		else
			ResetPerformanceStatsPos();
	}
	else
		ResetPerformanceStatsPos();

	// enable timeline rendering
	const double timelineHeight = 18.0;
	EnableTimeline(timelineHeight);

	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	RenderSplitViews(numMultiChannels);

	// post rendering
	PostRendering();
}


void ViewWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	//Classifier* classifier = mViewWidget->GetClassifier();
	ViewPlugin* plugin = mViewWidget->GetPlugin();
	
	// get channel and its properties
	const MultiChannel& channels = plugin->GetMultiChannel(index);
	
	// channel signal range
	double rangeMin = channels.GetMinValue();
	double rangeMax = channels.GetMaxValue();

	//CORE_ASSERT(rangeMin < rangeMax);
	if (rangeMin >= rangeMax)
	{
		// last resort: if channel has no valid range (e.g. because all samples have the same value) we just invent a range around this value
		const double rangeCenter = rangeMin;
		rangeMin = rangeCenter - 1.0;
		rangeMax = rangeCenter + 1.0;
	}

	// channel highlight flag overrides mouse highlight
	isHighlighted |= channels.IsHighlighted();

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// settings, feel free to change
	QColor gridColor		= ColorPalette::Shared::GetGridQColor();
	QColor subGridColor		= ColorPalette::Shared::GetDarkSubGridQColor();	
	QColor textColor		= ColorPalette::Shared::GetTextQColor();
	QColor channelLabelColor= ColorPalette::Shared::GetTextQColor();
	QColor backgroundColor	= ColorPalette::Shared::GetDarkBackgroundQColor();
	QColor areaBgColor		= ColorPalette::Shared::GetBackgroundQColor();


	if (isHighlighted == true)
	{
		int factor = 120;
		backgroundColor	= backgroundColor.lighter(factor);
		areaBgColor		= areaBgColor.lighter(factor);
		gridColor		= gridColor.lighter(factor);
		subGridColor	= subGridColor.lighter(factor);
	}


	// automatically calculated, do not change these
	const double areaStartX		= mViewWidget->mLeftTextWidth;
	const double areaWidth		= width - areaStartX;
	const double maxTime		= GetEngine()->GetElapsedTime().InSeconds();			// assumes all channels have same elapsed time

	// draw background rect
	AddRect( 0, 0, width, height, FromQtColor(backgroundColor) );

	// draw area background rect
	AddRect( areaStartX, 0, areaWidth, height, FromQtColor(areaBgColor) );
	RenderRects();

	// calculate the time scale
	double timeRange = plugin->GetTimeRange();
	
	bool drawLatencyMarker = plugin->GetShowLatencyMarker();

	// RENDER CHART

	// draw horizontal line (only) grid
	uint32 numSplits, numSubSplits;
	OpenGLWidget2DHelpers::AutoCalcChartSplits( height, &numSplits, &numSubSplits );
	OpenGLWidget2DHelpers::RenderHGrid( this, numSplits, FromQtColor(gridColor), numSubSplits, FromQtColor(subGridColor), areaStartX, 0.0, areaWidth, height );
	// extend the min/max lines for each signal into the left text area so they are visually separated
	OpenGLWidget2DHelpers::RenderHGrid( this, 1, FromQtColor(gridColor), 0, FromQtColor(subGridColor), 0, 0.0, mViewWidget->mLeftTextWidth, height );

	// render the multichannel signals
	const OpenGLWidget2DHelpers::EChartRenderStyle style = (OpenGLWidget2DHelpers::EChartRenderStyle)plugin->GetSampleStyle();

	const uint32 numChannels = channels.GetNumChannels();

	float textY = 0.f;
	const float textMargin = 2.0f;

	for (uint32 i=0; i<numChannels; ++i )
	{
		Channel<double>* channel = channels.GetChannel(i)->AsType<double>();
		const Color& color = mViewWidget->mPlugin->GetChannelColor(index, i);
		OpenGLWidget2DHelpers::RenderChart( this, channel, color, style, timeRange, maxTime, rangeMin, rangeMax, areaStartX, width, height, height,  drawLatencyMarker);

		// Render channels text
		if (channel->GetSourceNameString().IsEmpty() == false)
			mTempString.Format("%s - %s", channel->GetSourceName(), channel->GetName());
		else
			mTempString.Format("%s", channel->GetName());

		RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), color, areaStartX+textMargin, textY, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
		textY = textY + GetTextHeight() + textMargin;
	}

	// Now Render all lines at once
	if (style == OpenGLWidget2DHelpers::LOLLIPOP || style == OpenGLWidget2DHelpers::CROSS)
		RenderLines(2.5);
	else
		RenderLines(1.5);


	// RENDER TEXT

	//const int textHeight = GetTextHeight();
	//const int textRectWidth = areaStartX - 5;

	// render max value on top
	mTempString.Format("%.2f", rangeMax);
	RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), textColor, areaStartX-textMargin, 0, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_RIGHT );

	// render min value at the bottom
	mTempString.Format("%.2f", rangeMin);
	RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), textColor, areaStartX-textMargin, height, OpenGLWidget::ALIGN_BOTTOM | OpenGLWidget::ALIGN_RIGHT );

	// render values for the in between splits
	for (uint32 i=1; i<numSplits; ++i)
	{
		double y =  (numSplits-i)*(height/numSplits);

		const double value = ClampedRemapRange( (double)i/numSplits, 0.0, 1.0, rangeMin, rangeMax );

		mTempString.Format("%.2f", value);
		RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), textColor, areaStartX-textMargin, y, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_RIGHT );
	}
}


void ViewWidget::RenderCallback::RenderTimeline(double x, double y, double width, double height)
{
	// base class render
	OpenGLWidgetCallback::RenderTimeline( x, y, width, height );

	// draw area background rect
	AddRect( 0, 0, width, height, FromQtColor(QColor(40,40,40)) );
	RenderRects();

	Classifier* classifier = mViewWidget->GetClassifier();
	if (classifier == NULL)
		return;
	
	// automatically calculated, do not change these
	const double areaStartX		= mViewWidget->mLeftTextWidth;
	const double areaWidth		= width - areaStartX;
		
	QColor color = ColorPalette::Shared::GetTextQColor();
	const double timeRange = mViewWidget->GetPlugin()->GetTimeRange();
	const double maxTime = GetEngine()->GetElapsedTime().InSeconds();
	
	OpenGLWidget2DHelpers::RenderTimeline( this, FromQtColor(color), timeRange, maxTime, areaStartX, y, areaWidth, height, mTempString );
}
