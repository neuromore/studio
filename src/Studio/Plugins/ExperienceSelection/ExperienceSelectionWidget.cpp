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
#include <Graph/GraphImporter.h>
#include "ExperienceSelectionWidget.h"
#include "ExperienceSelectionPlugin.h"
#include "../Experience/ExperienceWidget.h"
#include <Backend/FileSystemGetRequest.h>
#include <Backend/FileSystemGetResponse.h>
#include "../../MainWindow.h"
#include "../../VisualizationManager.h"

using namespace Core;

// constructor
ExperienceSelectionWidget::ExperienceSelectionWidget(QWidget* parent, ExperienceSelectionPlugin* plugin) : QScrollArea(parent)
{
	mPlugin		= plugin;
	mMainWidget	= NULL;
	mIsLoading	= false;

	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

	setWidgetResizable(true);
	setFocusPolicy(Qt::StrongFocus);

	// web data cache
	mCache = new WebDataCache( "Cache/Experiences/Meta/", this);
	connect(mCache, SIGNAL(FinishedDownload()), this, SLOT(OnFinishedPreloading()));

	// enable max alive time restriction
	//mCache->Log();
	const uint32 maxAliveTimeInDays = 7;
	mCache->RestrictAliveTime( maxAliveTimeInDays );
	mCache->CleanCache();
	//mCache->Log();

	// resize timer (delay ReInit() so it doesnt trigger exessively)
	mResizeEventTimer = new QTimer(this);
	connect( mResizeEventTimer, &QTimer::timeout, this, &ExperienceSelectionWidget::OnResizeFinished);
	mResizeEventTimer->setSingleShot(true);
	mResizeEventTimer->setInterval(100);

	// connect to signal that gives the start for the experience
	connect( GetQtBaseManager()->GetExperienceAssetCache(), SIGNAL(FinishedPreloading()), this, SLOT(OnAssetDownloadFinished()) );

	mLayoutMargin		= 10;
	mChannelSpacing		= 5;
	mBackButtonSize		= 40;
	mTargetTileSize		= 300;

	// update the classifier list
	ReInit();
}


// destructor
ExperienceSelectionWidget::~ExperienceSelectionWidget()
{
}


uint32 ExperienceSelectionWidget::GetTileSize() const
{
	uint32 numItemsPerRow = GetNumItemsPerRow();

	uint32 channelSpacing = 0;
	if (numItemsPerRow > 0)
		channelSpacing = (numItemsPerRow-1) * mChannelSpacing;

	uint32 result = (width() - 2*mLayoutMargin - channelSpacing) / numItemsPerRow;

	return result;
}


// calculate how many folders we want to show in a given row
uint32 ExperienceSelectionWidget::GetNumItemsPerRow() const
{
	const int realWidth = width()- 2*mLayoutMargin;
	if (realWidth <= mTargetTileSize)
		return 1;

	return realWidth / mTargetTileSize + (realWidth % mTargetTileSize != 0); // integer ceil
}


void ExperienceSelectionWidget::resizeEvent(QResizeEvent* event)
{
	if (CalcNumFolders() > 0)
		mResizeEventTimer->start();
	else
		QScrollArea::resizeEvent(event);
}


// initialize interface information
void ExperienceSelectionWidget::ReInit(bool downloadAssets)
{
	// hide and delete the main widget later
	if (mMainWidget != NULL)
	{
		mMainWidget->hide();
		mMainWidget->deleteLater();
		mMainWidget = NULL;
	}

	// create new main widget
	mMainWidget = new ExperienceSelectionBackgroundWidget();
	setWidget(mMainWidget);

	// enable/disable interaction while session is running
	const bool sessionRunning = GetSession()->IsRunning();
	setEnabled(sessionRunning == false);

	// fill in data
	if (CalcNumFolders() > 0)
		CreateWidgetsForFolders(downloadAssets);
	else
		CreateWidgetsForFiles(downloadAssets);

	// stop a running visualization
	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
		GetManager()->GetVisualizationManager()->Stop();
}


