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

#ifndef __NEUROMORE_EXPERIENCEPLUGIN_H
#define __NEUROMORE_EXPERIENCEPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include "ExperienceWidget.h"
#include <QVBoxLayout>
#include <QPushButton>


class ExperiencePlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		ExperiencePlugin();
		virtual ~ExperiencePlugin();

		// overloaded
		const char* GetName() const override											{ return "Experience"; }
		static const char* GetStaticTypeUuid()											{ return "1aea9056-26e3-11e5-b345-feff819cdc9f"; }
		const char* GetRuleName() const override										{ return "PLUGIN_Experience"; }
		Plugin* Clone() override														{ return new ExperiencePlugin(); }
		bool NeedRegularUpdateInterface() override										{ return true; }

		bool Init() override;
		void UpdateInterface() override;

		// EVENTS ----------------------------------------------
		void OnActiveExperienceChanged(Experience* experience) override;
		// -----------------------------------------------------

	private:
		ExperienceWidget*					mExperienceWidget;
		Core::String						mTempString;
};


#endif
