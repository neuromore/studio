/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_OSCPACKETLISTENER_H
#define __NEUROMORE_OSCPACKETLISTENER_H

// include required headers
#include "../QtBaseConfig.h"
#include <Config.h>
#include <Core/String.h>
#include <Core/Mutex.h>
#include <Networking/OscMessageQueue.h>
#include <Networking/OscPacket.h>
#include <Networking/OscPacketPool.h>
#include <Networking/OscPacketParser.h>

#include <QUdpSocket>
#include <QTimer>
#include <QNetworkSession>


// a simple OscServer that parses incoming osc messages and puts them into a queue
class QTBASE_API OscServer : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		OscServer(uint32 listenPort, uint32 remoteHostPort);
		virtual ~OscServer();

		void Reset();
		void ReInit()									{ Reset(); Init(); }
		void Init();
		bool IsPortOpen()	const						{ return mUdpSocket->isOpen(); }

		// listening
		void SetListenPort(uint32 port)					{ mUdpPort = port; }
		uint32 GetListenPort()	const					{ return mUdpPort; }

		// remote host
		void SetRemoteHost(QHostAddress host)			{ mRemoteHost = host; }
		QHostAddress GetRemoteHost() const				{ return mRemoteHost; }
		void SetLocalEndpoint(QHostAddress host)		{ mLocalEndpoint = host; }
		QHostAddress GetLocalEndpoint() const			{ return mLocalEndpoint; }

		void SetRemoteUdpPort(uint32 port)				{ mRemoteUdpPort = port; }
		uint32 GetRemoteUdpPort()	const				{ return mRemoteUdpPort; }
		
		// send osc messages
		OscPacket* GetEmptyOscPacket()					{ return mPacketPool.AcquirePacket(); }
		void SendOscMessageQueued(OscPacket* packet);

		// osc statistics
		uint32 GetNumPooledPacketsUsed() const			{ return mPacketPool.GetNumUsedPackets(); }
		uint32 GetNumPooledPacketsFree() const			{ return mPacketPool.GetNumFreePackets(); }
		uint32 GetNumPacketsTransmitted() const			{ return mNumPacketsTransmitted; }
		uint32 GetNumPacketsReceived() const			{ return mNumPacketsReceived; }
		uint32 GetNumBytesTransmitted() const			{ return mNumBytesTransmitted; }
		uint32 GetNumBytesReceived() const				{ return mNumBytesReceived; }
		
		
	private slots:
		void OnReceiveUdpDatagram();
		void OnRealtimeUpdate();
		
	private:
		void SendUdpDatagram(const char* data, uint32 numBytes);
		void RemoveProcessedPackets();

		// Input
		QUdpSocket*				mUdpSocket;						// The UDP socket for receiving OSC messages
		QUdpSocket*				mUdpOutSocket;						// The UDP socket for sending OSC messages
		uint32					mUdpPort;						// UDP port on local machine for listening

		// Output
		QHostAddress			mRemoteHost;					// remote host that receives outgoing packets
		uint32					mRemoteUdpPort;					// udp port on remote host
		QHostAddress			mLocalEndpoint;					// local end point to bind to
		
		OscPacketPool			mPacketPool;					// packet memory pool for received and transmitted packets

		QTimer*					mTimer;

		// Statistics
		uint32					mNumPacketsTransmitted;
		uint32					mNumPacketsReceived;
		uint32					mNumBytesTransmitted;
		uint32					mNumBytesReceived;
};


#endif