void ExperienceSelectionWidget::CreateWidgetsForFolders(bool downloadAssets)
{
	// create grid layout in our main widget
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin( mLayoutMargin );
	vLayout->setAlignment(Qt::AlignTop);
	mMainWidget->setLayout(vLayout);

	// add the back button
	if ((mItems.Size() > 0 && HasParent() == true) || mItems.Size() == 0)
	{
		ImageButton* backButton = new ImageButton("Images/Icons/HistoryArrowLeft.png", mBackButtonSize, "Click to go back");
		connect(backButton, &ImageButton::clicked, this, &ExperienceSelectionWidget::OnBackButtonClicked);
		vLayout->addWidget( backButton );
	}

	// create grid layout in our main widget
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setSpacing( mChannelSpacing );
	gridLayout->setAlignment(Qt::AlignTop);
	vLayout->addLayout(gridLayout);

	// get some information about the file hierarchy item
	const uint32 numItemsPerRow	= GetNumItemsPerRow();
	const uint32 numItems = mItems.Size();
	const uint32 tileSize = GetTileSize();

	// iterate through all items
	for (uint32 i=0; i<numItems; ++i)
	{
		const FileSystemItem& item = mItems[i];

		// only do for folders
		if (item.IsFolder() == false)
			continue;

		const uint32 row	= i / numItemsPerRow;
		const uint32 column	= i % numItemsPerRow;

		ExperiencePushButtonWidget* button = new ExperiencePushButtonWidget(NULL, item);

		//button->setText( item.GetName() );
		button->setToolTip( item.GetSummary() );
		button->setObjectName("TransparentButton");
		button->setFixedSize( tileSize, tileSize );

		// icon
		if (item.GetIconUrlString().IsEmpty() == false)
		{
			// load icon
			if (mCache->FileExists(item.GetIconUrl()) == true)
			{
				Core::String imageLocalFilename = mCache->GetCacheFilenameForUrl(item.GetIconUrl());
				if (QFile::exists(imageLocalFilename.AsChar()) == true)
				{
					button->InitImages( imageLocalFilename.AsChar() );
					button->setIconSize( QSize(tileSize, tileSize) );
										
					// remove text, as we got an icon
					button->setText( "" );
				}
			}
			else
			{
				if (downloadAssets == true)
				{
					// add the icon to the cache download queue
					mCacheDownloadQueue.Add( item.GetIconUrl() );
				}
			}
		}

		connect(button, &ImageButton::clicked, this, &ExperienceSelectionWidget::OnItemButtonClicked);

		gridLayout->addWidget( button, row, column );
	}

	// finally set the height of the main widget to accomodate the whole grid layout in the vertical direction
	const uint32 numRows = numItems / numItemsPerRow + (numItems % numItemsPerRow != 0); // integer ceil
	const uint32 channelSpacing = (numRows > 0 ? (numRows-1) * mChannelSpacing : 0);
	const uint32 panelHeight = numRows*tileSize + 2*mLayoutMargin + channelSpacing;
	mMainWidget->setFixedHeight(panelHeight);

	// load files	
	if (downloadAssets == true && mCacheDownloadQueue.IsEmpty() == false)
		mCache->Download( mCacheDownloadQueue );
}


void ExperienceSelectionWidget::CreateWidgetsForFiles(bool downloadAssets)
{
	// create grid layout in our main widget
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setAlignment(Qt::AlignTop);
	vLayout->setSpacing(0);
	mMainWidget->setLayout(vLayout);

	// add the back button
	// mItems.Size() == 0: check is for empty folders (as we do not know upfront that they are empty)
	if ((mItems.Size() > 0 && HasParent() == true) || mItems.Size() == 0)
	{
		ImageButton* backButton = new ImageButton("Images/Icons/HistoryArrowLeft.png", mBackButtonSize, "Click to go back");
		connect(backButton, &ImageButton::clicked, this, &ExperienceSelectionWidget::OnBackButtonClicked);
		vLayout->addWidget(backButton);
	}

	// background image
	if (HasParent() == true)
	{
		const FileSystemItem& parentItem = mParents[0];

		// download background image
		String backgroundUrl = parentItem.GetBackgroundUrlString();
		if (downloadAssets == true)
		{
			// add the background to the cache download queue
			if (backgroundUrl.IsEmpty() == false && mCache->FileExists(backgroundUrl.AsChar()) == false)
				mCacheDownloadQueue.Add(backgroundUrl);
		}

		// set background image
		if (mCache->FileExists(backgroundUrl.AsChar()) == true)
		{
			String backgroundFilename = mCache->GetCacheFilenameForUrl( backgroundUrl.AsChar() );
			mMainWidget->SetImage( backgroundFilename.AsChar() );
		}
	}

	// iterate through the file system items
	const uint32 numItems = mItems.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		const FileSystemItem& item = mItems[i];

		// only do for experiences
		if (item.IsExperience() == false)
			continue;

		// add experience widget
		ExperienceSelectionItemWidget* experienceWidget = NULL;
		
		if (HasParent() == true)
			experienceWidget = new ExperienceSelectionItemWidget( this, item, mParents[0] );
		else
			experienceWidget = new ExperienceSelectionItemWidget( this, item );

		vLayout->addWidget( experienceWidget );

		// add separator line
		if (i<numItems-1)
		{
			// separator line
			int height = 1;
			QFrame* line = new QFrame();
			line->setObjectName(QString::fromUtf8("line"));
			line->setGeometry(QRect(0, 0, mMainWidget->width(), height));
			line->setFrameShape(QFrame::HLine);
			line->setStyleSheet( "border-width: 0px; color: rgba(0, 0, 0, 0%); background-color: rgba(255, 255, 255, 15%);");
			line->setMaximumHeight(height);
			
			vLayout->addWidget(line);
		}
	}

	// load files
	if (downloadAssets == true && mCacheDownloadQueue.IsEmpty() == false)
		mCache->Download(mCacheDownloadQueue);
}


