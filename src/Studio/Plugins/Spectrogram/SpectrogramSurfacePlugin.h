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

#ifndef __NEUROMORE_SPECTROGRAMSURFACEPLUGIN_H
#define __NEUROMORE_SPECTROGRAMSURFACEPLUGIN_H

#include "SpectrogramPlugin.h"
#include "../../Config.h"
#include "../../Widgets/ChannelSelectionWidget.h"
#include <Core/String.h>
#include <Sensor.h>

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dsurface.h>
#include <DSP/FFTProcessor.h>

using namespace QtDataVisualization;


// wrap the Q3DSurface widget to get access to the resizeEvent
class SpectrogramSurfaceWidget : public Q3DSurface
{
	Q_OBJECT

	public:
		SpectrogramSurfaceWidget(const QSurfaceFormat * format = 0, QWindow * parent = 0) : Q3DSurface (format, parent)  {}
			
		void resizeEvent(QResizeEvent * event) override
		{
			emit WidgetResized(event->size().width(), event->size().height());
			Q3DSurface::resizeEvent(event);
		}

	signals:
		void WidgetResized(int w, int h);
};

class SpectrogramSurfacePlugin : public SpectrogramPlugin
{
	Q_OBJECT

	//
	// data proxy : spectrumchannel to surface graph, with own spectrum analyzer
	//
	class SurfaceDataProxy : public QSurfaceDataProxy, public DataProxy
	{
		public:
			SurfaceDataProxy( double duration )
			{
				mData = new QSurfaceDataArray();
				mChannel = NULL;
				mNumSamples = 0;
				mSampleRate = 0;
				mDuration = duration;
				mNumBins = 0;
				mConvertToDezibel = false;
				mLastTime = 0.0;
				mLastIndex = 0;
					
				mSpectrumAnalyzer.Init();
				mSpectrumAnalyzer.GetOutput()->SetBufferSize(0);
			}

			~SurfaceDataProxy() 
			{
			}

			virtual void Update();								// copy new spectrum values to data proxy
			virtual void ReInit();								// recalculate samples and mesh size
			void Clear();										// clear arrays

			void SetChannel(Channel<double>* channel)			{ mChannel = channel; ReInit(); }
			Channel<double>* GetChannel()						{ return mChannel; }

			void SetDuration(double duration)					{ mDuration = duration; ReInit(); }
			void SetFrequencyRange(double min, double max)		{ mMinFrequency = min; mMaxFrequency = max; }

			void SetConvertToDezibel(bool enable = true)		{ if (mConvertToDezibel != enable) { mConvertToDezibel = enable; ReInit(); } }

			FFTProcessor* GetSpectrumAnalyzer()					{ return &mSpectrumAnalyzer; }
				
		private:
			QSurfaceDataArray*	mData;						// deallocated by QT

			Channel<double>*	mChannel;					// the displayed channel
			FFTProcessor		mSpectrumAnalyzer;			// one spectrum analyzer for each channel
			double				mSampleRate;				// sampling rate of the spectrum sampler
				
			double				mDuration;					// displayed intervalsize in seconds
			uint32				mNumSamples;				// number of spectrums to display 
			uint32				mNumBins;					// number of frequency bins (size of mesh in frequency direction)
			uint32				mNumShiftSamples;			
			uint32				mWindowFunctionType;		

			bool				mConvertToDezibel;			// convert spectrum values from uV to uVdB for rendering

			double				mLastTime;					// timestamp of last spectrum during last update()
			uint32				mLastIndex;					// (running) index of last spectrum during last update()

			double				mMinFrequency;				// displayed frequency range (for min/max calculation)
			double				mMaxFrequency;				// displayed frequency range (for min/max calculation)
	};



		//// custom time axis formatter for fixing the time axis scaling
		//class TimeAxisFormatter : public QValue3DAxisFormatter
		//{
		//	public:
		//		TimeAxisFormatter(QObject *parent = 0) : QValue3DAxisFormatter(parent) {}
		//		TimeAxisFormatter(QValue3DAxisFormatterPrivate *d, QObject *parent = 0) : QValue3DAxisFormatter(d, parent) {}
		//		~TimeAxisFormatter() {}

		//		void recalculate() override
		//		{
		//			QValue3DAxisFormatter::recalculate();
		//			QVector<float> pos = gridPositions();
		//		}
		//		QString	stringForValue(qreal value, const QString & format) const { return QString().sprintf(format.toUtf8(), value); }
		//};

	public:
		SpectrogramSurfacePlugin();
		SpectrogramSurfacePlugin(const char* typeUuid);
		virtual ~SpectrogramSurfacePlugin();

		// overloaded
		const char* GetName() const	override			{ return "3D Spectrogram"; }
		static const char* GetStaticTypeUuid()			{ return "c38f75da-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override		{ return "PLUGIN_SpectrogramSurface"; }
		Plugin* Clone()	override						{ return new SpectrogramSurfacePlugin(); }
		bool AllowMultipleInstances() const override	{ return true; }

		// overloaded functions
		void RegisterAttributes() override;
		bool Init() override;
		void RealtimeUpdate() override;

		QValue3DAxis*	CreateXAxis();		// frequency axis
		QValue3DAxis*	CreateZAxis();		// time axis

		// settings
		inline ColorMapper::EColorMapping GetColorMapping()									{ return (ColorMapper::EColorMapping)GetInt32AttributeByName("colorMapping"); }
		void SetColorMapping(ColorMapper::EColorMapping colorMapping);

		inline bool GetShow0HzBin()															{ return GetBoolAttributeByName("show0HzBin"); }
		void SetShow0HzBin(bool show);

	private slots:
		void OnChannelSelectionChanged(Channel<double>* channel);
		void OnIntervalLengthSliderChanged(int value);
		void OnMinFrequencyChanged(double value);
		void OnMaxFrequencyChanged(double value);

	protected slots:
		// event handlers
		void OnAttributeChanged(Property* property);


	protected:
		SpectrogramSurfaceWidget*	mGraph;

	private:
		// data viz stuff
		SurfaceDataProxy*			mDataProxy;
		
		QSurface3DSeries*			mSeries;

		// top bar widget
		ChannelSelectionWidget*		mChannelSelectionWidget;
	
	protected:
		// settings widgets
		void AddSettings(DataVisualizationSettingsWidget* settingsWidget);

		// color mapping
		ColorMapper					mColorMapper;

		QSlider*					mIntervalLengthSlider;
		QLabel*						mIntervalLengthLabel;
		double						mIntervalLength;
		double						mMaxIntervalLength;

		// axis helpers
		void UpdateValueAxis();

		// implements base class method
		void UpdateTimeAxis();
};

#endif

#endif

