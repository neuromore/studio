/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "NetworkServer.h"
#include <Core/LogManager.h>
#include <Networking/OscFeedbackPacket.h>
#include <EngineManager.h>
#include "../QtBaseConfig.h"
#include "NetworkMessageData.h"
#include "NetworkServerClient.h"


using namespace Core;

// constructor
NetworkServer::NetworkServer(uint32 portTCP, uint32 portUDPServer, uint32 portUDPClient)
{
	LogDetailedInfo("Constructing Network Server...");

	mTcpServer = NULL;
	mUdpSocket				= NULL;
	mAnnounceTimer			= NULL;
	mRealtimeUpdateTimer	= NULL;
	mStarterClient			= NULL;
	mVisualizationClient	= NULL;
	mEventHandler			= NULL;

	// zero statistics
	mNumMessagesTransmitted = 0;
	mNumMessagesReceived = 0;
	mNumBytesTransmitted = 0;
	mNumBytesReceived = 0;

	// ports
	mTcpServerPort = portTCP;
	mUdpServerPort = portUDPServer;
	mUdpClientPort = portUDPClient;

	// sensible defaults
	mIsListenerEnabled = true;
	mIsServerAnnounceEnabled = true;
	mBroadcastServerAnnounce = false;
	mAnnounceInterval = 1;			// announce every second
	SetClientTimeout(5);			// disconnect client after 10 seconds
	mRealtimeUpdateRate = 60;		// 60 UDP updates per second
}


// destructor
NetworkServer::~NetworkServer()
{
	LogDetailedInfo("Destructing Network Server...");
	Reset();
}


void NetworkServer::Reset()
{
	// delete event handler
	if (mEventHandler != NULL)
	{
		CORE_EVENTMANAGER.RemoveEventHandler(mEventHandler);
		delete mEventHandler;
	}

	// get rid of all clients
	ClearClients();

	// remove all announce packets
	const uint32 numPackets = mAnnouncePackets.Size();
	for (uint32 i = 0; i<numPackets; ++i)
		delete mAnnouncePackets[i];
	mAnnouncePackets.Clear();

	// destruct announce timer
	if (mAnnounceTimer != NULL)
		mAnnounceTimer->stop();
	delete mAnnounceTimer;
	mAnnounceTimer = NULL;

	// destruct real-time timer
	if (mRealtimeUpdateTimer != NULL)
		mRealtimeUpdateTimer->stop();
	delete mRealtimeUpdateTimer;
	mRealtimeUpdateTimer = NULL;

	// destruct tcp server
	if (mTcpServer != NULL)
		mTcpServer->close();
	delete mTcpServer;
	mTcpServer = NULL;

	delete mUdpSocket;
	mUdpSocket = NULL;

	mNumBytesReceived = 0;
	mNumBytesTransmitted = 0;
	mNumMessagesReceived = 0;
	mNumMessagesTransmitted = 0;
}



// initialize the server
void NetworkServer::Init()
{
	LogDetailedInfo("Initializing Network Server...");

	// register with event manager
	mEventHandler = new ClientEventHandler(this);
	CORE_EVENTMANAGER.AddEventHandler(mEventHandler);

	// create the sending socket
	mUdpSocket = new QUdpSocket(this);

	// initialize the socket bind mode, reuse UDP adresses
	QAbstractSocket::BindMode bindMode = QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint;

	// bind the udp sockets
	mUdpSocket->bind(mUdpServerPort, bindMode);
	connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(OnReceiveUdpDatagram()));

	// initialize TCP server
	mTcpServer = new QTcpServer(this);
	connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
	connect(mTcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(OnNetworkError(QAbstractSocket::SocketError)));

	//--------------------------------------------------------------------------------------------------------------------------------------------------

	// init server announce
	// initialize local ip addresses
	FindLocalIPAddresses();

	// initialize server announce packets (one per ip address)
	UpdateAnnouncePackets();
			
	if (mIsServerAnnounceEnabled == true)
	{
		mAnnounceTimer = new QTimer(this);
		mAnnounceTimer->setInterval( 1000 * mAnnounceInterval );
		connect( mAnnounceTimer, SIGNAL( timeout() ), this, SLOT( OnTimerSearchForClients() ) );
		mAnnounceTimer->start();
	}

	// send data down to clients in realtime
	mRealtimeUpdateTimer = new QTimer(this);
	mRealtimeUpdateTimer->setInterval( 1000 / mRealtimeUpdateRate );
	connect( mRealtimeUpdateTimer, SIGNAL(timeout()), this, SLOT(OnTimerSendRealtimeUpdateToClients()) );

	if (mIsListenerEnabled == true)
	{
		if (mTcpServer->isListening() == false)
			mTcpServer->listen(QHostAddress::AnyIPv4, mTcpServerPort);
	}
}


