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

#ifndef __NEUROMORE_RAWWAVEFORMWIDGET_H
#define __NEUROMORE_RAWWAVEFORMWIDGET_H

// include required headers
#include "../../Config.h"
#include "../../Rendering/OpenGLWidget.h"
#include <BciDevice.h>


// forward declaration
class RawWaveformPlugin;

class RawWaveformWidget : public OpenGLWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		RawWaveformWidget(RawWaveformPlugin* plugin, QWidget* parent);
		virtual ~RawWaveformWidget();

		// render frame
		void paintGL() override final;

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:
				RenderCallback(RawWaveformWidget* parent);
				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height) override;

				// waveform
				void Render(BciDevice* headset, Sensor* sensor, double windowWidth, double windowHeight);
				void RenderWave2D(Channel<double>* channel, bool useAutoScale, double amplitudeScale, double timeRange, double height, double yCenter, double xStart, double xEnd, double windowHeight);
				void Render2DCircle(double posX, double posY, double radius, uint32 numSteps, const Core::Color& color);

				// grid
				void RenderGrid(uint32 numSensors, uint32 numVerticalDivs, uint32 waveCellHeight, uint32 xStart, uint32 xEnd, uint32 windowHeight, const Core::Color& gridColor, const Core::Color& gridSubColor, double timeScale, double timeRange);

				// time axis
				void RenderTimeAxis(uint32 xEnd, uint32 xStart, uint32 windowHeight, double maxTime, double timeRange);

			private:
				RawWaveformWidget*	mParent;
				Core::String		mTempString;

				// style
				float				mLineWidth;
				Core::Color		mTextColor;
				Core::Color		mGridSubColor;
				Core::Color		mGridColor;
				Core::Color		mAxisColor;
		};

		RawWaveformPlugin*		mPlugin;
		RenderCallback*			mRenderCallback;
};


#endif
