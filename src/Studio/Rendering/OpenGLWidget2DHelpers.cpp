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
#include "OpenGLWidget2DHelpers.h"
#include <QtBaseConfig.h>
#include <Core/Math.h>
#include <Core/Time.h>


using namespace Core;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Grids
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// render horizontal lines (only) grid
void OpenGLWidget2DHelpers::RenderHGrid(OpenGLWidgetCallback* callback, uint32 numSplits, const Color& mainColor, uint32 numSubSplits, const Color& subColor, double xOffset, double yOffset, double width, double height)
{
	const double splitDelta		= height / (double)numSplits;
	const double subSplitDelta	= splitDelta / (double)numSubSplits;
	const double xStart			= xOffset;
	const double xEnd			= xOffset + width;

	// render main horizontal lines
	for (uint32 i=0; i<=numSplits; ++i)
	{
		double splitY = yOffset + height - i*splitDelta;
		callback->AddLine( xStart, splitY, mainColor, xEnd, splitY, mainColor );
	}

	// render sub horizontal lines
	for (uint32 i=0; i<numSplits; ++i)
	{
		double splitY = yOffset + height - i*splitDelta;

		for (uint32 j=1; j<numSubSplits; ++j)
		{
			double subSplitY = splitY - j*subSplitDelta;
			callback->AddLine( xStart, subSplitY, subColor, xEnd, subSplitY, subColor );
		}
	}

	callback->RenderLines();
}


void OpenGLWidget2DHelpers::RenderGrid(OpenGLWidgetCallback* callback, GridInfo* gridInfo, const Core::Color& mainColor, const Core::Color& subColor, double xStart, double xEnd, double yStart, double yEnd)
{
	const double width			= xEnd - xStart;
	const double height			= yEnd - yStart;
	const double splitDeltaY	= height / (double)gridInfo->mNumYSplits;
	const double subSplitDeltaY	= splitDeltaY / (double)gridInfo->mNumYSubSplits;
	const double splitDeltaX	= width / (double)gridInfo->mNumXSplits;
	const double subSplitDeltaX	= splitDeltaX / (double)gridInfo->mNumXSubSplits;

	// render main horizontal lines
	for (uint32 i=0; i<=gridInfo->mNumYSplits; ++i)
	{
		double splitY = yStart + height - i*splitDeltaY;
		callback->AddLine( xStart, splitY, mainColor, xEnd, splitY, mainColor );
	}

	// render sub horizontal lines
	if (gridInfo->mNumYSubSplits > 1)
	{
		for (uint32 i=0; i<gridInfo->mNumYSplits; ++i)
		{
			double splitY = yStart + height - i*splitDeltaY;

			for (uint32 j=1; j<gridInfo->mNumYSubSplits; ++j)
			{
				double subSplitY = splitY - j*subSplitDeltaY;
				callback->AddLine( xStart, subSplitY, subColor, xEnd, subSplitY, subColor );
			}
		}
	}


	// render main vertical lines
	for (uint32 i=0; i<=gridInfo->mNumXSplits; ++i)
	{
		double splitX = xStart + width - i*splitDeltaX;
		callback->AddLine( splitX, yStart, mainColor, splitX, yEnd, mainColor );
	}

	// render sub vertical lines
	if (gridInfo->mNumXSubSplits > 1)
	{
		for (uint32 i=0; i<gridInfo->mNumXSplits; ++i)
		{
			double splitX = xStart + width - i*splitDeltaX;

			for (uint32 j=1; j<gridInfo->mNumXSubSplits; ++j)
			{
				double subSplitX = splitX - j*subSplitDeltaX;
				callback->AddLine( subSplitX, yStart, subColor, subSplitX, yEnd, subColor );
			}
		}
	}

	callback->RenderLines();
}


