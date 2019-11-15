/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKCLIENT_H
#define __NEUROMORE_NETWORKCLIENT_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/String.h>
#include <Core/Timer.h>
#include "NetworkMessage.h"
#include "NetworkMessageEvent.h"
#include "NetworkMessageConfig.h"

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QTcpSocket>
#include <QTcpServer>


class QTBASE_API NetworkClient : public QObject
{
	Q_OBJECT

	public:

		enum ClientType
		{
			Undefined = 0,
			Visualization = 1,
			Starter = 2,
			Studio = 3,
		};

		// constructor & destructor
		NetworkClient(uint32 portTCP, uint32 portUDPServer, uint32 portUDPClient, const char* name, uint32 clientType);
		virtual ~NetworkClient();

		// initialize the network sockets
		void Init();			

		// config client
		inline void SetUseAutoConnect(bool enabled)					{ mUseAutoConnect = enabled; }

		// connect to a server
		bool IsConnected();
		void OpenConnection (const Core::String& hostAddress);
		void CloseConnection ();

		// send message to server via TCP
		bool SendMessageTCP(NetworkMessage* message);

		// send message to server via UDP
		bool SendMessageUDP(NetworkMessage* message);

	signals:
		void MessageReceived(NetworkMessage* message);
		void ConnectionOpened();
		void ConnectionClosed();

	private slots:
		void OnReceiveUdpDatagram();
		void OnReceiveData();
		void OnNetworkError(QAbstractSocket::SocketError socketError);
		void OnConnect();
		void OnDisconnect();

	private:
		
		// core classes
		QTcpSocket*				mTcpSocket;				// TCP socket
		QUdpSocket*				mUdpSocket;				// UDP socket
		
		QHostAddress			mServerIP;				// Server IP
		uint32					mTcpServerPort;			// TCP port the server listenes on
		uint32					mUdpClientPort;			// UDP port on client side
		uint32					mUdpServerPort;			// UDP port on server side

		NetworkMessageConfig*	mConfigMessage;			// config message (sent as reply to every server announce)
		bool					mUseAutoConnect;		// automatically connect to server on received server announce


		// helpers
		NetworkMessage::Header	mTcpHeader;			// received message header for parsing

};


#endif
