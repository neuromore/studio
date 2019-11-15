/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "WindowFunctionWidget.h"
#include <Core/LogManager.h>
#include <DSP/Spectrum.h>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>


using namespace Core;

// constructor
WindowFunctionWidget::WindowFunctionWidget(WindowFunction* windowFunction, Domain domain, QWidget* parent) : QWidget(parent)
{
	mWindowFunction = windowFunction;
	mDomain			= domain;

	setMinimumHeight( 186 );
}


// destructor
WindowFunctionWidget::~WindowFunctionWidget()
{
}


// render the background grid
void WindowFunctionWidget::RenderGridBackground(QPainter& painter, QPoint upperLeft, QPoint lowerRight, int32 spacing, int32 numSubGridLinesPerCell)
{
	// grid line color
	painter.setPen( QColor(40, 40, 40) );

	// calculate the start and end ranges in 'scrolled and zoomed out' coordinates
	// we need to render sub-grids covering that area
	const int32 startX		= upperLeft.x();
	const int32 startY		= upperLeft.y();
	const int32 endX		= lowerRight.x();
	const int32 endY		= lowerRight.y();
	const int32 width		= endX - startX;
	const int32 height		= endY - startY;
	const int32 numHLines	= height / spacing;
	const int32 numVLines	= width / spacing;

	QPen gridPen, subgridPen;
	gridPen.setColor( QColor(58, 58, 58) );
	subgridPen.setColor( QColor(55, 55, 55) );

	// draw subgridlines first
	painter.setPen( subgridPen );

	// draw vertical lines
	for (int32 i=0; i<=numVLines; ++i)
	{
		if (i % numSubGridLinesPerCell == 0)
			continue;

		const int32 x = startX + i * spacing;
		painter.drawLine(x, startY, x, endY);
	}

	// draw horizontal lines
	for (int32 i=0; i<=numHLines; ++i)
	{
		if (i % numSubGridLinesPerCell == 0)
			continue;

		const int32 y = startY + i * spacing;
		painter.drawLine(startX, y, endX, y);
	}

	// draw render grid lines
	painter.setPen( gridPen );

	// draw vertical lines
	for (int32 i=0; i<=numVLines; ++i)
	{
		if (i % numSubGridLinesPerCell != 0)
			continue;

		const int32 x = startX + i * spacing;
		painter.drawLine(x, startY, x, endY);
	}

	// draw horizontal lines
	for (int32 i=0; i<=numHLines; ++i)
	{
		if (i % numSubGridLinesPerCell != 0)
			continue;

		const int32 y = startY + i * spacing;
		painter.drawLine(startX, y, endX, y);
	}
}


