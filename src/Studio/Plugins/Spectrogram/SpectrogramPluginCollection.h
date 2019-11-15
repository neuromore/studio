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

#ifndef __NEUROMORE_SPECTROGRAMPLUGINCOLLECTION_H
#define __NEUROMORE_SPECTROGRAMPLUGINCOLLECTION_H

// include required headers
#include "../../Config.h"
#include "SpectrogramPlugin.h"
#include "SpectrogramSurfacePlugin.h"
#include "SpectrogramBandsPlugin.h"

#ifdef USE_QTDATAVISUALIZATION

#include <QtDataVisualization/QAbstract3DInputHandler>


// 2D Spectrogram
class Dummy3DInputHandler : public QtDataVisualization::QAbstract3DInputHandler
{
	Q_OBJECT
	public:
		explicit Dummy3DInputHandler(QObject *parent = 0) {}

		virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos) {}
		virtual void wheelEvent(QWheelEvent *event) {}
};

// TODO move class into its own cpp and header files
class SpectrogramWaterfallPlugin : public SpectrogramSurfacePlugin
{
	Q_OBJECT

	public:
		SpectrogramWaterfallPlugin() : SpectrogramSurfacePlugin(GetStaticTypeUuid())	{}
		virtual ~SpectrogramWaterfallPlugin()											{}

		const char* GetName() const	override											{ return "Waterfall Spectrogram"; }
		static const char* GetStaticTypeUuid()											{ return "c38f9cef-7eca-11e4-b4a9-0800200c9a66"; }
		Plugin* Clone()	override														{ return new SpectrogramWaterfallPlugin(); }

	public:
		bool Init() override
		{
			bool success = SpectrogramSurfacePlugin::Init();

			// disable perspective projection
			SetCameraOrthoView(true);

			// set camera position and rotation
			SetCameraPreset(Q3DCamera::CameraPresetDirectlyAbove);
			mGraph->scene()->activeCamera()->setXRotation(90);
			mGraph->scene()->activeCamera()->setYRotation(90);
			
			// remove mouse input by setting an empty input handler
			mGraph->setActiveInputHandler(new Dummy3DInputHandler());

			// connect graph widget resize signal to slot
			connect(mGraph, SIGNAL(WidgetResized(int,int)), this, SLOT(OnWidgetResized(int,int)));

			return success;
		}

		// hide some of the settings
		void AddSettings(DataVisualizationSettingsWidget* settingsWidget) override
		{
			// set autoscale, hide settings
			mUseAutoScaling= true;
			delete mAutoScalingCheckBox;
			mAutoScalingCheckBox = NULL;

			Core::AttributeSettings* attributeSettings	= FindAttributeSettingsByInternalName("cameraAnimation");	if (attributeSettings != NULL)		attributeSettings->SetVisible(false);
			attributeSettings							= FindAttributeSettingsByInternalName("orthogonalView");	if (attributeSettings != NULL)		attributeSettings->SetVisible(false);
			attributeSettings							= FindAttributeSettingsByInternalName("cameraPreset");		if (attributeSettings != NULL)		attributeSettings->SetVisible(false);
			
			// add baseclass settings
			SpectrogramSurfacePlugin::AddSettings(settingsWidget);
		}

		void RealtimeUpdate() override
		{
			SpectrogramSurfacePlugin::RealtimeUpdate();
		}
		
	private slots:
		void OnWidgetResized(int w, int h)
		{
			const double aspectRatio = (double)h / (double)w;
			GetGraph()->setHorizontalAspectRatio(aspectRatio);

			double zoomLevel;
			if (aspectRatio < 1.0)
				zoomLevel = 75.0 / aspectRatio;
			else
				zoomLevel = 105.0 * aspectRatio;

			GetGraph()->scene()->activeCamera()->setZoomLevel(zoomLevel);

			//LogInfo("aspectRatio=%f, zoomLevel=%f", aspectRatio, zoomLevel);
			
			QVector3D target;
			target.setX( -0.17f ); // move it a bit upwards
			target.setY( 0.0f );
			target.setZ( 0.1f ); // move it a bit to the right (so that the text fits)

			GetGraph()->scene()->activeCamera()->setTarget( target );
		}
};

#endif

#endif
