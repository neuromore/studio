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

#ifndef __NEUROMORE_EXPERIENCEWIDGET_H
#define __NEUROMORE_EXPERIENCEWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <QWidget>
#include <QMovie>
#include <QTimer>
#include <QBuffer>
#include <QNetworkReply>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QHBoxLayout>
#include <Audio/MediaContent.h>
#include <Audio/TonePlayer.h>

// VideoPlayer
#include "../../VideoPlayer.h"

// for system master volume control
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#include <endpointvolume.h>
#endif

class ExperienceWidget : public QWidget, public Core::EventHandler
{
	Q_OBJECT
	CORE_LOGDEBUG_DISABLE("ExperienceWidget", GetReadableType(), this);

	public:
		ExperienceWidget(QWidget* parent=NULL);
		virtual ~ExperienceWidget();

		void UpdateInterface();

		static void DrawPixmapKeepAspectRatio(QWidget* widget, QPainter& painter, const QPixmap& pixmap);

		// EVENTS ----------------------------------------------
		// experience events
		void OnExitStateReached(uint32 exitStatus) override;

		void OnStopSession() override;
		
		void OnPlayAudio(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override;
		void OnStopAudio(const char* url) override;
		void OnSeekAudio(const char* url, uint32 millisecs) override;
		void OnPauseAudio(const char* url, bool unpause) override;

		void OnPlayVideo(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override;
		void OnStopVideo() override;
		void OnSeekVideo(const char* url, uint32 millisecs) override;
		void OnPauseVideo(const char* url, bool unpause) override;

		void OnShowImage(const char* url) override;
		void OnHideImage() override;

		void OnShowText(const char* text, const Core::Color& color) override;
		void OnHideText() override;

		void OnShowTextInput(const char* text, uint32 id) override;
		void OnHideTextInput() override;

		void OnSetBackgroundColor(const Core::Color& color) override;
		
		void OnSetFourZoneAVEColors(const float* red, const float* green, const float* blue, const float* alpha) override;
		void OnHideFourZoneAVE() override;

		void OnShowButton(const char* text, uint32 buttonId) override;
		void OnClearButtons() override;

		void OnActiveStateMachineChanged(StateMachine* stateMachine) override;
		
		// classifier events
		void OnActiveClassifierChanged(Classifier* classifier) override;
		void OnNodeAdded(Graph* graph, Node* node) override;
		void OnRemoveNode(Graph* graph, Node* node) override;
		void OnGraphReset(Graph* graph) override;
		// -----------------------------------------------------

		void SetVolume(float normalizedVolume, const char* filename);
		void SetGlobalVolume(float normalizedVolume);

		void SetPosition(float position, const char* filename);

		inline void SetBlendColor(const QColor& color) 
		{ 
			mBlendColor = color;
			mPixmapBlend.fill(color);
			update();
		}
		inline void SetBlendOpacity(const float opacity) 
		{
			if (mBlendOpacity != opacity)
			{
				mBlendOpacity = opacity;
				update();
			}
		}

		MediaContent* FindMediaContent(const char* filename);

		void Clear();

		// state transition button condition
		void AddButton(const char* text, uint32 buttonId);
		void ClearButtons();

		// clear animated gif
		void ClearAnimatedGif();

	private slots:
		void OnGifAnimationTimer();
		void OnRefreshTimer();

		void OnVideoLooped(QString url);

		void OnButtonClicked();

		 void ShowImage(QImage image);
		 void ShowImage(QPixmap pixmap);

		 void OnFinishedPreloading();

	private:
		void paintEvent(QPaintEvent* event) override;

		QNetworkAccessManager*		mNetworkAccessManager;
		Core::Array<MediaContent*>	mMediaContents;

		QTimer*						mRefreshTimer;

		void SetTimerEnabled(bool isEnabled);
		QTimer*						mGifTimer;
		QBuffer*					mGifBuffer;
		QMovie*						mGifMovie;
		
		QPixmap						mPixmap;
		QPixmap						mPixmapBlend;
		QColor						mBlendColor;
		float						mBlendOpacity;

		QString						mText;
		QColor						mTextColor;

		QColor						mBackgroundColor;
		Core::Array<QColor>			mAVEZoneColors;

		QVBoxLayout*    mMainLayout;

		QPlainTextEdit* mTextEdit;
		QHBoxLayout*    mTextEditLayout;

		Core::Array<QPushButton*>	mButtons;
		QHBoxLayout*				mButtonLayout;

		// .mp4 etc. movie
		QPixmap						mMoviePixmap;
		OpenCVVideoPlayer*			mVideoPlayer;

		// generated tone playback
		Core::Array<TonePlayer*>	mTonePlayers;

#ifdef NEUROMORE_PLATFORM_WINDOWS
		// windows master volume control
		void SetSystemMasterVolume(float normalizedVolume);
		IAudioEndpointVolume*		mEndpointVolume;
#endif
};

#endif
