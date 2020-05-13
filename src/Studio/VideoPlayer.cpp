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

// include required headers
#include "VideoPlayer.h"
#include <Core/LogManager.h>

using namespace Core;

OpenCVVideoPlayer::OpenCVVideoPlayer(QObject *parent) : 
   mPlayer(this),
   mIsPlaying(false),
   mIsPaused(false),
   mCurrentPlayCount(0),
   mNumPlayCount(0),
   mImage(64, 64, QImage::Format_RGB32),
   mDefaultImage(64, 64, QImage::Format_RGB32)
{
   mDefaultImage.fill(Qt::black);
   mPlayer.setVideoOutput(this);
   connect(&mPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(OnMediaStatusChanged(QMediaPlayer::MediaStatus)));
}

OpenCVVideoPlayer::~OpenCVVideoPlayer()
{
   mPlayer.stop();
   Clear();
}

void OpenCVVideoPlayer::UpdateDefaultImage()
{
   const QSize videoSize = mImage.size();
   const QSize defaultSize = mDefaultImage.size();

   if (videoSize != defaultSize)
   {
      mDefaultImage = QImage( videoSize, QImage::Format_RGB32 );
      mDefaultImage.fill( Qt::black );
   }
}

void OpenCVVideoPlayer::SetToDefaultImage()
{
   UpdateDefaultImage();
   mImage = mDefaultImage;
}

bool OpenCVVideoPlayer::Load(const char* url, WebDataCache* cache)
{
   Clear();

   // save url and try resolve to cached local file
   mUrl = url;
   mKey = cache->GetCacheFilenameForUrl(url);

   // cached/local-file mode
   if (mUrl != mKey)
      mPlayer.setMedia(QUrl::fromLocalFile(mKey.AsChar()));

   // streaming mode
   else
      mPlayer.setMedia(QUrl(mUrl.AsChar()));

   return true;
}

bool OpenCVVideoPlayer::Clear()
{
   Stop();
   mUrl.Clear();
   mKey.Clear();
   return true;
}

void OpenCVVideoPlayer::Play(int numPlayCount)
{
   mIsPlaying = true;
   mIsPaused = false;
   mCurrentPlayCount = 0;
   mNumPlayCount = numPlayCount;

   mPlayer.setVolume(100);
   mPlayer.play();
}

void OpenCVVideoPlayer::Stop()
{
   mIsPlaying = false;
   mIsPaused = false;
   mPlayer.stop();
}

void OpenCVVideoPlayer::OnMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
   if (status == QMediaPlayer::EndOfMedia)
   {
      // increase the current number of loops
      mCurrentPlayCount++;

      // loop forever or not yet as often as requested
      if (mNumPlayCount < 0 || mCurrentPlayCount < mNumPlayCount)
         mPlayer.play();

      // full reset
      else
         Stop();
   }
}
