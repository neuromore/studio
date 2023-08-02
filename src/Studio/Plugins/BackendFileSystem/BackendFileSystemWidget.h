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

#ifndef __NEUROMORE_FILESYSTEMWIDGET_H
#define __NEUROMORE_FILESYSTEMWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/StandardHeaders.h>
#include <Core/Json.h>
#include <ImageButton.h>
#include <User.h>
#include <Graph/Graph.h>
#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <Widgets/SearchBoxWidget.h>
#include <QLineEdit>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <EngineManager.h>


// forward declaration
class BackendFileSystemPlugin;
class BackendFileSystem;

#define FOLDER_TYPE				"folder"
#define ITEM_TYPE_CLASSIFIER	"classifier"
#define ITEM_TYPE_STATEMACHINE	"statemachine"
#define ITEM_TYPE_EXPERIENCE	"experience"

// testing mode: allow all operations
//#define BACKEND_FILESYSTEM_ALLOWALL


class BackendFileSystemWidget : public QWidget
{
	Q_OBJECT

	public:
		class SelectionItem
		{
			public:
				SelectionItem();
				SelectionItem(const char* name, const char* path, const char* type, const char* id, const Creud& creud, int revision, const char* parentType, const char* parentUuid, const Creud& parentCrued);

				// name
				const char* GetName() const													{ return mName.AsChar(); }
				const Core::String& GetNameString() const									{ return mName; }

				// path
				const char* GetPath() const													{ return mPath.AsChar(); }
				const Core::String& GetPathString() const									{ return mPath; }
				
				// type
				const char* GetType() const													{ return mType.AsChar(); }
				const Core::String& GetTypeString() const									{ return mType; }

				// id
				const char* GetUuid() const													{ return mUuid.AsChar(); }
				const Core::String& GetUuidString() const									{ return mUuid; }
				
				// helpers
				inline bool operator==(const SelectionItem& item) const						{ return ( (mName==item.mName) && (mPath==item.mPath) && (mType==item.mType) && (mUuid == item.mUuid) && (mCreud == item.mCreud) && (mRevision == item.mRevision)); }
				inline bool IsValid() const													{ return ( (GetNameString().IsEmpty() == false && GetUuidString().IsEmpty() == false) ); }

				inline Core::String GetExtension() const 									{ return GetTypeString() == "CLASSIFIER"   ? ".cs.json" : GetTypeString() == "STATEMACHINE" ? ".sm.json" : GetTypeString() == "EXPERIENCE"   ? ".xp.json" : ".json"; }

				const Creud& GetCreud() const												{ return mCreud; }
				bool IsFolder() const														{ return GetTypeString().IsEqual(FOLDER_TYPE) == true; }

				// parent info
				bool GetParentIsFolder()													{ return GetParentTypeString().IsEqual(FOLDER_TYPE) == true; }
				bool HasParent() const														{ return (mParentUuid.IsEmpty() == false); }

				const char* GetParentType() const											{ return mParentType.AsChar(); }
				const Core::String& GetParentTypeString() const								{ return mParentType; }

				const char* GetParentUuid() const											{ return mParentUuid.AsChar(); }
				const Core::String& GetParentUuidString() const								{ return mParentUuid; }

				const Creud& GetParentCreud() const											{ return mParentCreud; }

			private:
				void SetName(const char* name)												{ mName = name; }
				void SetPath(const char* path)												{ mPath = path; }
				void SetType(const char* type)												{ mType = type; }
				void SetUuid(const char* uuid)												{ mUuid = uuid; }

				void SetParentType(const char* type)										{ mParentType = type; }
				void SetParentUuid(const char* uuid)										{ mParentUuid = uuid; }

				Creud				mCreud;
				Core::String		mName;
				Core::String		mPath;
				Core::String		mUuid;
				Core::String		mType;
				int					mRevision;

				// parent infos
				Core::String		mParentType;
				Core::String		mParentUuid;
				Creud				mParentCreud;
		};

		class CollapseState
		{
			public:
				CollapseState(const char* uuid, bool isCollapsed)							{ SetIsCollapsed(isCollapsed); SetUuid(uuid); }

				void SetUuid(const char* uuid)												{ mUuid = uuid; }
				const char* GetUuid() const													{ return mUuid.AsChar(); }
				const Core::String& GetUuidString() const									{ return mUuid; }

				void SetIsCollapsed(bool isCollapsed)										{ mIsCollapsed = isCollapsed; }
				bool IsCollapsed() const													{ return mIsCollapsed; }

			private:
				Core::String		mUuid;
				bool				mIsCollapsed;
		};