void OpenGLWidget2DHelpers::RenderEquallySpacedYLabels(OpenGLWidgetCallback* callback, String& tempString, const QColor& textColor, double minValue, double maxValue, uint32 numSplits, double x, double y, double width, double height)
{
	const double range = Max<double>(minValue, maxValue) - Min<double>(minValue, maxValue);

	double splitRange = 0.0;
	if (numSplits > 0)
		splitRange = range / numSplits;

	//QRect textRect;
	//const double textHeight = callback->GetTextHeight();
	//const int textMarginX = 2;

	// render min value at the bottom
	tempString.Format("%.2f", minValue);
	//textRect = QRect(x, y+height-textHeight, width, textHeight);

	callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, width, height, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_RIGHT );

	// render values for the in between splits
	for (uint32 i=1; i<numSplits; ++i)
	{
		double y = (numSplits-i)*(height/numSplits);

		const double value = ClampedRemapRange( (double)i/numSplits, 0.0, 1.0, minValue, maxValue );

		// draw the timestamp label
		if (splitRange > 5.0)
			tempString.Format("%.0f", value);
		else if (splitRange > 1.0)
			tempString.Format("%.1f", value);
		else
			tempString.Format("%.2f", value);

		//textRect = QRect(x, y+height-yOffset-(textHeight/2), width, textHeight);

		callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, width, y, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_RIGHT );
	}

	// render max value on top
	if (splitRange > 5.0)
		tempString.Format("%.0f", maxValue);
	else if (splitRange > 1.0)
		tempString.Format("%.1f", maxValue);
	else
		tempString.Format("%.2f", maxValue);

	//textRect = QRect(x, y, width, textHeight);

	callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, width, 0, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_RIGHT );
}


void OpenGLWidget2DHelpers::RenderEquallySpacedXLabels(OpenGLWidgetCallback* callback, String& tempString, const char* unit, const QColor& textColor, double minValue, double maxValue, uint32 numSplits, double x, double y, double width, double height)
{
	const double range = Max<double>(minValue, maxValue) - Min<double>(minValue, maxValue);

	double splitRange = 0.0;
	if (numSplits > 0)
		splitRange = range / numSplits;

	//QRect textRect;
	for (uint32 i=0; i<=numSplits; ++i)
	{
		double xOffset = x + i*(width/numSplits);

		const double value = ClampedRemapRange( (double)i/numSplits, 0.0, 1.0, minValue, maxValue );

		// draw the timestamp label
		if (splitRange > 1.0)
			tempString.Format("%.0f %s", value, unit);
		else if (splitRange > 0.1)
			tempString.Format("%.1f %s", value, unit);
		else
			tempString.Format("%.2f %s", value, unit);

		//double textWidth = callback->CalcTextWidth( tempString.AsChar() );
		//const int rectHeight = 9;
		//const int rectY = y + 3;
		
		if (i == 0)
		{
			//textRect = QRect(x, rectY, textWidth, rectHeight);
			callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, x, y+height, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_LEFT );
		}
		else if (i == numSplits)
		{
			//textRect = QRect(x+width-textWidth, rectY, textWidth, rectHeight);

			callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, x+width, y+height, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_RIGHT );
		}
		else
		{
			//double textX = xOffset-(textWidth*0.5);
			//textRect = QRect(textX, rectY, textWidth, rectHeight);
			if (xOffset > 0)
			{
				callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), textColor, xOffset, y+height, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_CENTER );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Signal Rendering
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// calculate automatic splitting
void OpenGLWidget2DHelpers::AutoCalcChartSplits(double height, uint32* outNumSplits, uint32* outNumSubSplits)
{
	*outNumSplits = 2; *outNumSubSplits = 0;

	if (height > 200)
	{
		*outNumSplits = 10; *outNumSubSplits = 5;
		return;
	}

	if (height > 75)
	{
		*outNumSplits = 4; *outNumSubSplits = 2;
		return;
	}
}


