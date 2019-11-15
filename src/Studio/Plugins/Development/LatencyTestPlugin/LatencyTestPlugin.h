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

#ifndef __NEUROMORE_LATENCYTESTPLUGIN_H
#define __NEUROMORE_LATENCYTESTPLUGIN_H

// include required headers
#include "../../../Config.h"
#include <PluginSystem/Plugin.h>
#include "LatencyTestWidget.h"


class LatencyTestPlugin : public Plugin
{
	Q_OBJECT
	public:
		// constructor & destructor
		LatencyTestPlugin();
		virtual ~LatencyTestPlugin();

		// general
		const char* GetName() const override						{ return "Latency Test"; }
		static const char* GetStaticTypeUuid()						{ return "41ac4110-d86b-11e4-b9d6-1681e6b88ec1"; }
		const char* GetRuleName() const override					{ return "PLUGIN_Development"; }
		Plugin* Clone() override									{ return new LatencyTestPlugin(); }

		// initialization
		void RegisterAttributes() override;
		bool Init() override;

		// settings
		double GetThreshold()										{ return GetFloatAttributeByName("threshold"); }

	private:
		LatencyTestWidget* mLatencyWidget;
};


#endif
