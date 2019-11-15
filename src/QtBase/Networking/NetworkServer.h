/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_NETWORKSERVER_H
#define __NEUROMORE_NETWORKSERVER_H

// include required headers
#include "../QtBaseConfig.h"
#include "NetworkServerClient.h"
#include "NetworkMessage.h"
#include <Core/String.h>
#include <Core/EventManager.h>
#include <Core/EventHandler.h>
#include <Networking/OscPacket.h>
#include <Networking/OscFeedbackPacket.h>
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QTcpSocket>
#include <QTcpServer>


class QTBASE_API NetworkServer : public QObject
{
	Q_OBJECT
	public:
		// constructor & destructor
		NetworkServer(uint32 portTCP, uint32 portUDPServer, uint32 portUDPClient);
		virtual ~NetworkServer();

		// initialize the server
		void Reset();
		void ReInit();
		void Init();
		void ClearClients();

		// config the server
		void SetTcpPort(uint32 port)								{ mTcpServerPort = port; }
		uint32 GetTcpPort()											{ return mTcpServerPort; }
		void SetUdpServerPort(uint32 port)							{ mUdpServerPort = port; }
		uint32 GetUdpServerPort()									{ return mUdpServerPort; }
		void SetUdpClientPort(uint32 port)							{ mUdpClientPort = port; }
		uint32 GetUdpClientPort()									{ return mUdpClientPort; }

		// broadcast server announce to whole network?
		void SetBroadcastServerAnnounce(bool enabled);
		bool GetBroadcastServerAnnounce()							{ return mBroadcastServerAnnounce; }

		// send announce every x seconds
		void SetAnnounceInterval(double seconds)					{ mAnnounceInterval = seconds; }
		double GetAnnounceInterval()								{ return mAnnounceInterval; }

		// client times out and is removed after this many seconds
		void SetClientTimeout(double seconds);
		double GetClientTimeout()									{ return mClientTimeout; }
		void SetRealtimeUpdateRate(uint32 frequency)				{ mRealtimeUpdateRate = frequency; }
		
		void SetServerAnnounceEnabled(bool enabled = true);
		void SetRealtimeFeedbackEnabled(bool enabled = true);
		void SetListenerEnabled(bool enabled = true)				{ mIsListenerEnabled = enabled; }
		bool GetListenerEnabled() const								{ return mIsListenerEnabled; }

		NetworkServerClient* GetClient(uint32 index) const			{ return mClients[index]; }
		uint32 GetNumClients() const								{ return mClients.Size(); }

		uint32 GetNumClients(NetworkClient::ClientType type) const;
		NetworkServerClient* GetClient(NetworkClient::ClientType type, uint32 index) const;

		NetworkServerClient* GetClientByHostAddress(const QHostAddress& hostAddress) const;

		NetworkServerClient* GetPrimaryStarterClient()				{ return mStarterClient; }
		NetworkServerClient* GetPrimaryVisualizationClient()		{ return mVisualizationClient; }

		// Sending messages via UDP
		void BroadcastUdpDatagram(const char* data, uint32 numBytes);
		void SendUdpDatagram(const char* data, uint32 numBytes, NetworkClient::ClientType type = NetworkClient::Undefined);

		// server status
		inline bool IsOnline() const								{ if (mTcpServer != NULL) { return mTcpServer->isListening(); }  return false; }
		uint32 GetNumMessagesTransmitted() const;
		uint32 GetNumMessagesReceived() const;	
		uint32 GetNumBytesTransmitted() const;
		uint32 GetNumBytesReceived() const;
	
	private slots:
		// realtime update udp messages
		void OnTimerSendRealtimeUpdateToClients();

		// broadcast message for searching clients
		void OnTimerSearchForClients();
		
		void OnNewConnection();									// process incoming tcp connections from new clients
		void OnNetworkError(QAbstractSocket::SocketError socketError);
		void OnClientChanged(NetworkServerClient* client);
		void OnClientDisconnected(NetworkServerClient* client);
	
		void OnReceiveUdpDatagram();

		void DeterminePrimaryClients();

