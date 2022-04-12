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
#include "ViewSpectrumWidget.h"
#include "ViewSpectrumPlugin.h"
#include <Core/Math.h>
#include <Core/LogManager.h>
#include <Core/EventManager.h>
#include <ColorPalette.h>
#include <QPainter>

using namespace Core;

// constructor
ViewSpectrumWidget::ViewSpectrumWidget(ViewSpectrumPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	mRenderCallback = new RenderCallback(this, this);
	SetCallback( mRenderCallback );

	mPlugin				= plugin;
	mEmptyText			= "No signals";

	mLeftTextWidth		= 0.0;

	// fill the grid info
	mGridInfo.mUnitX = "Hz";

	setMinimumHeight(150);
}


// destructor
ViewSpectrumWidget::~ViewSpectrumWidget()
{
	delete mRenderCallback;
}


// render frame
void ViewSpectrumWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	// get channel and its properties
	const uint32 numSpectrums = mPlugin->GetNumMultiChannels();

	// set the performance statistics text position
	mLeftTextWidth = 40;
	if (numSpectrums > 0)
		SetPerformanceStatsPos( mLeftTextWidth + 5, 16 );
	else
		ResetPerformanceStatsPos();
	
	if (numSpectrums == 0)
		RenderEmpty();

	uint32 numAreasInColumn = Math::Sqrt(numSpectrums) + 0.5;
	RenderSplitViews( numSpectrums, numAreasInColumn );

	// post rendering
	PostRendering();
}


