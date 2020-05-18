/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_MEDIACONTENT_H
#define __NEUROMORE_MEDIACONTENT_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include "../Backend/WebDataCache.h"
#include <QMediaPlayer>
#include <QAbstractVideoSurface>

class QTBASE_API MediaContent : public QAbstractVideoSurface
{
   Q_OBJECT

   public:
      // constructor
      MediaContent(QObject* parent, WebDataCache* cache, const char* url);

      // destructor
      ~MediaContent();

      ///////////////////////////////////////////////////////////////////////////////////////////////

      // from QAbstractVideoSurface, specify supported formats
      inline QList<QVideoFrame::PixelFormat> supportedPixelFormats(
         QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override
      {
         return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB32;
      }

      // from QAbstractVideoSurface, do nothing with the frame (audio only)
      inline bool present(const QVideoFrame& frame) override
      {
         return true;
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////

      void Play(double normalizedVolume, int32 numLoops, bool resetNumCurrentLoops=true);
      void Stop();
      void Pause()                           { mMediaPlayer.pause(); }
      void Continue()                        { mMediaPlayer.play(); }
      void SetPosition(uint64 pos)           { mMediaPlayer.setPosition(pos); }
      void SetVolume(float normalizedVolume) { mMediaPlayer.setVolume(normalizedVolume*100.0); }

      uint64 GetPosition() const                { return mMediaPlayer.position(); }
      uint64 GetDuration() const                { return mMediaPlayer.duration(); }
      bool IsAudioAvailable() const             { return mMediaPlayer.isAudioAvailable(); }
      bool HasFinished() const                  { return mHasFinished; }
      const char* GetUrl() const                { return mUrl.AsChar(); }
      const Core::String& GetUrlString() const  { return mUrl; }

   signals:
      void Looped(const Core::String& url);

   private slots:
      void OnMediaStatusChanged(QMediaPlayer::MediaStatus status);

   private:
      void OnLooped(const Core::String& url);

      QMediaPlayer   mMediaPlayer;
      Core::String   mUrl;
      float          mVolume;
      int32          mCurrentLoops;
      int32          mMaxLoops;
      bool           mHasFinished;
};

#endif
