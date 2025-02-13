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

#ifndef __NEUROMORE_OPENGLWIDGET2DHELPERS_H
#define __NEUROMORE_OPENGLWIDGET2DHELPERS_H

#include "../Config.h"
#include "OpenGLWidget.h"


class OpenGLWidget2DHelpers
{
	public:
		// render horizontal lines (only) grid
		// NOTE: used for feedback plugin
		static void RenderHGrid(OpenGLWidgetCallback* callback, uint32 numSplits, const Core::Color& mainColor, uint32 numSubSplits, const Core::Color& subColor, double xOffset, double yOffset, double width, double height);

		struct GridInfo
		{
			// x-axis
			double			mMinX;
			double			mMaxX;
			Core::String	mUnitX;
			uint32			mNumXSplits;
			uint32			mNumXSubSplits;

			// y-axis
			double			mMinY;
			double			mMaxY;
			Core::String	mUnitY;
			uint32			mNumYSplits;
			uint32			mNumYSubSplits;
		};

		static void RenderGrid(OpenGLWidgetCallback* callback, GridInfo* gridInfo, const Core::Color& mainColor, const Core::Color& subColor, double xStart, double xEnd, double yStart, double yEnd);
		static void RenderEquallySpacedYLabels(OpenGLWidgetCallback* callback, Core::String& tempString, const QColor& textColor, double minValue, double maxValue, uint32 numSplits, double x, double y, double width, double height, bool horizontalView = false, bool drawFromRight = false, uint32 numSubsplits = 0);
		static void RenderEquallySpacedXLabels(OpenGLWidgetCallback* callback, Core::String& tempString, const char* unit, const QColor& textColor, double minValue, double maxValue, uint32 numSplits, double x, double y, double width, double height, bool horizontalView = false, bool drawFromRight = false);
		//
		// chart rendering
		//

		enum EChartRenderStyle { BOX, BAR, LOLLIPOP, CROSS, LINE};
		
		// NOTE: used for feedback plugin
		static void AutoCalcChartSplits(double height, uint32* outNumSplits, uint32* outNumSubSplits);
		static void RenderChart(OpenGLWidgetCallback* callback, Channel<double>* channel, const Core::Color& color, EChartRenderStyle style, double timeRange, double maxTime, double rangeMin, double rangeMax, int32 xStart, int32 xEnd, int32 yStart, int32 height, bool drawLatencyMarker = false);
		
		// sample render functions
		typedef void (CORE_CDECL *RenderSampleFunction)(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static double GetSampleSize(EChartRenderStyle style, double chartWidth, double chartHeight, double numSamples);

		static void CORE_CDECL RenderBoxSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static void CORE_CDECL RenderBoxSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static void CORE_CDECL RenderBarSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static void CORE_CDECL RenderBarSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
	    static void CORE_CDECL RenderLollipopSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
	    static void CORE_CDECL RenderLollipopSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static void CORE_CDECL RenderCrossSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
		static void CORE_CDECL RenderCrossSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
	    static void CORE_CDECL RenderLineSample(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);
	    static void CORE_CDECL RenderLineSampleClipped(OpenGLWidgetCallback* callback, double value, double xPos, double yPos, double previousXPos, double previousYPos, double xStart, double xEnd, double yStart, double yEnd, double size, const Core::Color& lighterColor, Core::Color& darkerColor, Core::Color& valueColor);

		//
		// Timeline Rendering
		//		NOTE: used for feedback plugin
		static void RenderTimeline(OpenGLWidgetCallback* callback, const Core::Color& color, double timeRange, double maxTime, double x, double y, double width, double height, Core::String& tempString, bool scaleInMins = false);

		//
		// Spectrum Rendering
		//		NOTE: used for spectrum plugin
		static void RenderSpectrum(OpenGLWidgetCallback* callback, Spectrum* spectrum, const Core::Color& color, uint32 minBinIndex, uint32 maxBinIndex, double rangeMin, double rangeMax, double xStart, double xEnd, double yStart, double yEnd);
		static void RenderSpectrumChart(OpenGLWidgetCallback* callback, Spectrum* spectrum, const Core::Color& color, uint32 minBinIndex, uint32 maxBinIndex, double rangeMin, double rangeMax, double xStart, double xEnd, double yStart, double yEnd, bool horizontalView, bool drawFromRight);

	private:
		static RenderSampleFunction SelectSampleRenderFunction(EChartRenderStyle style, bool clipped = false);
		static Core::Color barColorByFrequency(uint32 binIndex);
};


#endif
