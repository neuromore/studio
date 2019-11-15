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

#ifndef __NEUROMORE_OPENCVVIDEOPLAYER
#define __NEUROMORE_OPENCVVIDEOPLAYER

// include required headers
#include "../Config.h"
#ifdef OPENCV_SUPPORT

#include <opencv2/opencv.hpp>

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <Audio/MediaContent.h>
#include <Backend/WebDataCache.h>
#include <time.h>


// OpenCV video player
class OpenCVVideoPlayer : public QThread
{
	Q_OBJECT

	public:
		// constructor & destructor
		OpenCVVideoPlayer(QObject *parent=NULL);
		virtual ~OpenCVVideoPlayer();

		bool Load(const char* url, const char* key, WebDataCache* cache);
		bool Clear();

		void Play(int numPlayCount=-1);
		void Pause()											{ mIsPaused = true; if (mMediaContent != NULL) mMediaContent->Pause(); }
		void Continue()											{ mIsPaused = false; if (mMediaContent != NULL) mMediaContent->Continue();}
		void Stop();

		bool IsPlaying() const									{ return mIsPlaying; }
		void SetCurrentFrame(int frameNumber)					{ if (mVideoCapture != NULL) mVideoCapture->set(CV_CAP_PROP_POS_FRAMES, frameNumber); }
		//void SetPosition(double milliseconds)					{ SetCurrentFrame( milliseconds * 0.001 * mFrameRate ); }
		void SetPositionNormalized(double normalizedPos)		{ if (normalizedPos < 0.0) normalizedPos = 0.0; if (normalizedPos > 1.0) normalizedPos = 1.0; SetCurrentFrame( normalizedPos * GetNumFrames() ); }

		double GetFrameRate()									{ return mFrameRate; }
		double GetCurrentFrame()								{ return mVideoCapture->get(CV_CAP_PROP_POS_FRAMES); }
		double GetNumFrames()									{ return mVideoCapture->get(CV_CAP_PROP_FRAME_COUNT); }

		// in milliseconds
		double GetPosition()									{ return (GetCurrentFrame() / mFrameRate) * 1000.0; }
		double GetDuration()									{ return (GetNumFrames() / mFrameRate) * 1000.0; }

		uint32 GetWidth()										{ return mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH); }
		uint32 GetHeight()										{ return mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT); }

	signals:
		// called when a new video frame got rendered
		void ProcessedImage(QImage image);

		// fired when video playback reached the end
		void ReachedEnd(QString url);

	protected:
		void run();

	protected slots:
		void OnMediaLooped(const Core::String& url);
	
	private:
		void UpdateDefaultImage();
		void SetToDefaultImage();

		Core::String		mUrl;
		Core::String		mKey;
		bool				mIsPlaying;
		bool				mIsPaused;
		QMutex				mMutex;
		QWaitCondition		mWaitCondition;
		cv::Mat				mFrame;
		cv::Mat				mRgbFrame;
		double				mFrameRate;
		cv::VideoCapture*	mVideoCapture;
		QImage				mImage;
		QImage				mDefaultImage;
		int					mNumPlayCount;
		int					mCurrentPlayCount;

		// audio
		MediaContent*		mMediaContent;
};

#endif

#endif
