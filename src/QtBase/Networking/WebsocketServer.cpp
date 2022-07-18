/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

#include "WebsocketServer.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>

#include <Engine/EngineManager.h>
#include <Engine/Networking/WebsocketProtocol.h>
#include <Engine/Graph/StateTransitionUrlOpenedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerStartedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerStoppedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerPausedCondition.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

QT_USE_NAMESPACE

WebsocketServer::WebsocketServer(quint16 port, bool debug, QObject* parent) :
   QObject(parent),
   mWebSocketServer(new QWebSocketServer(QStringLiteral("Studio Server"),
      QWebSocketServer::NonSecureMode, this)),
   mTimerFeedbacks(new QTimer(this)),
   mJsonBuf(),
   mJsonWriter(mJsonBuf)
{
   CORE_EVENTMANAGER.AddEventHandler(this);

   // configure timer
   mTimerFeedbacks->setTimerType(Qt::PreciseTimer);

   // start listening
   if (mWebSocketServer->listen(QHostAddress::Any, port))
   {
      qDebug() << "Studio Websocket Server listening on port" << port;
      connect(mWebSocketServer, &QWebSocketServer::newConnection,
         this, &WebsocketServer::onNewConnection);
      connect(mWebSocketServer, &QWebSocketServer::closed, 
         this, &WebsocketServer::closed);
      connect(mTimerFeedbacks, &QTimer::timeout, 
         this, &WebsocketServer::sendFeedbacks);
   }
}

WebsocketServer::~WebsocketServer()
{
   CORE_EVENTMANAGER.RemoveEventHandler(this);

   mWebSocketServer->close();
   qDeleteAll(mClients.begin(), mClients.end());
}

// SOCKET EVENTS

void WebsocketServer::onNewConnection()
{
   QWebSocket* pClient = mWebSocketServer->nextPendingConnection();

   qDebug() << "socketConnected:" << pClient;
   
   // connect signals
   connect(pClient, &QWebSocket::textMessageReceived, this, &WebsocketServer::processTextMessage);
   connect(pClient, &QWebSocket::binaryMessageReceived, this, &WebsocketServer::processBinaryMessage);
   connect(pClient, &QWebSocket::disconnected, this, &WebsocketServer::socketDisconnected);
   
   // add to client list
   mClients << pClient;

   // init the client with the current state
   if (Classifier* c = GetEngine()->GetActiveClassifier())
      sendMessage(mMessageOnClassifierLoaded);
   if (StateMachine* m = GetEngine()->GetActiveStateMachine())
      sendMessage(mMessageOnStateMachineLoaded);
   if (Experience* e = GetEngine()->GetActiveExperience())
      sendMessage(mMessageOnExperienceLoaded);
   if (Session* s = GetSession())
   {
      if (s->IsRunning())
      {
         sendMessage(mMessageOnSessionStarted);
         sendMessage(mMessageOnFeedbackLayout);
      }
   }
}

void WebsocketServer::socketDisconnected()
{
   QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
   qDebug() << "socketDisconnected:" << pClient;
   if (pClient) {
      mClients.removeAll(pClient);
      pClient->deleteLater();
   }
}

void WebsocketServer::processTextMessage(QString message)
{
   QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());

   // try to parse it
   if (!mMessageRecv.read(message.toUtf8().data()))
   {
   qDebug() << "Failed to parse WSMessage: " << message;
   return;
   }

   qDebug() << "Received WSMessage " << mMessageRecv.type() << " from client " << pClient;

   // Run according handler
   if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_URL_OPENED))
   {
      WSMessageOnUrlOpened& msg = *(WSMessageOnUrlOpened*)&mMessageRecv;
      handleOnUrlOpened(msg.getUrl());
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_STARTED))
   {
      handleOnBrowserPlayerStarted();
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_STOPPED))
   {
      handleOnBrowserPlayerStopped();
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_PAUSED))
   {
      handleOnBrowserPlayerPaused();
   }
   else
      qDebug() << "Unhandled WSMessage: " << mMessageRecv.type();
}

