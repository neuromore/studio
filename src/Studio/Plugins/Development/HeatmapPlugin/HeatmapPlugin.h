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

#ifndef __NEUROMORE_HEATMAPPLUGIN_H
#define __NEUROMORE_HEATMAPPLUGIN_H

// include required headers
#include "../../../Config.h"
#include <PluginSystem/Plugin.h>
#include "HeatmapWidget.h"


class HeatmapPlugin : public Plugin
{
	Q_OBJECT
	public:
		// constructor & destructor
		HeatmapPlugin();
		virtual ~HeatmapPlugin();

		// general
		const char* GetName() const override						{ return "Heatmap Plugin"; }
		static const char* GetStaticTypeUuid()						{ return "03b493ce-060d-11e5-a6c0-1697f925ec7b"; } // TODO: generate new Type 1 UUID for plugin and never change it again (https://www.uuidgenerator.net/)
		const char* GetRuleName() const override					{ return "PLUGIN_Development"; } // TODO: coordinate security privilege with Stefan so that he can let the neuromore Cloud about the new plugin
		Plugin* Clone() override									{ return new HeatmapPlugin(); }

		// initialization
		void RegisterAttributes() override;
		bool Init() override;

		// settings
		inline double GetMultiView()								{ return GetBoolAttributeByName("multiView"); }

	private:
		HeatmapWidget* mHeatmapWidget;
};


#endif
