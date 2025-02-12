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
#include "Spectrogram2DWidget.h"
#include "Spectrogram2DPlugin.h"

using namespace Core;

// constructor
Spectrogram2DWidget::Spectrogram2DWidget(Spectrogram2DPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	mRenderCallback = new RenderCallback(this, this);
	SetCallback( mRenderCallback );

	mPlugin				= plugin;
	mEmptyText			= "No active device";

	mMultiView			= false;
	mLeftTextWidth		= 0.0;

	mGridColor			= ColorPalette::Shared::GetAxisQColor();
	mSubGridColor		= ColorPalette::Shared::GetSubGridQColor();
	mTextColor			= ColorPalette::Shared::GetDarkTextQColor();
	mFeedbackNameColor	= ColorPalette::Shared::GetTextQColor();
	mBackgroundColor	= ColorPalette::Shared::GetDarkBackgroundQColor();
	mAreaBgColor		= ColorPalette::Shared::GetBackgroundQColor();

	// fill the grid info
	mGridInfo.mUnitX = "Hz";

	setMinimumHeight(150);
}

// destructor
Spectrogram2DWidget::~Spectrogram2DWidget()
{
	delete mRenderCallback;
}


// render frame
void Spectrogram2DWidget::paintGL()
{
	uint32 numSpectrums = mSpectrums.Size();
	mLeftTextWidth = 50; // TODO HACK
/*	Classifier* classifier = GetClassifier();
	if (classifier != NULL)
	{
		double maxTextWidth = 0.0;

		numFeedbackScores = classifier->GetNumFeedbackScores();
		for (uint32 i=0; i<numFeedbackScores; ++i)
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
	}*/

	SetPerformanceStatsPos( mLeftTextWidth + 5, 13 );

	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	// render single or multi view
	if (mMultiView == false)
	{
		if (numSpectrums == 0)
			RenderEmpty();
		else
			Render();
	}
	else
	{
		uint32 numAreasOnLine;
		if (!mHorizontalView){
			numAreasOnLine = Math::Sqrt(numSpectrums) + 0.5;
		}
		else
		{
			numAreasOnLine = 2;
			if (numSpectrums % 2 == 1)
			{
				--numSpectrums;
			}
		}
		RenderSplitViews( numSpectrums, numAreasOnLine, 2.0, mHorizontalView );
	}

	// post rendering
	PostRendering();
}


