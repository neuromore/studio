/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_WINDOWFUNCTIONWIDGET_H
#define __NEUROMORE_WINDOWFUNCTIONWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <DSP/WindowFunction.h>
#include <DSP/FFT.h>
#include <QWidget>


class QTBASE_API WindowFunctionWidget : public QWidget
{
	Q_OBJECT
	public:
		enum Domain
		{
			DOMAIN_TIME			= 0,
			DOMAIN_FREQUENCY	= 1
		};

		// constructor & destructor
		WindowFunctionWidget(WindowFunction* windowFunction, Domain domain=DOMAIN_TIME, QWidget* parent=NULL);
		virtual ~WindowFunctionWidget();

	private:
		void RenderGridBackground(QPainter& painter, QPoint upperLeft, QPoint lowerRight, int32 spacing, int32 numSubGridLinesPerCell);
		void paintEvent(QPaintEvent* event);

		Core::String	mTempString;
		WindowFunction* mWindowFunction;
		FFT				mFFT;
		Domain			mDomain;
};


#endif
