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

#ifndef __NEUROMORE_SPECTROGRAMBANDSPLUGIN_H
#define __NEUROMORE_SPECTROGRAMBANDSPLUGIN_H

// include required headers
#include "SpectrogramPlugin.h"
#include "../../Config.h"
#include "../../Widgets/ChannelMultiSelectionWidget.h"
#include <Core/String.h>
#include <Sensor.h>
#include <DSP/FFTProcessor.h>

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dbars.h>

using namespace QtDataVisualization;

class SpectrogramBandsPlugin : public SpectrogramPlugin
{
	Q_OBJECT
	// time-based data proxy for surface graphs
	class ChannelBandsDataProxy : public QBarDataProxy, public DataProxy
	{
		public:
			ChannelBandsDataProxy()
			{
				mData = new QBarDataArray();
				mConvertToDezibel = false;
			}
			ChannelBandsDataProxy(Core::Array<Channel<double>*> channels)
			{
				mChannels = channels;
				mData = new QBarDataArray();
			}
			~ChannelBandsDataProxy()
			{
				const uint32 numAnalyzers = mSpectrumAnalyzers.Size();
				for (uint32 i=0; i<numAnalyzers; i++)
					delete mSpectrumAnalyzers[i];
				mSpectrumAnalyzers.Clear();
			}
			
			void ReInit();
			void Update();

			void AddChannel(Channel<double>* channel)			{ mChannels.Add(channel); }
			Channel<double>* GetChannel(uint32 index)			{ return mChannels[index]; }
			void Clear()										{ mChannels.Clear(); }

			FFTProcessor* GetSpectrumAnalyzer(uint32 index)		{ return mSpectrumAnalyzers[index]; }
			
			inline uint32 GetNumBins() const					{ return GetEngine()->GetSpectrumAnalyzerSettings()->GetNumFrequencyBands(); }
			inline uint32 GetNumChannels() const				{ return (uint32)(mChannels.Size()); }
			
			void SetConvertToDezibel(bool enable = true)		{ mConvertToDezibel = enable; }

		private:
			QBarDataArray*						mData;				// deallocated by QT
			Core::Array<Channel<double>*>		mChannels;
			Core::Array<FFTProcessor*>			mSpectrumAnalyzers;
			double								mMaxFrequency;
			bool								mConvertToDezibel;	// convert spectrum values from uV to uVdB for rendering
			uint32								mNumBins;			// number of frequency bins (size of mesh in frequency direction)
			uint32								mNumShiftSamples;			
			uint32								mWindowFunctionType;		

	};

	public:
		SpectrogramBandsPlugin();
		virtual ~SpectrogramBandsPlugin();

		// overloaded
		const char* GetName() const	override			{ return "3D Frequency Bands"; }
		static const char* GetStaticTypeUuid()			{ return "c38f75db-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override		{ return "PLUGIN_SpectrogramBands"; }
		Plugin* Clone() override						{ return new SpectrogramBandsPlugin(); }
		bool AllowMultipleInstances() const override	{ return true; }

		// overloaded functions
		void RegisterAttributes() override;
		bool Init() override;
		void RealtimeUpdate() override;

		QCategory3DAxis* CreateChannelAxis();
		QCategory3DAxis* CreateBandsAxis();
		QStringList GetChannelAxisLabels();
		QStringList GetBandsAxisLabels();

		// settings
		inline ColorMapper::EColorMapping GetColorMapping()									{ return (ColorMapper::EColorMapping)GetInt32AttributeByName("colorMapping"); }
		void SetColorMapping(ColorMapper::EColorMapping colorMapping);

	private slots:
		void OnChannelSelectionChanged();
		void OnMinFrequencyChanged(double value)				{ /*mGraph->axisX()->setMin(value);*/ }
		void OnMaxFrequencyChanged(double value)				{ /*mGraph->axisX()->setMax(value);*/ }

		void OnAttributeChanged(Property* property);

	private:	
		// data viz stuff
		ChannelBandsDataProxy*		mDataProxy;
		Q3DBars*					mGraph;
		QBar3DSeries*				mSeries;

		// color mapping
		ColorMapper					mColorMapper;

		// top bar widget
		ChannelMultiSelectionWidget* mChannelMultiSelectionWidget;

		// settings widgets
		void AddSettings( DataVisualizationSettingsWidget* settingsWidget );

		void UpdateValueAxis();
};

#endif


#endif
