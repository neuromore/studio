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

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "ExperienceWidget.h"
#include "Graph/StateTransitionButtonCondition.h"
#include "Graph/StateTransitionAudioCondition.h"
#include "Graph/StateTransitionVideoCondition.h"
#include "Graph/VolumeControlNode.h"
#include "Graph/ScreenBrightnessNode.h"

// for system master volume control
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#include <stdio.h>
	#include <windows.h>
	#include <mmdeviceapi.h>
	#include <endpointvolume.h>
#endif

using namespace Core;

// constructor
ExperienceWidget::ExperienceWidget(QWidget* parent) : 
   QWidget(parent), 
   mPixmapBlend(64, 64), 
   mBlendColor(Qt::black), 
   mBlendOpacity(0.0f)
{
	mGifMovie = NULL;

	// create the button layout
	mButtonLayout = new QHBoxLayout();
	mButtonLayout->setAlignment( Qt::AlignBottom );
	setLayout( mButtonLayout );

	CORE_EVENTMANAGER.AddEventHandler(this);

	// create gif animation timer
	LogDebug("Experience Widget: Starting experience widget gif animation timer ...");
	mGifTimer = new QTimer(this);
	connect( mGifTimer, SIGNAL(timeout()), this, SLOT(OnGifAnimationTimer()) );
	SetTimerEnabled(false);

	// create refresh timer
	LogDebug("Experience Widget: Starting experience widget refresh timer ...");
	mRefreshTimer = new QTimer(this);
	connect( mRefreshTimer, SIGNAL(timeout()), this, SLOT(OnRefreshTimer()) );
	const uint32 refreshFPS = 30;
	mRefreshTimer->setTimerType( Qt::PreciseTimer );
	mRefreshTimer->start( 1000 / refreshFPS );

	setMinimumSize( QSize( 50, 50) );

	// create the network access manager for downloading images etc.
	mNetworkAccessManager = new QNetworkAccessManager(this);

	// create the video player
	mVideoPlayer = new OpenCVVideoPlayer(this);
	connect(mVideoPlayer, SIGNAL(ProcessedImage(QImage)), this, SLOT(ShowImage(QImage)));
	connect(mVideoPlayer, SIGNAL(ReachedEnd(QString)), this, SLOT(OnVideoLooped(QString)));

	// start pre-loading assets in case a state machine already got loaded
	QTimer::singleShot(1000, this, SLOT(PreloadAssets())); // wait one 1 second and then start pre-loading data

	// AVE pattern and background
	mBackgroundColor = QColor(60, 60, 59);
	mAVEZoneColors.Resize(4);
	for (uint32 i = 0; i < 4; ++i)
		mAVEZoneColors[i] = mBackgroundColor;

	// init blend image
	SetBlendColor(mBlendColor);

#ifdef NEUROMORE_PLATFORM_WINDOWS

	// system master volume control
	HRESULT hr;
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	mEndpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&mEndpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;
#endif
}


// destructor
ExperienceWidget::~ExperienceWidget()
{
	Clear();

	CORE_EVENTMANAGER.RemoveEventHandler(this);


#ifdef NEUROMORE_PLATFORM_WINDOWS
	
	// system master volume control
	if (mEndpointVolume != NULL) 
	{
		mEndpointVolume->Release();
		CoUninitialize();
	}
#endif

}


void ExperienceWidget::SetTimerEnabled(bool isEnabled)
{
	if (isEnabled == true)
	{
		const uint32 updateFPS = 30;
		mGifTimer->setTimerType( Qt::PreciseTimer );
		mGifTimer->start( 1000 / updateFPS );
	}
	else
	{
		mGifTimer->stop();
	}
}


void ExperienceWidget::OnGifAnimationTimer()
{
	if (mGifMovie != NULL)
		ShowImage( mGifMovie->currentPixmap() );
}


