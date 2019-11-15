/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "OscServer.h"
#include "OscServer.h"
#include <Core/LogManager.h>
#include <EngineManager.h>

#include <QNetworkConfigurationManager>


using namespace Core;

// constructor
OscServer::OscServer(uint32 listenPort, uint32 sendingPort)
{
	LogDetailedInfo("Constructing OSC server ...");
	mUdpSocket	= NULL;
	mUdpOutSocket	= NULL;
	mUdpPort = listenPort;
	mRemoteHost = QHostAddress::LocalHost;
	mLocalEndpoint = QHostAddress::Null;
	mRemoteUdpPort = sendingPort;

	mNumPacketsReceived = 0;
	mNumPacketsTransmitted = 0;
	mNumBytesReceived = 0;
	mNumBytesTransmitted = 0;
	
	// initialize the osc packet pool
	mPacketPool.Resize( 1024 );
}


// destructor
OscServer::~OscServer()
{
	LogDetailedInfo("Destructing OSC server ...");
	Reset();
}


void OscServer::Reset()
{
	if (mUdpSocket != NULL)
		mUdpSocket->close();
	delete mUdpSocket;
	delete mUdpOutSocket;

	mUdpSocket = NULL;
	mUdpOutSocket = NULL;

	mNumPacketsReceived = 0;
	mPacketPool.Clear();

	if (mTimer != NULL)
	{
		mTimer->stop();
		mTimer->deleteLater();
	}

	// zero statistics
	mNumPacketsReceived = 0;
	mNumPacketsTransmitted = 0;
	mNumBytesReceived = 0;
	mNumBytesTransmitted = 0;
}

void OscServer::Init()
{
	LogDetailedInfo("Initializing OSC listener ...");

	// create the receiving socket
	mUdpSocket = new QUdpSocket(this);
	mUdpOutSocket = new QUdpSocket(this);

	// initialize the socket bind mode, reuse UDP adresses
	QAbstractSocket::BindMode bindMode = QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint;

	// bind the udp sockets
        //if (mLocalEndpoint.isNull() == true)
                mUdpSocket->bind(mUdpPort, bindMode);
        //else 
        //        mUdpSocket->bind(mLocalEndpoint, mUdpPort, bindMode);
        
 
        connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(OnReceiveUdpDatagram()));

	mUdpOutSocket->bind(mLocalEndpoint, mRemoteUdpPort, bindMode);

	const uint32 FPS = 100;
	mTimer = new QTimer(this);
	mTimer->setTimerType(Qt::PreciseTimer);
	mTimer->start(1000 /FPS);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(OnRealtimeUpdate()));
}


void OscServer::OnRealtimeUpdate()
{
	// hackfix: call the slot on a regular basis
	// timer fix for the Qt signal bug
	// TODO: HACK: REMOVEME
	// Qt 5.5.0 doesn't seem to be able to handle a lot of fastly incoming network messages; the signal stops firing in this case
	OnReceiveUdpDatagram();


	// send outgoing packets
	Array<OscPacket*>& packetQueue = GetOscMessageRouter()->GetOutputPacketQueue();
	const uint32 numPackets = packetQueue.Size();
	for (uint32 i = 0; i < numPackets; ++i)
	{
		// get packet
		OscPacket* packet = packetQueue[i];

		// send packet out to client
		SendUdpDatagram(packet->GetData(), packet->GetSize());

		// mark all messages in packet as processed
		packet->SetIsReady();
	}
	packetQueue.Clear();
}


// read incoming osc messages that arrive on the UDP port
void OscServer::OnReceiveUdpDatagram()
{
	// there may be more than one datagram in the socket buffer
	while (mUdpSocket->hasPendingDatagrams())
	{
		// receive udp datagram
		QHostAddress senderAddress;
		quint16 senderPort;
		uint32 packetSize = mUdpSocket->pendingDatagramSize();
		CORE_ASSERT(packetSize <= 1024);
		
		// get an osc packet from pool
		OscPacket1k* packet = mPacketPool.AcquirePacket();
		
		// read data into packet
		mUdpSocket->readDatagram(packet->GetData(), packetSize, &senderAddress, &senderPort);
		packet->Read(packetSize);
		
		// push packet into packet router
		GetEngine()->GetOscMessageRouter()->RouteMessages(packet->GetOscPacketParser());

		mNumPacketsReceived++;
		mNumBytesReceived += packetSize;
	}
	
	// push processed packets back into the pool
	// we do this only after a the pool reaches a level (e.g. 10%) to be more efficient 
	const float scrubFactor = 0.1f;
	if (mPacketPool.GetNumUsedPackets() > scrubFactor * mPacketPool.GetNumPackets())
		mPacketPool.ReleaseProcessedPackets();
}


// send UDP network datagram to the remote host
void OscServer::SendUdpDatagram(const char* data, uint32 numBytes)
{
	// transmit the data to our remote host
	qint64 bytesSend = mUdpOutSocket->writeDatagram(data, numBytes, mRemoteHost, mRemoteUdpPort);

	// count bytes
	if (bytesSend > 0)
	{
		mNumPacketsTransmitted++;
		mNumBytesTransmitted += bytesSend;
	}

	// in case this ever happens
    // TODO: self comparison?? bug?
	//if (numBytes != numBytes)
	//	LogError("OscServer: error sending OSC packet (writeDatagram wrote less bytes than the packet has)");

}
