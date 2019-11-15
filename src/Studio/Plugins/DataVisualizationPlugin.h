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

#ifndef __NEUROMORE_DATAVISUALIZATIONPLUGIN_H
#define __NEUROMORE_DATAVISUALIZATIONPLUGIN_H

// include required headers
#include "../Config.h"
#include <PluginSystem/Plugin.h>
#include <QPushButton>
#include "DataVisualizationSettingsWidget.h"

#ifdef USE_QTDATAVISUALIZATION

// forward declaration
class PropertyWidget;

class DataVisualizationPlugin : public Plugin
{
	Q_OBJECT
	public:
		DataVisualizationPlugin(const char* typeUuid);
		virtual ~DataVisualizationPlugin();

		// overloaded main init function
		bool InitLayout(QWidget* renderWidget, QWidget* topLeftWidget=NULL);

		// overload this function in your plugin implementations in order to fill the settings widget
		virtual void AddSettings(DataVisualizationSettingsWidget* settingsWidget)			{}
	
	public slots:
		void OnCurrentFpsChanged( double fps );

	protected:
		// settings interface
		DataVisualizationSettingsWidget*	mSettingsWidget;

		// render interface
		QWidget*							mRenderWidget;

		QLabel*								mFpsLabel;
		Core::String						mTempString;
};

#endif

#endif