// render a channel
void OpenGLWidget2DHelpers::RenderChart(OpenGLWidgetCallback* callback, Channel<double>* channel, const Color& color, EChartRenderStyle style, double timeRange, double maxTime, double rangeMin, double rangeMax, int32 xStart, int32 xEnd, int32 yStart, int32 height, bool drawLatencyMarker)
{
	// draw only if the parameters are valid 
	if (channel == NULL || xEnd < xStart)
		return;

	if (channel->GetNumSamples() == 0 || channel->GetSampleRate() <= 0)
		return;

	//
	// Sample / Pixel / Timeranges
	//

	//const double sampleRate = channel->GetSampleRate();

	// calc remaining parameters
	//const int32 numPixels = xEnd - xStart;					// width of the chart area in pixels
	double minTime = maxTime - timeRange;						// time of first pixel
	const uint32 maxSampleIndex = channel->GetMaxSampleIndex();	// index of last sample
	uint32 minSampleIndex = channel->FindIndexByTime(minTime);	// index of first sample (clamped to valid index and rounded downwards -> may lie outside of left border)

	CORE_ASSERT(maxSampleIndex >= minSampleIndex);


	//
	// Latency calculations for latency marker and clipping
	//
	const double latency = channel->GetLatency();
	uint32 markerX = xEnd;
	uint32 xClippingEnd = xEnd; // place clip edge to the most right by default
	if (latency != 0 && drawLatencyMarker == true)
	{
		// pixel xcoord of latency line
		const double latencyTime = maxTime - latency;
		markerX = RemapRange(latencyTime, minTime, maxTime, xStart,  xEnd);
		
		xClippingEnd = markerX;
	}

	//
	// Render Samples
	//

	// select render functions
	RenderSampleFunction sampleRenderer = SelectSampleRenderFunction(style, false);
	RenderSampleFunction clippedSampleRenderer = SelectSampleRenderFunction(style, true);

	// initialize helper variables
	double x, y;			// current datapoint as pixel coordinates
	double value, time;		// current datapoint as value/time
	double previousX, previousY;

	// sample size
	const uint32 numSamples = maxSampleIndex - minSampleIndex;
	const double displayedTimeRange = numSamples / channel->GetSampleRate();
	const double usedPixels = displayedTimeRange * (xEnd - xStart) / timeRange;
	double size = GetSampleSize(style, usedPixels, height, numSamples);

	// color range
	Color lighterColor = FromQtColor( ToQColor(color).lighter(110) );
	Color darkerColor = FromQtColor( ToQColor(color).darker(170) );
	Color valueColor;		// color of the pixel at the value position, interpolated between lighter/darker color
	
	////////////////////////////////////////////////////////////////////
	//// 0) Setup: calculate previousX and previousY using the sample at minSampleIndex (which lies outside of the drawing area)
	////
	{ // modified copy of loop body of 2)
		// get time of the sample at minSampleIndex-1 (which may lie outside of the drawing area)
		time = channel->GetSampleTime(minSampleIndex).InSeconds() - 1.0 / channel->GetSampleRate();

		// map time to pixel x-coordinate
		previousX = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// clamp and remap value to y coordinate
		value = channel->GetSample(minSampleIndex);
		previousY = ClampedRemapRange(value, rangeMin, rangeMax, yStart, 0);

		// FIX due to antialiasing problems : round the coords to int and add the twiddle factor
		//previousX = (int32)previousX + 0.375;
		previousY = (int32)previousY + 0.375;
	}

	//////////////////////////////////////////////////////////////////
	// 1) draw the first two visible sample 
	for (uint32 i=minSampleIndex; (i < minSampleIndex + 2) && (i <= maxSampleIndex); ++i)
	{ // copy of loop body of 2)
		
		// get time of sample
		time = channel->GetSampleTime(i).InSeconds();

		// map time to pixel x-coordinate
		x = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// clamp and remap value to y coordinate
		value = channel->GetSample(i);
		y = ClampedRemapRange(value, rangeMin, rangeMax, yStart, 0);

		y = (int32)y + 0.375;

		// calculate the color for the top point (value) of the line
		float normalizedValue = ClampedRemapRange( value, rangeMin, rangeMax, 0.0, 1.0 );
		valueColor = LinearInterpolate<Color>( darkerColor, lighterColor, normalizedValue );

		// draw the clipped sample
		clippedSampleRenderer(callback, value, x, y, previousX, previousY, xStart, xEnd, yStart, 0, size, lighterColor, darkerColor, valueColor);
	
		// store the current x and y pos for the next sample
		previousX = x;
		previousY = y;
	}

	//////////////////////////////////////////////////////////////////
	// 2) draw all middle samples
	if (maxSampleIndex > 0)
	for (uint32 i = minSampleIndex + 2; i < maxSampleIndex-1; i++)
	{
		// get time of sample
		time = channel->GetSampleTime(i).InSeconds();

		// map time to pixel x-coordinate
		x = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// if a sample lies outside of the right border (i.e. if input channel has to many samples) we break out of the loop immediately
		if (x > xEnd)
			break;

		// clamp and remap value to y coordinate
		value = channel->GetSample(i);
		y = ClampedRemapRange(value, rangeMin, rangeMax, yStart, 0);

		y = (int32)y + 0.375;

		// calculate the color for the top point (value) of the line
		float normalizedValue = ClampedRemapRange( value, rangeMin, rangeMax, 0.0, 1.0 );
		valueColor = LinearInterpolate<Color>( darkerColor, lighterColor, normalizedValue );

		// draw the sample
		sampleRenderer(callback, value, x, y, previousX, previousY, xStart, xEnd, yStart, 0, size, lighterColor, darkerColor, valueColor);
	
		// store the current x and y pos for the next cycle
		previousX = x;
		previousY = y;
	}

	// FIXME use the delay to calculate the last visible sample - clip only this one and do not draw the rest

	//////////////////////////////////////////////////////////////////
	// 3) draw the last sample clipped (mostly a copy of loop body)
	if (maxSampleIndex > 0)
	for (uint32 i = maxSampleIndex-1; i <= maxSampleIndex; i++)
	{ // copy of loop body of 2)

		// get time of very first sample (which lies outside of the drawing area)
		time = channel->GetSampleTime(i).InSeconds();

		// map time to pixel x-coordinate
		x = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// clamp and remap value to y coordinate
		value = channel->GetSample(i);
		y = ClampedRemapRange(value, rangeMin, rangeMax, yStart, 0);

		y = (int32)y + 0.375;

		// calculate the color for the top point (value) of the line
		float normalizedValue = ClampedRemapRange( value, rangeMin, rangeMax, 0.0, 1.0 );
		valueColor = LinearInterpolate<Color>( darkerColor, lighterColor, normalizedValue );

		// draw the clipped sample
		clippedSampleRenderer(callback, value, x, y, previousX, previousY, xStart, xClippingEnd, yStart, 0, size, lighterColor, darkerColor, valueColor);
		
		// store the current x and y pos for the next cycle
		previousX = x;
		previousY = y;
	}

	//
	// Render latency marker (if latency distance is greater than two pixels)
	//
	
	if (drawLatencyMarker == true)
	{
		// calculate how many pixels the signal 'lags behind'
		//const double numPixelsLag = xEnd - RemapRange(maxTime - latency, minTime, maxTime, xStart, xEnd);
		//if (numPixelsLag > 2.0)
			callback->AddLine(markerX, yStart, lighterColor, markerX, 0, lighterColor);
	}

}