void WebsocketServer::processBinaryMessage(QByteArray message)
{
   QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
   qDebug() << "Binary Message received:" << message;
}

// ENGINE EVENTS

void WebsocketServer::OnStartSession()
{
   if (Session* s = GetSession())
   {
      mMessageOnSessionStarted.setSession(s);
      sendMessage(mMessageOnSessionStarted);

      mMessageOnFeedbackLayout.setClassifier(GetEngine()->GetActiveClassifier());
      sendMessage(mMessageOnFeedbackLayout);
      mTimerFeedbacks->start(100);
   }
}
void WebsocketServer::OnStopSession()
{
   sendMessage(mMessageOnSessionStopped);
}
void WebsocketServer::OnActiveClassifierChanged(Classifier* classifier)
{
   if (!classifier)
      return;
   mMessageOnClassifierLoaded.setClassifier(classifier);
   sendMessage(mMessageOnClassifierLoaded);
}
void WebsocketServer::OnActiveStateMachineChanged(StateMachine* stateMachine)
{
   if (!stateMachine)
      return;
   mMessageOnStateMachineLoaded.setStateMachine(stateMachine);
   sendMessage(mMessageOnStateMachineLoaded);
}
void WebsocketServer::OnActiveExperienceChanged(Experience* experience)
{
   if (!experience)
      return;
   mMessageOnExperienceLoaded.setExperience(experience);
   sendMessage(mMessageOnExperienceLoaded);
}

void WebsocketServer::OnNodeAdded(Graph* graph, Node* node)
{
   if (graph->GetType() == Classifier::TYPE_ID)
   {
   }
}

void WebsocketServer::OnRemoveNode(Graph* graph, Node* node)
{
   if (graph->GetType() == Classifier::TYPE_ID)
   {
   }
}

void WebsocketServer::OnConnectionAdded(Graph* graph, Connection* connection)
{
   if (graph->GetType() == Classifier::TYPE_ID)
   {
   }
}

void WebsocketServer::OnRemoveConnection(Graph* graph, Connection* connection)
{
   if (graph->GetType() == Classifier::TYPE_ID)
   {
   }
}

void WebsocketServer::OnOpenUrl(const char* url)
{
   mMessageOpenUrl.setUrl(url);
   sendMessage(mMessageOpenUrl);
}

void WebsocketServer::OnBrowserStartPlayer(double progress, bool fullscreen)
{
   mMessageBrowserStartPlayer.setProgress(progress);
   mMessageBrowserStartPlayer.setFullscreen(fullscreen);
   sendMessage(mMessageBrowserStartPlayer);
}

void WebsocketServer::OnBrowserStopPlayer()
{
   sendMessage(mMessageBrowserStopPlayer);
}

void WebsocketServer::OnBrowserPausePlayer()
{
   sendMessage(mMessageBrowserPausePlayer);
}


// MESSAGE SENDERS

void WebsocketServer::sendJson(rapidjson::Document& json)
{
   // prepare buffer
   mJsonBuf.Clear();
   mJsonWriter.Reset(mJsonBuf);

   // serialize
   json.Accept(mJsonWriter);

   // ref to serialized string
   const char* str = mJsonBuf.GetString();

   // send to all
   for (QWebSocket* s : mClients)
      s->sendTextMessage(str);
}

void WebsocketServer::sendFeedbacks()
{
   Classifier* c = GetEngine()->GetActiveClassifier();
   Session*    s = GetSession();

   if (!c || !s || !s->IsRunning())
      return;

   // custom feedback nodes count
   const uint32_t NUMFEEDBACKS = c->GetNumCustomFeedbackNodes();

   // iterate custom feedback nodes
   for (uint32_t i = 0; i < NUMFEEDBACKS; i++)
   {
      // create binary message
      if (!mMessageBinary.createMessageCustomFeedback(c, i))
         continue;

      // send to all
      for (QWebSocket* s : mClients)
         s->sendBinaryMessage(mMessageBinary.mBuffer);
   }
}