// overloaded paint event
void WindowFunctionWidget::paintEvent(QPaintEvent* event)
{
	//return;
	// get the rect of the widget
	QRect rect = this->rect();
	const uint32 widgetWidth = rect.width();
	const uint32 widgetHeight = rect.height();

	// init the painter
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.resetTransform();

	// paint the background using the background color
	QColor bgColor = QColor(48, 48, 48);
	painter.setPen( bgColor );
	painter.setBrush( bgColor );
	painter.drawRect( 0, 0, widgetWidth, widgetHeight );

	const int32 spacing			= 10;
	QPoint upperLeft			= QPoint( spacing*3-4, 13 );
	QPoint lowerRight			= QPoint( widgetWidth - spacing/2, widgetHeight - 13 );

	const int32 renderStartX	= upperLeft.x();
	const int32 renderStartY	= upperLeft.y();
	const int32 renderEndX		= lowerRight.x();
	const int32 renderEndY		= lowerRight.y();
	const int32 renderWidth		= renderEndX - renderStartX;
	const int32 renderHeight	= renderEndY - renderStartY;
	const int32 halfRenderWidth = renderWidth / 2;
	const int32 halfRenderHeight = renderHeight / 2;

	const int32 subSections		= 5;
	const int32 numSubLines		= 4;
	const int32 gridSpacing		= renderHeight / (subSections * numSubLines);

	// render the grid
	RenderGridBackground( painter, upperLeft, lowerRight, gridSpacing, numSubLines );
	painter.setPen( QColor(200, 200, 200) );
	int32 lastX, lastY;

	// needed for frequency domain
	const double minDecibels	= -120.0;
	const double maxDecibels	=    0.0;
    const double decibelRange	= Math::AbsD( maxDecibels - minDecibels );
	const int32 numBins         = 128; // the number of samples used to draw the frequency response
	
	////////////////////////////////////////////////////////////////////
	// Time domain
	////////////////////////////////////////////////////////////////////
	if (mDomain == DOMAIN_TIME)
	{
		// render the window function itself on top of the grid
		for (int32 i=0; i<renderWidth; ++i)
		{
			// evaluate the window function
			const int32 x = renderStartX + i;
			const int32 y = renderStartY + renderHeight + mWindowFunction->Evaluate( i, renderWidth ) * -renderHeight;

			// make sure the last x and y values are valid, even when in the first loop
			if (i == 0)
			{
				lastX = x;
				lastY = y;
			}

			// render a line per pixel
			painter.drawLine(lastX, lastY, x, y);

			// remember the last evaluated point for the next loop
			lastX = x;
			lastY = y;
		}
	}
	////////////////////////////////////////////////////////////////////
	// Frequency domain
	////////////////////////////////////////////////////////////////////
	else
	{
		painter.setBrush( QColor(255, 255, 0) );

		// initialize the fast fourier transform
		mFFT.Init(numBins);
		double* inputData = mFFT.GetInput();

		// evaluate the window function and use it as input data for the FFT
		for (uint32 i=0; i<numBins; ++i)
			inputData[i] = mWindowFunction->Evaluate( i, numBins );

		// perform forward FFT
		mFFT.CalcFFT();
		Complex* complexSpectrum = mFFT.GetOutput();

		//LogError("-------------------------------------------------------------------------------");

		const double deltaPixelStepPerBin = (double)renderWidth / (double)numBins;
		for (int32 i=0; i<numBins; i+= 1)
		{
			const double frequency			= complexSpectrum[i].Norm();
			const double frequencyDecibels	= Clamp<double>( Spectrum::GetFrequencyDecibels(frequency), minDecibels, maxDecibels );
			const double normalizedValue	= (frequencyDecibels - minDecibels) / decibelRange;

			//const double normalizedValue	= inputData[sampleIndex].Magnitude();

			//LogError( "sampleIndex=%i, value=%f", sampleIndex,normalizedValue  );

			// evaluate the window function
			const int32 x = renderStartX + (double)i * deltaPixelStepPerBin;
			const int32 y = renderStartY + renderHeight + normalizedValue * -renderHeight;

			// make sure the last x and y values are valid, even when in the first loop
			if (i == 0)
			{
				lastX = x;
				lastY = y;
			}

			// render a line per pixel
			painter.drawLine(lastX, lastY, x, y);
			/*const qreal lobeWidth	= normalizedValue * 10.0;
			const qreal lobeHeight	= normalizedValue * renderHeight;
			const qreal lobeCenterX	= x;
			const qreal lobeCenterY	= renderEndY;
			QPointF lobeCenter(lobeCenterX, lobeCenterY);
			painter.drawEllipse(lobeCenter, lobeWidth*0.5, lobeHeight*0.5 );*/

			// remember the last evaluated point for the next loop
			lastX = x;
			lastY = y;
		}
	}

	////////////////////////////////////////////////////////////////////
	// y axis
	////////////////////////////////////////////////////////////////////
	QFont font;
	int32 fontPixelSize = 8;
	font.setPixelSize(fontPixelSize);
	painter.setFont( font );

	if (mDomain == DOMAIN_TIME)
	{
		// draw amplitude scaling
		for (uint32 i=0; i<=subSections; ++i)
		{
			const int32 y = renderStartY + i * numSubLines * gridSpacing;
			const double normalizedValue = (subSections-i) * (1.0f/(float)subSections);
			mTempString.Format( "%.1f", normalizedValue );
			painter.drawText( renderStartX-12, y+(fontPixelSize/2), mTempString.AsChar() );
		}
	}
	if (mDomain == DOMAIN_FREQUENCY)
	{
		// draw decibels scaling
		for (uint32 i=0; i<=subSections; ++i)
		{
			const int32 y = renderStartY + i * numSubLines * gridSpacing;
			const double normalizedValue = (subSections-i) * (1.0f/(float)subSections);
			mTempString.Format( "%.0f", normalizedValue * decibelRange + minDecibels );
			painter.drawText( renderStartX-12, y+(fontPixelSize/2), mTempString.AsChar() );
		}
	}


	////////////////////////////////////////////////////////////////////
	// y axis
	////////////////////////////////////////////////////////////////////
	if (mDomain == DOMAIN_TIME)
	{
		// render start and end sample text
		painter.drawText( renderStartX, renderEndY+fontPixelSize, "0" );
		painter.drawText( renderEndX-(fontPixelSize*3)/2+1, renderEndY+fontPixelSize, "N-1" );
	}
	if (mDomain == DOMAIN_FREQUENCY)
	{
		// render start and end sample text
		painter.drawText( renderStartX, renderEndY+fontPixelSize, "-64" );
		painter.drawText( renderEndX-(fontPixelSize*2)/2+1, renderEndY+fontPixelSize, "64" );
	}

	fontPixelSize = 10;
	font.setPixelSize(fontPixelSize);
	painter.setFont( font );
	QFontMetrics fontMetrics(font);

	// render the top and bottom descriptions
	if (mDomain == DOMAIN_TIME)			painter.drawText( renderStartX + halfRenderWidth-fontMetrics.width("Time Domain")/2, fontPixelSize, "Time Domain" );
	if (mDomain == DOMAIN_FREQUENCY)	painter.drawText( renderStartX + halfRenderWidth-fontMetrics.width("Frequency Domain")/2, fontPixelSize, "Frequency Domain" );
	if (mDomain == DOMAIN_TIME)			painter.drawText( renderStartX + halfRenderWidth-fontMetrics.width("Samples")/2, renderEndY+fontPixelSize, "Samples" );
	if (mDomain == DOMAIN_FREQUENCY)	painter.drawText( renderStartX + halfRenderWidth-fontMetrics.width("Bins")/2, renderEndY+fontPixelSize, "Bins" );

	if (mDomain == DOMAIN_FREQUENCY)	painter.drawText( renderStartX/2, renderEndY/2, "DOES NOT WORK GOOD YET" );

	// render amplitude text rotated
	painter.rotate( -90.0 );
	if (mDomain == DOMAIN_TIME)			painter.drawText( -renderStartY-halfRenderHeight-fontMetrics.width("Amplitude")/2, fontPixelSize, "Amplitude" );
	if (mDomain == DOMAIN_FREQUENCY)	painter.drawText( -renderStartY-halfRenderHeight-fontMetrics.width("Decibels")/2, fontPixelSize, "Decibels" );
}
