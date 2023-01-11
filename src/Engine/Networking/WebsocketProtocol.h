/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as
** appearing in the file neuromore-class-exception.md included in the
** packaging of this file. Please review the following information to
** ensure the neuromore Public License requirements will be met:
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __NEUROMORE_WEBSOCKETPROTOCOL_H
#define __NEUROMORE_WEBSOCKETPROTOCOL_H

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <Engine/Graph/Classifier.h>
#include <Engine/Graph/StateMachine.h>
#include <Engine/Experience.h>
#include <Engine/Session.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////
// JSON/TEXT
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Base Class for JSON Websocket Messages
/// </summary>
class WSMessage
{
public:
   rapidjson::Document d;

   class Type
   {
   public:
      static constexpr char ON_CLASSIFIER_LOADED[]      = "ON_CLASSIFIER_LOADED";
      static constexpr char ON_STATEMACHINE_LOADED[]    = "ON_STATEMACHINE_LOADED";
      static constexpr char ON_EXPERIENCE_LOADED[]      = "ON_EXPERIENCE_LOADED";
      static constexpr char ON_SESSION_STARTED[]        = "ON_SESSION_STARTED";
      static constexpr char ON_SESSION_STOPPED[]        = "ON_SESSION_STOPPED";
      static constexpr char ON_FEEDBACK_LAYOUT[]        = "ON_FEEDBACK_LAYOUT";
      static constexpr char OPEN_URL[]                  = "OPEN_URL";
      static constexpr char ON_URL_OPENED[]             = "ON_URL_OPENED";
      static constexpr char BROWSER_START_PLAYER[]      = "BROWSER_START_PLAYER";
      static constexpr char BROWSER_STOP_PLAYER[]       = "BROWSER_STOP_PLAYER";
      static constexpr char BROWSER_PAUSE_PLAYER[]      = "BROWSER_PAUSE_PLAYER";
      static constexpr char ON_BROWSER_PLAYER_STARTED[] = "ON_BROWSER_PLAYER_STARTED";
      static constexpr char ON_BROWSER_PLAYER_STOPPED[] = "ON_BROWSER_PLAYER_STOPPED";
      static constexpr char ON_BROWSER_PLAYER_PAUSED[]  = "ON_BROWSER_PLAYER_PAUSED";
      static constexpr char ON_IMPERSONATION[]          = "IMPERSONATION";
   };

   inline WSMessage() : d(rapidjson::kObjectType) { }
   inline WSMessage(const char* type) : d(rapidjson::kObjectType) 
   {
      assert(type != 0);

      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      // create header and data sub objects
      Value header(kObjectType);
      Value data(kObjectType);

      // set header values
      header.AddMember("type", StringRef(type), alloc);

      // add both
      d.AddMember("header", header, alloc);
      d.AddMember("data", data, alloc);
   }

   inline virtual const char* msgtype() const { return 0; }

   inline const char* type() const { return d["header"]["type"].GetString(); }

   inline bool isvalid() const 
   {
      if (!d.IsObject() || !d.HasMember("header") || !d.HasMember("data"))
         return false;

      auto& head = d["header"];
      auto& data = d["data"];

      if (!head.IsObject() || !data.IsObject() || !head.HasMember("type"))
         return false;

      auto& type = head["type"];

      if (!type.IsString())
         return false;

      if (msgtype() && 0 != ::strcmp(type.GetString(), msgtype()))
         return false;

      return true;
   }

   inline bool write(rapidjson::Writer<rapidjson::StringBuffer>& buffer)
   {
      return d.Accept(buffer);
   }

   inline bool read(const char* json)
   {
      d.Parse(json);
      return isvalid();
   }
};

/// <summary>
/// Sent from Studio to connected clients in case a classifier was loaded
/// </summary>
class WSMessageOnClassifierLoaded : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_CLASSIFIER_LOADED; }
   inline WSMessageOnClassifierLoaded() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      Value& data = d["data"];
      Value name(kStringType);
      data.AddMember("name", name, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;

      auto& data = d["data"];
      return data.HasMember("name") && data["name"].IsString();
   }
   inline void setClassifier(Classifier* c)
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      if (!c)
         return;

      // data section
      Value& data = d["data"];

      // set name
      data["name"].SetString(c->GetName(), alloc);
   }
};