void NetworkServer::UpdateAnnouncePackets()
{
	// remove all announce packets
	const uint32 numPackets = mAnnouncePackets.Size();
	for (uint32 i = 0; i<numPackets; ++i)
		delete mAnnouncePackets[i];
	mAnnouncePackets.Clear();

	// initialize server announce packets (one per ip address)
	const uint32 numIPs = mLocalIPAddresses.Size();
	for (uint32 i = 0; i<numIPs; i++)
	{
		OscPacket* announce = new OscPacket(500);
		announce->BeginWrite();
		announce->BeginMessage("/studio/ip");
		announce->WriteValue(FromQtString(mLocalIPAddresses[i].toString()).AsChar());
		announce->EndMessage();
		announce->EndWrite();

		mAnnouncePackets.Add(announce);
	}
}


// find and fill all local ip addresses
void NetworkServer::FindLocalIPAddresses()
{
	QHostAddress					ipAddress;
	String							ipAddressString;
	QNetworkConfigurationManager	networkConfigManager;
	QNetworkConfiguration			networkConfig = networkConfigManager.defaultConfiguration();
	QNetworkSession					networkSession(networkConfig);
	QNetworkInterface				networkInterface = networkSession.interface();

	// get the list of all ip addresses (this provides at least two ip addresses (1x IPv4 and 1x IPv6) per machine)
	QList<QHostAddress> ipAddressList = networkInterface.allAddresses();
	
	// clear list first
	mLocalIPAddresses.Clear();

	if (mBroadcastServerAnnounce == false)
	{
		mLocalIPAddresses.Add( QHostAddress::LocalHost );
		return;
	}

	// iterate through all ip addresses in the list and filter out loopback and link-local addresses
	QList<QHostAddress>::const_iterator ipAddressIterator;
	for (ipAddressIterator = ipAddressList.begin(); ipAddressIterator != ipAddressList.end(); ++ipAddressIterator)
	{
		// get the current ip address
		ipAddress = *ipAddressIterator;
		ipAddressString = FromQtString( ipAddress.toString() );
		
		// FIXME: we ignore ipv6 for now
		if (ipAddress.protocol() == QAbstractSocket::IPv6Protocol)
			continue;

		// if broadcast is disable we allow only loopback devices
		if (mBroadcastServerAnnounce == false && ipAddress == QHostAddress::LocalHost)
			continue;

		// don't use link-local addresses and loopback adapter
		if (ipAddress.isInSubnet(QHostAddress("169.254.0.0"), 16) || ipAddress.isLoopback() == true)
			continue;

		// add the ip address to our list
		mLocalIPAddresses.Add( ipAddress );
	}

	// log
	const uint32 numLocalIps = mLocalIPAddresses.Size();
	LogInfo("Local ips: %i", numLocalIps);
	for (uint32 i=0; i<numLocalIps; ++i)
		LogInfo( "%i: %s", i, mLocalIPAddresses[i].toString().toLatin1().data() );
}


// broadcast UDP network datagram	
void NetworkServer::BroadcastUdpDatagram(const char* data, uint32 numBytes)
{
	// broadcast datagram to the network
	qint64 bytesSend = mUdpSocket->writeDatagram(data, numBytes, QHostAddress::Broadcast, mUdpClientPort);

	// counte bytes
	if (bytesSend > 0)
	{
		mNumMessagesTransmitted++;
		mNumBytesTransmitted += bytesSend;
	}
}