// render callback
void ViewSpectrumWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// settings, feel free to change
	QColor gridColor		= ColorPalette::Shared::GetGridQColor();			// was: QColor(137, 137, 136);
	QColor subGridColor		= ColorPalette::Shared::GetSubGridQColor();			// was: QColor(74,74,73);
	QColor textColor		= ColorPalette::Shared::GetDarkTextQColor();		// was:QColor(100,100,100);
	QColor channelLabelColor= ColorPalette::Shared::GetTextQColor();			// was:QColor(218, 218, 218);
	QColor backgroundColor	= ColorPalette::Shared::GetBackgroundQColor();		// was:QColor(40, 40, 40);
	QColor areaBgColor		= ColorPalette::Shared::GetBackgroundQColor();		// was:QColor(60,60,59);

	const MultiChannel& channels = mViewWidget->mPlugin->GetMultiChannel(index);

	CORE_ASSERT (channels.GetNumChannels() > 0);

	isHighlighted |= channels.IsHighlighted();

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
	const double areaEndX		= areaStartX+areaWidth;

	const double areaStartY		= height - 15.0;
	const double areaEndY		= 0.0;
	const double areaHeight		= areaStartY;

	// draw background rect
	AddRect( 0.0, 0.0, width, height, FromQtColor(backgroundColor) );

	// draw area background rect
	AddRect( areaStartX, 0.0, areaWidth, areaStartY, FromQtColor(areaBgColor) );

	RenderRects();

	// TODO this will be much cleaner with V2 Channels
	// get first spectrum from first channel, so we know how they are configured
	Channel<Spectrum>* channel = channels.GetChannel(0)->AsType<Spectrum>();
	if (channel->GetNumSamples() == 0)
		return;
	const Spectrum& spectrum = channel->GetLastSample();

	uint32 numBins = 0;
	
	// min/max freq and min/max bin index
	const double minFreq = mViewWidget->mPlugin->GetMinFrequency();
	const double maxFreq = mViewWidget->mPlugin->GetMaxFrequency();

	// find maximum/minimum spectrum values
	if (spectrum.GetNumBins() > 1)
	{
		numBins = spectrum.GetNumBins();

		double max = 0.0;
		const uint32 numChannels = channels.GetNumChannels();
		for (uint32 i = 0; i<numChannels; ++i)
		{
			Channel<Spectrum>* multiChannel = channels.GetChannel(i)->AsType<Spectrum>();
			if (multiChannel->GetNumSamples() == 0)
				continue;
			
			const Spectrum& multiSpectrum = multiChannel->GetLastSample();
			const uint32 minBinIndex = multiSpectrum.CalcBinIndex(minFreq);
			const uint32 maxBinIndex = multiSpectrum.CalcBinIndex(maxFreq);
			const double spectrumMax = multiSpectrum.CalcMaxBin(minBinIndex, maxBinIndex);
			max = Max<double>(max, spectrumMax);
		}


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

	double rangeMin = 0;
	const double rangeMax = mCurMaxRange;

	mViewWidget->mGridInfo.mNumXSplits	= Math::NextPowerOfTwo( mParent->devicePixelRatio() * (width / 100) );
	mViewWidget->mGridInfo.mNumXSubSplits	= 0;
	mViewWidget->mGridInfo.mNumYSplits	= Math::NextPowerOfTwo( mParent->devicePixelRatio() * (height / 75) );
	mViewWidget->mGridInfo.mNumYSubSplits	= 4;

	OpenGLWidget2DHelpers::RenderGrid( this, &mViewWidget->mGridInfo, FromQtColor(gridColor), FromQtColor(subGridColor), areaStartX, areaEndX, areaStartY, areaEndY );

	if (numBins == 0)
		return;


	//// render single spectrum
	//if (multiView == true)
	//{
	//	const uint32 minBinIndex = spectrum.CalcBinIndex(minFreq);
	//	const uint32 maxBinIndex = spectrum.CalcBinIndex(maxFreq);
	//	// need to get the pointer, just for the renderer (cant use const ref)
	//	Spectrum* spectrumRef = channel->GetLastSampleRef();
	//	OpenGLWidget2DHelpers::RenderSpectrumChart(this, spectrumRef, barColor, minBinIndex, maxBinIndex, rangeMin, rangeMax, areaStartX, areaEndX, areaStartY, areaEndY);
	//}
	//else
	//{

	const uint32 numChannels = channels.GetNumChannels();
	for (uint32 i = 0; i<numChannels; ++i)
	{
		Channel<Spectrum>* multiChannel = channels.GetChannel(i)->AsType<Spectrum>();
		if (multiChannel->GetNumSamples() == 0)
			continue;

		Spectrum* spectrum = multiChannel->GetLastSampleRef();

		const uint32 minBinIndex = spectrum->CalcBinIndex(minFreq);
		const uint32 maxBinIndex = spectrum->CalcBinIndex(maxFreq);
		const Color& color = mViewWidget->mPlugin->GetChannelColor(index, i);

		OpenGLWidget2DHelpers::RenderSpectrum(this, spectrum, color, minBinIndex, maxBinIndex, rangeMin, rangeMax, areaStartX, areaEndX, areaStartY, areaEndY);
	}

	// iterate through all channels and render all spectrums

	// RENDER TEXT

	const float textMargin = 2.0f;
	//const int textHeight = GetTextHeight();
	//const int textRectWidth = areaStartX - 5;

	// render Y axis labels
	OpenGLWidget2DHelpers::RenderEquallySpacedYLabels( this, mTempString, textColor, rangeMin, rangeMax, mViewWidget->mGridInfo.mNumYSplits, 0.0, 0.0, areaStartX-textMargin, areaHeight );

	// render X axis labels
	OpenGLWidget2DHelpers::RenderEquallySpacedXLabels( this, mTempString, "Hz", textColor, minFreq, maxFreq, mViewWidget->mGridInfo.mNumXSplits, areaStartX, areaHeight, width - areaStartX, height - areaHeight );

	// render channel name
	//if (multiView == true)
	//{
		mTempString.Format("%s - %s", channel->GetSourceName(), channel->GetName());
		RenderText( mTempString.AsChar(), GetOpenGLWidget()->GetDefaultFontSize(), channelLabelColor, areaStartX+textMargin, 0, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
	//}
}