/// <summary>
/// Sent from Studio to connected clients in case a statemachine was loaded
/// </summary>
class WSMessageOnStateMachineLoaded : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_STATEMACHINE_LOADED; }
   inline WSMessageOnStateMachineLoaded() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      Value& data = d["data"];
      Value name(kStringType);
      data.AddMember("name", name, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;

      auto& data = d["data"];
      return data.HasMember("name") && data["name"].IsString();
   }
   inline void setStateMachine(StateMachine* s)
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      if (!s)
         return;

      // data section
      Value& data = d["data"];

      // set name
      data["name"].SetString(s->GetName(), alloc);
   }
};

/// <summary>
/// Sent from Studio to connected clients in case a statemachine was loaded
/// </summary>
class WSMessageOnExperienceLoaded : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_EXPERIENCE_LOADED; }
   inline WSMessageOnExperienceLoaded() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      Value& data = d["data"];
      Value name(kStringType);
      data.AddMember("name", name, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;

      auto& data = d["data"];
      return data.HasMember("name") && data["name"].IsString();
   }
   inline void setExperience(Experience* s)
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      if (!s)
         return;

      // data section
      Value& data = d["data"];

      // set name
      data["name"].SetString(s->GetName(), alloc);
   }
};

/// <summary>
/// Sent from Studio to connected clients in case a session was started.
/// </summary>
class WSMessageOnSessionStarted : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_SESSION_STARTED; }
   inline WSMessageOnSessionStarted() : WSMessage(msgtype()) 
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
      Value start(kStringType);
      data.AddMember("start", start, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;
      auto& data = d["data"];
      return data.HasMember("start") && data["start"].IsString();
   }
   inline void setSession(Session* session)
   {
      if (!session)
         return;
      auto& data = d["data"];
      data["start"].SetString(
         session->GetStartTime().AsUtcString().AsChar(), 
         d.GetAllocator());
   }
};

/// <summary>
/// Sent from Studio to connected clients in case a session was stopped
/// </summary>
class WSMessageOnSessionStopped : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_SESSION_STOPPED; }
   inline WSMessageOnSessionStopped() : WSMessage(msgtype()) { }
};

/// <summary>
/// Sent from Studio to connected clients in case a classifier was loaded
/// </summary>
class WSMessageOnFeedbackLayout : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_FEEDBACK_LAYOUT; }
   inline WSMessageOnFeedbackLayout() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      Value& data = d["data"];
      Value feedbacks(kArrayType);
      data.AddMember("feedbacks", feedbacks, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;

      auto& data = d["data"];

      return data.HasMember("feedbacks") && data["feedbacks"].IsArray();
   }
   inline void setClassifier(Classifier* c)
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();

      if (!c)
         return;

      // data section
      Value& data = d["data"];

      // array with feedbacks
      Value& feedbacks = data["feedbacks"];

      // clear old ones
      feedbacks.Clear();

      // custom feedback nodes count
      const uint32_t NUMFEEDBACKS = c->GetNumCustomFeedbackNodes();

      // iterate nodes
      for (uint32_t i = 0; i < NUMFEEDBACKS; i++)
      {
         CustomFeedbackNode* n = c->GetCustomFeedbackNode(i);
         MultiChannel* ch = n->GetNumInputPorts() > 0 ? 
            n->GetInputPort(0).GetChannels() : 0;

         // channels on reader
         const uint32_t NUMCHANNELS = ch ? ch->GetNumChannels() : 0;

         // get samplerate
         const double SAMPLERATE = ch ? n->GetSampleRate(
            n->GetSignalResolution(), 
            ch->GetChannel(0)) : 0.0;

         // channels of feedback
         rapidjson::Value channels;
         channels.SetArray();
         for (uint32_t j = 0; j < NUMCHANNELS; j++)
         {
            Value name(ch->GetChannel(j)->GetName(), alloc);
            channels.PushBack(name, alloc);
         }

         // feedback
         rapidjson::Value feedback;
         feedback.SetObject();
         Value name(n->GetName(), alloc);
         feedback.AddMember("name", name, alloc);
         feedback.AddMember("sample_rate", SAMPLERATE, alloc);
         feedback.AddMember("channels", channels, alloc);

         // add to feedbacks
         feedbacks.PushBack(feedback, alloc);
      }
   }
};