// send UDP network datagram to all clients of specified type
// if type=0 then the message will be sent to all clients
void NetworkServer::SendUdpDatagram(const char* data, uint32 numBytes, NetworkClient::ClientType type)
{
	// flag to check if one of the clients is localhost
	bool sentToLocalhost = false;

	// iterate over all clients and send over the given datagram if specified type matches. 
	const uint32 numClients = GetNumClients();
	for (uint32 i = 0; i < numClients; ++i)
	{
		if (type == NetworkClient::Undefined || mClients[i]->mClientType == type)
		{
			QHostAddress host = mClients[i]->mHostAddress;
			qint64 bytesSend = mUdpSocket->writeDatagram( data, numBytes, host, mUdpClientPort );
			
			// count bytes
			if (bytesSend > 0)
			{
				mNumMessagesTransmitted++;
				mNumBytesTransmitted += bytesSend;
			}

			// set flag if this is localhost
			if (host == QHostAddress::LocalHost)
				sentToLocalhost = true;
		}
	}

	// send to localhost, if no local client is running
	if (sentToLocalhost == false)
	{
		//LogInfo( "Sending %i '%s' bytes to localhost at port %i", numBytes, data, mUdpClientPort );
		qint64 bytesSend = mUdpSocket->writeDatagram( data, numBytes, QHostAddress::LocalHost, mUdpClientPort );

		// count bytes
		if (bytesSend > 0)
		{
			mNumMessagesTransmitted++;
			mNumBytesTransmitted += bytesSend;
		}
	}
}


// read incoming UDP messages and connect to server
void NetworkServer::OnReceiveUdpDatagram()
{
	// there may be more than one datagram
	while (mUdpSocket->hasPendingDatagrams())
	{
		// receive udp datagram
		QHostAddress senderAddress;
		quint16 senderPort;
		QByteArray* inData = new QByteArray();	// deleted by message object
		NetworkMessage::Header header;
		uint32 messageSize = mUdpSocket->pendingDatagramSize();
		
		if (messageSize == 0)
			return;

		// TODO check if datagrams always arrive as a whole
		//if (messageSize < sizeof(NetworkMessage::Header))
		//	return; // TODO error, datagram too short to contain a header // should not happen due to UDP datagram size?

		inData->resize(messageSize);
		mUdpSocket->readDatagram(inData->data(), messageSize, &senderAddress, &senderPort);

		// parse message header
		memcpy(&header, inData->data(), sizeof(NetworkMessage::Header));

		// check if datagram was received completely
		const uint32 expectedMessageSize =  header.mMessageLength;
		if (messageSize != expectedMessageSize)
		{
			LogWarning ("Warning: Received a bad packet (size missmatch, got %i bytes but header says %i)", messageSize, expectedMessageSize);
			continue;
		}

		// upcast message (only types that are used in praxis)
		if (header.mMessageType == NetworkMessageData::TYPE_ID)
		{
			NetworkMessageData* message = new NetworkMessageData(inData);
			
			// increase stats
			mNumMessagesReceived++;
			mNumBytesReceived += message->GetPayloadSize();

			emit MessageReceived(NULL, message);
		}
		//else if (header.mMessageType == NetworkMessageAnnounce::TYPE_ID)
		//{
		//	NetworkMessageData* message = new NetworkMessageData(inData);
		//	
		//	// increase stats
		//	mNumMessagesReceived++;
		//	mNumBytesReceived += message->GetPayloadSize();

		//	// ignore server announce packages (no use in the server)
		//	delete message;
		//}
		else
		{
			LogWarning ("Received an unexpected message via UDP (TypeID=%i).", header.mMessageType);
		}
	}
}



// enable/disable announce timer
void NetworkServer::SetServerAnnounceEnabled(bool enable)
{
	mIsServerAnnounceEnabled = enable;

	if (mAnnounceTimer != NULL)
	{
		if (enable)
			mAnnounceTimer->start();
		else
			mAnnounceTimer->stop();
	}
}


// enable/disable realtime update timer
void NetworkServer::SetRealtimeFeedbackEnabled(bool enable)
{
	if (mRealtimeUpdateTimer == NULL)
		return;

	if (enable)
		mRealtimeUpdateTimer->start();
	else
		mRealtimeUpdateTimer->stop();
}



