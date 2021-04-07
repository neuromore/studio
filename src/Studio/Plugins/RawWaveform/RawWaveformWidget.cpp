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
#include "RawWaveformWidget.h"
#include "RawWaveformPlugin.h"
#include <Core/LogManager.h>
#include <ColorPalette.h>
#include "../../Rendering/OpenGLWidget2DHelpers.h"
#include <QPainter>


using namespace Core;
extern uint mData_2[50];
int j = 4;
// constructor
RawWaveformWidget::RawWaveformWidget(RawWaveformPlugin* plugin, QWidget* parent) : OpenGLWidget(parent)
{
	// create the render callback
	mRenderCallback = new RenderCallback(this);
	SetCallback( mRenderCallback );

	mPlugin		= plugin;
	mEmptyText	= "No active device";
}


// destructor
RawWaveformWidget::~RawWaveformWidget()
{
	// destroy the render callback
	delete mRenderCallback;
}


// render frame
void RawWaveformWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	// get access to the active headset
	BciDevice* headset = NULL;
	Device* selectedDevice = mPlugin->GetChannelSelectionWidget()->GetSelectedDevice();
	if (selectedDevice != NULL && selectedDevice->GetBaseType() == BciDevice::BASE_TYPE_ID)
		headset = static_cast<BciDevice*>(selectedDevice);

	// render single view
	if (headset == NULL)
		RenderEmpty();
	else
		Render();

	// post rendering
	PostRendering();
}


// constructor
RawWaveformWidget::RenderCallback::RenderCallback(RawWaveformWidget* parent) : OpenGLWidgetCallback(parent)
{
	mParent			= parent;

	mLineWidth		= 1.1f;
	mTextColor		= ColorPalette::Shared::GetTextColor();
	mGridSubColor	= ColorPalette::Shared::GetDarkSubGridColor();
	mGridColor		= ColorPalette::Shared::GetDarkGridColor();
	mAxisColor		= ColorPalette::Shared::GetAxisColor();
}


// render callback
void RawWaveformWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{
	// get the time delta since the last render call
	//const double timeDelta = mParent->GetTimeDelta();

	// base class render
	OpenGLWidgetCallback::Render( index, isHighlighted, x, y, width, height );

	// draw background rect
	AddRect( 0, 0, width, height, ColorPalette::Shared::GetDarkBackgroundColor() );
	RenderRects();

	ChannelMultiSelectionWidget* channelSelectionWidget = mParent->mPlugin->GetChannelSelectionWidget();

	// get access to the active headset
	BciDevice* headset = NULL;
	Device* selectedDevice = channelSelectionWidget->GetSelectedDevice();
	if (selectedDevice != NULL && selectedDevice->GetBaseType() == BciDevice::BASE_TYPE_ID)
		headset = static_cast<BciDevice*>(selectedDevice);

	// render callback per sensor
	Render( headset, NULL, width, height);
}


