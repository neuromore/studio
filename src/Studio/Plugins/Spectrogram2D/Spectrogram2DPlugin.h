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

#ifndef __NEUROMORE_LINESPECTRUMPLUGIN_H
#define __NEUROMORE_LINESPECTRUMPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include "Spectrogram2DWidget.h"
#include <AttributeWidgets/Property.h>
#include "../../Widgets/ChannelMultiSelectionWidget.h"
#include <DSP/FFTProcessor.h>


// universal waveform plugin
class Spectrogram2DPlugin : public Plugin
{
	Q_OBJECT

	public:
		// constructor and destructor
		Spectrogram2DPlugin();
		virtual ~Spectrogram2DPlugin();

		// overloaded
		enum { CLASS_ID = 0x00000359 };
		static const char* GetStaticTypeUuid()										{ return "c38fc3f4-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetName() const override										{ return "2D Spectrogram"; }
		Plugin* Clone() override													{ return new Spectrogram2DPlugin(); }
		bool AllowMultipleInstances() const override								{ return true; }
		const char* GetRuleName() const override									{ return "PLUGIN_RawSpectrumPlugin"; }

		// overridden methods
		void RegisterAttributes() override;
		bool Init() override;
		void ReInit();
		void RealtimeUpdate() override;
		bool NeedRegularUpdateInterface() override									{ return true; } // TODO: remove this later
		void UpdateInterface() override;

		// settings
		inline bool GetMultiView()													{ return GetBoolAttributeByName("multiView"); }
		inline bool GetShow0HzBin()													{ return GetBoolAttributeByName("show0HzBin"); }
		void SetMultiView(bool multiView)											{ if (mSpectrumWidget != NULL) mSpectrumWidget->SetMultiView(multiView); }
		inline double GetAverageInterval()											{ return GetFloatAttributeByName("averageInterval"); }
		void SetAverageInterval(double length);
		
	private slots:
		void OnChannelSelectionChanged();
		void OnAttributeChanged(Property* property);
	
	private:
		// selected channels
		Core::Array<Channel<double>*>		mChannels;					// list of the selected channels				// TODO get rid of these
		Core::Array<FFTProcessor*>			mSpectrumAnalyzers;			// one FFT processor for each channel
		Core::Array<Spectrum>				mAverageSpectra;			// holds the current average (the display values)

		ChannelMultiSelectionWidget*		mChannelSelectionWidget;
		Spectrogram2DWidget*				mSpectrumWidget;
		Core::Array<Spectrogram2DWidget::SpectrumElement> mSpectrumElements;

		uint32								mNumSamples;
		uint32								mNumBins;
		uint32								mNumShiftSamples;
		uint32								mWindowFunctionType;
};


#endif