/// <summary>
/// Sent from Studio to Plugin if an URL should be opened
/// </summary>
class WSMessageOpenUrl : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::OPEN_URL; }
   inline WSMessageOpenUrl() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
      Value url(kStringType);
      data.AddMember("url", url, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;
      auto& data = d["data"];
      return data.HasMember("url") && data["url"].IsString();
   }
   inline void setUrl(const char* url)
   {
      if (!url)
         return;
      auto& data = d["data"];
      data["url"].SetString(url, d.GetAllocator());
   }
   inline const char* getUrl()
   {
      return d["data"]["url"].GetString();
   }
};

/// <summary>
/// Sent from Studio to Plugin if embedded player should be started
/// </summary>
class WSMessageBrowserStartPlayer : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::BROWSER_START_PLAYER; }
   inline WSMessageBrowserStartPlayer() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
      Value progress(kNumberType);
      data.AddMember("progress", progress, alloc);
      Value fullscreen(kNumberType);
      data.AddMember("fullscreen", fullscreen, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;
      auto& data = d["data"];
      return data.HasMember("progress") && data.HasMember("fullscreen");
   }
   inline void setProgress(double progress)
   {
      auto& data = d["data"];
      data["progress"].SetDouble(progress);
   }
   inline double getProgress()
   {
      return d["data"]["progress"].GetDouble();
   }
   inline void setFullscreen(bool fullscreen)
   {
      auto& data = d["data"];
      data["fullscreen"].SetBool(fullscreen);
   }
   inline bool getFullscreen()
   {
      return d["data"]["fullscreen"].GetBool();
   }
};

/// <summary>
/// Sent from Studio to Plugin if embedded player should be stopped
/// </summary>
class WSMessageBrowserStopPlayer : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::BROWSER_STOP_PLAYER; }
   inline WSMessageBrowserStopPlayer() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
   }
};

/// <summary>
/// Sent from Studio to Plugin if embedded player should be paused
/// </summary>
class WSMessageBrowserPausePlayer : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::BROWSER_PAUSE_PLAYER; }
   inline WSMessageBrowserPausePlayer() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
   }
};

/// <summary>
/// Sent from Plugin to Studio in case an URL was succesfully opened
/// </summary>
class WSMessageOnUrlOpened : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_URL_OPENED; }
   inline WSMessageOnUrlOpened() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
      Value url(kStringType);
      data.AddMember("url", url, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;
      auto& data = d["data"];
      return data.HasMember("url") && data["url"].IsString();
   }
   inline void setUrl(const char* url)
   {
      if (!url)
         return;
      auto& data = d["data"];
      data["url"].SetString(url, d.GetAllocator());
   }
   inline const char* getUrl() const
   {
      return d["data"]["url"].GetString();
   }
};

/// <summary>
/// Sent from Plugin to Studio in case a player was started
/// </summary>
class WSMessageOnBrowserPlayerStarted : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_BROWSER_PLAYER_STARTED; }
   inline WSMessageOnBrowserPlayerStarted() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
   }
   inline bool isvalid() const
   {
      return WSMessage::isvalid();
   }
};

/// <summary>
/// Sent from Plugin to Studio in case a player was stopped
/// </summary>
class WSMessageOnBrowserPlayerStopped : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_BROWSER_PLAYER_STOPPED; }
   inline WSMessageOnBrowserPlayerStopped() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
   }
   inline bool isvalid() const
   {
      return WSMessage::isvalid();
   }
};

/// <summary>
/// Sent from Plugin to Studio in case a player was paused
/// </summary>
class WSMessageOnBrowserPlayerPaused : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_BROWSER_PLAYER_PAUSED; }
   inline WSMessageOnBrowserPlayerPaused() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
   }
   inline bool isvalid() const
   {
      return WSMessage::isvalid();
   }
};