// render callback
void RawWaveformWidget::RenderCallback::Render(BciDevice* headset, Sensor* sensor, double windowWidth, double windowHeight)
{
	if (headset == NULL || windowHeight == 0 || windowWidth == 0)
		return;

	ChannelMultiSelectionWidget* channelSelectionWidget = mParent->mPlugin->GetChannelSelectionWidget();

	const double	settingsTimeRange		= mParent->mPlugin->GetTimeRange();
	const bool		showVoltages			= mParent->mPlugin->GetShowVoltages();
	const bool		showTimes				= mParent->mPlugin->GetShowTimes();
	const bool		useAutoScale			= mParent->mPlugin->GetAutoScaleY();
	const double	settingsAmplitudeScale	= mParent->mPlugin->GetAmplitudeScale();
	const bool		showSignalName			= true;

	// get the number of visible sensors (number of checked checkboxes)
	const uint32 numVisibleSensors = channelSelectionWidget->GetNumSelectedChannels();

	// time or x-axis scaling
	//double sampleRate = headset->GetSampleRate();

	const double timeRange = settingsTimeRange;

	// get elapsed time from first sensors (assume they all have same elapsed time)
	double elapsedTime = 0;
	const uint32 numSensors = headset->GetNumSensors();
	if (numSensors > 0)
	{
		Sensor* sensor = headset->GetSensor(0);
		if (sensor != NULL)
			elapsedTime = sensor->GetChannel()->GetElapsedTime().InSeconds();
	}

	// reduce some from the usable height cause of the times text rendering at the bottom
	uint32 usableHeight = windowHeight;
	if (showTimes == true)
		usableHeight -= 15;

	// the vertical cell size for one wave
	double waveCellHeight;
	if (numVisibleSensors != 0)
		waveCellHeight = (double)usableHeight / numVisibleSensors;
	else
		waveCellHeight = usableHeight;

	// number of vertical divisions per cell
	const float numVerticalDivs = 6;

	// calculate some scaling values
	const float gridSpacing = waveCellHeight / numVerticalDivs;

	// get the current local cursor pos
	QPoint localCursorPos = mParent->mapFromGlobal( QCursor::pos() );

	// helper values to make sure we don't render the whole wave, but only what is visible
	const double xStart	= 0;
	const double xEnd	= windowWidth;

	// render the grid
	RenderGrid(numVisibleSensors, (uint32)numVerticalDivs, waveCellHeight, xStart, xEnd, windowHeight, mGridColor, mGridSubColor, timeRange, elapsedTime);

	// return directly after rendering the grid in case there is no neuro headset
	if (headset == NULL)
		return;

	// get the sensor index whose checkbox is on mouse overed
	const uint32 highlightedSensorIndex = channelSelectionWidget->GetHighlightedIndex();
	uint32 numActuallyRendered = 0;

	// get the number of sensors and iterate through them
	for (uint32 i = 0; i < numSensors; ++i)
	{
		// get the current sensor and skip it in case it is no valid neuro sensor
		Sensor* sensor = headset->GetSensor(i);
		if (sensor == NULL)
			continue;

		Channel<double>* channel = sensor->GetChannel();

		// check if we need to render the signal for the given sensor
		if (channelSelectionWidget->IsChannelSelected(channel) == false)
			continue;

		if (channel->IsEmpty() == true)
			continue;

		// calculate the row offset so that we don't render the waves all over each other but distribute them nicely over the screen
		double yOffset = numActuallyRendered * waveCellHeight + gridSpacing * numVerticalDivs / 2.0;
		numActuallyRendered++;

		// absolute value range that must fit in wave cell, calculated from units/division
		const double amplitudeScale = settingsAmplitudeScale * numVerticalDivs;


		// render the signal
		const double circleLeftShift = 10;
		RenderWave2D(channel, useAutoScale, amplitudeScale, timeRange, waveCellHeight, yOffset, xStart, xEnd - circleLeftShift, windowHeight);

		// highlight the wave in case:
		// 1. the checkbox is hovered
		// 2. the channel highlight flag is set
		// 3. the mouse is inside the corresponding wave cell
		if (highlightedSensorIndex == i || channel->IsHighlighted() == true ||
			(localCursorPos.x() >= 0 && localCursorPos.x() < windowWidth && (localCursorPos.y() > yOffset - 0.5f * waveCellHeight) && (localCursorPos.y() < yOffset + 0.5f * waveCellHeight)))
			RenderLines(2.0 * mLineWidth);
		else
			RenderLines(mLineWidth);

		// render signal name
		if (showSignalName == true)
		{ 
			// draw the value of the last sample
			RenderText( channel->GetName(), mParent->GetDefaultFontSize(), channel->GetColor(), 5, yOffset, OpenGLWidget::ALIGN_BOTTOM | OpenGLWidget::ALIGN_LEFT);
		}

		// render voltage
		if (showVoltages == true)
		{ 
			// draw the value of the last sample
			if (channel->GetNumSamples() > 0)
			{
				mTempString.Format("%.2f uV", channel->GetLastSample());
				RenderText( mTempString.AsChar(), mParent->GetDefaultFontSize(), mTextColor, windowWidth, yOffset, OpenGLWidget::ALIGN_BOTTOM | OpenGLWidget::ALIGN_RIGHT );
			}
		}
	}

	// render timing values
	if (showTimes == true)
		RenderTimeAxis(xEnd, xStart, windowHeight, elapsedTime, timeRange);
}


