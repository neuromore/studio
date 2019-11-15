/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include <Core/String.h>
#include <Core/LogManager.h>
#include "NetworkServerClient.h"
#include "NetworkMessage.h"
#include "NetworkMessageEvent.h"
#include "NetworkMessageConfig.h"

#include "../QtBaseManager.h"
#include <QHostAddress>
#include <QTcpSocket>


using namespace Core;

// constructor
NetworkServerClient::NetworkServerClient(QTcpSocket* socket)
{
	mClientType = NetworkClient::Undefined;
	mName = "Unknown";
	mNumFailedPings = 0;
	mTcpHeader.mMessageLength = 0;
	mTcpHeader.mMessageType = 0;
	
	mConfig = new Json();
	mIsInitialized = false;
	
	// zero statistics
	mNumMessagesTransmitted = 0;
	mNumMessagesReceived = 0;
	mNumBytesTransmitted = 0;
	mNumBytesReceived = 0;

	mTcpSocket = socket;
	connect(mTcpSocket, SIGNAL(readyRead()), this, SLOT(OnReceiveData()));
	connect(mTcpSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnect()));
	
	mHostAddress = mTcpSocket->peerAddress();
}


NetworkServerClient::~NetworkServerClient()
{
	// silence tcp socket, so it does not fire an ondisconnect signal (the server would try to delete the client)
	mTcpSocket->blockSignals(true);
	mTcpSocket->close();
	delete mConfig;
}


// initalize client from identification message
void NetworkServerClient::Update(NetworkMessageConfig* msg)
{
	// set members (essential config elements)
	mClientType = (NetworkClient::ClientType)msg->GetClientType();
	mName = msg->GetName();

	// parse config (contains more information than the members)
	mConfig->Parse( msg->GetString() );

	mIsInitialized = true;

	emit ConfigChanged( this );
}


// receive TCP data
void NetworkServerClient::OnReceiveData()
{
	// mark client as alive
	mNumFailedPings = 0;

	const uint32 headerSize = sizeof(NetworkMessage::Header);

	//LogError("NetworkServerClient::OnReceiveData(): mTcpHeader.mMessageLength=%i", mTcpHeader.mMessageLength);

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

	//LogError("NetworkServerClient::OnReceiveData(): int numBytes = mTcpHeader.mMessageLength - headerSize;=%i", numBytes);

	// we have to wait until enough data is available in the buffer
	if (mTcpSocket->bytesAvailable() < numBytes)
		return;

	// read message body
	QByteArray messageBody = mTcpSocket->read( numBytes );

	// allocate memory for message data
	QByteArray* mTcpData = new QByteArray( (int)mTcpHeader.mMessageLength, '\0' );

	char* dataPtr = mTcpData->data();

	//LogError("NetworkServerClient::OnReceiveData(): dataPtr=%s", dataPtr);

	// copy header and body
	memcpy( dataPtr, &mTcpHeader, headerSize );
	dataPtr += sizeof( mTcpHeader );
	memcpy( dataPtr, messageBody.data(), numBytes );

	// reset length counter so the next message can be received
	mTcpHeader.mMessageLength = 0;

	// check message type m create appropriate message object from data and emit message signal
	switch (mTcpHeader.mMessageType)
	{
		// client config message
		case NetworkMessageConfig::TYPE_ID:
		{
			// ignore config message if client is already initialized
			if (mIsInitialized == false)
			{
				NetworkMessageConfig msg( mTcpData );
			
				// increase stats
				mNumBytesReceived += msg.GetPayloadSize();

				// initialize / update client configuration
				if (mIsInitialized == false)
					Update(&msg);
			}
		} break;
			
		// json event
		case NetworkMessageEvent::TYPE_ID:
		{
			NetworkMessageEvent* msg = new NetworkMessageEvent(mTcpData);
			
			// increase stats
			mNumBytesReceived += msg->GetPayloadSize();

			emit MessageReceived( this, msg );
			
		} break;

		// unknown (unprocessable) message
		default: 
			LogDebug("ERROR: Received unknown network message (ID is %i)", mTcpHeader.mMessageType);
	}
	
	// increase stats
	mNumMessagesReceived++;
}


void NetworkServerClient::OnNetworkError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
		case QAbstractSocket::RemoteHostClosedError:
			LogInfo("Connection closed by remote host %s", FromQtString(mHostAddress.toString()).AsChar() );
			break;
		default:
		LogError("Network Error in Server: %s", FromQtString(mTcpSocket->errorString()).AsChar());
	}

	// check if client is now disconnected
	bool disconnected = false;
	if (mTcpSocket == NULL)
		disconnected = true;
	else
		if ((mTcpSocket->isOpen() && mTcpSocket->isReadable()) == false)
			disconnected = true;
	
	if (disconnected)
		OnDisconnect();
}


void NetworkServerClient::OnDisconnect()
{
	emit Disconnected(this);
}


// send a message to the client
void NetworkServerClient::WriteMessage(NetworkMessage* msg)
{
	mTcpSocket->write(msg->GetRawData(), msg->GetSize());
	
	// increase stats
	mNumBytesTransmitted += msg->GetPayloadSize();
	mNumMessagesTransmitted++;
}


// force close of tcp channel (blocking)
void NetworkServerClient::Disconnect()
{
	mTcpSocket->close();
	mTcpSocket->waitForDisconnected();
}