void ExperienceWidget::OnRefreshTimer()
{
	// get access to the active classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier != NULL)
	{
		uint32 numFeedBackNodes = classifier->GetNumCustomFeedbackNodes();
		bool isSystemVolumeControlUsed = false;
		bool isStudioVolumeControlUsed = false;

		Node* node = nullptr;
		for (uint32 i = 0; i < numFeedBackNodes; ++i) {
			node = classifier->GetCustomFeedbackNode(i);
			const char* nodeTypeUuid = node->GetTypeUuid();

			if (nodeTypeUuid == VolumeControlNode::Uuid()) {
				VolumeControlNode* feedbackNode = static_cast<VolumeControlNode*>(node);

				if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false) {
						if (!isStudioVolumeControlUsed && feedbackNode->GetMode() == VolumeControlNode::STUDIO_VOLUME) {
							isStudioVolumeControlUsed = true;
							SetGlobalVolume( feedbackNode->GetCurrentValue() );
						} else if (feedbackNode->GetMode() == VolumeControlNode::SINGLE_MEDIA_FILE) {
							isSystemVolumeControlUsed = true;
							SetVolume(feedbackNode->GetCurrentValue(), feedbackNode->GetFileUrl());
						} else if (feedbackNode->GetMode() == VolumeControlNode::TONE) {
							String nodeName = feedbackNode->GetToneName();
							for (uint32 j = 0; j < mTonePlayers.Size(); ++j) {
								if (mTonePlayers[j]->GetNode().GetName() == nodeName) {
									mTonePlayers[j]->SetVolume(feedbackNode->GetCurrentValue());
								}
							}
						}
#ifdef NEUROMORE_PLATFORM_WINDOWS
						else if (!isSystemVolumeControlUsed && feedbackNode->GetMode() == VolumeControlNode::OVERALL_SYSTEM_VOLUME) {
							SetSystemMasterVolume( feedbackNode->GetCurrentValue() );
						}
#endif
				}
			} else if (nodeTypeUuid == ScreenBrightnessNode::Uuid()) {
				ScreenBrightnessNode* feedbackNode = static_cast<ScreenBrightnessNode*>(node);
				if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false) {
					SetBlendOpacity(1.0f - Clamp(feedbackNode->GetCurrentValue(), 0.0, 1.0));
				}
			}
		}
	}
}


// called when all assets got downloaded
void ExperienceWidget::OnFinishedPreloading()
{
	LogInfo("Finished asset preloading");

	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine != NULL)
	{
		stateMachine->Start();
	}

	// FIXME this crashes the engine sometimes during layout switch?!
	// reset engine
	GetEngine()->Reset();
}


void ExperienceWidget::Clear()
{
	// hide the current image and shown text
	OnHideImage();
	OnHideText();
	OnStopVideo();

	// clear all buttons
	ClearButtons();

	// get rid of all media contents
	const uint32 numMediaItems = mMediaContents.Size();
	for (uint32 i=0; i<numMediaItems; ++i)
	{
		mMediaContents[i]->Stop();
		delete mMediaContents[i];
	}

	mMediaContents.Clear();
}


// add a new button condition button
void ExperienceWidget::AddButton(const char* text, uint32 buttonId)
{
	QPushButton* button = new QPushButton( text, this );
	button->setProperty( "buttonId", buttonId );
	connect( button, SIGNAL(clicked()), this, SLOT(OnButtonClicked()) );
	mButtons.Add(button);
	mButtonLayout->addWidget(button);
}


// get rid of all button condition buttons
void ExperienceWidget::ClearButtons()
{
	const uint32 numButtons = mButtons.Size();
	for (uint32 i=0; i<numButtons; ++i)
		mButtons[i]->deleteLater();
	mButtons.Clear();
}


// called when the button got clicked
void ExperienceWidget::OnButtonClicked()
{
	QPushButton* button = qobject_cast<QPushButton*>( sender() );

	uint32 buttonId = button->property("buttonId").toInt();

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

			// check if the current condition is a button condition
			if (condition->GetType() == StateTransitionButtonCondition::TYPE_ID)
			{
				StateTransitionButtonCondition* buttonCondition = static_cast<StateTransitionButtonCondition*>(condition);

				// check if the condition id equals the button id
				if (buttonCondition->GetID() == buttonId)
				{
					// click the button
					buttonCondition->ButtonClicked();

					//LogDebug( "Button '%s' clicked.", button->text().toLatin1().data() );
					return;
				}
			}
		}
	}
}


void ExperienceWidget::ShowImage(QImage image)
{
	if (image.isNull() == true)
		return;

	mPixmap = QPixmap::fromImage(image);
	update();
}


void ExperienceWidget::ShowImage(QPixmap pixmap)
{
	mPixmap = pixmap;
	update();
}