//
//// render 2D wave for the given channel										   
//void OpenGLWidget2DHelpers::RenderChart(OpenGLWidgetCallback* callback, Channel<double>* channel, const Color& color, EChartRenderStyle style, double timeScale, double maxTime, double rangeMin, double rangeMax, int32 xStart, int32 xEnd, int32 yStart, int32 height)
//{
//	if (channel == NULL)
//		return;
//
//	// FIXME drawing should also work with a single sample (e.g. very low sample rate)
//	if (channel->GetNumSamples() < 2)
//		return;
//
//	int32			xStartPixel			= xStart;
//	int32			xEndPixel			= xEnd;
//	double			numPixels			= xEndPixel - xStartPixel;
//	const double	sampleRate			= channel->GetSampleRate();
//
//	// drawing offset for time alignment
//	const double    timeOffset			= maxTime - channel->GetLastSampleTime();							// last sample is shiftet by this many seconds to the left
//	const double	sampleOffset		= Math::Floor(timeOffset * sampleRate);								// the chart will be shiftet this many samples to the left
//	const double	pixelOffset			= sampleOffset * timeScale;											// the chart will be shiftet this many pixels to the left
//
//	// indices of the mostleft and mostright sample
//	double			maxSampleIndex		= channel->GetMaxSampleIndex() + sampleOffset;						
//	double			minSampleIndex		= Math::Ceil(maxSampleIndex - (1.0 / timeScale) * numPixels + sampleOffset);
//	
//	// recalculate the drawing area, including the offset
//	xEndPixel = xEndPixel - pixelOffset;
//
//	// if chart does not fill the area, calculate the real pixel coordinate of the leftmost sample
//	if (minSampleIndex < 0.0)
//	{
//		const uint32 numNonDrawableSamples = -1 * minSampleIndex;
//		const double xStartOffset = numNonDrawableSamples * timeScale; 
//		xStartPixel += xStartOffset;
//	}
//
//	numPixels = xEndPixel - xStartPixel;
//	
//	CORE_ASSERT(xStartPixel >= 0);
//	//CORE_ASSERT(xEndPixel < xEnd);
//
//	// clamp indices to a valid range (also rounds to an even number indices)
//	maxSampleIndex = Min<uint32>(maxSampleIndex, channel->GetMaxSampleIndex());	
//	minSampleIndex = Max<double>(minSampleIndex, channel->GetMinSampleIndex());
//	const uint32 numSamples	= maxSampleIndex - minSampleIndex;
//
//	// timerange of drawn samples
//	const double maxSampleTime = channel->GetSampleTime(maxSampleIndex);
//	const double minSampleTime = channel->GetSampleTime(minSampleIndex);
//	const double timeRange = maxSampleTime - minSampleTime;
//
//	// number of samples within a pixel
//	double samplesPerPixel = 0;
//	if (numPixels > 0)
//		samplesPerPixel = (sampleRate*timeRange) / numPixels;
//
//	// initialize some helper variables for the line rendering
//	double xPos, value;
//	//double lastPixel = Max<int32>( xStart, xStartPixel + (numPixels * ( channel->GetSampleTime(minSampleIndex) - minSampleTime) / (maxSampleTime - minSampleTime)) );
//	double lastPixel = xStartPixel;
//	int32 currentPixel;
//
//	// color range
//	Color color2 = FromQtColor( ToQColor(color).lighter(110) );
//	Color color1 = FromQtColor( ToQColor(color).darker(170) );
//	Color adjustedColor2;
//
//	// iterate through the samples (starting with the second sample) and draw one rect (of lines) for each sample
//	for (uint32 i=minSampleIndex; i<=maxSampleIndex; i++)
//	{
//		// calculate pixel coordinate from sample timestamp
//		const double sampleTime = channel->GetSampleTime(i);
//
//		// map time to x-coordinate
//		xPos = RemapRange(sampleTime, minSampleTime, maxSampleTime, xStartPixel,  xEndPixel);
//
//		// clamp and remap value to y coordinate
//		value = ClampedRemapRange( channel->GetSample(i), rangeMin, rangeMax, yStart, 0 );
//
//		// round to avoid some of the aliasing!?
//		value = (int32)(value + 0.375);
//
//		// calculate the color for the top point (value) of the line
//		float colorInterpolate = ClampedRemapRange( channel->GetSample(i), rangeMin, rangeMax, 0.0, 1.0 );
//		adjustedColor2 = LinearInterpolate<Color>( color1, color2, colorInterpolate );
//
//		currentPixel = xPos;
//
//		if (samplesPerPixel > 0.9)
//			callback->AddLine( xPos, yStart, color1, xPos, value, adjustedColor2 );
//		else
//		{
//			// draw a block of multiple lines
//			for (double p=lastPixel+1; p<=currentPixel; p+=1.0)
//				callback->AddLine( p, yStart, color1, p, value, adjustedColor2 );
//		}
//		
//		// store the current time and value for the next cycle
//		lastPixel = currentPixel;
//	}
//
//	// init lastTime/lastValue for drawing the first sample correctly
//	/*lastTime = xStartPixel + ((double)numPixels * ( channel->GetSampleTime(minSampleIndexRounded) - minSampleTime) / (maxSampleTime - minSampleTime));
//	lastValue  = channel->GetSample(minSampleIndexRounded) + yStart;
//
//	// iterate through the samples (starting with the second sample) and draw the vertices between them
//	for (uint32 i=minSampleIndexRounded+1; i<maxSampleIndex; i++)
//	{
//		// calculate pixel coordinate from sample timestamp
//		const double sampleTime = channel->GetSampleTime(i);
//		time = xStartPixel + ((double)numPixels * (sampleTime - minSampleTime) / (maxSampleTime - minSampleTime));
//		
//		value = ClampedRemapRange( channel->GetSample(i), rangeMin, rangeMax, yStart, 0 );
//
//		// draw line from the last sample to the current one
//		callback->AddLine( lastTime + 0.375, lastValue + 0.375, time + 0.375, value + 0.375 );
//
//		// store the current time and value for the next cycle
//		lastTime = time;
//		lastValue = value;
//	}
//
//	callback->RenderLines( color.lighter(200) );*/
//
//	// old test
//	/*double sampleX, sampleY;
//	const uint32 numSamples = channel->GetNumSamples();
//	for (uint32 i=1; i<numSamples; ++i)
//	{
//		sampleY = ClampedRemapRange( channel->GetSample(i), rangeMin, rangeMax, height, 0 );
//		sampleX = areaStartX+i;
//
//		AddLine( sampleX, height, sampleX, sampleY );
//	}*/
//
//	// render the lines
//	callback->RenderLines();
//}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Samples
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// get the sample size used buy the render functions (definition of size may vary.. most of the time its half sample size for reduced number of operations)
double OpenGLWidget2DHelpers::GetSampleSize(EChartRenderStyle style, double chartWidth, double chartHeight, double numSamples)
{
	// pixels per sample
	const double pps = chartWidth / numSamples;

	// select the size depending on 
	switch (style)
	{
		default:
		case BOX:		
		case LINE:
			// those samples have no size
			return 0; 

		case BAR:		// halfsize
			// clamp to <= 10% of width and <= 1/3 of samplespacing and >= 1 pixel
			return (pps > 1.0 ? Min(pps/3.0, chartWidth / 10.0) : 1.0);

		case LOLLIPOP:	// halfsize
			// sample with is <= 10% of height and <= 1/3 of samplespacing and >= 4 pixel
			return (pps > 2.0 ? Min(pps/3.0, chartHeight / 20.0) : 2.0);

		case CROSS:		// halfsize
			// sample width is <= 10% of height and <= 1/3 of samplespacing and >= 2 pixel
			return (pps > 1.0 ? Min(pps/3.0, chartHeight / 20.0) : 1.0);
	}
}