// update interface information
void ExperienceSelectionWidget::UpdateInterface()
{
}


void ExperienceSelectionWidget::OnItemButtonClicked()
{
	ExperiencePushButtonWidget* button = qobject_cast<ExperiencePushButtonWidget*>( sender() );

	const FileSystemItem& item = button->GetItem();
	//LogInfo( "Button %s clicked", item->GetName() );

	AsyncLoadFromBackend( item.GetItemId() );
}


void ExperienceSelectionWidget::OnBackButtonClicked()
{
	AsyncLoadFromBackend( mCurrentItem.GetParentId() );
}


BackendFileSystem* ExperienceSelectionWidget::GetFileSystem() const	
{
	return GetQtBaseManager()->GetBackendInterface()->GetFileSystem();
}


// load item from backend and update the ui
void ExperienceSelectionWidget::AsyncLoadFromBackend(const char* itemId)
{
	if (GetAuthenticationCenter()->IsInterfaceAllowed() == false)
		return;

	// don't double load the same thing
	if (mIsLoading == true)
		return;

	const uint32 page = 0;
	const uint32 size = 1024;

	// disable the interface before doing the rest call and enable the downloading state
	setEnabled(false);
	mIsLoading = true;

	mCurrentItem.SetParentId( mCurrentItem.GetParentId() );
	mCurrentItem.SetItemId(	itemId );

	//LogInfo( "ExperienceSelectionWidget::AsyncLoadFromBackend: parentId=%s, itemId=%s", mCurrentItem.GetParentId(), mCurrentItem.GetItemId() );

	// 1. construct /filesystem/get request
	FileSystemGetRequest request( GetUser()->GetToken(), GetSessionUser()->GetIdString(), page, size, "store", itemId, "[experience,folder]");

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT, false, QNetworkRequest::AlwaysNetwork );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// rewind the state
		setEnabled(true);
		mIsLoading = false;

		// 3. construct and parse response
		FileSystemGetResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		mParents		= response.mParents;
		mItems			= response.mItems;
		mTotalPages		= response.mTotalPages;
		mPageIndex		= response.mPageIndex;

		mCacheDownloadQueue.Clear();

		ReInit();
		UpdateInterface();

		// preload assets for next time
		mCache->Download( mCacheDownloadQueue );
	});
}


uint32 ExperienceSelectionWidget::CalcNumFolders() const
{
	uint32 result = 0;

	const uint32 numItems = mItems.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		if (mItems[i].GetTypeString().IsEqualNoCase("folder") == true)
			result++;
	}

	return result;
}


uint32 ExperienceSelectionWidget::CalcNumFiles() const
{
	uint32 result = 0;

	const uint32 numItems = mItems.Size();
	for (uint32 i=0; i<numItems; ++i)
	{
		if (mItems[i].GetTypeString().IsEqualNoCase("folder") == false)
			result++;
	}

	return result;
}


bool ExperienceSelectionWidget::HasParent() const
{
	return mParents.Size() > 0;
}



void ExperienceSelectionWidget::OnAssetDownloadFinished()
{
#ifdef NEUROMORE_BRANDING_ANT
	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
		GetLayoutManager()->SwitchToLayoutByName("Experience Trainer");
#else
	if (GetUser()->FindRule("STUDIO_SETTING_EasyWorkflow") != NULL)
		GetLayoutManager()->SwitchToLayoutByName("Experience Player");
#endif
}


