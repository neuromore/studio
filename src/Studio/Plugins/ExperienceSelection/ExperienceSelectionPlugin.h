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

#ifndef __NEUROMORE_EXPERIENCESELECTIONPLUGIN_H
#define __NEUROMORE_EXPERIENCESELECTIONPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include <DockHeader.h>
#include <EngineManager.h>
#include "ExperienceSelectionWidget.h"


class ExperienceSelectionPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		ExperienceSelectionPlugin();
		virtual ~ExperienceSelectionPlugin();

		// overloaded
		const char* GetName() const override											{ return "Experience Selection"; }
		static const char* GetStaticTypeUuid()											{ return "a3e35c30-c4e0-11e5-9912-ba0be0483c18"; }
		Plugin* Clone() override														{ return new ExperienceSelectionPlugin(); }
		const char* GetRuleName() const override										{ return "PLUGIN_ExperienceSelection"; }

		// overloaded main init function
		bool Init() override;
		void OnPostAuthenticationInit() override										{ mWidget->AsyncLoadFromBackend(""); }
		void OnAfterLoadLayout() override;

		// EVENTS ----------------------------------------------
		void OnStartSession() override final											{ mWidget->setEnabled(false); }
		void OnStopSession() override final												{ mWidget->setEnabled(true); }
		// -----------------------------------------------------

	private:
		ExperienceSelectionWidget* mWidget;
};


#endif
