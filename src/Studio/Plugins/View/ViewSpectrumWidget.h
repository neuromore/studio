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

#ifndef __NEUROMORE_VIEWSPECTRUMWIDGET_H
#define __NEUROMORE_VIEWSPECTRUMWIDGET_H

// include required headers
#include "../../Config.h"
#include "../../Rendering/OpenGLWidget2DHelpers.h"
#include <DSP/Spectrum.h>


// forward declaration
class ViewSpectrumPlugin;

class ViewSpectrumWidget : public OpenGLWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		ViewSpectrumWidget(ViewSpectrumPlugin* plugin, QWidget* parent);
		virtual ~ViewSpectrumWidget();

		void paintGL() override final;

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:
				RenderCallback(ViewSpectrumWidget* widget, OpenGLWidget* parent) : OpenGLWidgetCallback(parent) { mViewWidget = widget; mCurMaxRange = 0.0; mSlowedMaxRange = 1.0; }
				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height);

			private:
				ViewSpectrumWidget*		mViewWidget;
				double					mCurMaxRange;		// current max range
				double					mSlowedMaxRange;	// max range for smooth interpolating

				Core::String			mTempString;

		};

		friend class RenderCallback;

		ViewSpectrumPlugin*					mPlugin;
		RenderCallback*						mRenderCallback;
		OpenGLWidget2DHelpers::GridInfo		mGridInfo;

		double								mLeftTextWidth;
};


#endif
