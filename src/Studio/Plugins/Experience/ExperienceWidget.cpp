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
#include "ExperienceWidget.h"
#include <Core/EventManager.h>
#include "Graph/StateTransitionButtonCondition.h"
#include "Graph/StateTransitionAudioCondition.h"
#include "Graph/StateTransitionVideoCondition.h"
#include "../../AppManager.h"
#include <EngineManager.h>
#include <QtBaseManager.h>
#include <QSoundEffect>
#include <QPainter>
#include <QFileInfo>
#include <QBuffer>
#include <QMessageBox>

// for system master volume control
#ifdef NEUROMORE_PLATFORM_WINDOWS
	#include <stdio.h>
	#include <windows.h>
	#include <mmdeviceapi.h>
	#include <endpointvolume.h>
	
	#include <QSettings>
#endif

using namespace Core;

// constructor
ExperienceWidget::ExperienceWidget(QWidget* parent) : QWidget(parent)
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


	// display gamma control
	mGDI32 = ::LoadLibrary(L"gdi32.dll");
	if (mGDI32 != NULL)
	{
		//Get the addresses of GetDeviceGammaRamp and SetDeviceGammaRamp API functions.
		mGetDeviceGammaRamp = (Type_SetDeviceGammaRamp)GetProcAddress(mGDI32, "GetDeviceGammaRamp");
		mSetDeviceGammaRamp = (Type_SetDeviceGammaRamp)GetProcAddress(mGDI32, "SetDeviceGammaRamp");

		const bool haveOriginalGamma = (mGetDeviceGammaRamp != NULL && mGetDeviceGammaRamp(GetDC(NULL), mOriginalGammaArray));
		if (mGetDeviceGammaRamp == NULL || mSetDeviceGammaRamp == NULL || haveOriginalGamma == false)
		{
			::FreeLibrary(mGDI32);
			mGDI32 = NULL;
			mGetDeviceGammaRamp = NULL;
			mSetDeviceGammaRamp = NULL;
		}

		// get gamma-range registry entry and check if it was changed yet
		QSettings gammaSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ICM", QSettings::NativeFormat);
		
		const int currentValue = gammaSettings.value("GdiIcmGammaRange", QVariant::Int).toInt();
		if (currentValue != 0x100)
		{
			//QMessageBox::warning(this, "Display Gamma Range", "Display Gamma range is limited.", QMessageBox::Ok);
			LogInfo("GdiIcmGammaRange: %i", currentValue);
		}

		// try to change it (never worked for me even with admin privileges)
		if (gammaSettings.isWritable() == true)
			gammaSettings.setValue("GdiIcmGammaRange", 256);

	}

	// set to invalid value
	mLastScreenBrightness = -1;

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
	
	// reset gamma back to normal, if we changed it at least once 
	if (mLastScreenBrightness != -1.0)
		if (SetDeviceGammaRamp(GetDC(NULL), mOriginalGammaArray) == false)
			LogError("Error reset the display gamma ramp back to original values.");

	// unload screen gamma control
	if (mGDI32 != NULL)
		::FreeLibrary(mGDI32);
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
		// find volume node
		Node* node = classifier->FindNodeByName("Volume", CustomFeedbackNode::TYPE_ID);
		if (node != NULL)
		{
			CustomFeedbackNode* feedbackNode = static_cast<CustomFeedbackNode*>(node);
			if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false)
				SetGlobalVolume( feedbackNode->GetCurrentValue() );
		}


#ifdef NEUROMORE_PLATFORM_WINDOWS
		// find master volume node
		node = classifier->FindNodeByName("MasterVolume", CustomFeedbackNode::TYPE_ID);
		if (node != NULL)
		{
			CustomFeedbackNode* feedbackNode = static_cast<CustomFeedbackNode*>(node);
			if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false)
				SetSystemMasterVolume( feedbackNode->GetCurrentValue() );
		}

		// find screen brightness node
		node = classifier->FindNodeByName("ScreenBrightness", CustomFeedbackNode::TYPE_ID);
		if (node != NULL)
		{
			CustomFeedbackNode* feedbackNode = static_cast<CustomFeedbackNode*>(node);
			if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false)
				SetScreenBrightness( feedbackNode->GetCurrentValue() );
		}
		else
		{
			// magic mode
			node = classifier->FindNodeByName("LSD!", CustomFeedbackNode::TYPE_ID);
			if (node != NULL)
			{
				CustomFeedbackNode* feedbackNode = static_cast<CustomFeedbackNode*>(node);
				if (feedbackNode->IsInitialized() == true && feedbackNode->IsEmpty() == false)
					SetScreenLSD( feedbackNode->GetCurrentValue() );
			}
		}