void ExperienceWidget::DrawPixmapKeepAspectRatio(QWidget* widget, QPainter& painter, const QPixmap& pixmap)
{
	int		widgetWidth		= widget->width();
	int		widgetHeight	= widget->height();
	QRectF	widgetTarget	= QRectF(0, 0, widgetWidth, widgetHeight);

	// C) image
	if (pixmap.isNull() == false)
	{
		int pixmapWidth			= pixmap.width();
		int pixmapHeight		= pixmap.height();
		const float widthRatio	= (float)widgetWidth / (float)pixmapWidth;
		const float heightRatio	= (float)widgetHeight / (float)pixmapHeight;
		
		if (widthRatio > heightRatio)
		{
			float newHeight		= pixmapHeight * widthRatio;
			float heightDiff	= newHeight - widgetHeight;
			float scaledOffsetY	= 0.5 * heightDiff;
			float offsetY		= scaledOffsetY * (1.0f / widthRatio);
			
			QRectF source( 0, offsetY, pixmapWidth, pixmapHeight-2.0*offsetY );
			painter.drawPixmap( widgetTarget, pixmap, source );

			//LogInfo("1: Target: (%.0f, %.0f, %.0f, %.0f), Source: (%.0f, %.0f, %.0f, %.0f)", target.x(), target.y(), target.width(), target.height(), source.x(), source.y(), source.width(), source.height());
		}
		else
		{
			float newWidth		= pixmapWidth * heightRatio;
			float widthDiff		= newWidth - widgetWidth;
			float scaledOffsetX	= 0.5 * widthDiff;
			float offsetX		= scaledOffsetX * (1.0f / heightRatio);

			QRectF source( offsetX, 0, pixmapWidth-2.0*offsetX, pixmapHeight );
			painter.drawPixmap( widgetTarget, pixmap, source );

			//LogInfo("2: Target: (%.0f, %.0f, %.0f, %.0f), Source: (%.0f, %.0f, %.0f, %.0f)", target.x(), target.y(), target.width(), target.height(), source.x(), source.y(), source.width(), source.height());
		}
	}
}


void ExperienceWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setOpacity(1.0f);

    if (mPixmapBlend.size() != size())
       mPixmapBlend = mPixmapBlend.scaled(size());

	int widgetWidth = width();
	int widgetHeight = height();
	QRectF widgetTarget = QRectF(0, 0, widgetWidth, widgetHeight);

	// A) background color
	painter.setBrush(mBackgroundColor);
	painter.setPen(mBackgroundColor);
	painter.drawRect(rect());

	// AVE is non-prod only
#ifndef PRODUCTION_BUILD
	
	// B) 4-Zone AVE as rects
	const float halfWidth = widgetWidth / 2.0f;
	const float zoneWidth = 0.2f * halfWidth;
	const float dividerWidth = 0.05f * halfWidth;

	QRectF zoneLL(0, 0, zoneWidth, widgetHeight);												// left zone of left eye
	painter.setBrush(mAVEZoneColors[0]);
	painter.setPen(mAVEZoneColors[0]);
	painter.drawRect(zoneLL);

	QRectF zoneLR(halfWidth - zoneWidth - dividerWidth, 0, zoneWidth, widgetHeight);			// right zone of left eye
	painter.setBrush(mAVEZoneColors[1]);
	painter.setPen(mAVEZoneColors[1]);
	painter.drawRect(zoneLR);

	QRectF zoneRL(halfWidth + dividerWidth, 0, zoneWidth, widgetHeight);						// left zone of right eye
	painter.setBrush(mAVEZoneColors[2]);
	painter.setPen(mAVEZoneColors[2]);
	painter.drawRect(zoneRL);

	QRectF zoneRR(widgetWidth - zoneWidth, 0, zoneWidth, widgetHeight);							// right zone of right eye
	painter.setBrush(mAVEZoneColors[3]);
	painter.setPen(mAVEZoneColors[3]);
	painter.drawRect(zoneRR);

