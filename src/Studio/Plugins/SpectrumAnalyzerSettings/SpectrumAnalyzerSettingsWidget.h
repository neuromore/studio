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

#ifndef __NEUROMORE_SPECTRUMANALYZERSETTINGSWIDGET_H
#define __NEUROMORE_SPECTRUMANALYZERSETTINGSWIDGET_H

#include "../../Config.h"
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <Slider.h>
#include <EngineManager.h>
#include <Slider.h>
#include <Widgets/ColorMappingWidget.h>
#include <Widgets/WindowFunctionWidget.h>



// forward declaration
class SpectrumAnalyzerSettingsPlugin;

class SpectrumAnalyzerSettingsWidget : public QWidget
{
	Q_OBJECT

	public:
		SpectrumAnalyzerSettingsWidget(QWidget* parent, SpectrumAnalyzerSettingsPlugin* plugin);
		virtual ~SpectrumAnalyzerSettingsWidget();

		void SetFFTOrder(uint32 newFFTOrder, bool updateSlider);
		void SetNumWindowShiftSamples( uint32 numSampels, bool updateSlider );
		void UpdateInterface();

		void AddWidget(const char* name, QWidget* widget);
		void AddLayout(const char* name, QLayout* layout);

	private slots:
		void OnFFTOrderChanged(int newValue);
		void OnNumWindowShiftSamplesChanged( int numSamples );
		void OnWindowFunctionChanged( int newIndex );

	protected:
		SpectrumAnalyzerSettingsPlugin*		mPlugin;
		Core::String						mTempString;
		QGridLayout*						mGridLayout;
		uint32								mRow;

		// FFT Order
		QSlider*							mSliderFFTOrder;
		QLabel*								mLabelFFTOrder;

		// window shifting
		QSlider*							mSliderNumWindowShiftSamples;
		QLabel*								mLabelNumWindowShiftSamples;

		// window function
		QComboBox*							mComboWindowFunction;
		WindowFunctionWidget*				mWindowFunctionTime;
		WindowFunctionWidget*				mWindowFunctionFreq;
};


#endif
