/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_PLOTWIDGET_H
#define __NEUROMORE_PLOTWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <QWidget>


class QTBASE_API PlotWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		PlotWidget(QWidget* parent=NULL);
		virtual ~PlotWidget();


	protected:
		void RenderGridBackground(QPainter& painter, QPoint upperLeft, QPoint lowerRight, int32 spacing, int32 numSubGridLinesPerCell);
		// TODO also add functions for grid, axis, labels, functionplot
	private:
		//void paintEvent(QPaintEvent* event);


		// the function to plot
		virtual double EvaluateAt(double x) = 0;

		// axis range
		virtual double GetXMin() = 0;
		virtual double GetXMax() = 0;
		virtual double GetYMin() = 0;
		virtual double GetYMax() = 0;

		// axis (linear by default) // TODO define direction: display position to func value or reverse?
		virtual double MapXAxis(double value)		{ return value; }
		virtual double MapYAxis(double value)		{ return value; }

		// axis labels
		virtual const char* GetXAxisLabel() = 0;
		virtual const char* GetYAxisLabel() = 0;

		Core::String	mTempString;
};


#endif