// failed experiment:
// unselect all other items
/*void ExperienceSelectionWidget::OnItemSelected(ExperienceSelectionItemWidget* item)
{
	const uint32 numItems = mMainWidget->layout()->count();
	for (uint32 i=0; i<numItems; ++i)
	{
		QWidget* widget = mMainWidget->layout()->itemAt(i)->widget();
		if (widget != item)
		{
			ExperienceSelectionItemWidget* itemWidget = dynamic_cast<ExperienceSelectionItemWidget*>(widget);
			if (itemWidget != NULL)
				itemWidget->Unselect();
		}
	}
}*/



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExperienceWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExperienceSelectionItemWidget::ExperienceSelectionItemWidget(ExperienceSelectionWidget* parent, const FileSystemItem& item, const FileSystemItem& parentItem) : QWidget(parent)
{
	mItem = item;
	mParentItem = parentItem;
	mExperienceSelectionWidget = parent;

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setAlignment(Qt::AlignTop);
	setLayout(hLayout);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setMargin( 15 );
	vLayout->setAlignment(Qt::AlignLeft);
	hLayout->addLayout(vLayout);

	mIsHovered = false;

	// name
	Color color = parentItem.GetColor();
	QColor qColor = QColor(ToQColor(color));
	String colorHexString = qColor.name().toUtf8().data();

	String nameString;
	nameString.Format("<font color=\"%s\">%s</font>", colorHexString.AsChar(), item.GetName());

	mNameLabel = new QLabel( nameString.AsChar() );
	mNameLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	mNameLabel->setWordWrap(true);
	QFont font = mNameLabel->font();
	font.setBold(mIsHovered);
	font.setPixelSize(18);
	mNameLabel->setFont(font);
	vLayout->addWidget(mNameLabel);

	// summary
	QLabel* mSummaryLabel = new QLabel(item.GetSummary());
	mSummaryLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	mSummaryLabel->setWordWrap(true);
	vLayout->addWidget(mSummaryLabel);

	// load button
	const int loadButtonSize = 40;
	mLoadButton = new ImageButton("Images/Icons/Play.png", loadButtonSize, "Load");
	connect( mLoadButton, SIGNAL(clicked()), this, SLOT(OnGoButtonClicked()) );
	hLayout->addWidget(mLoadButton);

	mLoadButton->setMinimumHeight(loadButtonSize);
	mLoadButton->setMinimumWidth(loadButtonSize);

	UpdateInterface();
}


void ExperienceSelectionItemWidget::paintEvent(QPaintEvent* event)
{
	if (mIsHovered == true)
	{
		QPainter painter(this);
		painter.setPen( Qt::NoPen );
		painter.setBrush( QColor(255, 255, 255, 20) );
		painter.drawRect( rect() );
	}

	if (mIsSelected == true)
	{
		QPainter painter(this);
		painter.setPen( Qt::NoPen );
		painter.setBrush( QColor(0, 159, 227, 50) );
		painter.drawRect( rect() );
	}
}


// on mouse enter
void ExperienceSelectionItemWidget::enterEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = true;
	UpdateInterface();
}


// on mouse leave
void ExperienceSelectionItemWidget::leaveEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = false;
	UpdateInterface();
}


void ExperienceSelectionItemWidget::UpdateInterface()
{
	mLoadButton->setVisible( mIsHovered );
	update();
}


void ExperienceSelectionItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	OnGoButtonClicked();
}


void ExperienceSelectionItemWidget::mousePressEvent(QMouseEvent* event)
{
	//mIsSelected = true;
	//UpdateInterface();
}


void ExperienceSelectionItemWidget::Unselect()
{
	//mIsSelected = false; 
	//UpdateInterface(); 
}


void ExperienceSelectionItemWidget::OnGoButtonClicked()
{
	LogInfo( "Load experience '%s'", mItem.GetName() );

	// load experience
	GetFileManager()->OpenExperience( FileManager::LOCATION_BACKEND, mItem.GetItemId(), mItem.GetName() );
}


void ExperienceSelectionBackgroundWidget::SetImage(const char* filename)
{
	mFilename = filename;
	mPixmap = QPixmap(filename);
}


void ExperienceSelectionBackgroundWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	ExperienceWidget::DrawPixmapKeepAspectRatio( this, painter, mPixmap );
}