//
// Timer events
//

// broadcast server announce via UDP
void NetworkServer::OnTimerSearchForClients()
{
	// get the number of ip addresses and iterate through them
	const uint32 numIPAddresses = mLocalIPAddresses.Size();

	if (mBroadcastServerAnnounce == true)
	{
		// broadcast announce packet containing this ip
		for (uint32 i=0; i<numIPAddresses; ++i)
		{
			BroadcastUdpDatagram( mAnnouncePackets[i]->GetData(), mAnnouncePackets[i]->GetSize() );
		}
	}
	else
	{
		for (uint32 i=0; i<numIPAddresses; ++i)
		{
			SendUdpDatagram( mAnnouncePackets[i]->GetData(), mAnnouncePackets[i]->GetSize() );
		}
	}

	// increase failed ping count for all active clients
	IncreaseClientPingCount();

	// remove all inactive clients
	AutoRemoveClients();
}


// send data pack to visualization clients 
void NetworkServer::OnTimerSendRealtimeUpdateToClients()
{
	mOscFeedbackPacket.Write();

	// send our realtime messages
	SendUdpDatagram(mOscFeedbackPacket.GetData(), mOscFeedbackPacket.GetSize());
}


//
// TCP/UDP events
//

// accept incoming TCP connection, add new client
void NetworkServer::OnNewConnection()
{
	// get socket of new connection
	QTcpSocket* socket = mTcpServer->nextPendingConnection();
	connect( socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnNetworkError(QAbstractSocket::SocketError) ) );

	QByteArray inData;
	QHostAddress senderIP;

	NetworkServerClient* client = new NetworkServerClient(socket);
	AddClient(client); 
}


void NetworkServer::OnNetworkError(QAbstractSocket::SocketError socketError)
{
	String errorMessage = FromQtString(mTcpServer->errorString());
	LogDebug("Network Error in Server: %s (ErrorCode: %i)", errorMessage.AsChar(), socketError);

	emit ServerError(errorMessage);
}


//
// Client management
//

// add a client
void NetworkServer::AddClient(NetworkServerClient* client)
{
	mClients.Add( client );

	// forward clientchanged and messageevent signal
	connect( client, SIGNAL( ConfigChanged( NetworkServerClient* ) ), this, SLOT( OnClientChanged( NetworkServerClient* ) ) );
	connect( client, SIGNAL( Disconnected( NetworkServerClient* ) ), this, SLOT( OnClientDisconnected( NetworkServerClient* ) ) );
	connect( client, SIGNAL( MessageReceived( NetworkServerClient*, NetworkMessage* ) ), this, SIGNAL( MessageReceived( NetworkServerClient*, NetworkMessage* ) ) );

	DeterminePrimaryClients();

	// emit the client added signal
	emit ClientAdded(client);
}


// get rid of all clients
void NetworkServer::ClearClients()
{
	// remove all clients (the right way)
	while (mClients.Size() > 0)
		RemoveClient(mClients.GetLast());
}


// get number of clients that are of the specified client type
uint32 NetworkServer::GetNumClients(NetworkClient::ClientType type) const
{
	uint32 numMatched = 0;

	// get the number of clients, iterate, check type and count them
	const uint32 numClients = mClients.Size();
	for (uint32 i = 0; i < numClients; ++i)
		if (mClients[i]->mClientType == type)
			numMatched++;
	
	return numMatched;
}


// get n'th client of specified type
NetworkServerClient* NetworkServer::GetClient(NetworkClient::ClientType type, uint32 index) const
{
	CORE_ASSERT(index < GetNumClients(type));		

	uint32 numMatched = 0;
	
	const uint32 numClients = mClients.Size();
	for (uint32 i = 0; i < numClients; ++i)
	{
		if (mClients[i]->mClientType == type)
		{
			if (numMatched == index)
				return mClients[i];

			numMatched++;
		}
	}

	return NULL;
}