#endif

	// C) image
	if (mPixmap.isNull() == false)
	{
		int pixmapWidth		= mPixmap.width();
		int pixmapHeight	= mPixmap.height();
		const float widthRatio = (float)widgetWidth / (float)pixmapWidth;
		const float heightRatio = (float)widgetHeight / (float)pixmapHeight;
		
		if (widthRatio > heightRatio)
		{
			float newHeight		= pixmapHeight * widthRatio;
			float heightDiff	= newHeight - widgetHeight;
			float scaledOffsetY	= 0.5 * heightDiff;
			float offsetY		= scaledOffsetY * (1.0f / widthRatio);
			
			QRectF source( 0, offsetY, pixmapWidth, pixmapHeight-2.0*offsetY );
			painter.drawPixmap( widgetTarget, mPixmap, source );

			//LogInfo("1: Target: (%.0f, %.0f, %.0f, %.0f), Source: (%.0f, %.0f, %.0f, %.0f)", target.x(), target.y(), target.width(), target.height(), source.x(), source.y(), source.width(), source.height());
		}
		else
		{
			float newWidth		= pixmapWidth * heightRatio;
			float widthDiff		= newWidth - widgetWidth;
			float scaledOffsetX	= 0.5 * widthDiff;
			float offsetX		= scaledOffsetX * (1.0f / heightRatio);

			QRectF source( offsetX, 0, pixmapWidth-2.0*offsetX, pixmapHeight );
			painter.drawPixmap( widgetTarget, mPixmap, source );

			//LogInfo("2: Target: (%.0f, %.0f, %.0f, %.0f), Source: (%.0f, %.0f, %.0f, %.0f)", target.x(), target.y(), target.width(), target.height(), source.x(), source.y(), source.width(), source.height());
		}
	}

	// D) BlendImage
	painter.setOpacity(mBlendOpacity);
	painter.drawPixmap(0, 0, mPixmapBlend);
	painter.setOpacity(1.0);

	// set painter pend and brush to text color
	painter.setPen( mTextColor );
	painter.setBrush( mTextColor );

	QFont font;

	const uint32 textLength = mText.size();
	const float textScaler = Core::RemapRange( textLength, 0, 160, 48, 8 );

	font.setPixelSize( textScaler );
	painter.setFont( font );
	painter.drawText( QRect( 0, 0, width(), height() ), Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, mText );
	
    painter.end();
}


// update displayed interface information
void ExperienceWidget::UpdateInterface()
{
}


void ExperienceWidget::ClearAnimatedGif()
{
	if (mGifMovie != NULL)
	{
		SetTimerEnabled(false);
		mGifMovie->stop();
		mGifMovie->deleteLater();
		mGifMovie = NULL;
		mGifBuffer->deleteLater();
		mGifBuffer = NULL;
	}
}


MediaContent* ExperienceWidget::FindMediaContent(const char* filename)
{
	const uint32 numMediaItems = mMediaContents.Size();
	for (uint32 i=0; i<numMediaItems; ++i)
	{
		if (mMediaContents[i]->GetUrlString().IsEqual(filename) == true)
			return mMediaContents[i];
	}

	return NULL;
}


// set the volume of a given audio or video content
void ExperienceWidget::SetVolume(float normalizedVolume, const char* filename)
{
	MediaContent* mediaContent = FindMediaContent(filename);
	if (mediaContent != NULL)
		mediaContent->SetVolume(normalizedVolume);
	else if (mVideoPlayer->GetUrl() == filename)
		mVideoPlayer->SetVolumeNormalized(normalizedVolume);
}


// set the global volume (adjust all audio contents + video)
void ExperienceWidget::SetGlobalVolume(float normalizedVolume)
{
	mVideoPlayer->SetVolumeNormalized(normalizedVolume);
	const uint32 numMediaItems = mMediaContents.Size();
	for (uint32 i=0; i<numMediaItems; ++i)
		mMediaContents[i]->SetVolume(normalizedVolume);
}

#ifdef NEUROMORE_PLATFORM_WINDOWS

// set the system master volume (platform specific)
void ExperienceWidget::SetSystemMasterVolume(float normalizedVolume)
{
#ifdef NEUROMORE_PLATFORM_WINDOWS
	if (mEndpointVolume != NULL)
	{
		mEndpointVolume->SetMasterVolumeLevelScalar(normalizedVolume, NULL);
	}

#endif

	// TODO add osx and linux master volume (if possible)

	// TODO use events if we want to do have master control in the app, too.
}
#endif