// selector for render functions
OpenGLWidget2DHelpers::RenderSampleFunction OpenGLWidget2DHelpers::SelectSampleRenderFunction(EChartRenderStyle style, bool clipped)
{
	if (clipped == true)
	{
		// select the clipping render functions
		switch (style)
		{
			default:
			case BOX:		return RenderBoxSampleClipped;
			case BAR:		return RenderBarSampleClipped;
			case LOLLIPOP:	return RenderLollipopSampleClipped;
			case CROSS:		return RenderCrossSampleClipped;
			case LINE:		return RenderLineSampleClipped;
		}
	}
	else
	{
		// select the normal render functions
		switch (style)
		{
			default:
			case BOX:		return RenderBoxSample;
			case BAR:		return RenderBarSample;
			case LOLLIPOP:	return RenderLollipopSample;
			case CROSS:		return RenderCrossSample;
			case LINE:		return RenderLineSample;
		}
	}
}


// render a filled area between the previous and the current sample
void OpenGLWidget2DHelpers::RenderBoxSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// use previous xPos to calc the pos of the next sample
	const double nextXpos = xPos + (xPos - previousXPos);

	// draw area
	const double xFrom = xPos;
	const double xTo = nextXpos;
	for (double x = xFrom; x <= xTo; x += 1.0)
		callback->AddLine(x, yStart, darkerColor, x, yPos, valueColor);
}


