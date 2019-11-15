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

// include required headers
#include "NetworkTestClientPlugin.h"
#include <Core/LogManager.h>
#include <Networking/NetworkClient.h>
#include <Networking/NetworkServerClient.h>
#include <EngineManager.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>


#define TESTCLIENT_TCP_PORT	1337
#define TESTCLIENT_UDP_PORT	4242

using namespace Core;

// constructor
NetworkTestClientPlugin::NetworkTestClientPlugin() : Plugin(GetStaticTypeUuid())
{
	// Init stuff
	mLogOutput = NULL;
	mNetworkClient = NULL;
	mTcpServerPort = TESTCLIENT_TCP_PORT;
	mUdpClientPort = TESTCLIENT_UDP_PORT;
	mUdpServerPort = TESTCLIENT_UDP_PORT;
	mCurrentTest = None;
	mHostAddress = "127.0.0.1";
	mPacketSize = 512;
	mBurstSize = 10;
	mBurstRate = 100;
	mMaxAllowedLagTime = 5.0;	// 5 seconds max latency

	// Timer for sending packets
	connect( &mTimer, SIGNAL(timeout()), this, SLOT(OnTimerEvent()) );

	// The actual UDP packet that will be sent
	mPacket = new NetworkMessageData(mPacketSize);

	// Log Callback
	mLogCallback = new NetworkTestLogCallback(this);
	CORE_LOGMANAGER.AddLogCallback(mLogCallback);
}


// destructor
NetworkTestClientPlugin::~NetworkTestClientPlugin()
{
	CORE_LOGMANAGER.RemoveLogCallback( mLogCallback, false );
	delete mLogCallback;

	delete mPacket;
	delete mNetworkClient;
}


// spacer helper
QWidget* GetHSpacer()
{
	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	return spacer;
}

// init after the parent dock window has been created
bool NetworkTestClientPlugin::Init()
{
	//
	// four row layout: two for input, two for output
	// 
	
	// top row: inputs
	mReconnectButton = new QPushButton("(Re-)Connect");
	mStartButton = new QPushButton("Test Latency");
	mResetButton = new QPushButton("Stop / Reset");
	mHostAddressInput = new QLineEdit(mHostAddress.AsChar());				mHostAddressInput->setMaximumWidth(100);
	mTcpPortInput = new QLineEdit(String(TESTCLIENT_TCP_PORT).AsChar());	mTcpPortInput->setMaximumWidth(50);
	mUdpPortInput = new QLineEdit(String(TESTCLIENT_UDP_PORT).AsChar());	mUdpPortInput->setMaximumWidth(50);
	mPacketSizeInput = new QLineEdit(String(mPacketSize).AsChar());			mPacketSizeInput->setMaximumWidth(50);
	mBurstSizeInput = new QLineEdit(String(mBurstSize).AsChar());			mBurstSizeInput->setMaximumWidth(50);
	mBurstRateInput = new QLineEdit(String((int)mBurstRate).AsChar());		mBurstRateInput->setMaximumWidth(50);

	connect (mReconnectButton, SIGNAL(clicked()), this, SLOT(OnReconnect()));
	connect (mResetButton, SIGNAL(clicked()), this, SLOT(OnStopTest()));
	connect (mStartButton, SIGNAL(clicked()), this, SLOT(OnStartTest()));

	// row 1: server config
	QHBoxLayout* hLayout1 = new QHBoxLayout();
	hLayout1->setMargin(0);
	hLayout1->setSpacing(0);
	hLayout1->addWidget(mReconnectButton);
	hLayout1->addWidget(new QLabel("  to server at "));
	hLayout1->addWidget(mHostAddressInput);
	hLayout1->addWidget(new QLabel("  using TCP port "));
	hLayout1->addWidget(mTcpPortInput);
	hLayout1->addWidget(new QLabel("  and UDP port "));
	hLayout1->addWidget(mUdpPortInput);
	hLayout1->addWidget(GetHSpacer());

	// row2: test config
	QHBoxLayout* hLayout2 = new QHBoxLayout();
	hLayout2->setMargin(0);
	hLayout2->setSpacing(0);
	hLayout2->addWidget(mStartButton);
	hLayout2->addWidget(mResetButton);
	hLayout2->addWidget(GetHSpacer());
	hLayout2->addWidget(new QLabel("  Test configuration: "));
	hLayout2->addWidget(GetHSpacer());
	hLayout2->addWidget(new QLabel("Bytes/Packet: "));
	hLayout2->addWidget(mPacketSizeInput);
	hLayout2->addWidget(GetHSpacer());
	hLayout2->addWidget(new QLabel("Packets/Burst: "));
	hLayout2->addWidget(mBurstSizeInput);
	hLayout2->addWidget(GetHSpacer());
	hLayout2->addWidget(new QLabel("Bursts/Second: "));
	hLayout2->addWidget(mBurstRateInput);
	
	////////////////////////////////////////////////////
	
	// second row: status labels
	mPacketCounterLabel = new QLabel("");
	mPacketStatisticsLabel = new QLabel("");
	mThroughputLabel = new QLabel("");
	mLatencyLabel = new QLabel("");

	// row 3 : test statistics
	QHBoxLayout* hLayout3 = new QHBoxLayout();
	hLayout3->addWidget(mPacketCounterLabel);
	hLayout3->addWidget(GetHSpacer());
	hLayout3->addWidget(mPacketStatisticsLabel);
	hLayout3->addWidget(GetHSpacer());
	hLayout3->addWidget(mThroughputLabel);
	hLayout3->addWidget(GetHSpacer());
	hLayout3->addWidget(mLatencyLabel);

	// third row: log output window
	mLogOutput = new QTextEdit("");

	// vertical window layout (toobar 1 + toolbar 2 + console)
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addLayout(hLayout1);
	vLayout->addLayout(hLayout2);
	vLayout->addLayout(hLayout3);
	vLayout->addWidget(mLogOutput);

	// main widget containing the vertical layout
	QWidget* mainWidget = new QWidget();
	mainWidget->setLayout(vLayout);
	mDock->SetContents(mainWidget);

	// reset and update labels
	OnReset();
	UpdateInterface();
	return true;
}