// seek media to given position (in seconds)
void ExperienceWidget::SetPosition(float position, const char* filename)
{
	uint64 milliseconds = position * 1000.0f;
	MediaContent* mediaContent = FindMediaContent(filename);
	if (mediaContent != NULL)
		mediaContent->SetPosition(milliseconds);
}


void ExperienceWidget::OnVideoLooped(QString url)
{
	// get the active state machine
	StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
	if (stateMachine == NULL)
		return;

	String coreUrl = FromQtString(url);

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

			// check if the current condition is a video condition
			if (condition->GetType() == StateTransitionVideoCondition::TYPE_ID)
			{
				StateTransitionVideoCondition* videoCondition = static_cast<StateTransitionVideoCondition*>(condition);

				// check if the condition id equals the button id
				if (coreUrl.IsEqual(videoCondition->GetUrl()) == true)
				{
					// increase the loop counter
					videoCondition->Looped();

					//LogDebug( "VideoCondition increased loop counter '%i'.", audioCondition->GetLoops() );
					return;
				}
			}
		}
	}
}


void ExperienceWidget::OnExitStateReached(uint32 exitStatus)
{
	// TODO: stop session automatically
	Clear();
}

void ExperienceWidget::OnStopSession()
{
	Clear();
}


void ExperienceWidget::OnPlayAudio(const char* url, int32 numLoops,  double beginAt, double volume, bool allowStreaming)
{
	WebDataCache* cache = GetQtBaseManager()->GetExperienceAssetCache()->GetCache();

	MediaContent* media = FindMediaContent( url );
	if (media == NULL)
	{
		media = new MediaContent( this, cache, url );
		mMediaContents.Add( media );
		//LogInfo("Loading %s", filename);
	}
	else
	{
		//LogInfo("ALREADY FOUND %s", filename);
		media->Stop();
	}

	media->Play( volume, numLoops );
}


void ExperienceWidget::OnStopAudio(const char* url)
{
	MediaContent* media = FindMediaContent(url);
	if (media != NULL)
		media->Stop();
}



void ExperienceWidget::OnSeekAudio(const char* url, uint32 millisecs)
{
	MediaContent* media = FindMediaContent(url);
	if (media != NULL)
		media->SetPosition(millisecs);
}


void ExperienceWidget::OnPauseAudio(const char* url, bool unpause)
{
	MediaContent* media = FindMediaContent(url);
	if (media == NULL)
		return;

	if (unpause == false)
		media->Pause();
	else
		media->Continue();
}


void ExperienceWidget::OnPlayVideo(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream)
{
   ClearAnimatedGif();

	WebDataCache* cache = GetQtBaseManager()->GetExperienceAssetCache()->GetCache();

	bool movieLoaded = mVideoPlayer->Load(url, cache);
	if (movieLoaded == false)
		LogError( "ExperienceWIdget: Loading movie '%s' failed. Cannot play it back.", url );

	mVideoPlayer->Play(numLoops);
}


void ExperienceWidget::OnStopVideo()
{
	mVideoPlayer->Clear();
	ShowImage( QPixmap() );
}



void ExperienceWidget::OnSeekVideo(const char* url, uint32 position)
{
	
}


void ExperienceWidget::OnPauseVideo(const char* url, bool unpause)
{
	// TODO implement pause/unpause
	if (unpause == false)
		mVideoPlayer->Pause();
	else
		mVideoPlayer->Continue();
}


void ExperienceWidget::OnShowImage(const char* url)
{
	LogDebug("Experience Widget: OnShowImage(url='%s')", url);

	WebDataCache* cache = GetQtBaseManager()->GetExperienceAssetCache()->GetCache();
	if (cache->FileExists(url) == true)
	{
		// file cached route

		String filename = cache->GetCacheFilenameForUrl(url);

		QFile file(filename.AsChar());
		if (file.open(QIODevice::ReadOnly) == false)
		{
			LogError("ExperienceWidget: Cannot load image '%s'.", filename.AsChar());
			CORE_ASSERT(false); // something with the preloading is fucked
			return;
		}

		QByteArray data = file.readAll();
		file.close();

		if (filename.ExtractFileExtension().IsEqualNoCase("gif") == false)
		{
			// read data from QNetworkReply here
			mPixmap.loadFromData(data);

			ClearAnimatedGif();
			SetTimerEnabled(false);

			ShowImage(mPixmap);
		}
		else
		{
			ClearAnimatedGif();

			mGifBuffer = new QBuffer();
			mGifBuffer->setData(data);

			mGifMovie = new QMovie();
			mGifMovie->setCacheMode( QMovie::CacheAll );
			mGifMovie->setDevice(mGifBuffer);

			if (mGifMovie->isValid())
			{
				mGifMovie->start();
				SetTimerEnabled(true);
			}
			else
				LogError("ExperienceWidget: Cannot play gif '%s'. Because: %s", filename.AsChar(), mGifMovie->lastErrorString().toStdString().c_str());
		}
	}
}


