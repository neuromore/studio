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

#ifndef __NEUROMORE_EEGELECTRODEPLACEMENTPLUGIN_H
#define __NEUROMORE_EEGELECTRODEPLACEMENTPLUGIN_H

// include required headers
#include "../../../Config.h"
#include <PluginSystem/Plugin.h>
#include "EEGElectrodePlacementPlugin.h"
#include "../../../Widgets/EEGElectrodesWidget.h"


class EEGElectrodePlacementPlugin : public Plugin
{
	Q_OBJECT
	public:
		// constructor & destructor
		EEGElectrodePlacementPlugin();
		virtual ~EEGElectrodePlacementPlugin();

		// general
		const char* GetName() const override						{ return "EEG Electrode Placement Plugin"; }
		static const char* GetStaticTypeUuid()						{ return "3dce2f62-f89c-11e4-a322-1697f925ec7b"; }
		const char* GetRuleName() const override					{ return "PLUGIN_Development"; }
		Plugin* Clone() override									{ return new EEGElectrodePlacementPlugin(); }

		// initialization
		void RegisterAttributes() override;
		bool Init() override;

		// settings
		inline double GetMultiView()								{ return GetBoolAttributeByName("multiView"); }

	private:
		EEGElectrodesWidget*	mElectrodesWidget;
};


#endif