// update interface information
void NetworkTestClientPlugin::UpdateInterface()
{
	
	// calculate num lsost packets
	if (mSequenceNumber > 1500)
		mNumPacketsLost = CalculateLostPackets(mSequenceNumber-1500, mSequenceNumber-500);

	// packet stats
	mTempString.Format("sn out %i / in %i", mSequenceNumber, mLastReceivedSequenceNumber);
	mPacketCounterLabel->setText(mTempString.AsChar());

	mTempString.Format("packets out %i  / in %i / lost %i / lost (1k) %i", mNumPacketsSent, mNumPacketsReceived, mNumPacketsLostTotal, mNumPacketsLost);
	mPacketStatisticsLabel->setText(mTempString.AsChar());
	
	mTempString.Format("avg latency (ms) %.05f", mAverageLatency*1000.0);
	mLatencyLabel->setText(mTempString.AsChar());


	// throughput
	if (mCurrentTest != None)
	{
		mTestDuration = mPerfTimer.GetTime().InSeconds();
		const double theoretical = (double)(mPacketSize * mBurstSize * mBurstRate) / 1024.0;
		const double realOutgoing = (double)mTotalBytesSent / 1024.0 / mTestDuration;
		const double realIncoming = (double)mTotalBytesReceived / 1024.0 / mTestDuration;
		mTempString.Format("kBit required %.2f / real out %.2f / real in %.2f", theoretical, realOutgoing, realIncoming);
	}
	else
	{
		// use current input values for calculation
		const double theoretical = (double)(mPacketSizeInput->text().toInt() * mBurstSizeInput->text().toInt() * mBurstRateInput->text().toDouble()) / 1024.0;
		mTempString.Format("kBit required %.2f / real out %.2f / real in %.2f", theoretical, 0.0, 0.0);
	}
	mThroughputLabel->setText(mTempString.AsChar());

}

