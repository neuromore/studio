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

#ifndef __NEUROMORE_NETWORKTESTCLIENTPLUGIN_H
#define __NEUROMORE_NETWORKTESTCLIENTPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/LogCallbacks.h>
#include <PluginSystem/Plugin.h>
#include <Networking/NetworkClient.h>
#include <Networking/NetworkMessageData.h>
#include <unordered_map>

#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

/**
 * network client plugin for testing our networking
 *
 */
class NetworkTestClientPlugin : public Plugin
{
	Q_OBJECT
	public:
		NetworkTestClientPlugin();
		virtual ~NetworkTestClientPlugin();

		// overloaded
		const char* GetName() const override											{ return "Network Test Client"; }
		static const char* GetStaticTypeUuid()											{ return "c38f75df-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override										{ return "PLUGINS_Development"; }
		Plugin* Clone() override														{ return new NetworkTestClientPlugin(); }
		bool NeedRegularUpdateInterface() override										{ return true; }

		// overloaded main init function
		bool Init() override;

		// called regularily to update the information on the interface
		void UpdateInterface() override;

		// log one line to the console textedit (used by log callback)
		void LogLine( const char* text )												{ if (mLogOutput != NULL) mLogOutput->append( text ); }

		// resize message and update timestamp
		void PreparePacket(bool resize=false);

	private slots:
		void OnReconnect();
		void OnReset();
		
		void OnStartTest();
		void OnStopTest();
		void OnTimerEvent();

		// slots for network client signals
		void OnConnectionOpened();
		void OnConnectionClosed();
		void OnPacketReceived(NetworkMessage* message);

	private:

		enum TestType
		{
			None,
			Latency,
			Throughput,
		};

		TestType			mCurrentTest;				// currently running type of test

		// Networking and config
		NetworkClient*		mNetworkClient;

		uint32				mTcpServerPort;
		uint32				mUdpServerPort;
		uint32				mUdpClientPort;
		Core::String		mHostAddress;

		NetworkMessageData* mPacket;					// the packet that will be sent
		QTimer				mTimer;						// emits packets with a specified frequency

		// test config
		uint32				mPacketSize;				// number of bytes per packets (payload excluding header)
		uint32				mBurstSize;					// number of packets per burst
		double				mBurstRate;					// number of bursts per second
		double				mMaxAllowedLagTime;			// number of seconds the packets are allowed to be out of order

		// sequence number tracking
		uint32 CalculateLostPackets(uint32 minSequenceNumber, uint32 maxSequenceNumber);
		uint32				mSequenceNumber;			// counter for outgoing packets
		uint32				mLastReceivedSequenceNumber;// sequence number of the last incoming packet
		std::unordered_map<uint32,uint32> mReceivedSequenceNumbers; // hashtable containing all received sequence numbers (value is not used)

		// packet stats
		uint32				mNumPacketsSent;			// number of sent packets
		uint32				mNumPacketsReceived;		// number of received packets
		uint32				mNumPacketsLost;			// number of packets that got lost (last 1000 sequence numbers)
		uint32				mNumPacketsLostTotal;		// total number of packets that got lost
		// throughput
		uint32				mTotalBytesSent;			// bytes sent
		uint32				mTotalBytesReceived;		// bytes received
		double				mTestDuration;
		// latency
		double				mLatencySum;
		uint32				mNumLatencyMeasurements;
		double				mAverageLatency;


		// UI Elements top row
		QLineEdit*			mHostAddressInput;
		QLineEdit*			mTcpPortInput;
		QLineEdit*			mUdpPortInput;
		QPushButton*		mReconnectButton;
		QLineEdit*			mPacketSizeInput;
		QLineEdit*			mBurstSizeInput;
		QLineEdit*			mBurstRateInput;
		QPushButton*		mStartButton;
		QPushButton*		mResetButton;

		// UI Elements second row
		QLabel*				mPacketCounterLabel;
		QLabel*				mPacketStatisticsLabel;
		QLabel*				mThroughputLabel;
		QLabel*				mLatencyLabel;
		
		// log window
		QTextEdit*			mLogOutput;
		
		void SetInterfaceEnabled (bool enabled = true);

		// helpers
		Core::String		mTempString;
		Core::Timer			mPerfTimer;

	private:
		class NetworkTestLogCallback : public QObject, public Core::LogCallback
		{
			public:
				enum { TYPE_ID = 0x023337 };
				NetworkTestLogCallback(NetworkTestClientPlugin* reference )	: LogCallback()				{ mPlugin = reference; }
				virtual ~NetworkTestLogCallback()														{}
				uint32 GetType() const																	{ return TYPE_ID; }

				void Log(const char* text, Core::ELogLevel logLevel)									{ mPlugin->LogLine(text); }

			private:
				NetworkTestClientPlugin*	mPlugin;
		};

		NetworkTestLogCallback* mLogCallback;

};


#endif
