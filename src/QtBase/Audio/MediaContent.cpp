/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "MediaContent.h"
#include <QFileInfo>
#include <EngineManager.h>
#include "Graph/StateTransitionButtonCondition.h"
#include "Graph/StateTransitionAudioCondition.h"
#include "Graph/StateTransitionVideoCondition.h"

using namespace Core;

// constructor
MediaContent::MediaContent(QObject* parent, WebDataCache* cache, const char* url) : 
   QAbstractVideoSurface(parent),
   mMediaPlayer(this),
   mUrl(url)
{
   mMediaPlayer.setVideoOutput(this);

   connect(&mMediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(OnMediaStatusChanged(QMediaPlayer::MediaStatus)));

   if (cache->FileExists(url) == true)
   {
      // file cached route
      String filename = cache->GetCacheFilenameForUrl(url);
      QFileInfo fileInfo(filename.AsChar());
      QUrl localUrl = QUrl::fromLocalFile(fileInfo.absoluteFilePath());

      //LogInfo("Url: %s", localUrl.toString().toLatin1().constData() );
      //LogInfo("Filename: %s", filename.AsChar() );
      mMediaPlayer.setMedia(localUrl);
   }
   else
   {
      // not cached, stream from the web
      mMediaPlayer.setMedia(QUrl(url));
   }

   mHasFinished = false;
   mCurrentLoops = 0;
   mMaxLoops = 0;
}

MediaContent::~MediaContent()
{
}

void MediaContent::Play(double normalizedVolume, int32 numLoops, bool resetNumCurrentLoops)
{
   if (resetNumCurrentLoops == true)
      mCurrentLoops = 0;

   mMaxLoops = numLoops;
   mVolume = normalizedVolume;
   mMediaPlayer.setVolume( mVolume * 100.0 );
   mMediaPlayer.play();
}

void MediaContent::Stop()
{
   mMediaPlayer.stop();
}

void MediaContent::OnMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
   //LogInfo("%i", status);

   if (status == QMediaPlayer::EndOfMedia)
   {
      //LogInfo("QMediaPlayer::EndOfMedia");

      // increase the current number of loops
      mCurrentLoops++;

      // do we want to loop?
      if (mMaxLoops < 0)
         Play(mVolume, mMaxLoops, false);
      else
      {
         // check if we reached the number of maximum loops already
         if (mCurrentLoops < mMaxLoops)
            Play(mVolume, mMaxLoops, false);
      }

      // increase loop counter and play again in case wanted
      OnLooped( mUrl );
      emit Looped( mUrl );
   }
}

void MediaContent::OnLooped(const String& url)
{
   // get the active state machine
   StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
   if (stateMachine == NULL)
      return;

   // get the number of transitions and iterate through them
   const uint32 numTransitions = stateMachine->GetNumConnections();
   for (uint32 i=0; i<numTransitions; ++i)
   {
      StateTransition* transition = stateMachine->GetTransition(i);

      // get the number of conditions and iterate through them
      const uint32 numConditions = transition->GetNumConditions();
      for (uint32 j=0; j<numConditions; ++j)
      {
         StateTransitionCondition* condition = transition->GetCondition(j);

         // check if the current condition is a audio condition
         if (condition->GetType() == StateTransitionAudioCondition::TYPE_ID)
         {
            StateTransitionAudioCondition* audioCondition = static_cast<StateTransitionAudioCondition*>(condition);

            // check if the condition id equals the button id
            if (url.IsEqual(audioCondition->GetUrl()) == true)
            {
               // increase the loop counter
               audioCondition->Looped();

               //LogInfo( "AudioCondition increased loop counter '%i'.", audioCondition->GetLoops() );
               return;
            }
         }
      }
   }
}