/// <summary>
/// Sent from Client to Studio to impersonate into an user
/// </summary>
class WSMessageOnImpersonation : public WSMessage
{
public:
   inline virtual const char* msgtype() const override { return Type::ON_IMPERSONATION; }
   inline WSMessageOnImpersonation() : WSMessage(msgtype())
   {
      using namespace rapidjson;
      auto& alloc = d.GetAllocator();
      Value& data = d["data"];
      Value userId(kStringType);
      Value firstName(kStringType);
      Value lastName(kStringType);
      data.AddMember("uuid", userId, alloc);
      data.AddMember("firstName", firstName, alloc);
      data.AddMember("lastName", lastName, alloc);
   }
   inline bool isvalid() const
   {
      if (!WSMessage::isvalid())
         return false;
      auto& data = d["data"];
      if (!data.HasMember("uuid") || !data["uuid"].IsString())
         return false;
      const char* s = data["uuid"].GetString();
      if (::strlen(s) != 36)
         return false; // uuid string has 36 characters
      if (s[8] != '-' || s[13] != '-' || s[18] != '-' || s[23] != '-')
         return false; // with four hyphens
      return           // and 32 hex characters
         isxdigit(s[0])  && isxdigit(s[1])  && isxdigit(s[2])  && isxdigit(s[3])  &&
         isxdigit(s[4])  && isxdigit(s[5])  && isxdigit(s[6])  && isxdigit(s[7])  &&
         isxdigit(s[9])  && isxdigit(s[10]) && isxdigit(s[11]) && isxdigit(s[12]) &&
         isxdigit(s[14]) && isxdigit(s[15]) && isxdigit(s[16]) && isxdigit(s[17]) &&
         isxdigit(s[19]) && isxdigit(s[20]) && isxdigit(s[21]) && isxdigit(s[22]) &&
         isxdigit(s[24]) && isxdigit(s[25]) && isxdigit(s[26]) && isxdigit(s[27]) &&
         isxdigit(s[28]) && isxdigit(s[29]) && isxdigit(s[30]) && isxdigit(s[31]) &&
         isxdigit(s[32]) && isxdigit(s[33]) && isxdigit(s[34]) && isxdigit(s[35]);
   }
   inline const char* getUuid() const
   {
      return d["data"].HasMember("uuid") ? d["data"]["uuid"].GetString() : 0;
   }
   inline const char* getFirstName() const
   {
      return d["data"].HasMember("firstName") ? d["data"]["firstName"].GetString() : 0;
   }
   inline const char* getLastName() const
   {
      return d["data"].HasMember("lastName") ? d["data"]["lastName"].GetString() : 0;
   }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BINARY
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Binary Websocket Message
/// </summary>
class WSBMessage
{
public:
   QByteArray mBuffer;

   static constexpr uint16_t TYPE_CUSTOM_FEEDBACK = 1;

   enum Type : uint16_t
   {
      CUSTOM_FEEDBACK = TYPE_CUSTOM_FEEDBACK
   };

   inline bool createMessageCustomFeedback(Classifier* c, const uint16_t index)
   {
      if (!c)
         return false;

      // the node and its input reader
      CustomFeedbackNode* n = c->GetCustomFeedbackNode(index);
      MultiChannelReader* r = n->GetInputReader();

      // channels on reader
      const uint32_t NUMCHANNELS = r ? r->GetNumChannels() : 0;

      // should have at least one..
      if (NUMCHANNELS == 0)
         return false;

      mBuffer.clear();
      mBuffer.append((const char*)&TYPE_CUSTOM_FEEDBACK, sizeof(TYPE_CUSTOM_FEEDBACK));
      mBuffer.append((const char*)&index, sizeof(index));

      for (uint32_t j = 0; j < NUMCHANNELS; j++)
      {
         double v = n->GetCurrentValue();
         mBuffer.append((const char*)&v, sizeof(v));
      }

      return true;
   }
};

#endif
