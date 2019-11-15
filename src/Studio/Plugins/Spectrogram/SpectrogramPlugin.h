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

#ifndef __NEUROMORE_SPECTROGRAMPLUGIN_H
#define __NEUROMORE_SPECTROGRAMPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/String.h>
#include <EngineManager.h>
#include <Sensor.h>
#include "../DataVisualizationPlugin.h"
#include <AttributeWidgets/AttributeSetGridWidget.h>

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/q3dtheme.h>
#include <QtDataVisualization/qabstract3dgraph.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QtDataVisualization/qcategory3daxis.h>
#include <QtDataVisualization/qvalue3daxis.h>

using namespace QtDataVisualization;

#define SPECTROGRAMPLUGIN_TIME_FREQUENCY_RATIO 10.0

class SpectrogramPlugin : public DataVisualizationPlugin
{
	Q_OBJECT

	class DataProxy
	{
		public:
			DataProxy() { mMinValue = 0; mMaxValue = 1; }
			virtual ~DataProxy() {}
			inline double GetMinValue()		{ return mMinValue; }
			inline double GetMaxValue()		{ return mMaxValue; }

		protected:
			double mMinValue;
			double mMaxValue;
	};

	public:
		SpectrogramPlugin(const char* typeUuid);
		virtual ~SpectrogramPlugin();

		// plugin overrides
		virtual void RealtimeUpdate();

		void RegisterAttributes() override;

		void ReInit();

		// init for this base class
		bool Init(QAbstract3DGraph* graph, QWidget* topLeftWidget, DataProxy* dataProxy);
		
		// getter for abstract graph
		inline QAbstract3DGraph* GetGraph()	const									{ return mGraph; }

		// axis helpers
		QValue3DAxis* GetValueAxis() const											{ return mValueAxis; }
		void StyleValueAxis(QValue3DAxis* inOutAxis);
		//void StyleCategoryAxis(QCategory3DAxis* inOutAxis);

		// value axis type
		enum ValueAxisType { LINEAR, LOGARITHMIC, DEZIBEL } mValueAxisType;

		// default theme for use by all spectrogram plugins
		Q3DTheme* GetDefaultTheme();
		Q3DTheme* mDefaultTheme;

		// create a QLinearGradient from our ColorMapper gradients
		static QLinearGradient GetColorGradient(int index);

		// settings widget actions for use by the derived class
		virtual void OnMinFrequencyChanged(double value) = 0;
		virtual void OnMaxFrequencyChanged(double value) = 0;
		
		// set new frequency range, called by derived class
		void SetFrequencyRange(double minFrequency, double maxFrequency);

		// autoscale the axis
		void AutoScale();
		void SetAutoScalingHysteresis(double factor)										{ mAutoScalingHysteresis = factor; }


		// attributes
		inline bool IsCameraAutoRotating()													{ return GetBoolAttributeByName("cameraAnimation"); }

		// camera ortho view
		inline bool GetCameraOrthoView()													{ return GetBoolAttributeByName("orthogonalView"); }
		void SetCameraOrthoView(bool isOrtho);

		// camera preset
		inline int32 GetCameraPresetIndex()													{ return GetInt32AttributeByName("cameraPreset"); }
		void SetCameraPreset(Q3DCamera::CameraPreset cameraPreset);
		void SetCameraPresetIndex(int32 index);

		// value axis type
		inline int32 GetValueAxisTypeIndex()												{ return GetInt32AttributeByName("valueAxisType"); }
		void SetValueAxisTypeIndex(int32 index);

	public slots:
		// event handlers
		void OnAttributeChangedBaseClass(Property* property);

		void OnMinValueSliderChanged(int value);
		void OnMaxValueSliderChanged(int value);
		void OnAutoScalingCheckBoxClicked(bool checked);
		void OnMinFrequencySliderChanged(int value);
		void OnMaxFrequencySliderChanged(int value);

	protected:
		// main widgets and graph class
		QAbstract3DGraph*	mGraph;
		QWidget*			mGraphWidget;
		QWidget*			mTopLeftWidget;
		DataProxy*			mDataProxy;

		AttributeSetGridWidget* mAttributeSetGridWidget;

		// value axis helpers
		QValue3DAxis*		CreateValueAxis(ValueAxisType type);
		virtual void		UpdateValueAxis() = 0;

		// more stuff for controling the value axis
		QValue3DAxis*		mValueAxis;
		double				mMinValue;
		double				mMaxValue;
		double				mAutoScalingHysteresis;

		// temporary string for stringformating
		QString				mTmpQString;

	// settings widgets are accessible for derived classes
	protected:
		void AddSettings(DataVisualizationSettingsWidget* settingsWidget);

		// camera
		QComboBox*								mCameraPresetComboBox;
		Core::Array<Q3DCamera::CameraPreset>	mCameraPresetList;

		// VALUE
		QSlider*								mMinValueSlider;
		QLabel*									mMinValueLabel;
		QSlider*								mMaxValueSlider;
		QLabel*									mMaxValueLabel;
		QCheckBox*								mAutoScalingCheckBox;
		bool									mUseAutoScaling;
		double									mCurrentMinValue;
		double									mCurrentMaxValue;

		// FREQUENCY
		QSlider*								mMinFrequencySlider;
		double									mMinFrequency;
		QLabel*									mMinFrequencyLabel;
		QSlider*								mMaxFrequencySlider;
		double									mMaxFrequency;
		QLabel*									mMaxFrequencyLabel;

		Core::Timer								perfTimer;
};

#endif

#endif
