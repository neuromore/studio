/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_WEBSOCKETSERVER_H
#define __NEUROMORE_WEBSOCKETSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>

#include <Engine/Networking/WebsocketProtocol.h>
#include <Engine/User.h>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class WebsocketServer : public QObject, public Core::EventHandler
{
   Q_OBJECT
public:
   explicit WebsocketServer(quint16 port, bool debug = false, QObject* parent = nullptr);
   ~WebsocketServer();

private:
   // MESSAGE SENDERS
   void sendJson(rapidjson::Document& json);
   inline void sendMessage(WSMessage& msg) { sendJson(msg.d); }

   // ENGINE EVENTS
   virtual void OnStartSession() override;
   virtual void OnStopSession() override;
   virtual void OnActiveClassifierChanged(Classifier* classifier) override;
   virtual void OnActiveStateMachineChanged(StateMachine* stateMachine)override;
   virtual void OnActiveExperienceChanged(Experience* experience) override;
   virtual void OnNodeAdded(Graph* graph, Node* node) override;
   virtual void OnRemoveNode(Graph* graph, Node* node) override;
   virtual void OnConnectionAdded(Graph* graph, Connection* connection) override;
   virtual void OnRemoveConnection(Graph* graph, Connection* connection) override;
   virtual void OnOpenUrl(const char* url) override;
   virtual void OnBrowserStartPlayer(double progress, bool fullscreen) override;
   virtual void OnBrowserStopPlayer() override;
   virtual void OnBrowserPausePlayer() override;

Q_SIGNALS:
   void impersonated(const User& user);
   void closed();

private Q_SLOTS:
   void onNewConnection();
   void processTextMessage(QString message);
   void processBinaryMessage(QByteArray message);
   void socketDisconnected();
   void sendFeedbacks();

private:
   void handleOnUrlOpened(const WSMessageOnUrlOpened& msg);
   void handleOnBrowserPlayerStarted(const WSMessageOnBrowserPlayerStarted& msg);
   void handleOnBrowserPlayerStopped(const WSMessageOnBrowserPlayerStopped& msg);
   void handleOnBrowserPlayerPaused(const WSMessageOnBrowserPlayerPaused& msg);
   void handleOnImpersonation(const WSMessageOnImpersonation& msg);

private:
   void impersonateOrCreateUser();
   void createUser();

private:
   // qt websockets
   QWebSocketServer*  mWebSocketServer;
   QList<QWebSocket*> mClients;

   // feedbacks timer
   QTimer* mTimerFeedbacks;

   // rapidjson
   rapidjson::StringBuffer                    mJsonBuf;
   rapidjson::Writer<rapidjson::StringBuffer> mJsonWriter;

   // messages
   WSMessage                     mMessageRecv;
   WSMessageOnClassifierLoaded   mMessageOnClassifierLoaded;
   WSMessageOnStateMachineLoaded mMessageOnStateMachineLoaded;
   WSMessageOnExperienceLoaded   mMessageOnExperienceLoaded;
   WSMessageOnSessionStarted     mMessageOnSessionStarted;
   WSMessageOnSessionStopped     mMessageOnSessionStopped;
   WSMessageOnFeedbackLayout     mMessageOnFeedbackLayout;
   WSMessageOpenUrl              mMessageOpenUrl;
   WSMessageBrowserStartPlayer   mMessageBrowserStartPlayer;
   WSMessageBrowserStopPlayer    mMessageBrowserStopPlayer;
   WSMessageBrowserPausePlayer   mMessageBrowserPausePlayer;
   WSMessageOnImpersonation      mMessageOnImpersonation;

   // binary message
   WSBMessage mMessageBinary;

   // user from impersonation
   User mImpersonationUser;
};

#endif //__NEUROMORE_WEBSOCKETSERVER_H