// reset button clicked
void NetworkTestClientPlugin::OnReset()
{
	// reset everything to zero
	mSequenceNumber = 0;
	mLastReceivedSequenceNumber = 0;
	mNumPacketsSent = 0;
	mNumPacketsReceived = 0;
	mNumPacketsLost = 0;
	mNumPacketsLostTotal = 0;
	mTotalBytesSent = 0;
	mTotalBytesReceived = 0;
	mTestDuration = 0;
	mNumLatencyMeasurements = 0;
	mLatencySum = 0;
	mAverageLatency = 0;

	// clear hashtable
	mReceivedSequenceNumbers.clear();
	mReceivedSequenceNumbers.reserve(10E4);

	// enable buttons
	SetInterfaceEnabled(true);
}


// reconnect button clicked
void NetworkTestClientPlugin::OnReconnect()
{
	mHostAddress = FromQtString(mHostAddressInput->text());
	mTcpServerPort = (uint32)mTcpPortInput->text().toInt();
	mUdpClientPort = (uint32)mUdpPortInput->text().toInt();
	mUdpServerPort = mUdpClientPort+1;


	
	// create new instance (due to port changes and just to be safe)
	if (mNetworkClient != NULL)
	{
		mNetworkClient->CloseConnection();
		delete mNetworkClient;
	}

	mNetworkClient = new NetworkClient(mTcpServerPort,  mUdpServerPort, mUdpClientPort, "Network Test Client Studio Plugin", NetworkClient::ClientType::Studio);
	
	// connect signals
	connect (mNetworkClient, SIGNAL(ConnectionOpened()), this, SLOT(OnConnectionOpened()));
	connect (mNetworkClient, SIGNAL(ConnectionClosed()), this, SLOT(OnConnectionClosed()));
	connect (mNetworkClient, SIGNAL(MessageReceived(NetworkMessage*)), this, SLOT(OnPacketReceived(NetworkMessage*)));

	mNetworkClient->OpenConnection(mHostAddress);

	if (mNetworkClient->IsConnected() == false)
	{
		LogError("Connection failed, aborting Test!");
		return;
	}

	mCurrentTest = Latency;
}

// start button pressed
void NetworkTestClientPlugin::OnStartTest()
{
	OnReset();
	SetInterfaceEnabled(false);
	mStartButton->setEnabled(false);

	// get test config values from inputs
	mPacketSize = (uint32)mPacketSizeInput->text().toInt();
	mBurstSize = (uint32)mBurstSizeInput->text().toInt();
	mBurstRate = (uint32)mBurstRateInput->text().toDouble();
	
	PreparePacket(true);

	LogInfo("Starting Test NOW!");

	// start timers
	mTimer.start(1000.0 / mBurstRate);
	mPerfTimer.Reset();

	mCurrentTest = Latency;
}

// stop test
void NetworkTestClientPlugin::OnStopTest()
{
	if (mCurrentTest == None)
	{
		OnReset();
		mLogOutput->clear();
		return;
	}

	mCurrentTest = None;
	mTimer.stop();
	SetInterfaceEnabled(true);

	LogInfo ("Calculating total number of lost packets ..");
	// calculate total number of lost packets
	mNumPacketsLostTotal = CalculateLostPackets(1,mSequenceNumber);

	LogInfo ("----------------------- Test complete --------------------------");
	LogInfo ("Sent %i packets during %.01f seconds ", mNumPacketsSent, mTestDuration);
	LogInfo ("Lost %i (%.03f %%) packets", mNumPacketsLostTotal, (double)mNumPacketsLostTotal/(double)mNumPacketsSent);

}


// TCP connection just opened
void NetworkTestClientPlugin::OnConnectionOpened()
{
	mStartButton->setEnabled(true);
}


// TCP connection just closed
void NetworkTestClientPlugin::OnConnectionClosed()
{
	OnStopTest();
}


