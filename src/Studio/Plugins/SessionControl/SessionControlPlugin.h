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

#ifndef __NEUROMORE_SESSIONCONTROLPLUGIN_H
#define __NEUROMORE_SESSIONCONTROLPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/String.h>
#include <Core/EventSource.h>
#include <PluginSystem/Plugin.h>
#include <Slider.h>
#include <Networking/NetworkServer.h>
//#include "SessionErrorWidget.h"
#include "PreSessionWidget.h"
#include "WhileSessionWidget.h"
#include "ClientInfoWidget.h"
#include "StageControlWidget.h"
#include "SessionInfoWidget.h"

#include <QTimer>
#include <QWidget>
#include <QMovie>


/**
 *
 *
 */
class SessionControlPlugin : public Plugin, private Core::EventSource, public Core::EventHandler
{
	Q_OBJECT
	public:
		SessionControlPlugin();
		virtual ~SessionControlPlugin();

		// overloaded
		const char* GetName() const override						{ return "Session Control"; }
		static const char* GetStaticTypeUuid()						{ return "c38f75d4-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override					{ return "PLUGIN_SessionControl"; }
		Plugin* Clone() override									{ return new SessionControlPlugin(); }
		bool NeedRegularUpdateInterface() override					{ return true; }
		void OnAfterLoadLayout() override;

		// overloaded main init function
		bool Init() override;
		void ReInit();

		// called regularily to update the information on the interface
		void UpdateInterface() override;

		// update widget visibility 
		void UpdateWidgets();


		// EVENTS
		void OnPreparedSession() override final;
		void OnSessionUserChanged(const User& user) override final;
		void OnActiveExperienceChanged(Experience* experience) override final				{ UpdateStartButton(); }
		void OnActiveClassifierChanged(Classifier* experience) override final				{ UpdateStartButton(); }
		void OnActiveStateMachineChanged(StateMachine* experience) override final			{ UpdateStartButton(); }
		void OnRemoveDevice(Device* device) override final;

	public slots:
		void ShowReport();
		void UpdateShowReportButton();

		void OnStart();
		void OnStop();
		void OnPause();
		void OnContinue();

		// client and network message callabacks
		void OnClientChanged   ( NetworkServerClient* client );
		void OnClientRemoved   ( NetworkServerClient* client );
		void OnMessageReceived ( NetworkServerClient* client, NetworkMessage* message );

		// cloud parameters
		void OnParametersSaved(bool success);
		void OnParametersLoaded(bool success);

	private:
		void Start();
		void Reset();
		
		void UpdateStartButton();

		// dialog stack
		PreSessionWidget*		mPreSessionWidget;
		WhileSessionWidget*		mWhileSessionWidget;
		StageControlWidget*		mStageControlWidget;
		ClientInfoWidget*		mClientInfoWidget;
		SessionInfoWidget*		mSessionInfoWidget;

		// additional codes for the sessioninfowidget
		enum EError
		{
			ERROR_NOCLASSIFIER, 
			ERROR_PERMISSIONS,
			ERROR_DEVICETEST,
			ERROR_PARAMETERS,
			ERROR_DESIGNERROR,
			ERROR_DEVICEMISSING,
		};

		// TODO: not the nicest way
		Core::String			mReportSessionId;

		void CheckStartRequirements();
		bool mCanStartSession;
		bool mLostClientError;
		bool mLostDeviceError;
};


#endif