// render callback
void Spectrogram2DWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	const bool show0HzBin = mParentWidget->mPlugin->GetShow0HzBin();
	const uint32 minBinIndex = show0HzBin == true ? 0 : 1;

	float textMargin = 2.0f;

	// settings, feel free to change
	QColor gridColor		= mParentWidget->mGridColor;
	QColor subGridColor		= mParentWidget->mSubGridColor;
	QColor textColor		= mParentWidget->mTextColor;
	QColor feedbackNameColor= mParentWidget->mFeedbackNameColor;
	QColor backgroundColor	= mParentWidget->mBackgroundColor;
	QColor areaBgColor		= mParentWidget->mAreaBgColor;

	if (isHighlighted == true)
	{
		int factor = 120;
		
		backgroundColor	= backgroundColor.lighter(factor);
		areaBgColor		= areaBgColor.lighter(factor);
		gridColor		= gridColor.lighter(factor);
		subGridColor	= subGridColor.lighter(factor);
	}

	// automatically calculated, do not change these
	const double areaStartX		= mParentWidget->mLeftTextWidth;
	const double areaWidth		= width - areaStartX;
	const double areaEndX		= areaStartX+areaWidth;

	const double areaStartY		= height - 13.0;
	const double areaEndY		= 0.0;
	const double areaHeight		= areaStartY;

	// draw background rect
	AddRect( 0.0, 0.0, width, height, FromQtColor(backgroundColor) );

	// draw area background rect
	AddRect( areaStartX, 0.0, areaWidth, areaStartY, FromQtColor(areaBgColor) );

	RenderRects();

	const uint32 numSpectrums = mParentWidget->mSpectrums.Size();
	SpectrumElement* spectrumElement = NULL;
	if (numSpectrums == 0 || (numSpectrums > 0 && mParentWidget->mSpectrums[0].mSpectrum == NULL))
		return;

	spectrumElement = &mParentWidget->mSpectrums[index];

	uint32 numBins = 0;
	if (spectrumElement != NULL && spectrumElement->mSpectrum->GetNumBins() > 1)
	{
		numBins = Min( spectrumElement->mSpectrum->GetNumBins(), spectrumElement->mSpectrum->CalcBinIndex(60) );

		double max = 0.0;
		for (uint32 i=0; i<numSpectrums; ++i)
		{
			const SpectrumElement& currentElement = mParentWidget->mSpectrums[i];
			const double spectrumMax = currentElement.mSpectrum->CalcMaxBin(minBinIndex, numBins-1);
			max = Max<double>(max, spectrumMax);
		}

		// old way: find closest power of two
		//rangeMax = LinearInterpolate( mCurMaxRange, max, mParent->GetTimeDelta() * 0.1 );
		//mCurMaxRange = rangeMax;
		//rangeMax = Math::NextPowerOfTwo( rangeMax );

		// new way: apply hysteresis and only rescale if the difference is large enough (uses different max value for each direction)

		// 1) slow down the maximum peak value
		mSlowedMaxRange = LinearInterpolate(mSlowedMaxRange, max, mParent->GetTimeDelta() * 0.5);
		
		// 2) large jump hysteresis if the actual max value is much larger or much smaller than the current max value, we skip the smoothing and reset the value directly
		if (max != 0.0)
		{
			const double maxFactor = 4.0;
			const double factor = Math::AbsD(mCurMaxRange / max);
			if (factor < 1 / maxFactor || factor > maxFactor)
			{
				mCurMaxRange = Math::NiceNumberCeil(max);
				mSlowedMaxRange = mCurMaxRange;
			}
		}

		// 2) normal jump hysteresis hysteresis
		//  -> update max range if  larger, or if it is 66% smaller than the current range
		const double rangeDiff = mSlowedMaxRange - mCurMaxRange;
		if (rangeDiff > 0.0 || -rangeDiff > 0.66 * mCurMaxRange)
		{
			mCurMaxRange = Math::NiceNumberCeil(max);
			mSlowedMaxRange = mCurMaxRange;
		}
	}


	// the range used for scaling
	double rangeMin = 0.0;
	double rangeMax = mCurMaxRange;



    const double minFrequency = spectrumElement->mSpectrum->CalcFrequency(minBinIndex);
    const double maxFrequency = spectrumElement->mSpectrum->CalcFrequency(numBins-1);


	mParentWidget->mGridInfo.mNumXSplits	= Math::NextPowerOfTwo( mParent->devicePixelRatio() * (width / 100) );
	mParentWidget->mGridInfo.mNumXSubSplits	= 0;
	mParentWidget->mGridInfo.mNumYSplits	= mParentWidget->GetMultiView() && mParentWidget->GetHorizontalView() // for horizontal view we want to densly place splits
												? ((maxFrequency - minFrequency) / (mParent->devicePixelRatio() * 5)) :
												Math::NextPowerOfTwo( mParent->devicePixelRatio() * (height / 75) );
	mParentWidget->mGridInfo.mNumYSubSplits	= 5;

	OpenGLWidget2DHelpers::RenderGrid( this, &mParentWidget->mGridInfo, FromQtColor(gridColor), FromQtColor(subGridColor), areaStartX, areaEndX, areaStartY, areaEndY );

	if (numBins == 0)
		return;

	// in the case of horizontal views, we show 2 charts per row, thus mParentWidget->mSpectrums.Size() / 2 - 1 is the first index of right column charts
	bool drawFromRight = (index % 2 == 0);
	// render spectrum
	if (mParentWidget->GetMultiView() == true)
	{
		OpenGLWidget2DHelpers::RenderSpectrumChart( this, spectrumElement->mSpectrum, spectrumElement->mColor, minFrequency, maxFrequency, rangeMin, rangeMax, areaStartX, areaEndX, areaStartY, areaEndY, mParentWidget->GetHorizontalView(), drawFromRight);
	}
	else
	{
		// iterate through all spectra and render each
		for (uint32 i=0; i<numSpectrums; ++i)
		{
			const SpectrumElement& currentElement = mParentWidget->mSpectrums[i];
			OpenGLWidget2DHelpers::RenderSpectrum(this, currentElement.mSpectrum, currentElement.mColor, minFrequency, maxFrequency, rangeMin, rangeMax, areaStartX, areaEndX, areaStartY, areaEndY);
		}
	}

	// RENDER TEXT

	//const int textHeight = GetTextHeight();
	const int textRectWidth = areaStartX - 5;

	auto horizontalView = mParentWidget->GetHorizontalView();
	if (mParentWidget->GetMultiView() == false || mParentWidget->GetHorizontalView() == false) {
		// render Y axis labels
		OpenGLWidget2DHelpers::RenderEquallySpacedYLabels( this, mTempString, textColor, rangeMin, rangeMax, mParentWidget->mGridInfo.mNumYSplits, 0.0, 0.0, textRectWidth, areaHeight );
		// render X axis labels
		OpenGLWidget2DHelpers::RenderEquallySpacedXLabels( this, mTempString, "Hz", textColor, minFrequency, maxFrequency, mParentWidget->mGridInfo.mNumXSplits, areaStartX, height-areaHeight, width-areaStartX, areaHeight );
	} else {
		// render Y axis labels
    	OpenGLWidget2DHelpers::RenderEquallySpacedYLabels( this, mTempString, textColor, minFrequency, maxFrequency, mParentWidget->mGridInfo.mNumYSplits, 0.0, 0.0, textRectWidth, areaHeight, horizontalView, drawFromRight,  mParentWidget->mGridInfo.mNumYSubSplits);
		// render X axis labels
    	OpenGLWidget2DHelpers::RenderEquallySpacedXLabels( this, mTempString, "Hz", textColor, rangeMin, rangeMax, mParentWidget->mGridInfo.mNumXSplits, areaStartX, height-areaHeight, width-areaStartX, areaHeight, horizontalView, drawFromRight );
	}

	// render feedback name
	if (spectrumElement != NULL && mParentWidget->GetMultiView() == true)
	{
		mTempString.Format("%s", spectrumElement->mName.AsChar());
		RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), spectrumElement->mColor, areaStartX+textMargin, 0, OpenGLWidget::ALIGN_TOP| OpenGLWidget::ALIGN_LEFT );
	}
}