// render 2D wave for the given channel										   
void RawWaveformWidget::RenderCallback::RenderWave2D(Channel<double>* channel, bool useAutoScale, double amplitudeScale, double timeRange, double height, double yCenter, double xStart, double xEnd, double windowHeight)
{
	//const int32 xStartPixel = xStart;
	//const int32 xEndPixel = xEnd - 10; // TODO: because of the circle so that we see the latest sample being added at that point
	//const int32 numPixels = xEndPixel - xStartPixel;

	//// get the maximum valid sample index (equals the mostright pixel)
	//const double maxSampleIndex = channel->GetMaxSampleIndex();

	//// first sample index (can be negative and nonexistant)
	//double minSampleIndex = maxSampleIndex - (1.0 / timeRange) * numPixels;

	//// rounded sample indices for iteration
	//uint32 minSampleIndexRounded = minSampleIndex + 0.5;
	////const uint32 maxSampleIndexRounded = maxSampleIndex + 0.5;
	//const uint32 numSamples = maxSampleIndex - minSampleIndexRounded;

	//// timerange between first and last _visible_ sample
	//const double maxSampleTime = channel->GetSampleTime(maxSampleIndex);
	//double minSampleTime = -1.0;
	//if (minSampleIndex < 0)
	//{
	//	minSampleIndex = 0;
	//	minSampleIndexRounded = 0;

	//	minSampleTime = maxSampleTime - 1.0 / (channel->GetSampleRate() * timeRange / numPixels);
	//}
	//else
	//{
	//	minSampleTime = channel->GetSampleTime(minSampleIndexRounded);
	//}

	//// initialize some helper variables for the line rendering
	//double lastTime, lastValue, time, value;
	//lastTime = lastValue = 0.0;

	//// find max/min of all displayed values for scaling
	//double rawMin = DBL_MAX;
	//double rawMax = -DBL_MAX;
	//double mean = 0;
	//uint32 numVals = 0;
	//for (uint32 i=minSampleIndexRounded; i<maxSampleIndex; ++i)
	//{
	//	double* rawValue = channel->GetSampleRef(i);
	//	rawMin = (*rawValue) < rawMin ? (*rawValue) : rawMin;
	//	rawMax = (*rawValue) > rawMax ? (*rawValue) : rawMax;
	//	mean += (*rawValue);
	//	numVals++;
	//}
	//if (numVals != 0)
	//	mean /= numVals;

	//// calculate waveform scaling parmeters
	//if (useAutoScale == true)
	//	amplitudeScale = rawMax - rawMin;
	//	//amplitudeScale = fmax(abs(rawMax), abs(rawMin)) * 2; // <- for dc-offset-corrected values
	//
	//const double valueScale = height / amplitudeScale * (-1);


	//// render axis
	//AddLine( xStart + 0.375, yCenter + 0.375, mAxisColor, xEnd + 0.375, yCenter + 0.375, mAxisColor );


	//// get the color assigned to the given sensor and assign it
	//const Core::Color& color = channel->GetColor();

	//// clamp min sample index to 0 (may be negative)
	//minSampleIndexRounded = Core::Max<double>(minSampleIndexRounded, 0);

	//// init lastTime/lastValue for drawing the first sample correctly
	//lastTime = xStartPixel + ((double)numPixels * ( channel->GetSampleTime(minSampleIndexRounded) - minSampleTime) / (maxSampleTime - minSampleTime));
	//lastValue  = valueScale * (channel->GetSample(minSampleIndexRounded) - mean) + yCenter;

	//// iterate through the samples (starting with the second sample) and draw the vertices between them
	//for (uint32 i = minSampleIndexRounded + 1; i <maxSampleIndex; i++)
	//{
	//	// calculate pixel coordinate from sample timestamp
	//	const double sampleTime = channel->GetSampleTime(i);
	//	time = xStartPixel + ((double)numPixels * (sampleTime - minSampleTime) / (maxSampleTime - minSampleTime));
	//	value = valueScale * (channel->GetSample(i) - mean) + yCenter;

	//	AddLine( lastTime + 0.375, lastValue + 0.375, color, time + 0.375, value + 0.375, color );

	//	// store the current time and value for the next cycle
	//	lastTime = time;
	//	lastValue = value;
	//}

	//// draw a circle indicator on the newest sample
	//const double circleSize = 3.0;
	//Render2DCircle(xEndPixel, lastValue, circleSize, 32, color);

	//// render time value from the last sample
	////mTempString.Format( "%.1f sec", rawTime );
	////RenderText( mTempString, ToQColor(sensorColor), QPoint( (windowWidth - 81), textPosY ) );

	// draw only if the parameters are valid 
	if (channel == NULL || xEnd < xStart)
		return;

	// FIXME drawing should also work with a single sample (e.g. very low sample rate)
	if (channel->GetNumSamples() == 0)
		return;

	//const double sampleRate = channel->GetSampleRate();

	// calc remaining parameters
	const uint32	maxSampleIndex	= channel->GetMaxSampleIndex();			// index of last sample

	const double	maxTime			= channel->GetElapsedTime().InSeconds();
	//const int32		numPixels		= xEnd - xStart;							// width of the chart area in pixels

	double			minTime			= maxTime - timeRange;						// time of first pixel
	uint32			minSampleIndex	= channel->FindIndexByTime(minTime);		// index of first sample (clamped to valid index and rounded downwards -> may lie outside of left border)

	CORE_ASSERT(maxSampleIndex >= minSampleIndex);

	// find max/min of all displayed values for scaling
	double rawMin = DBL_MAX;
	double rawMax = -DBL_MAX;
	double mean = 0;
	uint32 numVals = 0;
	for (uint32 i=minSampleIndex; i<=maxSampleIndex; ++i)
	{
		double* rawValue = channel->GetSampleRef(i);
		rawMin = (*rawValue) < rawMin ? (*rawValue) : rawMin;
		rawMax = (*rawValue) > rawMax ? (*rawValue) : rawMax;
		mean += (*rawValue);
		numVals++;
	}
	if (numVals != 0)
		mean /= numVals;

	// calculate waveform scaling parmeters
	if (useAutoScale == true)
		amplitudeScale = rawMax - rawMin;
		//amplitudeScale = fmax(abs(rawMax), abs(rawMin)) * 2; // <- replace above line with this for dc-offset-corrected values
	
	// handle zero amplitude scale (happens if all displayed values are the same)
	if (amplitudeScale < Math::epsilon)
	{
		// use arbitrary scale (does not matter as long it is not near 0)
		amplitudeScale = 1;
	}

	const double valueScale = (double)height / amplitudeScale * (-1);		// invert due to render coordinates

	//
	// Render
	//

	// initialize helper variables
	double x, y;			// current datapoint as pixel coordinates
	double value, time;		// current datapoint as value/time
	double previousX, previousY;

	// color range
	const Color& color = channel->GetColor();
	Color lighterColor = FromQtColor( ToQColor(color).lighter(110) );
	Color darkerColor = FromQtColor( ToQColor(color).darker(170) );
	Color previousColor, valueColor;		// color of the pixel at the value position, interpolated between lighter/darker color

	//// render axis
	AddLine( xStart + 0.375, yCenter + 0.375, mAxisColor, xEnd + 0.375, yCenter + 0.375, mAxisColor );
	
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
		previousY = yCenter + valueScale * (value - mean);

		// calculate the color for the top point (value) of the line
		float normalizedValue = ClampedRemapRange( value, rawMin, rawMax, 0.0, 1.0 );
		previousColor = LinearInterpolate<Color>( darkerColor, lighterColor, normalizedValue );

		// FIX due to antialiasing problems : round the coords to int and add the twiddle factor
		//previousX = (int32)previousX + 0.375;
		previousY = (int32)previousY + 0.375;
	}

	//////////////////////////////////////////////////////////////////
	// 1) draw all middle samples
	for (uint32 i = minSampleIndex; i <= maxSampleIndex; i++)
	{
		// get time of sample
		time = channel->GetSampleTime(i).InSeconds();

		// map time to pixel x-coordinate
		x = RemapRange(time, minTime, maxTime, xStart,  xEnd);

		// clamp and remap value to y coordinate

		value = int(((mData_2[j++] << 16 | mData_2[j++] << 8 | mData_2[j++]) << 8) >> 8);/*channel->GetSample(i);*/
		if (j > 27)
			j = 0;
		y = yCenter + valueScale * (value - mean);

		// FIX due to antialiasing problems : round the coords to int and add the twiddle factor
		//x = (int32)x + 0.375;
		y = (int32)y + 0.375;

		// calculate the color for the top point (value) of the line
		float normalizedValue = ClampedRemapRange( value, rawMin, rawMax, 0.0, 1.0 );
		valueColor = LinearInterpolate<Color>( darkerColor, lighterColor, normalizedValue );

		// draw the sample
		AddLine( previousX, previousY, previousColor, x, y, valueColor );
	
		// store the current x and y pos for the next cycle
		previousX		= x;
		previousY		= y;
		previousColor	= valueColor;
	}

	//// draw a circle indicator on the newest sample
	const double circleSize = 3.0;
	Render2DCircle(previousX, previousY, circleSize, 32, color);

	//// render time value from the last sample
	////mTempString.Format( "%.1f sec", rawTime );
	////RenderText( mTempString, ToQColor(sensorColor), QPoint( (windowWidth - 81), textPosY ) );
}