// event timer emits bursts of packets
void NetworkTestClientPlugin::OnTimerEvent()
{
	// send one burst of udp packets
	for (uint32 i = 0; i < mBurstSize; ++i)
	{
		// increase sequence number
		mSequenceNumber++;

		// update packet seqnumber and timestamp
		PreparePacket();

		// send packet
		const bool success = mNetworkClient->SendMessageUDP(mPacket);

		if (success == true)
		{
			// increase packet and byte counter 
			mNumPacketsSent++;
			mTotalBytesSent += mPacketSize;
		} 
		else
		{ 
			LogError ("Error sending UDP packet");
			mSequenceNumber--;
		}
	}
}


// prepare message before sending
void NetworkTestClientPlugin::PreparePacket(bool resize)
{
	// resize message payload, initialize with zero
	if (resize == true)
		mPacket->SetPayloadSize(mPacketSize);

	// set sequence number (increased elswhere)
	mPacket->SetSequenceNumber(mSequenceNumber);
	mPacket->WriteHeader();
	
	// get current timestamp
	const double currentTime = mPerfTimer.GetTime().InSeconds();

	// copy timestamp to the lowest four bytes
	char* payload = mPacket->GetPayload();
	memcpy(payload, &currentTime, sizeof(double));
}


// received packets
void NetworkTestClientPlugin::OnPacketReceived(NetworkMessage* message)
{
	// capture current timestamp immediately;
	const double currentTimestamp = mPerfTimer.GetTime().InSeconds();
	
	// check message type
	const uint32 messageType = message->GetHeader().mMessageType;
	const uint32 messageSize = message->GetHeader().mMessageLength;
	if (messageType == NetworkMessageData::TYPE_ID)
	{
		// cast to correct type
		NetworkMessageData* packet = static_cast<NetworkMessageData*>(message);
	
		mNumPacketsReceived++;
		mTotalBytesReceived += packet->GetPayloadSize();

		// check hashtable for sequence number
		const uint32 sequenceNumber = packet->GetSequenceNumber();

		auto itr = mReceivedSequenceNumbers.find(sequenceNumber);
		if (itr != mReceivedSequenceNumbers.end()) // duplicate packet
		{
			LogInfo("Received Duplicate Packet (SN %i). Dropping it!", sequenceNumber);
			return;
		}
		else
		{
			// store number in hashtable (hashmap, actually)
			mReceivedSequenceNumbers[sequenceNumber] = 0;

			mLastReceivedSequenceNumber = sequenceNumber;

			// parse and check packet timestamp
			double packetTimestamp;
			memcpy(&packetTimestamp, packet->GetPayload(), sizeof(double));
			
			// calculate latency
			const double delay = currentTimestamp - packetTimestamp; 
			mLatencySum += delay;
			mNumLatencyMeasurements++;
		}

	}
	else
	{
		LogInfo("Received Unknown Packet of type %i (size %i bytes)", messageType, messageSize);
	}

	// calc average latency
	if ( mNumLatencyMeasurements > 0)
		mAverageLatency = mLatencySum / mNumLatencyMeasurements;


}


uint32 NetworkTestClientPlugin::CalculateLostPackets(uint32 minSequenceNumber, uint32 maxSequenceNumber)
{
	uint32 numLostPackets = 0;

	// iterate over range, check hashtable, increase counter
	for (uint32 s = minSequenceNumber; s <= maxSequenceNumber; s++)
	{
		auto itr = mReceivedSequenceNumbers.find(s);
		if (itr == mReceivedSequenceNumbers.end())
			numLostPackets++;
	}
	
	return numLostPackets;
}


// interface disabling helper
void NetworkTestClientPlugin::SetInterfaceEnabled(bool enabled)
{
	// enable/disable all inputs except reset and button
	mReconnectButton->setEnabled(enabled);
	mStartButton->setEnabled(enabled);
	mHostAddressInput->setEnabled(enabled);
	mTcpPortInput->setEnabled(enabled);
	mUdpPortInput->setEnabled(enabled);
	mPacketSizeInput->setEnabled(enabled);
	mBurstSizeInput->setEnabled(enabled);
	mBurstRateInput->setEnabled(enabled);
}

