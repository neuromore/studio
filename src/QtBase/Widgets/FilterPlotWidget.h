/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */
/*
#ifndef __NEUROMORE_FILTERPLOTWIDGET_H
#define __NEUROMORE_FILTERPLOTWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <DSP/Filter.h>
#include "PlotWidget.h"

class QTBASE_API FilterPlotWidget : public PlotWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		FilterPlotWidget(QWidget* parent=NULL);
		virtual ~FilterPlotWidget();

		void ReInit(const Filter::FilterSettings& filter);

		virtual void mousePressEvent ( QMouseEvent * event ) override;

	private:
		// the function to plot
		double EvaluateAt(double x) override final;

		// axis range 
		double GetXMin() override final;
		double GetXMax() override final;
		double GetYMin() override final;
		double GetYMax() override final;

		// axis (linear by default) // TODO define direction: display position to func value or reverse?
		double MapXAxis(double value) override final	{ return value; }
		double MapYAxis(double value) override final	{ return value; }    // TODO use this to display y axis as decibel

		// axis labels
		const char* GetXAxisLabel() override final		{ return "Frequency (Hz)"; }
		const char* GetYAxisLabel() override final		{ return (mShowDecibel ? "Magnitude (dB)" : "Magnitude (linear)"); }

		void paintEvent(QPaintEvent* event);

		Filter*			mFilter;
		Core::String	mTempString;
		bool			mShowDecibel;
};


#endif
*/