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


class QTBASE_API MediaContent : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		MediaContent(QObject* parent, WebDataCache* cache, const char* url);
		virtual ~MediaContent();

		void Play(double normalizedVolume, int32 numLoops, bool resetNumCurrentLoops=true);
		void Stop();
		void Pause() const										{ if (mMediaPlayer != NULL) mMediaPlayer->pause(); }
		void Continue() const									{ if (mMediaPlayer != NULL) mMediaPlayer->play(); }
		void SetPosition(uint64 pos)							{ if (mMediaPlayer != NULL) mMediaPlayer->setPosition(pos); }
		void SetVolume(float normalizedVolume)					{ if (mMediaPlayer != NULL) mMediaPlayer->setVolume(normalizedVolume*100.0); }
		
		uint64 GetPosition() const								{ if (mMediaPlayer != NULL) return mMediaPlayer->position(); return 0; }
		uint64 GetDuration() const								{ if (mMediaPlayer != NULL) return mMediaPlayer->duration(); return 0; }

		bool IsAudioAvailable() const							{ if (mMediaPlayer != NULL) return mMediaPlayer->isAudioAvailable(); return false; }

		bool HasFinished() const								{ return mHasFinished; }

		const char* GetUrl() const								{ return mUrl.AsChar(); }
		const Core::String& GetUrlString() const				{ return mUrl; }

	signals:
		void Looped(const Core::String& url);

	private slots:
		void OnMediaPositionChanged(qint64 position);
		void OnMediaStatusChanged(QMediaPlayer::MediaStatus status);

	private:
		void OnLooped(const Core::String& url);

		QMediaPlayer*	mMediaPlayer;
		Core::String	mUrl;
		float			mVolume;
		int32			mCurrentLoops;
		int32			mMaxLoops;
		bool			mHasFinished;
};


#endif