		BackendFileSystemWidget(QWidget* parent, BackendFileSystemPlugin* plugin);
		virtual ~BackendFileSystemWidget();

		void ReInit();
		void UpdateInterface();

		// keyboard shortcuts
		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);

		// expand item by path, e.g. { "examples", "GettingStarted", "FocusTrainer"}
		bool ExpandByPath(const QStringList& itemPath);

	public slots:
		void Refresh(const QString& localfolder = "", const QString& cloudfolder = "");
		void OnCreateFile();
		void OnCreateFolder();
		void OnSearchFieldTextEdited(const QString & text);

		void OnSelectionChanged();
		void OnRemoveItem();
		void OnOpenItem();
		void OnRenameItem();
		void OnCopyFileToPersonalFolder();
		void OnContextMenuRetrieveItemRevision();

		void OnLoadFromDiskAndSaveToCloud();
		void OnCopyJsonToClipboard();
		void OnSaveToDisk();

		void OnSearchCleared();
		
		void OnItemClicked(QTreeWidgetItem* item, int column);
		void OnItemDoubleClicked(QTreeWidgetItem* item, int column);

		void OnContextMenu(const QPoint& point);

		// expanded or collapsed state changes
		void OnItemCollapsed(QTreeWidgetItem* item);
		void OnItemExpanded(QTreeWidgetItem* item);

		BackendFileSystem* GetFileSystem() const;

	private:
		class BackendFileSystemTreeWidget : public QTreeWidget
		{
			public:
				BackendFileSystemTreeWidget(BackendFileSystemWidget* parent)	{ mParent = parent; }

				QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;
				QStringList mimeTypes() const									{ QStringList result; result.append("text/plain"); return result; }

				void dropEvent(QDropEvent* event);
				void dragEnterEvent(QDragEnterEvent* event);
				void dragLeaveEvent(QDragLeaveEvent* event)						{ event->accept(); }
				void dragMoveEvent(QDragMoveEvent* event);

				Qt::DropActions supportedDropActions() const					{ return Qt::CopyAction | Qt::MoveAction; }

			private:
				BackendFileSystemWidget*	mParent;
				SelectionItem				mDraggedItem;
		};

		enum
		{
			USERDATA_ID			= Qt::UserRole + 0,
			USERDATA_PATH		= Qt::UserRole + 1,
			USERDATA_CREUD		= Qt::UserRole + 2,
			USERDATA_TYPE		= Qt::UserRole + 3,		
			USERDATA_REVISION	= Qt::UserRole + 4
		};

		void UpdateItems(QTreeWidgetItem* item = NULL);

		void OpenFile(SelectionItem selectionItem, int32 revision=-1);

		// add item helpers
		QTreeWidgetItem* AddFolderItem(const Core::Json::Item& jsonFolderItem, const Core::String& path, QTreeWidgetItem* parentItem);
		QTreeWidgetItem* AddFileItem(const Core::Json::Item& jsonFileItem, const Core::String& path, QTreeWidgetItem* parentItem);

		// selection helpers
		SelectionItem GetSelectedItem() const;
		//void SelectClassifier(SelectionItem selectedItem);
		SelectionItem CreateSelectionItem(QTreeWidgetItem* item) const;

		
		uint32 ApplyFilter(const QString& filterString, QTreeWidgetItem* item = NULL);
		void CollectSelectedItems();
		bool IsItemSelected(const char* id);
		SelectionItem* GetSingleSelectionItem(); // returns NULL in case nothing or multiple items are selected, returns the selection item if exactly one is selected

		void SetCollapseState(const char* uuid, bool isCollapsed);
		CollapseState* FindCollapsedState(const char* uuid);
		bool IsItemCollapsed(const char* uuid);

		QTreeWidgetItem* FindItemByPath(const QString& path, const QString& type);
		void UploadFolder(const QString& pathlocal, const QString& pathcloud);

		Core::Array<SelectionItem>		mSelectedItems;
		Core::Array<CollapseState>		mFolderCollapseStates;
		
		Core::Json						mJson;
		Core::String					mPersonalFolderUuid;

		QString							mLastSelectedFileDialogFolder;

		BackendFileSystemPlugin*		mPlugin;
		BackendFileSystemTreeWidget*	mTreeWidget;
		Core::String					mTempString;

		ImageButton*					mCreateFileButton;
		ImageButton*					mCreateFolderButton;
		
		ImageButton*					mRemoveButton;
		ImageButton*					mRefreshButton;

		SearchBoxWidget*				mSearchBox;
		size_t							mPendingUploads;
};


#endif
