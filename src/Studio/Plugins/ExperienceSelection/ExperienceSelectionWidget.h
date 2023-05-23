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

#ifndef __NEUROMORE_EXPERIENCESELECTIONWIDGET_H
#define __NEUROMORE_EXPERIENCESELECTIONWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <ImageButton.h>
#include <Backend/WebDataCache.h>
#include <Backend/FileSystemItem.h>
#include <QScrollArea>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>


// forward declaration
class ExperienceSelectionPlugin;
class BackendFileSystem;

#define FOLDER_TYPE				"folder"
#define ITEM_TYPE_CLASSIFIER	"classifier"
#define ITEM_TYPE_STATEMACHINE	"statemachine"
#define ITEM_TYPE_EXPERIENCE	"experience"

class ExperiencePushButtonWidget : public ImageButton
{
	Q_OBJECT
	public:
		ExperiencePushButtonWidget(QWidget* parent, const FileSystemItem& item) : ImageButton(parent)	{ mItem = item; }
		virtual ~ExperiencePushButtonWidget()															{}

		const FileSystemItem& GetItem() const															{ return mItem; }

	private:
		FileSystemItem mItem;
};

class ExperienceSelectionWidget;

class ExperienceSelectionItemWidget : public QWidget
{
		Q_OBJECT
	public:
		ExperienceSelectionItemWidget(ExperienceSelectionWidget* parent, const FileSystemItem& item, const QSize& size, const FileSystemItem& parentItem=FileSystemItem());
		virtual ~ExperienceSelectionItemWidget()														{}

		const FileSystemItem& GetItem() const															{ return mItem; }
		const FileSystemItem& GetParentItem() const														{ return mParentItem; }

		void Unselect();

	private slots:
		void OnGoButtonClicked();
		void onCopyButtonClicked();
		void onEditButtonClicked();

	private:
		void paintEvent(QPaintEvent* event) override;
		void enterEvent(QEvent* event) override;
		void leaveEvent(QEvent* event) override;
		void mouseDoubleClickEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;

		void UpdateInterface();

		ExperienceSelectionWidget*	mExperienceSelectionWidget;
		FileSystemItem				mItem;
		FileSystemItem				mParentItem;
		ImageButton*				mLoadButton;
		ImageButton*				mCopyButton;
		ImageButton*				mEditButton;
		QLabel*						mNameLabel;
		QLabel*						mSummaryLabel;
		bool						mIsHovered;
		bool						mIsSelected;
};


class ExperienceSelectionBackgroundWidget : public QWidget
{
	public:
		ExperienceSelectionBackgroundWidget(QWidget* parent=NULL) : QWidget(parent)	{}
		virtual ~ExperienceSelectionBackgroundWidget()								{}

		void SetImage(const char* filename);
		void paintEvent(QPaintEvent* event) override;

	private:
		Core::String		mFilename;
		QPixmap				mPixmap;
};


class ExperienceSelectionWidget : public QScrollArea
{
	Q_OBJECT
	friend class ExperienceSelectionItemWidget;

	public:
		ExperienceSelectionWidget(QWidget* parent, ExperienceSelectionPlugin* plugin);
		virtual ~ExperienceSelectionWidget();

		void ReInit(bool downloadAssets=true);
		void UpdateInterface();

		uint32 GetNumItemsPerRow() const;
		uint32 GetTileSize() const;

		static bool									mIsGoToExperience;
		static bool									mIsCopingExperience;


	public slots:
		void AsyncLoadFromBackend(const char* itemId);

		void OnResizeFinished()								{ ReInit(false); }
		void OnFinishedPreloading()							{ ReInit(false); }

		void OnItemButtonClicked();
		void OnBackButtonClicked();

		void OnAssetDownloadFinished();

		void onSkipButtonClicked();

		void onCreateFolderClicked();

		void onCreateExperienceClicked();

		//void OnItemSelected(ExperienceSelectionItemWidget* widget);

	protected:
		BackendFileSystem* GetFileSystem() const;

	private:
		void CreateWidgetsForFolders(bool downloadAssets);
		void CreateWidgetsForFiles(bool downloadAssets);

		uint32 CalcNumFolders() const;
		uint32 CalcNumFiles() const;
		bool HasParent() const;

		void resizeEvent(QResizeEvent* event);
		void createClassifier(const Core::String& fileName, const Core::String& folderId, const Core::String& jsonContent = "{}");
		void createStateMachine(const Core::String& fileName, const Core::String& folderId, const Core::String& jsonContent = "{}");
		void createExperienceFile(const Core::String& fileName, const Core::String& folderId);
		void copyExperienceFile(const Core::String& experienceName, const Core::String& classifierUUID, const Core::String& stateMachineUUID);

		QTimer*									mResizeEventTimer;

		ExperienceSelectionBackgroundWidget*	mMainWidget;

		WebDataCache*							mCache;
		Core::Array<Core::String>				mCacheDownloadQueue;

		ExperienceSelectionPlugin*				mPlugin;

		QPushButton*							mSkipBtn;

		QPushButton*							mCreateFolderBtn;

		QPushButton*							mCreateXPBtn;

		QPixmap									mBackgroundImage;

		uint32									mLayoutMargin;
		uint32									mChannelSpacing;
		uint32									mBackButtonSize;
		int32									mTargetTileSize;

		// file system item
		Core::Array<FileSystemItem>				mParents;
		Core::Array<FileSystemItem>				mItems;
		Core::String							mClassifierUUID;
		Core::String							mStateMachineUUID;
		// Core::String							mPersonalFolderID; // use this ID for copying XPs
		FileSystemItem							mCurrentItem;

		uint32									mTotalPages;
		uint32									mPageIndex;

		bool									mIsLoading;
		bool									mIsCreatingExperience;
};


#endif