void ExperienceWidget::OnHideImage()
{
	LogDebug("Experience Widget: OnHideImage()");
	ClearAnimatedGif();

	ShowImage( QPixmap() );
}


void ExperienceWidget::OnShowText(const char* text, const Core::Color& color)
{
	LogDebug("Experience Widget: OnShowText(text='%s')", text);
	mTextColor	= ToQColor(color);
	mText		= text;

	update();
}


void ExperienceWidget::OnHideText()
{
	LogDebug("Experience Widget: OnHideText()");
	mText = "";
	update();
}


void ExperienceWidget::OnSetBackgroundColor(const Core::Color& color)
{
	LogDebug("Experience Widget: OnSetBackgroundColor(r=%.2f g=%.2f b=%.2f)", color.r, color.g, color.b);
	mBackgroundColor = ToQColor(color);

	update();
}


void ExperienceWidget::OnSetFourZoneAVEColors(const float* red, const float* green, const float* blue, const float* alpha)
{
	LogDebug("Experience Widget: OnSetFourZoneAVEColors(..)");
	
	// copy over colors
	for (uint32 i = 0; i < 4; ++i)
		mAVEZoneColors[i] = ToQColor(Color(red[i], green[i], blue[i], alpha[i]));
	
	update();
}


void ExperienceWidget::OnHideFourZoneAVE()
{
	LogDebug("Experience Widget: OnHideFourZoneAVE()");

	// set alpha to zero
	for (uint32 i = 0; i < 4; ++i)
		mAVEZoneColors[i].setAlpha(0);

	update();
}


void ExperienceWidget::OnShowButton(const char* text, uint32 buttonId)
{
	AddButton( text, buttonId );
}


void ExperienceWidget::OnClearButtons()
{
	ClearButtons();
}



void ExperienceWidget::OnActiveStateMachineChanged(StateMachine* stateMachine)
{
	Clear();

	if (stateMachine == NULL)
		return;

	// FIXME why are we doing this ???
	stateMachine->Stop();
	stateMachine->Reset();
	stateMachine->Stop();

	// pre-load data
	//PreloadAssets();
}

void ExperienceWidget::OnActiveClassifierChanged(Classifier* classifier)
{
   const uint32_t nPlayers = mTonePlayers.Size();
   for (uint32_t i = 0; i < nPlayers; i++)
      delete mTonePlayers[i];
   
   mTonePlayers.Clear();

   if (classifier)
   {
      const uint32_t nToneGens = classifier->GetNumToneGeneratorNodes();
      for (uint32_t i = 0; i < nToneGens; i++)
         mTonePlayers.Add(new TonePlayer(*classifier->GetToneGeneratorNode(i)));
   }
}

void ExperienceWidget::OnNodeAdded(Graph* graph, Node* node)
{
   if (node && node->GetType() == ToneGeneratorNode::TYPE_ID)
      mTonePlayers.Add(new TonePlayer(*(ToneGeneratorNode*)node));
}

void ExperienceWidget::OnRemoveNode(Graph* graph, Node* node)
{
   if (node && node->GetType() == ToneGeneratorNode::TYPE_ID)
   {
      const uint32_t nPlayers = mTonePlayers.Size();
      for (uint32_t i = 0; i < nPlayers; i++) {
         if (&mTonePlayers[i]->GetNode() == node) {
            delete mTonePlayers[i];
            mTonePlayers.Remove(i);
            break;
         }
      }
   }
}

void ExperienceWidget::OnGraphReset(Graph* graph)
{
   /*const uint32_t nPlayers = mTonePlayers.Size();
   for (uint32_t i = 0; i < nPlayers; i++)
      mTonePlayers[i]->Reset();*/
}
