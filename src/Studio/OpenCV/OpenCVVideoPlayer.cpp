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
#include "OpenCVVideoPlayer.h"
#ifdef OPENCV_SUPPORT
#include <Core/LogManager.h>
#include <Core/Timer.h>
#include <chrono>
#include <thread>


using namespace Core;
using namespace cv;

// constructor
OpenCVVideoPlayer::OpenCVVideoPlayer(QObject *parent) : QThread(parent)
{
	mVideoCapture	= NULL;
	mIsPlaying		= false;
	mIsPaused		= false;
	mMediaContent	= NULL;

	mDefaultImage = QImage( 64, 64, QImage::Format_RGB16 );
	mDefaultImage.fill( Qt::black );
}


// destructor
OpenCVVideoPlayer::~OpenCVVideoPlayer()
{
	if (mMediaContent != NULL)
	{
		mMediaContent->Stop();
		delete mMediaContent;
	}

	Clear();
}


void OpenCVVideoPlayer::UpdateDefaultImage()
{
	QSize videoSize = mImage.size();
	QSize defaultSize = mDefaultImage.size();

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


// load a video from file
bool OpenCVVideoPlayer::Load(const char* url, const char* key, WebDataCache* cache)
{
	// cleanup the old video capture
	Clear();

	mUrl = url;
	mKey = key;

	mVideoCapture = new cv::VideoCapture(url);
	if (mVideoCapture->isOpened() == false)
	{
		LogError( "OpenCVVideoPlayer: Cannot load video '%s'.", url );
		return false;
	}

	mFrameRate = (int)mVideoCapture->get(CV_CAP_PROP_FPS);

	// audio
	mMediaContent = new MediaContent( parent(), cache, key );
	connect( mMediaContent, SIGNAL(Looped(const Core::String&)), this, SLOT(OnMediaLooped(const Core::String&)) );

	return true;
}


// clear
bool OpenCVVideoPlayer::Clear()
{
	Stop();

	wait();

	mMutex.lock();

	// cleanup the old video capture
	if (mVideoCapture != NULL)
	{
		mVideoCapture->release();
		delete mVideoCapture;
	}

	// cleanup the old media audio content
	if (mMediaContent != NULL)
	{
		mMediaContent->deleteLater();
		mMediaContent = NULL;
	}

	mVideoCapture = NULL;
	mUrl.Clear();

	mWaitCondition.wakeOne();
	mMutex.unlock();

	return true;
}


// play video
void OpenCVVideoPlayer::Play(int numPlayCount)
{
	// in case the thread is not running yet
	if (isRunning() == false)
	{
		start(LowPriority);
		mIsPlaying = true;
		mIsPaused = false;
		mCurrentPlayCount = 0;
		mNumPlayCount = numPlayCount;
		//mIsPaused = false;

		if (mMediaContent != NULL)
			mMediaContent->Play( 1.0, numPlayCount );
	}
}


// stop
void OpenCVVideoPlayer::Stop()
{
	mIsPlaying = false;
	mIsPaused = false;

	if (mMediaContent != NULL)
		mMediaContent->Stop();
}


// run the thread
void OpenCVVideoPlayer::run()
{
	// start and reset the timer
	Timer timer;

	while (mIsPlaying == true && mMediaContent != NULL)
	{
		// reset the timer
		timer.GetTimeDelta();

		if (mIsPaused == true)
			return;

		// let the audio drive the video in case there is audio in the video :)
		bool syncVideoToAudio = mMediaContent->IsAudioAvailable();

		// detect when we reached the end
		double currentFrame = GetCurrentFrame();
		double numFrames = GetNumFrames();

		// set the real update rate
		double delay = (1000.0 / mFrameRate);
		//if (syncVideoToAudio == true)
			//delay *= 0.2; // in case we're syncing video to audio, double the speed

		// let the audio drive the video in case there is audio in the video :)
		if (syncVideoToAudio == true)
		{
			// set the video to the correct frame
			//SetPosition( mMediaContent->GetPosition() );

			//const double normalizedAudioPos = (double)mMediaContent->GetPosition() / (double)mMediaContent->GetDuration();
			//SetPositionNormalized( normalizedAudioPos );

			const double audioTimePassedInSeconds = (double)mMediaContent->GetPosition() * 0.001;
			const double frame = audioTimePassedInSeconds * mFrameRate;

			SetCurrentFrame( frame );

			//LogInfo("Setting pos %.0f ms", (double)mMediaContent->GetPosition());
		}

		// read a frame from the video capture
		bool captureResult = true;
		if (mVideoCapture->read(mFrame) == false)
		{
			LogWarning("Capturing frame failed.");
			captureResult = false;
			SetToDefaultImage();
		}
		else
		{
			// translate the OpenCV data matrix to a Qt image
			if (mFrame.channels() == 3)
			{
				cv::cvtColor( mFrame, mRgbFrame, CV_BGR2RGB );
				mImage = QImage( (const unsigned char*)(mRgbFrame.data), mRgbFrame.cols, mRgbFrame.rows, QImage::Format_RGB888 );
			}
			else
			{
				mImage = QImage( (const unsigned char*)(mFrame.data), mFrame.cols, mFrame.rows, QImage::Format_Indexed8 );
			}
		}

		const double normalizedVideoPos = GetPosition() / mMediaContent->GetDuration();
		const double normalizedAudioPos = (double)mMediaContent->GetPosition() / (double)mMediaContent->GetDuration();

		//LogError("Audio=(%i/%i), AudioNorm=%f, Video=(%.0f/%.0f), VideoNorm=%f, DiffAbs=%f", (int)mMediaContent->GetPosition(), (int)mMediaContent->GetDuration(), (float)normalizedAudioPos, (float)GetPosition(), (float)GetDuration(), (float)normalizedVideoPos, normalizedAudioPos-normalizedVideoPos);

		//LogError("Audio=%f, Video=%f, Diff=%f", normalizedAudioPos, normalizedVideoPos, (normalizedVideoPos-normalizedAudioPos)*100.0);

		/*const double differenceInMs = 100.0;
		if (Math::AbsD(GetPosition() - mMediaContent->GetPosition()) > differenceInMs)
		{
			LogError("bigger tha %fms at %dms", differenceInMs, mMediaContent->GetPosition());
			SetPosition( mMediaContent->GetPosition() );
		}*/

		// check looping in case there is no audio
		if (syncVideoToAudio == false)
		{
			if (currentFrame >= numFrames)
			{
				emit ReachedEnd( mKey.AsChar() );
				mCurrentPlayCount++;

				if (mNumPlayCount == -1 || mCurrentPlayCount < mNumPlayCount)
				{
					// set it to the beginning again
					SetCurrentFrame( 0 );
				}
				else
				{
					Stop();
					SetToDefaultImage();
				}
			}
		}

		emit ProcessedImage( mImage );

		// get the time delta
		double timeDelta = timer.GetTimeDelta().InMilliseconds();
		//double time = timer.GetTime().InMilliseconds();

		// sleep only the real amount of time so that we're not delaying with our code
		double sleepInMs = delay - timeDelta;
		if (sleepInMs > 0.0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepInMs));
			//LogInfo("Video thread sleep %.1f ms", sleepInMs);
		}
	}

	SetToDefaultImage();
	emit ProcessedImage( mImage );
}


void OpenCVVideoPlayer::OnMediaLooped(const Core::String& url)
{
	emit ReachedEnd( mKey.AsChar() );
	mCurrentPlayCount++;

	if (mCurrentPlayCount >= mNumPlayCount && mNumPlayCount != -1)
	{
		Stop();
	}
}

#endif
