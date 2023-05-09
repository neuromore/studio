/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_TONEPLAYER_H
#define __NEUROMORE_TONEPLAYER_H

 // include required headers
#include "../QtBaseConfig.h"
#include <Engine/Core/StandardHeaders.h>
#include <Engine/Graph/ToneGeneratorNode.h>

#include <QTimer>
#include <QAudioOutput>

class TonePlayer : public QObject
{
   Q_OBJECT

protected:
   ToneGeneratorNode& mNode;
   QAudioFormat       mFormat;
   QTimer*            mTimer;
   QAudioOutput*      mOutput;
   QIODevice*         mIODevice;

public:
   TonePlayer(ToneGeneratorNode& node);
   ~TonePlayer();

   inline ToneGeneratorNode& GetNode()  { return mNode; }
   inline QAudio::State      GetState() { return mOutput ? mOutput->state() : QAudio::State::StoppedState; }

   inline void  SetVolume(qreal v) { if (mOutput) mOutput->setVolume(v); }
   inline qreal GetVolume()        { return mOutput ? mOutput->volume() : 0; }

private slots:
   void OnAudioDeviceStateChanged(QAudio::State newState);
   void OnAudioTimer();
};

#endif
