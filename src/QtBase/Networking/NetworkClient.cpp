/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "NetworkClient.h"
#include "NetworkMessageData.h"
#include "NetworkMessageConfig.h"
#include "NetworkMessageEvent.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include "../QtBaseManager.h"

using namespace Core;

// constructor
NetworkClient::NetworkClient(uint32 portTCP, uint32 portUDPServer, uint32 portUDPClient, const char* name, uint32 clientType)
{
	mUdpSocket = NULL;
	mTcpSocket = NULL;
	mTcpHeader.mMessageLength = 0;
	mTcpHeader.mMessageType = 0;
	mServerIP = QHostAddress::Null;

	// client config
	mTcpServerPort = portTCP;
	mUdpClientPort = portUDPClient;
	mUdpServerPort = portUDPServer;
	mUseAutoConnect = false;

	// setup config message
	mConfigMessage = new NetworkMessageConfig( clientType, name );

	// init client
	Init();
}


// destructor
NetworkClient::~NetworkClient()
{
	// cleanup network sockets
	if (mUdpSocket != NULL)
	{
		mUdpSocket->close();
		delete mUdpSocket;
	}

	if (mTcpSocket != NULL)
	{
		mTcpSocket->close();
	}

	delete mConfigMessage;
}


// initialize the client
void NetworkClient::Init()
{
	// create the sending socket
	mUdpSocket = new QUdpSocket(this);

	// initialize the socket bind mode, reuse address so client and host can run on the same machine
	QAbstractSocket::BindMode bindMode = QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint;

	// bind the socket
	mUdpSocket->bind( mUdpClientPort, bindMode );	
	connect(mUdpSocket, SIGNAL(readyRead()), this, SLOT(OnReceiveUdpDatagram()));
	
	mTcpSocket = new QTcpSocket(this);
	connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(OnReceiveData()));
	connect(mTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnNetworkError(QAbstractSocket::SocketError)));
	connect(mTcpSocket, SIGNAL(connected()), this, SLOT(OnConnect()));
	connect(mTcpSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnect()));
}


// is tcp connection valid?
bool NetworkClient::IsConnected()
{
	return mTcpSocket->isOpen();
}


// connect to server (blocking)
void NetworkClient::OpenConnection(const Core::String& hostAddress)
{

	// if its already connected, close socket first
	if (IsConnected())
		CloseConnection();

	mServerIP.setAddress(hostAddress.AsChar());
	mTcpSocket->connectToHost(mServerIP, mTcpServerPort);
	mTcpSocket->waitForConnected();
}


// disconnect from server (blocking)
void NetworkClient::CloseConnection()
{
	mTcpSocket->disconnectFromHost();
	mTcpSocket->waitForDisconnected();
}

// tcp connection just opened
void NetworkClient::OnConnect()
{
	LogInfo("Connected to server %s", FromQtString(mServerIP.toString()).AsChar());

	// send client config to server
	SendMessageTCP(mConfigMessage);
	
	emit ConnectionOpened();
}

// tcp has disconnected
void NetworkClient::OnDisconnect()
{
	LogInfo("Disconnected from server %s", FromQtString(mServerIP.toString()).AsChar());
	mServerIP = QHostAddress::Null;

	emit ConnectionClosed();
}

//
// Receiving Messages
//