// MESSAGE HANDLERS

void WebsocketServer::handleOnUrlOpened(const char* url)
{
   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   // get the number of transitions and iterate through them
   const uint32 numTransitions = stateMachine->GetNumConnections();
   for (uint32 i = 0; i < numTransitions; ++i)
   {
      StateTransition* transition = stateMachine->GetTransition(i);

      // get the number of conditions and iterate through them
      const uint32 numConditions = transition->GetNumConditions();
      for (uint32 j = 0; j < numConditions; ++j)
      {
         StateTransitionCondition* condition = transition->GetCondition(j);

         // check if the current condition is a button condition
         if (condition->GetType() == StateTransitionUrlOpenedCondition::TYPE_ID)
         {
            StateTransitionUrlOpenedCondition* urlCondition = 
               static_cast<StateTransitionUrlOpenedCondition*>(condition);

            // check if the condition id equals the button id
            if (0 == ::strcmp(urlCondition->GetUrl(), url))
            {
               urlCondition->SetOpened(true);
               return;
            }
         }
      }
   }
}

void WebsocketServer::handleOnBrowserPlayerStarted()
{
   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   // get the number of transitions and iterate through them
   const uint32 numTransitions = stateMachine->GetNumConnections();
   for (uint32 i = 0; i < numTransitions; ++i)
   {
      StateTransition* transition = stateMachine->GetTransition(i);

      // get the number of conditions and iterate through them
      const uint32 numConditions = transition->GetNumConditions();
      for (uint32 j = 0; j < numConditions; ++j)
      {
         StateTransitionCondition* condition = transition->GetCondition(j);

         // check if the current condition is a button condition
         if (condition->GetType() == StateTransitionBrowserPlayerStartedCondition::TYPE_ID)
         {
            StateTransitionBrowserPlayerStartedCondition* urlCondition =
               static_cast<StateTransitionBrowserPlayerStartedCondition*>(condition);

            urlCondition->SetCondition(true);
            return;
         }
      }
   }
}

void WebsocketServer::handleOnBrowserPlayerStopped()
{
   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   // get the number of transitions and iterate through them
   const uint32 numTransitions = stateMachine->GetNumConnections();
   for (uint32 i = 0; i < numTransitions; ++i)
   {
      StateTransition* transition = stateMachine->GetTransition(i);

      // get the number of conditions and iterate through them
      const uint32 numConditions = transition->GetNumConditions();
      for (uint32 j = 0; j < numConditions; ++j)
      {
         StateTransitionCondition* condition = transition->GetCondition(j);

         // check if the current condition is a button condition
         if (condition->GetType() == StateTransitionBrowserPlayerStoppedCondition::TYPE_ID)
         {
            StateTransitionBrowserPlayerStoppedCondition* urlCondition =
               static_cast<StateTransitionBrowserPlayerStoppedCondition*>(condition);

            urlCondition->SetCondition(true);
            return;
         }
      }
   }
}

void WebsocketServer::handleOnBrowserPlayerPaused()
{
   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   // get the number of transitions and iterate through them
   const uint32 numTransitions = stateMachine->GetNumConnections();
   for (uint32 i = 0; i < numTransitions; ++i)
   {
      StateTransition* transition = stateMachine->GetTransition(i);

      // get the number of conditions and iterate through them
      const uint32 numConditions = transition->GetNumConditions();
      for (uint32 j = 0; j < numConditions; ++j)
      {
         StateTransitionCondition* condition = transition->GetCondition(j);

         // check if the current condition is a button condition
         if (condition->GetType() == StateTransitionBrowserPlayerPausedCondition::TYPE_ID)
         {
            StateTransitionBrowserPlayerPausedCondition* urlCondition =
               static_cast<StateTransitionBrowserPlayerPausedCondition*>(condition);

            urlCondition->SetCondition(true);
            return;
         }
      }
   }
}