void RawWaveformWidget::RenderCallback::RenderTimeAxis(uint32 xEnd, uint32 xStart, uint32 windowHeight, double maxTime, double timeRange)
{
	OpenGLWidget2DHelpers::RenderTimeline( this, mTextColor, timeRange, maxTime, xStart, 0, xEnd - xStart, windowHeight, mTempString);
}


// NOTE much code was copied from RenderTimeAxis
void RawWaveformWidget::RenderCallback::RenderGrid(uint32 numSensors, uint32 numVerticalDivs, uint32 waveCellHeight, uint32 xStart, uint32 xEnd, uint32 windowHeight, const Color& gridColor, const Color& gridSubColor, double timeRange, double maxTime)
{
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

	// draw subdivision vertical grid lines first
	const double stepTime = 1.0 / timeSteps;	// dont get confused here :P
	const double stepTimeSub = stepTime / 10;	// dont get confused here :P
	for (double t=minTimeStamp-stepTime; t <= maxTimeStamp+stepTime; t+=stepTimeSub)
	{
		// find the x coordinate that corresponds to this time
		// NOTE: x-10 due to offset in RenderWave2D (dirty solution)
		//const double x = (t-minTime) * ((double)numPixels / timeRange) - 10.0;
		
		// find the x coordinate that corresponds to this time
		const double x = RemapRange(t, minTime, maxTime, xStart,  xEnd);
		AddLine(x, 0, gridSubColor, x, windowHeight, gridSubColor);
	}


	// draw main vertical grid lines
	for (double t=minTimeStamp-stepTime; t <= maxTimeStamp+stepTime; t+=stepTime)
	{
		// find the x coordinate that corresponds to this time
		// NOTE: x-10 due to offset in RenderWave2D (dirty solution)
		//const double x = (t-minTime) * ((double)numPixels / timeRange) - 10.0;
		const double x = RemapRange(t, minTime, maxTime, xStart,  xEnd);

		AddLine(x, 0, gridColor, x, windowHeight, gridColor);
	}


	// draw horizontal grid lines
	for (uint32 s=0; s<=numSensors; s++)
	{
		const double offset = s*waveCellHeight;
		for (uint32 i=0; i<=numVerticalDivs; i++)
		{
			const double y = offset + (double)waveCellHeight / numVerticalDivs * (double)i;

			AddLine(xStart, y, gridColor, xEnd, y, gridColor);
		}
	}

	RenderLines();
}


void RawWaveformWidget::RenderCallback::Render2DCircle(double posX, double posY, double radius, uint32 numSteps, const Color& color)
{
	const double maxAngle = 2.0*Math::pi;
	const double stepSize = maxAngle / numSteps;

	for (double angle=0.0; angle<=maxAngle; angle+=stepSize)
	{
		const double startX = posX + sin(angle) * radius;
		const double startY = posY + cos(angle) * radius;

		const double endX = posX + sin(angle*Math::halfPi) * radius;
		const double endY = posY + cos(angle*Math::halfPi) * radius;

		AddLine( startX, startY, color, endX, endY, color );
	}
}
