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

#ifndef __NEUROMORE_ENGINESTATUSPLUGIN_H
#define __NEUROMORE_ENGINESTATUSPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include <Networking/NetworkServer.h>
#include "../SessionControl/ClientInfoWidget.h"	// TODO move widget elswhere? remove from session control?
#include <QTextEdit>
#include <QLabel>


/**
 * engine status plugin
 */
class EngineStatusPlugin : public Plugin
{
	Q_OBJECT
	public:
		EngineStatusPlugin();
		virtual ~EngineStatusPlugin();

		// overloaded
		const char* GetName() const override											{ return "Engine Status"; }
		static const char* GetStaticTypeUuid()											{ return "c38f75e0-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override										{ return "PLUGIN_EngineStatus"; }
		Plugin* Clone() override														{ return new EngineStatusPlugin(); }
		bool NeedRegularUpdateInterface() override										{ return false; }

		// overloaded main init function
		bool Init() override;

		// called regularily to update the information on the interface
		void UpdateInterface() override;

		// log one line to the console textedit (used by log callback)
		void LogLine( const char* text )												{ if (mLogOutput != NULL) mLogOutput->append( text ); }

	private slots:
		void OnTimerTimeout();
		void OnClientListChanged();
		
	private:

		// studio and engine performance
		QLabel*				mEngineStatusLabel;
		QLabel*				mNeuroPerformanceLabel;
		QLabel*				mInterfacePerformanceLabel;
		QLabel*				mClassifierPerformanceLabel;
		QLabel*				mDeviceManagerPerformanceLabel;
		QLabel*				mOscRouterPerformanceLabel;
		QLabel*				mMemoryUsedLabel;
		QLabel*				mMemoryCurrentAllocsLabel;
		QLabel*				mMemoryAllocsLabel;
		QLabel*				mMemoryReallocsLabel;
		QLabel*				mMemoryFreesLabel;

		// network server information
		QLabel*				mServerStatusLabel;
		ClientInfoWidget*	mClientInfoWidget;
		QLabel*				mNumClientsLabel;
		QLabel*				mNumMessagesLabel;
		QLabel*				mNumBytesLabel;

		// osc listener information
		QLabel*				mOscListenerStatusLabel;
		QLabel*				mNumOscReceiversLabel;
		QLabel*				mNumOscPacketsReceivedLabel;
		QLabel*				mNumOscPacketPoolRXStatusLabel;
		QLabel*				mNumOscMessagesRouterStatusLabel;
		QLabel*				mNumOscPacketsSentLabel;
		QLabel*				mNumOscPacketPoolTXStatusLabel;

		// debug log
		QTextEdit*			mLogOutput;

		// helpers
		QWidget*			GetHSpacer();
		Core::String		mTempString;

		QTimer				mUpdateTimer;
		Core::Timer			mUpdateDeltaTimer;
		uint32				mLastMemoryUsed;
		uint32				mLastNumTotalAllocs;
		uint32				mLastNumAllocs;
		uint32				mLastNumReallocs;
		uint32				mLastNumFrees;
		uint32				mLastNumOscPacketsReceived;
		uint32				mLastNumOscPacketsSent;

	private:

		class EngineStatusLogCallback : public QObject, public Core::LogCallback
		{
			public:
				enum { TYPE_ID = 0x023337 };
				EngineStatusLogCallback(EngineStatusPlugin* reference )	: LogCallback()					{ mPlugin = reference; }
				virtual ~EngineStatusLogCallback()														{}
				uint32 GetType() const																	{ return TYPE_ID; }

				void Log(const char* text, Core::ELogLevel logLevel)									{ mPlugin->LogLine(text); }

			private:
				EngineStatusPlugin*	mPlugin;
		};

		EngineStatusLogCallback* mLogCallback;

};


#endif
