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
#include "FeedbackHistoryWidget.h"
#include "FeedbackPlugin.h"

using namespace Core;

// constructor
FeedbackHistoryWidget::FeedbackHistoryWidget(FeedbackPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	mRenderCallback = new RenderCallback(this, this);
	SetCallback( mRenderCallback );

	mLeftTextWidth		= 0.0;
	mPlugin				= plugin;
	mEmptyText			= "No active feedback";

	mGridColor			= ColorPalette::Shared::GetGridQColor();
	mSubGridColor		= ColorPalette::Shared::GetDarkSubGridQColor();
	mTextColor			= ColorPalette::Shared::GetDarkTextQColor();
	mFeedbackNameColor	= ColorPalette::Shared::GetTextQColor();
	mBackgroundColor	= ColorPalette::Shared::GetDarkBackgroundQColor();
	mAreaBgColor		= ColorPalette::Shared::GetBackgroundQColor();
}


// destructor
FeedbackHistoryWidget::~FeedbackHistoryWidget()
{
	delete mRenderCallback;
}


Classifier* FeedbackHistoryWidget::GetClassifier() const
{
	return GetEngine()->GetActiveClassifier();
}


// render frame
void FeedbackHistoryWidget::paintGL()
{
	uint32 numCustomFeedbackNodes = 0;
	Classifier* classifier = GetClassifier();
	if (classifier != NULL)
	{
		double maxTextWidth = 0.0;

		numCustomFeedbackNodes = classifier->GetNumCustomFeedbackNodes();
		for (uint32 i=0; i<numCustomFeedbackNodes; ++i)
		{
			CustomFeedbackNode* feedbackNode = classifier->GetCustomFeedbackNode(i);

			// calc range min text width
			mTempString.Format( "%.2f", feedbackNode->GetRangeMin() );
			maxTextWidth = Max<double>( maxTextWidth, mRenderCallback->CalcTextWidth(mTempString.AsChar()) );

			// calc range max text width
			mTempString.Format( "%.2f", feedbackNode->GetRangeMax() );
			maxTextWidth = Max<double>( maxTextWidth, mRenderCallback->CalcTextWidth(mTempString.AsChar()) );
		}

		mLeftTextWidth = maxTextWidth + 5; // +5 for spacing

		// align the fps stats
		SetPerformanceStatsPos( mLeftTextWidth+5, 16 );
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

	RenderSplitViews( numCustomFeedbackNodes );

	// post rendering
	PostRendering();
}


void FeedbackHistoryWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	Classifier* classifier = mFeedbackWidget->GetClassifier();

	// feedback info
	CustomFeedbackNode* feedbackNode	= classifier->GetCustomFeedbackNode(index);
	//const bool			isRanged		= feedbackNode->IsRanged();
	const double		rangeMin		= feedbackNode->GetRangeMin();
	const double		rangeMax		= feedbackNode->GetRangeMax();
	Channel<double>*	channel			= feedbackNode->GetOutputChannel(0);

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// settings, feel free to change
	QColor gridColor		= mFeedbackWidget->mGridColor;
	QColor subGridColor		= mFeedbackWidget->mSubGridColor;
	QColor textColor		= mFeedbackWidget->mTextColor;
	QColor feedbackNameColor= mFeedbackWidget->mFeedbackNameColor;
	QColor backgroundColor	= mFeedbackWidget->mBackgroundColor;
	QColor areaBgColor		= mFeedbackWidget->mAreaBgColor;
	QColor feedbackColor	= ToQColor( feedbackNode->GetColor() );
//	feedbackColor.setAlpha( 175 );

	if (isHighlighted == true)
	{
		int factor = 120;
		
		backgroundColor	= backgroundColor.lighter(factor);
		areaBgColor		= areaBgColor.lighter(factor);
		gridColor		= gridColor.lighter(factor);
		subGridColor	= subGridColor.lighter(factor);
		
		feedbackColor.setAlpha( 255 );
	}


	// automatically calculated, do not change these
	const double areaStartX		= mFeedbackWidget->mLeftTextWidth;
	const double areaWidth		= width - areaStartX;
	const double maxTime = channel->GetElapsedTime().InSeconds();			// assumes all channels have same elapsed time

	// draw background rect
	AddRect( 0, 0, width, height, FromQtColor(backgroundColor) );

	// draw area background rect
	AddRect( areaStartX, 0, areaWidth, height, FromQtColor(areaBgColor) );
	RenderRects();

	// calculate the time scale
	double timeRange = mFeedbackWidget->GetPlugin()->GetTimeRange();
	
	bool drawLatencyMarker = mFeedbackWidget->GetPlugin()->GetShowLatencyMarker();

	// RENDER CHART

	// draw horizontal line (only) grid
	uint32 numSplits, numSubSplits;
	OpenGLWidget2DHelpers::AutoCalcChartSplits( height, &numSplits, &numSubSplits );
	OpenGLWidget2DHelpers::RenderHGrid( this, numSplits, FromQtColor(gridColor), numSubSplits, FromQtColor(subGridColor), areaStartX, 0.0, areaWidth, height );

	// render feedback signal
	const OpenGLWidget2DHelpers::EChartRenderStyle style = (OpenGLWidget2DHelpers::EChartRenderStyle)mFeedbackWidget->GetPlugin()->GetSampleStyle();
	OpenGLWidget2DHelpers::RenderChart( this, channel, FromQtColor(feedbackColor), style, timeRange, maxTime, rangeMin, rangeMax, areaStartX, width, height, height,  drawLatencyMarker);

	// use thicker lines if styles with lines are selected
	if (style == OpenGLWidget2DHelpers::LINE || style == OpenGLWidget2DHelpers::LOLLIPOP || style == OpenGLWidget2DHelpers::CROSS)
		RenderLines(2.5);
	else
		RenderLines(1.5);

	// RENDER TEXT

	//const int textHeight = GetTextHeight();
	//const int textRectWidth = areaStartX - 5;
	const int textMarginX = 2;

	// render max value on top
	mTempString.Format("%.2f", rangeMax);
	RenderText( mTempString.AsChar(), mParent->GetDefaultFontSize(), textColor, areaStartX-textMarginX, 0.0, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_RIGHT );

	// render min value at the bottom
	mTempString.Format("%.2f", rangeMin);
	RenderText( mTempString.AsChar(), mParent->GetDefaultFontSize(), textColor, areaStartX-textMarginX, height-textMarginX, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_RIGHT );

	// render values for the in between splits
	for (uint32 i=1; i<numSplits; ++i)
	{
		double y = (numSplits-i)*(height/numSplits);

		const double value = ClampedRemapRange( (double)i/numSplits, 0.0, 1.0, rangeMin, rangeMax );

		mTempString.Format("%.2f", value);
		RenderText( mTempString.AsChar(), mParent->GetDefaultFontSize(), textColor, areaStartX-textMarginX, y, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_RIGHT );
	}

	// render feedback name
	mTempString.Format("%s", feedbackNode->GetName());
	RenderText( mTempString.AsChar(), mParent->GetDefaultFontSize(), feedbackNameColor, areaStartX+textMarginX, 0, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
}


void FeedbackHistoryWidget::RenderCallback::RenderTimeline(double x, double y, double width, double height)
{
	// base class render
	OpenGLWidgetCallback::RenderTimeline( x, y, width, height );

	// draw area background rect
	AddRect( 0, 0, width, height, FromQtColor(QColor(40,40,40)) );
	RenderRects();

	Classifier* classifier = mFeedbackWidget->GetClassifier();
	if (classifier == NULL)
		return;
	
	// automatically calculated, do not change these
	const double areaStartX		= mFeedbackWidget->mLeftTextWidth;
	const double areaWidth		= width - areaStartX;

	QColor color = QColor(255,255,255);
	const double timeRange = mFeedbackWidget->GetPlugin()->GetTimeRange();
	const double maxTime = GetEngine()->GetElapsedTime().InSeconds();
	
	if (classifier->GetNumCustomFeedbackNodes() > 0)
	{
		OpenGLWidget2DHelpers::RenderTimeline( this, FromQtColor(color), timeRange, maxTime, areaStartX, y, areaWidth, height, mTempString );
	}
}