// find client based on the given host address
NetworkServerClient* NetworkServer::GetClientByHostAddress(const QHostAddress& hostAddress) const
{
	// get the number of clients and iterate over them
	const uint32 numClients = mClients.Size();
	for (uint32 i = 0; i<numClients; ++i)
	{
		if (mClients[i]->mHostAddress == hostAddress)
			return mClients[i];
	}

	// no client with the given host address found
	return NULL;
}

		
void NetworkServer::SetClientTimeout(double seconds)
{ 
	mClientTimeout = seconds;  

	mMaxFailedPings = (uint32)ceil(seconds / mAnnounceInterval); 

	// clamp to two pings minimum
	if (mMaxFailedPings < 2)
		mMaxFailedPings = 2;
}


// increase number of failed ping counts for all clients
void NetworkServer::IncreaseClientPingCount()
{
	// get the number of clients, iterate over them and increase their number of failed pings
	const uint32 numClients = mClients.Size();
	for (uint32 i = 0; i < numClients; ++i)
		mClients[i]->IncreasePingCount();
}


// auto remove inactive clients
void NetworkServer::AutoRemoveClients()
{
	// get the number of clients and the max index
	const int32 numClients = (int32)mClients.Size();
	const int32 maxIndex = numClients - 1;

	// iterate over the clients from back to front
	for (int32 i = maxIndex; i >= 0; i--)
	{
		NetworkServerClient* client = mClients[i];
		if (client->mNumFailedPings >= mMaxFailedPings)
			RemoveClient(client);
	}
}


void NetworkServer::RemoveClient(NetworkServerClient* client)
{
	// find client in list, remove it
	const int32 numClients = (int32)mClients.Size();
	for (int32 i = 0; i < numClients; i++)
	{
		if (mClients[i] == client)
		{
			mClients.Remove(i);
			delete client;
			DeterminePrimaryClients();
			emit ClientRemoved( client );
		}
	}
}


void NetworkServer::OnClientChanged(NetworkServerClient* client)
{
	DeterminePrimaryClients();
	emit ClientChanged(client);
}


void NetworkServer::OnClientDisconnected(NetworkServerClient* client)
{
	RemoveClient(client);
}


void NetworkServer::DeterminePrimaryClients()
{
	mStarterClient = NULL;
	mVisualizationClient = NULL;

	uint32 numStarterClients = GetNumClients(NetworkClient::Starter);
	uint32 numVisClients = GetNumClients(NetworkClient::Visualization);

	// use the starter client that connected first
	if (numStarterClients > 0)
		mStarterClient = GetClient(NetworkClient::Starter, 0);
	
	// determine visualization client: use latest connected vis client
	if (numVisClients > 0)
		mVisualizationClient = GetClient(NetworkClient::Visualization, numVisClients - 1);
}



// server stats

uint32 NetworkServer::GetNumMessagesTransmitted() const
{
	uint32 total = 	mNumMessagesTransmitted; 
	const uint32 numClients = GetNumClients();
	for (uint32 i=0; i<numClients; i++)
		total += GetClient(i)->GetNumMessagesTransmitted();

	return total;
}


uint32 NetworkServer::GetNumMessagesReceived() const
{
	uint32 total = 	mNumMessagesReceived; 
	const uint32 numClients = GetNumClients();
	for (uint32 i=0; i<numClients; i++)
		total += GetClient(i)->GetNumMessagesReceived();

	return total;
}


uint32 NetworkServer::GetNumBytesTransmitted() const
{
	uint32 total = 	mNumBytesTransmitted; 
	const uint32 numClients = GetNumClients();
	for (uint32 i=0; i<numClients; i++)
		total += GetClient(i)->GetNumBytesTransmitted();

	return total;
}


uint32 NetworkServer::GetNumBytesReceived() const
{
	uint32 total = 	mNumBytesReceived; 
	const uint32 numClients = GetNumClients();
	for (uint32 i=0; i<numClients; i++)
		total += GetClient(i)->GetNumBytesReceived();

	return total;
}


void NetworkServer::SetBroadcastServerAnnounce(bool enabled)				
{
	mBroadcastServerAnnounce = enabled;
	
	FindLocalIPAddresses();
	UpdateAnnouncePackets();
}


void NetworkServer::ReInit()
{
	Reset();
	Init();
	SetRealtimeFeedbackEnabled(true);
}
