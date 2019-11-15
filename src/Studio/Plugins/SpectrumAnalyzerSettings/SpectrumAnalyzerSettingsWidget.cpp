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

// include the required headers
#include "SpectrumAnalyzerSettingsWidget.h"
#include "SpectrumAnalyzerSettingsPlugin.h"
#include <EngineManager.h>
#include "../../AppManager.h"
#include <QtBaseManager.h>


#define MIN_RANGE 1.0

using namespace Core;

// constructor
SpectrumAnalyzerSettingsWidget::SpectrumAnalyzerSettingsWidget(QWidget* parent, SpectrumAnalyzerSettingsPlugin* plugin) : QWidget(parent)
{
	mRow						= 0;
	mPlugin						= plugin;

	mGridLayout = new QGridLayout();
	mGridLayout->setMargin(0);

	// FFT order
	mSliderFFTOrder = new QSlider(Qt::Horizontal);
	mSliderFFTOrder->setRange( 5, 13 );
	connect( mSliderFFTOrder, SIGNAL( valueChanged( int ) ), this, SLOT( OnFFTOrderChanged( int ) ) );
	mLabelFFTOrder = new QLabel();
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setMargin(0);
	hLayout->setSpacing(3);
	hLayout->addWidget( mSliderFFTOrder );
	hLayout->addWidget( mLabelFFTOrder );
	AddLayout( "FFT Order", hLayout );

	// window shift 
	mSliderNumWindowShiftSamples = new QSlider( Qt::Horizontal );
	mSliderNumWindowShiftSamples->setRange( 1, 512 );
	connect( mSliderNumWindowShiftSamples, SIGNAL( valueChanged( int ) ), this, SLOT( OnNumWindowShiftSamplesChanged( int ) ) );
	mLabelNumWindowShiftSamples = new QLabel();
	hLayout = new QHBoxLayout();
	hLayout->setMargin( 0 );
	hLayout->setSpacing( 3 );
	hLayout->addWidget( mSliderNumWindowShiftSamples );
	hLayout->addWidget( mLabelNumWindowShiftSamples );
	AddLayout( "Shift", hLayout );

	// window function
	mComboWindowFunction = new QComboBox();
	for (uint32 i=0; i<WindowFunction::WINDOWFUNCTION_NUMFUNCTIONS; ++i)
		mComboWindowFunction->addItem( WindowFunction::GetName( (WindowFunction::EWindowFunction)i ) );

	WindowFunction* windowFunction = GetEngine()->GetSpectrumAnalyzerSettings()->GetWindowFunction();
	mComboWindowFunction->setCurrentIndex( (int)windowFunction->GetType() );
	connect( mComboWindowFunction, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWindowFunctionChanged(int)) );
	
	//// TODO use updated WindowFunctionWidget w/ Charts
	mWindowFunctionTime = new WindowFunctionWidget( windowFunction, WindowFunctionWidget::DOMAIN_TIME );
	//mWindowFunctionFreq = new WindowFunctionWidget( windowFunction, WindowFunctionWidget::DOMAIN_FREQUENCY );

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mComboWindowFunction);
	vLayout->addWidget(mWindowFunctionTime);
	//vLayout->addWidget(mWindowFunctionFreq);
	AddLayout( "Window Function", vLayout );


	setLayout( mGridLayout );
	UpdateInterface();
}


// destructor
SpectrumAnalyzerSettingsWidget::~SpectrumAnalyzerSettingsWidget()
{
}


void SpectrumAnalyzerSettingsWidget::AddWidget(const char* name, QWidget* widget)
{
	mGridLayout->addWidget( new QLabel(name), mRow, 0  );
	mGridLayout->addWidget( widget, mRow, 1 );
	mRow++;
}


void SpectrumAnalyzerSettingsWidget::AddLayout(const char* name, QLayout* layout)
{
	mGridLayout->addWidget( new QLabel(name), mRow, 0  );
	mGridLayout->addLayout( layout, mRow, 1 );
	mRow++;
}


// called when the FFT order slider got changed
void SpectrumAnalyzerSettingsWidget::OnFFTOrderChanged( int newValue )
{
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	settings->SetFFTOrder( newValue );
	UpdateInterface();
}


// called when the window shift samples slider got changed
void SpectrumAnalyzerSettingsWidget::OnNumWindowShiftSamplesChanged( int numSamples )
{
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	settings->SetNumWindowShiftSamples( numSamples );
	UpdateInterface();
}


// called when another window function got selected in the combo box
void SpectrumAnalyzerSettingsWidget::OnWindowFunctionChanged( int newIndex )
{
	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	settings->GetWindowFunction()->SetType( (WindowFunction::EWindowFunction)newIndex );
	UpdateInterface();
}

// update interface from the current analyzer settings settings
void SpectrumAnalyzerSettingsWidget::UpdateInterface()
{
	//BciDevice*	headset			= GetEngine()->GetActiveBci();
	//const uint32	sampleRate		= headset != NULL ? headset->GetSampleRate() : 0;

	SpectrumAnalyzerSettings* settings = GetEngine()->GetSpectrumAnalyzerSettings();
	uint32			fftOrder			= settings->GetFFTOrder();
	const uint32	numFFTSamples		= settings->GetNumFFTSamples();
	const uint32 	numShiftSamples		= settings->GetNumWindowShiftSamples();
	WindowFunction*	windowFunction		= settings->GetWindowFunction();
	const uint32	windowFunctionIndex = (uint32)windowFunction->GetType();

	// FFT Order 
	mSliderFFTOrder->setValue( fftOrder );
	mTempString.Format( "%i (%i Samples)", fftOrder, numFFTSamples );
	mLabelFFTOrder->setText( mTempString.AsChar() );

	// window shift samples
	mSliderNumWindowShiftSamples->setValue( numShiftSamples );
	mTempString.Format( "%i (%.1f %%)", numShiftSamples, (double)numShiftSamples / (double)numFFTSamples * 100.0 );
	mLabelNumWindowShiftSamples->setText( mTempString.AsChar() );

	// window function combobox
	mComboWindowFunction->setCurrentIndex( windowFunctionIndex );

	// window 
	// TODO update chart widgets here
	mWindowFunctionTime->update();
	//mWindowFunctionFreq->update();
}
