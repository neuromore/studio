/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKSERVERCLIENT_H
#define __NEUROMORE_NETWORKSERVERCLIENT_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/String.h>
#include "NetworkClient.h"
#include "NetworkMessage.h"
#include "NetworkMessageConfig.h"
#include "NetworkMessageEvent.h"

#include <QHostAddress>
#include <QTcpSocket>

class NetworkServer;

class QTBASE_API NetworkServerClient : public QObject
{
	Q_OBJECT
	public:
		// constructors & destructor
		NetworkServerClient(QTcpSocket* socket);
		virtual ~NetworkServerClient();

		bool IsInitialized()					{ return mIsInitialized; }

		// TODO imlement as private members, getters
		QHostAddress					mHostAddress;
		NetworkClient::ClientType		mClientType;
		Core::String					mName;

		Core::Json* GetConfig() const			{ return mConfig; }
		
		void   IncreasePingCount()				{ mNumFailedPings++; }
		uint32 mNumFailedPings;		

		void WriteMessage(NetworkMessage* msg);		// send a message to client
		void Disconnect();

		// server status
		inline bool IsConnected() const								{ if (mTcpSocket != NULL) { return mTcpSocket->isOpen(); }  return false; }
		inline uint32 GetNumMessagesTransmitted() const				{ return mNumMessagesTransmitted; }
		inline uint32 GetNumMessagesReceived() const				{ return mNumMessagesReceived; }
		inline uint32 GetNumBytesTransmitted() const				{ return mNumBytesTransmitted; }
		inline uint32 GetNumBytesReceived() const					{ return mNumBytesReceived; }

	signals:
		void MessageReceived ( NetworkServerClient* client, NetworkMessage* msg );
		void ConfigChanged   ( NetworkServerClient* client );
		void Disconnected    ( NetworkServerClient* client );

	private slots:
		void OnReceiveData();
		void OnNetworkError(QAbstractSocket::SocketError socketError);
		void OnDisconnect ();
		
	private:
		void Update(NetworkMessageConfig* msg);			// update client configuration from received config message

		Core::Json*		mConfig;						// client configuration as json
		bool			mIsInitialized;					// becomes true after the first received configuration

		QTcpSocket*				mTcpSocket;				// this client's tcp socket 
		NetworkMessage::Header	mTcpHeader;				// received message header

		// Statistics
		uint32			mNumMessagesTransmitted;
		uint32			mNumMessagesReceived;
		uint32			mNumBytesTransmitted;
		uint32			mNumBytesReceived;

};


#endif
