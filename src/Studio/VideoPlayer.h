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

#ifndef __NEUROMORE_VIDEOPLAYER
#define __NEUROMORE_VIDEOPLAYER

// include required headers
#include "Config.h"
#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QAbstractVideoSurface>
#include <QMediaPlayer>
#include <Backend/WebDataCache.h>
#include <time.h>

// internal qt function
QImage qt_imageFromVideoFrame(const QVideoFrame& f);

// Custom video player creating QImage from frames.
class OpenCVVideoPlayer : public QAbstractVideoSurface
{
   Q_OBJECT

   public:
      // constructor
      OpenCVVideoPlayer(QObject *parent=NULL);

      // destructor
      ~OpenCVVideoPlayer();

      ///////////////////////////////////////////////////////////////////////////////////////////////

      // from QAbstractVideoSurface, specify supported formats
      inline QList<QVideoFrame::PixelFormat> supportedPixelFormats(
         QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override
      {
         return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB32;
      }

      // from QAbstractVideoSurface, convert frame to image
      inline bool present(const QVideoFrame& frame) override
      {
         if (frame.isValid() && mIsPlaying)
         {
            mImage = qt_imageFromVideoFrame(frame);
            emit ProcessedImage(mImage);
            return true;
         }
         else
            return false;
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////

      bool Load(const char* url, WebDataCache* cache);
      bool Clear();
      void Play(int numLoops = -1);
      void Stop();

      inline void Pause()           { mIsPaused = true;  mPlayer.pause(); }
      inline void Continue()        { mIsPaused = false; mPlayer.play(); }
      inline bool IsPlaying() const { return mIsPlaying; }

      // in milliseconds
      inline qint64 GetPosition() const { return mPlayer.position(); }
      inline qint64 GetDuration() const { return mPlayer.duration(); }
      inline void   SetPosition(qint64 milliseconds) { mPlayer.setPosition(milliseconds); }
      inline void   SetPositionNormalized(double normalizedPos) 
      {
         if      (normalizedPos < 0.0) SetPosition(0);
         else if (normalizedPos > 1.0) SetPosition(GetDuration());
         else                          SetPosition(normalizedPos * GetDuration());
      }

   signals:
      // called when a new video frame got rendered
      void ProcessedImage(QImage image);

      // fired when video playback reached the end
      void ReachedEnd(QString url);

   protected slots:
      void OnMediaStatusChanged(QMediaPlayer::MediaStatus status);

   private:
      Core::String   mUrl;
      Core::String   mKey;
      bool           mIsPlaying;
      bool           mIsPaused;
      QImage         mImage;
      int            mNumPlayCount;
      int            mCurrentPlayCount;
      QMediaPlayer   mPlayer;
};

#endif