	signals:
		void ClientAdded     (NetworkServerClient* client);
		void ClientChanged   (NetworkServerClient* client);
		void ClientRemoved   (NetworkServerClient* client);
		void MessageReceived (NetworkServerClient* client, NetworkMessage* message);
		void ServerError	 (Core::String errorString);

	private:

		// UDP/TCP core components
		QTcpServer*							mTcpServer;		// TCP server
		QUdpSocket*							mUdpSocket;		// UDP socket (for outgoing packets)

		// Local endpoints
		void UpdateAnnouncePackets();
		void FindLocalIPAddresses();								// find all local ips
		Core::Array<QHostAddress>			mLocalIPAddresses;		// list of ip addresses from network adapters of this machine

		// on server announce message per local ip endpoint
		Core::Array<OscPacket*>				mAnnouncePackets;

		OscFeedbackPacket					mOscFeedbackPacket;

		// Server config
		uint32			mTcpServerPort;
		uint32			mUdpServerPort;
		uint32			mUdpClientPort;
		bool			mIsListenerEnabled;
		bool			mIsServerAnnounceEnabled;
		bool			mBroadcastServerAnnounce;
		double			mAnnounceInterval;
		double			mRealtimeUpdateRate;
		uint32			mMaxFailedPings;
		double			mClientTimeout;

		// Client management
		Core::Array<NetworkServerClient*>	mClients;			// array of connected clients
		void AddClient(NetworkServerClient* client);			// accept and add client
		void RemoveClient(NetworkServerClient* client);			// disconnect and remove a client
		void AutoRemoveClients();								// remove all clients with exeeding ping count
		void IncreaseClientPingCount();							// increment ping count of each client by 1

		// Primary clients
		NetworkServerClient* mStarterClient;
		NetworkServerClient* mVisualizationClient;

		// Statistics
		uint32			mNumMessagesTransmitted;
		uint32			mNumMessagesReceived;
		uint32			mNumBytesTransmitted;
		uint32			mNumBytesReceived;
	
		// Timer
		QTimer*			mAnnounceTimer;							// server announce timer
		QTimer*			mRealtimeUpdateTimer;					// UDP realtime update timer

		Core::String	mTempString;

	private:
		// client event handler (network server side: sends messages)
		class ClientEventHandler : public Core::EventHandler
		{
			public:
				ClientEventHandler(NetworkServer* server) : EventHandler() 
				{ 
					mNetworkServer = server; 
				}


				void OnSwitchApplication(const char* name) override
				{ 
					// construct app switch json command
					Core::Json command;
					Core::Json::Item rootItem = command.GetRootItem();
					rootItem.AddString("doSwitchLevel", name);

					// create json string
					Core::String jsonString;
					command.WriteToString(jsonString);

					// send message to ALL starter clients (there may be more than one, but usually not)
					int numStarterClients = mNetworkServer->GetNumClients(NetworkClient::Starter);
					for (int i = 0; i < numStarterClients; i++)
					{
						NetworkServerClient* client = mNetworkServer->GetClient(NetworkClient::Starter, i);

						// construct json event message and send it to client
						NetworkMessageEvent* jsonEvent = new NetworkMessageEvent(jsonString);
						client->WriteMessage(jsonEvent);
						delete jsonEvent;
					}
				}

				void OnSwitchStage(uint32 index) override
				{
					NetworkServerClient* client = mNetworkServer->GetPrimaryVisualizationClient();
					if (client == NULL)
						return;

					Core::Json command;
					Core::Json::Item rootItem = command.GetRootItem();
					rootItem.AddInt("doSwitchStage", index);
					Core::String jsonString;
					command.WriteToString(jsonString);

					NetworkMessageEvent* message = new NetworkMessageEvent(jsonString);
					client->WriteMessage(message);
					delete message;
				}



				void OnCommand(const char* command) override
				{
					NetworkServerClient* client = mNetworkServer->GetPrimaryVisualizationClient();
					if (client == NULL)
						return;

					NetworkMessageEvent* message = new NetworkMessageEvent(command);
					client->WriteMessage(message);
					delete message;
				}

			private:
				NetworkServer* mNetworkServer;
		};

		ClientEventHandler*				mEventHandler;
};


#endif