#endif
		
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

	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier != NULL)
	{
		classifier->Start();
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


// set the volume of a given media content
void ExperienceWidget::SetVolume(float normalizedVolume, const char* filename)
{
	MediaContent* mediaContent = FindMediaContent(filename);
	if (mediaContent != NULL)
		mediaContent->SetVolume(normalizedVolume);
}


// set the global volume (adjust all media contents)
void ExperienceWidget::SetGlobalVolume(float normalizedVolume)
{
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
	WebDataCache* cache = GetQtBaseManager()->GetExperienceAssetCache()->GetCache();
	String filename = cache->GetCacheFilenameForUrl(url);

   printf("ExperienceWidget: Loading %s", url);

	bool movieLoaded = mVideoPlayer->Load( filename.AsChar(), url, cache );
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

			mGifMovie = new QMovie( mGifBuffer );
			mGifMovie->setCacheMode( QMovie::CacheAll );
			mGifMovie->start();
			SetTimerEnabled(true);
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

#ifdef NEUROMORE_PLATFORM_WINDOWS

void ExperienceWidget::SetScreenBrightness(double brightness)
{
	if (mGDI32 == NULL)
		return;

	// clamp brightness to 0.05 so we never blank out the screen by accident (its really annoying)
	if (brightness < 0.05) brightness = 0.05;
	if (brightness > 1.95) brightness = 1.95;

	if (brightness == mLastScreenBrightness)
		return;

	HDC hGammaDC = GetDC(NULL); 
	if (hGammaDC != NULL)
	{
		for (int i = 0; i < 256; i++)
		{
			// the final one: linear ramp with two modes, 1..0 fades to black and 1..2 fades to white
			int value = (brightness <= 1.0 ? i * brightness * 256.0 : 65535 - (256-i) * (2.0 - brightness) * 256.0);

			// linear ramp modification -> behaves like 'contrast'
			//int value = i * brightness * 256.0;

			// gamma ramp modification -> behaves like 'gamma' 
			//int value = Math::PowD( i/256.0, brightness ) * 65535;

			// offset modification
			//int value = i * 256 + (brightness-1.0)*65535;

			// inverse ramp
			//int value = 65535 - i * brightness * 256.0;

			// randomized ramp :))
			//int value = i * 256.0 -  Random::RandD(-1,1) * brightness * 6000.0;

			// LSD Mode: many small ramps instead of one :)))
			//int rampsize = brightness * 256.0;
			//int value = (i % rampsize) * (65535/rampsize);

			// clamp
			if (value > 65535)
				value = 65535;
			if (value < 0)
				value = 0;

			mCurrentGammaArray[0][i] = (WORD)value;
			mCurrentGammaArray[1][i] = (WORD)value;
			mCurrentGammaArray[2][i] = (WORD)value;
		}

		//Set the GammaArray values into the display device context.
		if (mSetDeviceGammaRamp(hGammaDC, mCurrentGammaArray) == false)
			LogWarning ("Error setting display gamma");

		mLastScreenBrightness = brightness;
	}

	if (hGammaDC != NULL)
		ReleaseDC(NULL, hGammaDC);
}


void ExperienceWidget::SetScreenLSD (double micrograms)
{
	if (mGDI32 == NULL)
		return;

	if (micrograms <= 0.1)
		micrograms = 0.1;

	HDC hGammaDC = GetDC(NULL); 
	if (hGammaDC != NULL)
	{
		for (int i = 0; i < 256; i++)
		{
			const double rel = Math::FModD(micrograms/10.0,1.0);

			// magic
			const int rampsize = Math::FModD(Math::AbsD(42.0 / micrograms) + 23.0, 6553.0);
			const int value = abs(i % (rampsize*2) - rampsize) * 65535 / rampsize;
			const int r = abs (value % (65535*2) - 65535);
			const int g = abs ((value + (int32)(65535.0*rel/3.0)) % (65535*2) - 65535);
			const int b = abs ((value + (int32)(65535.0*rel/3.0*2.0)) % (65535*2) - 65535);
			 
			mCurrentGammaArray[0][i] = (WORD)r;
			mCurrentGammaArray[1][i] = (WORD)g;
			mCurrentGammaArray[2][i] = (WORD)b;
		}

		//Set the GammaArray values into the display device context.
		if (mSetDeviceGammaRamp(hGammaDC, mCurrentGammaArray) == false)
			LogWarning ("Error setting display gamma");

		// so everything resets...
		mLastScreenBrightness = 1;
	}

	if (hGammaDC != NULL)
		ReleaseDC(NULL, hGammaDC);
}

#endif

