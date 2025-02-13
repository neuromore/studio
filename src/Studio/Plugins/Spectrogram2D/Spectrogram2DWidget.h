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

#ifndef __NEUROMORE_SPECTROGRAM2DWIDGET_H
#define __NEUROMORE_SPECTROGRAM2DWIDGET_H

// include required headers
#include "../../Config.h"
#include "../../Rendering/OpenGLWidget2DHelpers.h"
#include <DSP/Spectrum.h>


// forward declaration
class Spectrogram2DPlugin;

class Spectrogram2DWidget : public OpenGLWidget
{
	Q_OBJECT

	public:
		struct SpectrumElement
		{
			SpectrumElement()															{ mSpectrum = NULL; }
			SpectrumElement(const char* name, const Core::Color& color)				{ mName = name; mColor = color; mSpectrum = NULL; }
			Core::String	mName;
			Core::Color mColor;
			Spectrum*		mSpectrum;
		};

		// constructor & destructor
		Spectrogram2DWidget(Spectrogram2DPlugin* plugin, QWidget* parent);
		virtual ~Spectrogram2DWidget();

		void paintGL() override final;

		void UpdateSpectrum(uint32 index, Spectrum* spectrum)							{ mSpectrums[index].mSpectrum = spectrum; }
		void ReInit(const Core::Array<SpectrumElement>& spectrums)						{ mSpectrums = spectrums; }
		uint32 GetNumberOfSpectrums()													{ return mSpectrums.Size(); }

		bool GetMultiView() const						{ return mMultiView; }
		void SetMultiView(bool multiView)				{ mMultiView = multiView; }
		void SetHorizontalView(bool horizontalView)		{ mHorizontalView = horizontalView; }
		bool GetHorizontalView() const { return mHorizontalView; }

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:
				RenderCallback(Spectrogram2DWidget* parentWidget, OpenGLWidget* parent) : OpenGLWidgetCallback(parent) { mParentWidget = parentWidget; mCurMaxRange = 0.0; mSlowedMaxRange = 1.0; }
				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height);

			private:
				Spectrogram2DWidget*	mParentWidget;
				Core::String			mTempString;
				double					mCurMaxRange;
				double					mSlowedMaxRange;
		};

		friend class RenderCallback;

		Spectrogram2DPlugin*				mPlugin;
		RenderCallback*						mRenderCallback;
		OpenGLWidget2DHelpers::GridInfo		mGridInfo;
		Core::Array<SpectrumElement>		mSpectrums;
		double								mLeftTextWidth;
		bool								mMultiView;
		bool								mHorizontalView = true;

		QColor								mGridColor;
		QColor								mSubGridColor;
		QColor								mTextColor;
		QColor								mFeedbackNameColor;
		QColor								mBackgroundColor;
		QColor								mAreaBgColor;
};


#endif
