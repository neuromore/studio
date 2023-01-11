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
#include <QtBase/QtBaseManager.h>

#include <Engine/EngineManager.h>
#include <Engine/Networking/WebsocketProtocol.h>
#include <Engine/Graph/StateTransitionUrlOpenedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerStartedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerStoppedCondition.h>
#include <Engine/Graph/StateTransitionBrowserPlayerPausedCondition.h>

#include <QtBase/Backend/UserGetRequest.h>
#include <QtBase/Backend/UserGetResponse.h>
#include <QtBase/Backend/UsersCreateRequest.h>
#include <QtBase/Backend/UsersCreateResponse.h>

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
      handleOnUrlOpened(*(WSMessageOnUrlOpened*)&mMessageRecv);
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_STARTED))
   {
      handleOnBrowserPlayerStarted(*(WSMessageOnBrowserPlayerStarted*)&mMessageRecv);
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_STOPPED))
   {
      handleOnBrowserPlayerStopped(*(WSMessageOnBrowserPlayerStopped*)&mMessageRecv);
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_BROWSER_PLAYER_PAUSED))
   {
      handleOnBrowserPlayerPaused(*(WSMessageOnBrowserPlayerPaused*)&mMessageRecv);
   }
   else if (0 == ::strcmp(mMessageRecv.type(), WSMessage::Type::ON_IMPERSONATION))
   {
      handleOnImpersonation(*(WSMessageOnImpersonation*)&mMessageRecv);
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

void WebsocketServer::handleOnUrlOpened(const WSMessageOnUrlOpened& msg)
{
   if (!msg.isvalid()) {
      qDebug() << "Failed to parse WSMessageOnUrlOpened:";
      return;
   }

   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   const char* url = msg.getUrl();

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

void WebsocketServer::handleOnBrowserPlayerStarted(const WSMessageOnBrowserPlayerStarted& msg)
{
   if (!msg.isvalid()) {
      qDebug() << "Failed to parse WSMessageOnBrowserPlayerStarted:";
      return;
   }

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

void WebsocketServer::handleOnBrowserPlayerStopped(const WSMessageOnBrowserPlayerStopped& msg)
{
   if (!msg.isvalid()) {
      qDebug() << "Failed to parse WSMessageOnBrowserPlayerStopped:";
      return;
   }

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

void WebsocketServer::handleOnBrowserPlayerPaused(const WSMessageOnBrowserPlayerPaused& msg)
{
   if (!msg.isvalid()) {
      qDebug() << "Failed to parse WSMessageOnBrowserPlayerPaused:";
      return;
   }

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

void WebsocketServer::handleOnImpersonation(const WSMessageOnImpersonation& msg)
{
   if (!msg.isvalid()) {
      qDebug() << "Failed to parse WSMessageOnImpersonation:";
      return;
   }

   // set data on user object
   mImpersonationUser.SetId(msg.getUuid());
   mImpersonationUser.SetFirstName(msg.getFirstName());
   mImpersonationUser.SetLastName(msg.getLastName());
   mImpersonationUser.SetEmail(0);
   mImpersonationUser.SetBirthday(0);
   mImpersonationUser.ClearParentCompanyIds();
   const uint32 numCompanies = GetUser()->GetNumParentCompanyIds();
   for (uint32 i = 0; i < numCompanies; i++)
      mImpersonationUser.AddParentCompanyId(GetUser()->GetParentCompanyId(i));

   // impersonate or create
   impersonateOrCreateUser();
}

// MESSAGE HANDLER SUBFUNCTIONS

void WebsocketServer::impersonateOrCreateUser()
{
   // construct request
   UserGetRequest request(
      GetUser()->GetToken(), 
      mImpersonationUser.GetId());

   // execute request
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
      UserGetResponse response(networkReply, true);

      // CASE1: User does not exist
      if (response.IsNotFoundError())
         createUser();

      // CASE2: Some real error
      else if (response.HasError())
         return;

      // CASE3: User exists
      else
         emit impersonated(response.GetUser());
   });
}

void WebsocketServer::createUser()
{
   // construct request
   UsersCreateRequest request(
      GetUser()->GetToken(), 
      mImpersonationUser.GetFirstName(), 
      mImpersonationUser.GetLastName(),
      mImpersonationUser.GetParentCompanyIds(),
      mImpersonationUser.GetEmail(),
      mImpersonationUser.GetBirthday(),
      mImpersonationUser.GetId(),
      3004);

   // execute request
   QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
   connect(reply, &QNetworkReply::finished, this, [reply, this]()
   {
      QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
      UsersCreateResponse response(networkReply);

      if (response.HasError()) {
         return;
      }

      // try impersonate again
      impersonateOrCreateUser();
   });
}
