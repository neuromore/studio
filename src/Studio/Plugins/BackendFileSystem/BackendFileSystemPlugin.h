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

#ifndef __NEUROMORE_BACKENDFILESYSTEMPLUGIN_H
#define __NEUROMORE_BACKENDFILESYSTEMPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include <DockHeader.h>
#include <EngineManager.h>
#include "BackendFileSystemWidget.h"
#include <QMenuBar>


class BackendFileSystemPlugin : public Plugin, public Core::EventHandler
{
	friend BackendFileSystemWidget;

	Q_OBJECT

	public:
		BackendFileSystemPlugin();
		virtual ~BackendFileSystemPlugin();

		// overloaded
		const char* GetName() const override											{ return "Back-End File System"; }
		static const char* GetStaticTypeUuid()											{ return "c38f75d1-7eca-11e4-b4a9-0800200c9a66"; }
		Plugin* Clone() override														{ return new BackendFileSystemPlugin(); }
		const char* GetRuleName() const override										{ return "PLUGIN_BackendFileSystem"; }

		// overloaded main init function
		bool Init() override;
		void OnPostAuthenticationInit() override										{ mFileSystemWidget->Refresh(); }
		void OnAfterLoadLayout() override												{ mFileSystemWidget->Refresh(); }

		bool ExpandByPath(const QStringList& itemPath);

		// EVENTS ----------------------------------------------
		void OnActiveClassifierChanged(Classifier* classifier) override final					{ mFileSystemWidget->UpdateInterface(); }
		void OnActiveStateMachineChanged(StateMachine* statemachine) override final				{ mFileSystemWidget->UpdateInterface(); }
		void OnActiveExperienceChanged(Experience* experience) override final					{ mFileSystemWidget->UpdateInterface(); }
		void OnStartSession() override final													{ mFileSystemWidget->UpdateInterface(); }
		void OnStopSession() override final														{ mFileSystemWidget->UpdateInterface(); }
		// -----------------------------------------------------
		
	private:
		BackendFileSystemWidget* mFileSystemWidget;
};


#endif
