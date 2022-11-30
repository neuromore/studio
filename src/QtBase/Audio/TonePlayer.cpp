/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "TonePlayer.h"
#include <Engine/Graph/ToneGeneratorNode.h>

using namespace Core;

TonePlayer::TonePlayer(ToneGeneratorNode& node) :
   mNode(node),
   mTimer(0),
   mOutput(0),
   mIODevice(0)
{
   static constexpr int BUFFERSIZE     = ToneGeneratorNode::AUDIOSAMPLERATE*4U/4U; // 250ms
   static constexpr int NOTIFYINTERVAL = 20;
   static constexpr int TIMERINTERVAL  = 50;

   // configure audioformat
   mFormat.setCodec("audio/pcm");
   mFormat.setChannelCount(1);
   mFormat.setSampleRate(ToneGeneratorNode::AUDIOSAMPLERATE);
   mFormat.setByteOrder(QAudioFormat::LittleEndian);
   mFormat.setSampleSize(32);
   mFormat.setSampleType(QAudioFormat::SampleType::Float);

   // check if format is supported
   QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
   if (!info.isFormatSupported(mFormat)) {
      Core::LogError("Audio output device does not support the selected format.");
      return;
   }

   // create audio output with specified format
   mOutput = new QAudioOutput(mFormat);
   mOutput->setBufferSize(BUFFERSIZE);
   mOutput->setNotifyInterval(NOTIFYINTERVAL);

   // validate notify interval
   if (mOutput->notifyInterval() != NOTIFYINTERVAL) {
      Core::LogWarning("Failed to set audio notify interval.");
   }

   // connect signal
   connect(mOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(OnAudioDeviceStateChanged(QAudio::State)));

   // start IO device
   mIODevice = mOutput->start();

   // failed to start it
   if (!mIODevice) {
      Core::LogError("Failed to create audio IO device.");
      return;
   }

   // create timer and start it
   mTimer = new QTimer(this);
   connect(mTimer, SIGNAL(timeout()), this, SLOT(OnAudioTimer()));
   mTimer->setTimerType(Qt::PreciseTimer);
   mTimer->start(TIMERINTERVAL);

   // set default volume
   SetVolume(1.0);
}

TonePlayer::~TonePlayer()
{
   if (mOutput) {
      mOutput->stop();
      delete mOutput;
      mOutput   = 0;
      mIODevice = 0;
   }
   if (mTimer) {
      delete mTimer;
      mTimer = 0;
   }
}

void TonePlayer::OnAudioDeviceStateChanged(QAudio::State newState)
{
   switch (newState) {
   case QAudio::IdleState:
      printf("IDLE STATE \n");
      break;

   case QAudio::StoppedState:
      printf("STOPPED STATE \n");
      break;

   case QAudio::ActiveState:
      printf("ACTIVE STATE\n");
      break;

   case QAudio::InterruptedState:
      printf("INTERRUPTED \n");
      break;

   default:
      printf("UNKNOWN STATE \n");
      break;
   }
}

void TonePlayer::OnAudioTimer()
{
   if (!mOutput || !mIODevice)
      return;

   char*    buf;
   uint32_t len;

   const int nperiod = mOutput->periodSize();

   // process available chunks
   while (mNode.GetChunk(buf, len)) 
   {
      // try to write it to output device
      const qint64 written = mIODevice->write(buf, len);
      if (written <= 0) {
         break;
      }
      else {
         mNode.DidChunk(written);
         //printf("AVAILABLE %05i WRITTEN %05i MISSED %05i PERIOD %05i \n", 
         //   (int)len, (int)written, (int)(len-written), (int)nperiod);
      }
   }
}
