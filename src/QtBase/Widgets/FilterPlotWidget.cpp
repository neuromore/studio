/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */
/*
// include required headers
#include "FilterPlotWidget.h"
#include <Core/LogManager.h>
#include <DSP/Filter.h>
#include <QPainter>
#include <QPen>


using namespace Core;

// constructor
FilterPlotWidget::FilterPlotWidget(QWidget* parent) : PlotWidget(parent)
{
	mFilter = NULL;
	mShowDecibel = false;
}


// destructor
FilterPlotWidget::~FilterPlotWidget()
{
	delete mFilter;
}


void FilterPlotWidget::mousePressEvent ( QMouseEvent * event )
{
	mShowDecibel = !mShowDecibel;
	update();
}


void FilterPlotWidget::ReInit(const Filter::FilterSettings& filter)
{
	// dealloc the previous filter
	delete mFilter;

	// try to create filter from the settings
	mFilter = Filter::Create(filter);

	// TODO cause a redraw
	update();
}


double FilterPlotWidget::EvaluateAt(double x)
{
	if (mFilter == NULL)
		return 0;

	if (mShowDecibel)
		return mFilter->GetMagnitudeDecibel(x);
	else
		return mFilter->GetMagnitude(x);
}



double FilterPlotWidget::GetXMin()
{ 
	if (mFilter == NULL)
		return 0;

	const double cutFreq = (mFilter->GetSettings().mConfiguration == Filter::HIGHPASS ?  mFilter->GetSettings().mHighCutFrequency : mFilter->GetSettings().mLowCutFrequency);
	const double margin = 5; // 5 Hz
	return  Core::Clamp(cutFreq - margin , 0.0, 100000.0); 
}


double FilterPlotWidget::GetXMax() 	
{ 
	if (mFilter == NULL)
		return 64;

	const double cutFreq = (mFilter->GetSettings().mConfiguration == Filter::LOWPASS ?  mFilter->GetSettings().mLowCutFrequency : mFilter->GetSettings().mHighCutFrequency);
	const double margin = 5; // 5 Hz
	return  Core::Clamp(cutFreq + margin, 0.0, 100000.0); 
}


double FilterPlotWidget::GetYMin()
{ 
	if (mShowDecibel == true)
	{
		const Filter::EFilterType type = mFilter->GetSettings().mType;
		const bool showAttenuation = (type == Filter::BUTTERWORTH || type == Filter::CHEBYSHEVII || type == Filter::ELLIPTIC );
		
		// use at least -60db but allow for higher attenuations
		if (showAttenuation == true)
			return Min(-60.0, -mFilter->GetSettings().mAttenuation - 10.0);
		else
			return -60.0;
	}
	else
	{
		return 0.0;
	}
}	


double FilterPlotWidget::GetYMax() 	
{ 
	return (mShowDecibel ? 2.0 : 1.0);  // dB max = +2db
}		



// overloaded paint event
void FilterPlotWidget::paintEvent(QPaintEvent* event)
{

	// PART 1: general plot code (belongs into PlotWidget but has to be split up first)

	// get the rect of the widget
	QRect rect = this->rect();
	const uint32 widgetWidth = rect.width();
	const uint32 widgetHeight = rect.height();

	// init the painter
	QPainter painter(this);
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
	const int32 oversampling = 20; // increase oversampling to display more details (peaks get lost)

	const int32 subSectionsX	= 4;
	const int32 subSectionsY	= 8;
	const int32 numSubLines		= 2;
	const int32 gridSpacingX	= renderWidth / (subSectionsX * numSubLines);
	const int32 gridSpacingY	= renderHeight / (subSectionsY * numSubLines);

	// GRID
	RenderGridBackground( painter, upperLeft, lowerRight, gridSpacingY, numSubLines );
	painter.setPen( QColor(200, 200, 200) );
	
	// FUNCTION GRAPH
	int32 lastX, lastY;
	for (int32 i=0; i<renderWidth * oversampling; ++i)
	{
		// get the x position
		const uint32 xPix = i/(double)oversampling + renderStartX;						// pixel space
		const double xNorm = (double)i / (double)renderWidth / (double)oversampling;	// union space
		const double xFunc = ClampedRemapRange(xNorm, 0.0, 1.0, GetXMin(), GetXMax());	// function space // TODO add x mapping function here

		// evaluate
		const double yFunc = EvaluateAt(xFunc);											// function space

		// convert to pixel space again
		const double yNorm = ClampedRemapRange (yFunc, GetYMin(), GetYMax(), 0.0, 1.0);	// union space
		const double yPix = renderStartY + renderHeight - yNorm * renderHeight;			// pixel space // TODO check direction / sign

		// first iteration is special, we can only draw between two points
		if (i > 0)
			painter.drawLine(lastX, lastY, xPix, yPix);

		// remember the last point for the next line
		lastX = xPix;
		lastY = yPix;
	}
	
	// AXIS VALUES
	QFont font;
	QFontMetrics fontMetrics(font);
	int32 fontPixelSize = 8;
	font.setPixelSize(fontPixelSize);
	painter.setFont( font );

	// HACK to display values correctly
	const bool useDecimalPointX = (GetXMax() - GetXMin() < 1.1);

	// draw x axis values
	for (uint32 i=0; i<=subSectionsX; ++i)
	{
		const int32 x = i * numSubLines * gridSpacingX;
		const double normalizedValue = i * (1.0f/(double)subSectionsX);
		const double displayValue = ClampedRemapRange(normalizedValue, 0.0, 1.0, GetXMin(), GetXMax());

		if (useDecimalPointX)
			mTempString.Format( "%.2f", displayValue );
		else
			mTempString.Format( "%.1f", displayValue );

		painter.drawText( renderStartX + x - fontMetrics.width(mTempString.AsChar())/2 , renderStartY+renderHeight+12, mTempString.AsChar() );
	}

	// HACK to display values correctly
	const bool useDecimalPointY = (GetYMax() - GetYMin() < 1.1);

	// draw y axis values
	for (uint32 i=0; i<=subSectionsY; ++i)
	{
		const int32 y = i * numSubLines * gridSpacingY;
		const double normalizedValue = (subSectionsY-i) * (1.0f/(double)subSectionsY);
		const double displayValue = ClampedRemapRange(normalizedValue, 0.0, 1.0, GetYMin(), GetYMax());
		
		if (useDecimalPointY)
			mTempString.Format( "%.2f", displayValue );
		else
			mTempString.Format( "%.1f", displayValue );

		painter.drawText( renderStartX-12, renderStartY + y + (fontPixelSize/2), mTempString.AsChar() );
	}

	// AXIS LABELS
	fontPixelSize = 10;
	font.setPixelSize(fontPixelSize);
	painter.setFont( font );
	painter.setPen( QColor(120, 120, 120) );

	// render x axis label
	const char* xAxisLabel = GetXAxisLabel();
	painter.drawText( renderStartX+halfRenderWidth - fontMetrics.width(xAxisLabel)/2, fontPixelSize, xAxisLabel );

	// render y axis label rotated
	painter.rotate( -90.0 );
	const char* yAxisLabel = GetYAxisLabel();
	painter.drawText( -renderStartY-halfRenderHeight-fontMetrics.width(yAxisLabel)/2, fontPixelSize, yAxisLabel);
	painter.rotate( 90.0 );
	
	
	// FILTER SPECIFIC: draw corner frequencies and attenuation
	const Filter::FilterSettings& filter = mFilter->GetSettings();

	// dashed line
	
	QPen dashedPen = QPen(QColor(120, 120, 120));
	dashedPen.setCapStyle( Qt::PenCapStyle::FlatCap );
	dashedPen.setStyle (Qt::PenStyle::DashLine);
	painter.setPen(dashedPen);

	// render low frequency bar for lowpass/bandpass/bandstop
	if (filter.mConfiguration != Filter::HIGHPASS) 
	{
		const double lowCutX = ClampedRemapRange(filter.mLowCutFrequency, GetXMin(), GetXMax(), renderStartX, renderStartX + renderWidth);
		painter.drawLine(lowCutX, renderStartY, lowCutX, renderStartY + renderHeight);
	}

	// render high frequency bar for highpass/bandpass/bandstop
	if (filter.mConfiguration != Filter::LOWPASS)
	{
		const double highCutX = ClampedRemapRange(filter.mHighCutFrequency, GetXMin(), GetXMax(), renderStartX, renderStartX + renderWidth);
		painter.drawLine(highCutX, renderStartY, highCutX, renderStartY + renderHeight);
	}

	// render attenuation as horizontal line
	if (filter.mType == Filter::BUTTERWORTH || filter.mType == Filter::CHEBYSHEVII || filter.mType == Filter::ELLIPTIC)
	{
		double attenuation = -filter.mAttenuation;
	
		// if y axis is linear: convert dB
		if (mShowDecibel == false)
			attenuation = Math::PowD (10.0, attenuation / 20.0);
		
		const double yNorm = ClampedRemapRange (attenuation, GetYMin(), GetYMax(), 0.0, 1.0);	// union space
		const double yPix = renderStartY + renderHeight - yNorm * renderHeight;					// pixel space // TODO check direction / sign

		painter.drawLine(renderStartX, yPix, renderStartX + renderWidth, yPix);
	}

}
*/