// render a filled area between the previous and the current sample, only draws on the draw area
void OpenGLWidget2DHelpers::RenderBoxSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// use previous xPos to calc the pos of the next sample
	const double nextXpos = xPos + (xPos - previousXPos);

	// sample is clipped
	if (nextXpos < xStart)
		return;

	// clip box area to drawable area
	const double xFrom = Clamp<double>(xPos, xStart, xEnd);
	const double xTo = Clamp<double>(nextXpos, xStart, xEnd);

	// draw area
	for (double x = xFrom; x <= xTo; x += 1.0)
		callback->AddLine(x, yStart, darkerColor, x, yPos, valueColor);
}


// render a bar of 1% chart width
void OpenGLWidget2DHelpers::RenderBarSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// area of the bar
	const double halfBarWidth = size / 2.0;
	const double xFrom =  xPos - halfBarWidth;
	const double xTo = xPos + halfBarWidth;

	// draw area
	for (double x=xFrom; x < xTo; x += 1.0)
		callback->AddLine(x, yStart, darkerColor, x, yPos, valueColor);
}


// render a bar of 1% chart width, only draws on the draw area
void OpenGLWidget2DHelpers::RenderBarSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// clipped area of the bar
	const double halfBarWidth = size / 2.0;

	// sample is clipped
	if (xPos + halfBarWidth < xStart)
		return;

	const double xFrom =  Clamp<double>(xPos - halfBarWidth, xStart, xEnd);
	const double xTo = Clamp<double>(xPos + halfBarWidth, xStart, xEnd);

	// draw area
	for (double x=xFrom; x < xTo; x += 1.0)
		callback->AddLine(x, yStart, darkerColor, x, yPos, valueColor);
}


// render a lollipop at the current position
void OpenGLWidget2DHelpers::RenderLollipopSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	callback->AddLine(xPos, yStart, darkerColor, xPos, yPos, valueColor);
	
	callback->AddLine(xPos - size, yPos - size, lighterColor, xPos + size, yPos + size, lighterColor);
	callback->AddLine(xPos + size, yPos - size, lighterColor, xPos - size, yPos + size, lighterColor);
}


// render a lollipop at the current position, only draws on the draw area
void OpenGLWidget2DHelpers::RenderLollipopSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// clip xpos
	if (xPos < xStart || xPos > xEnd)
		return;

	callback->AddLine(xPos, yStart, darkerColor, xPos, yPos, valueColor);
	
	// draw cross
	RenderCrossSample(callback, value, xPos, yPos, previousXPos, previousYPos, xStart, xEnd, yStart, yEnd, size, lighterColor, darkerColor, valueColor);
}



// render a dot with radius of 1% chartheight
void OpenGLWidget2DHelpers::RenderCrossSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// clip xpos
	if (xPos < xStart || xPos > xEnd)
		return;

	callback->AddLine(xPos - size, yPos - size, lighterColor, xPos + size, yPos + size, lighterColor);
	callback->AddLine(xPos + size, yPos - size, lighterColor, xPos - size, yPos + size, lighterColor);
}