// read incoming UDP messages and connect to server
void NetworkClient::OnReceiveUdpDatagram()
{
	// there may be more than one datagram
	while (mUdpSocket->hasPendingDatagrams())
	{
		// receive udp datagram
		QHostAddress senderAddress;
		quint16 senderPort;
		QByteArray* inData = new QByteArray();	// delete by message object
		NetworkMessage::Header header;
		uint32 messageSize = mUdpSocket->pendingDatagramSize();
		if (messageSize < sizeof(header))
			return; // TODO error, datagram too short to contain a header // should not happen due to UDP datagram size?

		inData->resize(messageSize);
		mUdpSocket->readDatagram(inData->data(), inData->size(), &senderAddress, &senderPort);

		// parse message header
		memcpy(&header, inData->data(), sizeof(header));

		switch (header.mMessageType)
		{
			// FIXME implement using osc packets
			//case NetworkMessageAnnounce::TYPE_ID:
			//{
			//	// open tcp connection
			//	if (IsConnected() == false)
			//	{
			//		// parse message, get server IP, connect to server
			//		NetworkMessageAnnounce message = NetworkMessageAnnounce(inData);
			//		String serverIP = message.GetServerIP();

			//		LogInfo("Received Server Announce from %s", serverIP.AsChar());
			//		
			//		// automatically connect to server, if enabled
			//		if (mUseAutoConnect == true)
			//			OpenConnection (serverIP);
			//	}
			//	else
			//	{	
			//		// reply with config message (keep-alive)
			//		SendMessageTCP(mConfigMessage);
			//	}
			//	
			//} break;

			case NetworkMessageData::TYPE_ID:
			{
				NetworkMessageData* message = new NetworkMessageData(inData);
				emit MessageReceived(message);
			} break;
			// unknown message ID
			default: 
				LogError("Received an unknown message  (TypeID=%i).", header.mMessageType);
		}
	}
}


// receive TCP data
void NetworkClient::OnReceiveData()
{
	const uint32 headerSize = sizeof(NetworkMessage::Header);

	// begin reading data
	if (mTcpHeader.mMessageLength == 0)
	{

		// wait until the full message header is available on the stream
		if (mTcpSocket->bytesAvailable() < headerSize)
			return;

		// read header, copy data to struct
		QByteArray headerData = mTcpSocket->read(headerSize);
		memcpy(&mTcpHeader, headerData.data(), headerSize);
	}

	int numBytes = mTcpHeader.mMessageLength - headerSize;

	// we have to wait until enough data is available in the buffer
	if (mTcpSocket->bytesAvailable() < numBytes)
		return;

	// read message body
	QByteArray messageBody = mTcpSocket->read(numBytes);

	// allocate memory for message data
	QByteArray* mTcpData = new QByteArray((int)mTcpHeader.mMessageLength, '\0');

	char* dataPtr = mTcpData->data();

	// copy header and body
	memcpy(dataPtr, &mTcpHeader, headerSize);
	dataPtr += sizeof(mTcpHeader);
	memcpy(dataPtr, messageBody.data(), numBytes);

	// reset length counter so the next message can be received
	mTcpHeader.mMessageLength = 0;

	// check message type m create appropriate message object from data and emit message signal
	switch (mTcpHeader.mMessageType)
	{
		// json event
		case NetworkMessageEvent::TYPE_ID:
		{
			NetworkMessageEvent* msg = new NetworkMessageEvent(mTcpData);
			emit MessageReceived(msg);
			return;
		}

		// unknown (unprocessable) message
		default: break;
			// TODO error output
	}
}


//
// Transmitting Messages
//


// send UDP network datagram to the server
bool NetworkClient::SendMessageUDP(NetworkMessage* message)
{
	const qint64 success = mUdpSocket->writeDatagram(message->GetRawData(), message->GetSize(), mServerIP, mUdpServerPort);
	return (success != -1);
}


// send a message to the server
bool NetworkClient::SendMessageTCP(NetworkMessage* message)
{
	const qint64 success = mTcpSocket->write(message->GetRawData(), message->GetSize());
	return (success != -1);
}




//
// Error Handling
//

// TCP errors
void NetworkClient::OnNetworkError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
		case QAbstractSocket::RemoteHostClosedError:
			LogInfo("Connection closed by server %s", FromQtString(mServerIP.toString()).AsChar() );
			mTcpSocket->close();
			break;
		default:
			LogError("Network Error in Client: %s", FromQtString(mTcpSocket->errorString()).AsChar());
	}
}


// deprecated debug

// simple tcp loopback for testing
//void NetworkClient::OnReceiveEvent(NetworkMessageEvent* msg)
//{
//	LogDebug("received event message");
//	WriteMessage(msg);
//	delete msg;
//}

