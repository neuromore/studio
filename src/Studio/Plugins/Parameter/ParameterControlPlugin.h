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

#ifndef __NEUROMORE_PARAMETERCONTROLPLUGIN_H
#define __NEUROMORE_PARAMETERCONTROLPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include "ParametersWidget.h"


// lists active devices
class ParameterControlPlugin : public Plugin
{
	Q_OBJECT
	public:
		ParameterControlPlugin();
		virtual ~ParameterControlPlugin();

		// overloaded
		const char* GetName() const override											{ return "Parameters"; }
		static const char* GetStaticTypeUuid()											{ return "a9f94048-a277-11e5-bf7f-feff819cdc9f"; }
		const char* GetRuleName() const override										{ return "PLUGIN_Devices"; } // TODO parameters rule
		Plugin* Clone() override														{ return new ParameterControlPlugin(); }
		bool NeedRegularUpdateInterface() override										{ return true; }

		// overloaded main init function
		bool Init() override;

		// called regularily to update the information on the interface
		void UpdateInterface() override													{ if (mMainWidget != NULL) mMainWidget->UpdateInterface(); }

	private:
		ParametersWidget* mMainWidget;
};


#endif