// render a dot with radius of 1% chartheight, only draws on the draw area
void OpenGLWidget2DHelpers::RenderCrossSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// clip xpos
	if (xPos < xStart || xPos > xEnd)
		return;
	
	callback->AddLine(xPos - size, yPos - size, lighterColor, xPos + size, yPos + size, lighterColor);
	callback->AddLine(xPos + size, yPos - size, lighterColor, xPos - size, yPos + size, lighterColor);
}


// render a line between the previous and current sample
void OpenGLWidget2DHelpers::RenderLineSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	const double xFrom = previousXPos;
	const double xTo = xPos;
	const double yFrom = previousYPos;
	const double yTo = yPos;

	// draw the line 
	callback->AddLine(xFrom, yFrom, lighterColor, xTo, yTo, lighterColor);
}



// render a line between the previous and current sample, only draws on the draw area
void OpenGLWidget2DHelpers::RenderLineSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size,const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor)
{ 
	// clip xpos
	if (xPos < xStart || xPos > xEnd)
		return;

	const double xFrom = Clamp<double>(previousXPos, xStart, xEnd);
	const double xTo = Clamp<double>(xPos, xStart, xEnd);
	const double yFrom = previousYPos;
	const double yTo = yPos;

	// Correct yFrom coordinate because the xFrom could have been clamped (first sample). We cannot draw half of a line if it reaches outside of the drawing area.
	const double yDelta = yPos - previousYPos;						// y-delta between both samples
	const double xDelta = xPos - previousXPos;						// x-delta between both samples
	const double xFromClampDelta = previousXPos - xFrom;			// x-delta that was 'removed' due to the clamping
	const double xClampRatio = xFromClampDelta / xDelta;			// normalized x-delta
	const double yFromCorrected = yFrom - yDelta * xClampRatio;		// shift y coord an appropriate amount

	// draw the line 
	callback->AddLine(xFrom, yFromCorrected, lighterColor, xTo, yTo, lighterColor);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timeline
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OpenGLWidget2DHelpers::RenderTimeline(OpenGLWidgetCallback* callback, const Color& color, double timeRange, double maxTime, double x, double y, double width, double height, String& tempString)
{
	// parameters
	uint32 xStart			= x;
	double xEnd				= x+width;
	uint32 windowHeight		= height;

	// calc remaining parameters
	const int32 numPixels = xEnd - xStart;							// width of the chart area in pixels
		  double minTime = maxTime - timeRange;						// time of first pixel
	
	// div by zero check
	if (IsClose<double>(timeRange, 0.0, Math::epsilon) == true)
		return;

	// calculate the optimal distance between two time labels (in seconds)
	const double timeStampSpacing = 50; // require at least this many pixel between (the centers of) two labels
	double timeSteps =  Math::NiceNumberFloor( ((double)numPixels / timeStampSpacing) / timeRange); 

	// first and last rounded timestamp within range
	const double minTimeStamp = Math::Floor( minTime * timeSteps + 0.5) / timeSteps;
	const double maxTimeStamp = Math::Ceil( maxTime * timeSteps + 0.5) / timeSteps;
		
	// print all the numbers within range
	// render one more timestamp on the left and right, so the text scrolls in and out instead of popping up
	const double stepTime = 1.0 / timeSteps;	// dont get confused here :P
	for (double t=minTimeStamp; t <= maxTimeStamp; t+=stepTime)
	{
		double time = t;

		// fix negative rounding bug (0 is displayed as '-0')
		if (Math::AbsD(time) < 0.001)
			time = 0.0;
		
		// do not render negative times
		if (time < 0.0)
			continue;


		// find the x coordinate that corresponds to this time
		const int32 x = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// draw a time tick
		callback->AddLine( x+0.375, windowHeight-9, color, x+0.375, windowHeight-14, color );

		// draw the timestamp label
		tempString.Format( "%.1f", time);
		//double textWidth = callback->CalcTextWidth( tempString.AsChar() );

		if (x > 0)
		{
			callback->RenderText( tempString.AsChar(), callback->GetOpenGLWidget()->GetDefaultFontSize(), color, x+0.375, windowHeight-1, OpenGLWidget::ALIGN_BASELINE | OpenGLWidget::ALIGN_CENTER );
		}
	}

	callback->RenderLines();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Spectrum Rendering
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// render spectrum
void OpenGLWidget2DHelpers::RenderSpectrum(OpenGLWidgetCallback* callback, Spectrum* spectrum, const Color& color, uint32 minBinIndex, uint32 maxBinIndex, double rangeMin, double rangeMax, double xStart, double xEnd, double yStart, double yEnd)
{
	if (spectrum == NULL)
		return;

	const uint32 numBins = spectrum->GetNumBins();
	if (numBins < 2)
		return;

	const uint32 rangeBins = maxBinIndex - minBinIndex;

	//const double xRange = xEnd - xStart;
	//const double yRange = yEnd - yStart;

	for (uint32 i=1; i<=rangeBins; ++i)
	{
		const uint32 lastBinIndex	= minBinIndex + i - 1;
		const uint32 binIndex		= minBinIndex + i;

		const double lastX	= ClampedRemapRange( lastBinIndex, minBinIndex, maxBinIndex, xStart, xEnd );
		const double lastY	= ClampedRemapRange( spectrum->GetBin(lastBinIndex), rangeMin, rangeMax, yStart, yEnd );

		const double x		= ClampedRemapRange( binIndex, minBinIndex, maxBinIndex, xStart, xEnd );
		const double y		= ClampedRemapRange( spectrum->GetBin(binIndex), rangeMin, rangeMax, yStart, yEnd );

		callback->AddLine( lastX, lastY, color, x, y, color );
	}

	// render the lines
	const double lineWidth = 2.0;
	callback->RenderLines(lineWidth);
}


// render spectrum
void OpenGLWidget2DHelpers::RenderSpectrumChart(OpenGLWidgetCallback* callback, Spectrum* spectrum, const Color& color, uint32 minBinIndex, uint32 maxBinIndex, double rangeMin, double rangeMax, double xStart, double xEnd, double yStart, double yEnd)
{
	if (spectrum == NULL)
		return;

	// invalid range
	if (maxBinIndex <= minBinIndex)
		return;

	const uint32 numBins = maxBinIndex - minBinIndex;

	uint32 maxAvailableBinIndex = spectrum->GetNumBins() - 1;

	// legacy mode for spectrogram2d plugin
	if (maxBinIndex >= maxAvailableBinIndex)
		maxBinIndex = maxAvailableBinIndex;

	const uint32 rangeBins = maxBinIndex - minBinIndex;

	const double xRange = xEnd - xStart;
	const double yRange = yEnd - yStart;

	Color color2 = FromQtColor( ToQColor(color).lighter(110) );
	Color color1 = FromQtColor( ToQColor(color).darker(170) );
	Color adjustedColor2;

	const double binWidth = ((xRange)/(double)numBins)-1.0;

	for (uint32 i=1; i<=rangeBins; ++i)
	{
		const uint32 lastBinIndex	= minBinIndex + i - 1;
		//const uint32 binIndex		= minBinIndex + i;

		const double lastX = ClampedRemapRange(lastBinIndex, minBinIndex, maxBinIndex, xStart, xEnd);
		const double lastY = ClampedRemapRange(spectrum->GetBin(lastBinIndex), rangeMin, rangeMax, yStart, yEnd);

		//const double x		= ClampedRemapRange( binIndex, minBinIndex, endBinIndex, xStart, xEnd );
		//const double y		= ClampedRemapRange( spectrum->GetBin(binIndex), rangeMin, rangeMax, yStart, yEnd );

		float colorInterpolate = ClampedRemapRange(spectrum->GetBin(lastBinIndex), rangeMin, rangeMax, 0.0, 1.0);
		adjustedColor2 = LinearInterpolate<Color>( color1, color2, colorInterpolate );

		OpenGLWidgetCallback::Rect rect;

		rect.mColor1 = rect.mColor2 = color1;
		rect.mColor3 = rect.mColor4 = adjustedColor2;

		const double binHeight = yRange+lastY;

		if (binWidth > 1)
		{
			/* 
			const double xFrom = lastX;
			const double yFrom = yStart;
			const double xTo = lastX + binWidth;
			const double yTo = yStart + binHeight;

			// draw rectangle using a batch of lines
			for (double x = xFrom; x < xTo; x += 1.0)
				callback->AddLine( x, yFrom, color1, x, yTo, adjustedColor2 );
			*/

			// 1
			rect.mX1 = lastX;
			rect.mY1 = yStart;

			// 2
			rect.mX2 = lastX + binWidth;
			rect.mY2 = yStart;

			// 3
			rect.mX3 = lastX + binWidth;
			rect.mY3 = yStart + binHeight;

			// 4
			rect.mX4 = lastX;
			rect.mY4 = yStart + binHeight;

			callback->AddRect( rect );
		}
		else
			callback->AddLine( lastX, yStart, color1, lastX, yStart+binHeight, adjustedColor2 );
	}

	callback->RenderLines();
	callback->RenderRects();
}
