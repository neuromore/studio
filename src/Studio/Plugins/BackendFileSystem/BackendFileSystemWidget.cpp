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
#include "BackendFileSystemWidget.h"
#include "BackendFileSystemPlugin.h"
#include <FileManager.h>
#include <Graph/GraphImporter.h>
#include <Graph/GraphExporter.h>
#include <Windows/EnterLabelWindow.h>
#include "CreateFileWindow.h"
#include "CreateFolderWindow.h"
#include <Backend/FileHierarchyGetRequest.h>
#include <Backend/FileHierarchyGetResponse.h>
#include <Backend/FilesCreateRequest.h>
#include <Backend/FilesCreateResponse.h>
#include <Backend/FilesGetRequest.h>
#include <Backend/FilesGetResponse.h>
#include <Backend/FilesUpdateRequest.h>
#include <Backend/FilesUpdateResponse.h>

using namespace Core;

// constructor
BackendFileSystemWidget::BackendFileSystemWidget(QWidget* parent, BackendFileSystemPlugin* plugin) : QWidget(parent)
{
	mPlugin = plugin;
	mLastSelectedFileDialogFolder = GetQtBaseManager()->GetAppDataFolder();

	// create the vertical main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);

	//
	// add the buttons
	//
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->setSpacing(6);
	buttonsLayout->setAlignment(Qt::AlignLeft);
	mainLayout->addLayout(buttonsLayout);

	mCreateFileButton	= new ImageButton("/Images/Icons/Plus.png", DEFAULT_ICONSIZE, "Create new file");
	mCreateFolderButton	= new ImageButton("/Images/Icons/PlusFolder.png", DEFAULT_ICONSIZE, "Create new folder");
	mRemoveButton		= new ImageButton("/Images/Icons/Minus.png", DEFAULT_ICONSIZE, "Delete selected file or folder");
	mRefreshButton		= new ImageButton("/Images/Icons/Refresh.png", DEFAULT_ICONSIZE, "Refresh");

	buttonsLayout->addWidget(mCreateFileButton);
	buttonsLayout->addWidget(mCreateFolderButton);
	buttonsLayout->addWidget(mRemoveButton);
	buttonsLayout->addWidget(mRefreshButton);

	connect(mCreateFileButton,		SIGNAL(released()), this, SLOT(OnCreateFile()));
	connect(mCreateFolderButton,	SIGNAL(released()), this, SLOT(OnCreateFolder()));
	connect(mRemoveButton,			SIGNAL(released()), this, SLOT(OnRemoveItem()));
	connect(mRefreshButton,			SIGNAL(released()), this, SLOT(Refresh()));

	// add toolbar spacer widget
	QWidget* spacerWidget = new QWidget(this);
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	buttonsLayout->addWidget(spacerWidget);
	
	// add the search field
	mSearchBox = new SearchBoxWidget(this);
	buttonsLayout->addWidget(mSearchBox);
	connect(mSearchBox, SIGNAL(TextChanged(const QString &)), this, SLOT(OnSearchFieldTextEdited(const QString &)));
	connect(mSearchBox, SIGNAL(TextCleared()), this, SLOT(OnSearchCleared()));
	
	//
	// add the tree widget
	//
	mTreeWidget = new BackendFileSystemTreeWidget(this);
	mTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	mTreeWidget->setSortingEnabled(false);
	mTreeWidget->setAlternatingRowColors(true);
	mTreeWidget->setAnimated(true);
	mTreeWidget->setContextMenuPolicy( Qt::CustomContextMenu );
	//mTreeWidget->setDragEnabled(true);
	mTreeWidget->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);

//#ifdef NEUROMORE_PLATFORM_WINDOWS
	mTreeWidget->setDragEnabled(true);
	mTreeWidget->setAcceptDrops(true);
	mTreeWidget->setDropIndicatorShown(true);
	//mTreeWidget->setDragDropMode( QAbstractItemView::DragDrop );
//#endif

	QStringList headerLabels;
	headerLabels << "Name";
	//headerLabels << "Type";
	mTreeWidget->setHeaderLabels(headerLabels);

	connect( mTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(OnSelectionChanged()) );
	connect(mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));
	connect( mTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemClicked(QTreeWidgetItem*, int)) );
	connect( mTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)) );

	connect( mTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(OnItemCollapsed(QTreeWidgetItem*)) );
	connect( mTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(OnItemExpanded(QTreeWidgetItem*)) );

	mainLayout->addWidget(mTreeWidget);
	setLayout(mainLayout);

	setFocusPolicy(Qt::StrongFocus);

	// update the classifier list
	ReInit();

	connect( GetFileSystem(), SIGNAL(FolderCreated()), this, SLOT(Refresh()) );
	connect( GetFileSystem(), SIGNAL(FolderUpdated()), this, SLOT(Refresh()) );
	connect( GetFileSystem(), SIGNAL(FolderDeleted()), this, SLOT(Refresh()) );

	connect( GetFileSystem(), SIGNAL(FileUpdated()), this, SLOT(Refresh()) );
	connect( GetFileSystem(), SIGNAL(FileDeleted()), this, SLOT(Refresh()) );
}


// destructor
BackendFileSystemWidget::~BackendFileSystemWidget()
{
}


// initialize interface information
void BackendFileSystemWidget::ReInit()
{
	mTreeWidget->blockSignals(true);


	// remove all items from the list box
	mTreeWidget->clear();

	Json::Item rootItem = mJson.GetRootItem();
	if (rootItem.IsNull() == false)
	{
		Json::Item dataItem = rootItem.Find("data");
		if (dataItem.IsNull() == false)
		{
			Json::Item rootFoldersItem	= dataItem.Find("rootFolders");
			if (rootFoldersItem.IsArray() == true)
			{
				const uint32 numRootFolders = rootFoldersItem.Size();
				for (uint32 i=0; i<numRootFolders; ++i)
				{
					Json::Item childItem = rootFoldersItem[i];
					AddFolderItem( childItem, "", NULL );
				}
			}
		}
	}

	// make sure the internal selected items list is up to date
	CollectSelectedItems();

	mTreeWidget->blockSignals(false);

	setEnabled(true);
}

// update interface information
void BackendFileSystemWidget::UpdateInterface()
{
#ifdef BACKEND_FILESYSTEM_ALLOWALL
	mCreateFolderButton->setEnabled(true);
	mCreateFileButton->setEnabled(true);
	mRemoveButton->setEnabled(true);
	return;
#endif

	// disable everything wile the session is running
	if (GetSession()->IsRunning() == true)
	{
		mTreeWidget->setEnabled(false);
		mRefreshButton->setEnabled(false);
		mRemoveButton->setEnabled(false);
		mCreateFolderButton->setEnabled(false);
		mCreateFileButton->setEnabled(false);
	}
	else
	{
		mTreeWidget->setEnabled(true);
		mRefreshButton->setEnabled(true);
		
		SelectionItem* singleSelectedItem = GetSingleSelectionItem();
		if (singleSelectedItem != NULL)
		{
			const bool enableRemove = (singleSelectedItem->HasParent() == true && singleSelectedItem->GetCreud().Delete() == true);
			mRemoveButton->setEnabled(enableRemove);

			// folder-only actions
			if (singleSelectedItem->IsFolder() == true)
			{
				// create file & folder
				const bool enableCreate = (singleSelectedItem->GetCreud().Create() == true);
				mCreateFolderButton->setEnabled(enableCreate);
				mCreateFileButton->setEnabled(enableCreate);
			}

			// file-only actions
			if (singleSelectedItem->IsFolder() == false)
			{
				// create file & folder
				const bool enableCreate = (singleSelectedItem->GetParentIsFolder() == true && singleSelectedItem->GetParentCreud().Create() == true);
				mCreateFolderButton->setEnabled(enableCreate);
				mCreateFileButton->setEnabled(enableCreate);
			}
		}
		else
		{
			mCreateFolderButton->setEnabled(false);
			mCreateFileButton->setEnabled(false);
			mRemoveButton->setEnabled(false);
		}
	}

	// filter items
	ApplyFilter(mSearchBox->GetText());

	// update the individual items (file permissions, open-state etc)
	UpdateItems();
}


// update the state of all individual items (e.g. change highlight if a file was opened)
void BackendFileSystemWidget::UpdateItems(QTreeWidgetItem* item)
{
	// root item
	if (item == NULL)
	{
		// start recursive descent on toplevel items
		const uint32 numTopLevelItems = mTreeWidget->topLevelItemCount();
		for (uint32 i = 0; i<numTopLevelItems; ++i)
			UpdateItems(mTreeWidget->topLevelItem(i));
	}
	// child items
	else 
	{
		// recurse on all children first
		const uint32 numChildItems = item->childCount();
		for (uint32 i = 0; i<numChildItems; ++i)
			UpdateItems(item->child(i));

		// get uuid and permissions for the file 
		String id = FromQtString( item->data(0, USERDATA_ID).toString() );
		Creud creud; creud.ConvertFromInt(item->data(0, USERDATA_CREUD).toInt());

		// update the item
		const uint32 numCols = item->columnCount();		// check all columns
		for (uint32 i = 0; i < numCols; ++i)
		{
			
			Classifier* classifier = GetEngine()->GetActiveClassifier();
			StateMachine* stateMachine = GetEngine()->GetActiveStateMachine();
			Experience* experience = GetEngine()->GetActiveExperience();

			// TODO we don't have central management of loaded files yet, so we have to check it agains the active graphs/experience
			// is this item loaded right now? graph or design
			const bool isLoaded =	(classifier != NULL && classifier->GetUuidString().IsEqual(id)) || 
									(stateMachine != NULL && stateMachine->GetUuidString().IsEqual(id)) ||
									(experience != NULL && experience->GetUuidString().IsEqual(id));
			
			// change font style if file is opened
			QFont font = item->font(i);
			font.setBold(isLoaded);
			font.setItalic(isLoaded);
			item->setFont(i, font);
			
			// darker color for readonly files
			if (creud.Update() == false)
				item->setTextColor(i, ColorPalette::Shared::GetTextQColor().darker(125));
			else // default color
				item->setTextColor(i, ColorPalette::Shared::GetTextQColor());

			// TODO use icons to show write+execute permissions

		}
		
	}
}


// reload file hiarchy from 
void BackendFileSystemWidget::Refresh()
{
	if (GetAuthenticationCenter()->IsInterfaceAllowed() == false)
		return;

	setEnabled(false);

	// 1. construct invite request
	FileHierarchyGetRequest request( GetUser()->GetToken(), GetSessionUser()->GetIdString() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FileHierarchyGetResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// clear any information related to the old hierarchy
		mJson = response.GetJson();
		mPersonalFolderUuid.Clear();

		ReInit();
		UpdateInterface();
	});
}


void BackendFileSystemWidget::OnSearchCleared()
{
	mTreeWidget->setFocus();
	ApplyFilter("");
}


// called when a key got pressed
void BackendFileSystemWidget::keyPressEvent(QKeyEvent* event)
{
	// find
	if (event->key() == Qt::Key_Escape && event->modifiers() & Qt::ControlModifier)
	{
		// move focus to file search field
		mSearchBox->setFocus();

		event->accept();
		return;
	}

	// escape was pressed while search box had focus: shift focus back to main widget
	if (event->key() == Qt::Key_Escape)
	{
		mSearchBox->ClearSearch();
	}

	// delete file
	if (event->key() == Qt::Key_Delete)
	{
		OnRemoveItem();
		event->accept();
		return;
	}


	return QWidget::keyPressEvent(event);
}


// called when a key got released
void BackendFileSystemWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape && event->modifiers() & Qt::ControlModifier)				{ event->accept(); return; }
	if (event->key() == Qt::Key_Delete)															{ event->accept(); return; }

	return QWidget::keyReleaseEvent(event);
}


// create selection item from a tree widget item
BackendFileSystemWidget::SelectionItem BackendFileSystemWidget::CreateSelectionItem(QTreeWidgetItem* item) const
{
	// skip directly if the item is invalid
	if (item == NULL)
		return SelectionItem();

	// extract the classifier filename and the library name
	String name		= FromQtString( item->text(0) );
	String type		= FromQtString( item->data(0, USERDATA_TYPE).toString() );
	String id		= FromQtString( item->data(0, USERDATA_ID).toString() );
	String path		= FromQtString( item->data(0, USERDATA_PATH).toString() );
	int creudInt	= item->data(0, USERDATA_CREUD).toInt();
	int revision	= item->data(0, USERDATA_REVISION).toInt();

	// creud
	Creud creud;
	creud.ConvertFromInt(creudInt);

	// parent info
	String parentType;
	String parentUuid;
	Creud parentCrued;
	QTreeWidgetItem* parentItem = item->parent();
	if (parentItem != NULL)
	{
		parentType	= FromQtString( parentItem->data(0, USERDATA_TYPE).toString() );
		parentUuid	= FromQtString( parentItem->data(0, USERDATA_ID).toString() );
		int parentCreudInt	= parentItem->data(0, USERDATA_CREUD).toInt();
		parentCrued.ConvertFromInt(parentCreudInt);
	}

	SelectionItem selectionItem( name.AsChar(), path.AsChar(), type.AsChar(), id.AsChar(), creud, revision, parentType.AsChar(), parentUuid.AsChar(), parentCrued );

	return selectionItem;
}


// get the single selected item from tree widget
BackendFileSystemWidget::SelectionItem BackendFileSystemWidget::GetSelectedItem() const
{
	// get the selected items and the number of them
	QList<QTreeWidgetItem*> selectedItems = mTreeWidget->selectedItems();
	
	// get the number of selected items and make sure only one is selected
	const uint32 numSelectedItems = selectedItems.count();
	if (numSelectedItems != 1)
		return SelectionItem();

	return CreateSelectionItem( selectedItems[0] );
}


// recursive filtering via search field
uint32 BackendFileSystemWidget::ApplyFilter(const QString& filterString, QTreeWidgetItem* item)
{ 
	// count and return number of visible elements
	uint32 numVisible = 0;

	// function entry point: begin filtering all top level items and start recursive decent
	if (item == NULL)
	{
		uint32 numShown = 0;
		// apply filter to all toplevel items and their children
		const uint32 numTopLevelItems = mTreeWidget->topLevelItemCount();
		for (uint32 i = 0; i<numTopLevelItems; ++i)
			numShown += ApplyFilter(filterString, mTreeWidget->topLevelItem(i));

		return numShown;
	}
	
	const uint32 numChildren = item->childCount();

	
	//
	// filter files and folders
	//

	// empty filter string: show all items
	if (filterString.isEmpty())
	{
		item->setHidden(false);

		for (uint32 i = 0; i < numChildren; ++i)
			numVisible += ApplyFilter(filterString, item->child(i));

		return numVisible;
	}
	else
	{
		// this is a file
		if (numChildren == 0)
		{
			// Note: current match type is case insensitive substring across all columns
			bool matches = false;
			const uint32 numCols = item->columnCount();		// check all columns
			for (uint32 i = 0; i < numCols && !matches; ++i)
				matches |= item->text(i).contains(filterString, Qt::CaseInsensitive);

			// hide or show
			item->setHidden(matches == false);

			if (matches == true)
				return 1;
			else
				return 0;
		}
		else  // this is a folder
		{
			// if its name matches show all files withing it but don't list subfolders (otherwise we'll see the whole subtree // TODO maybe this is the desired behavior?)
			bool folderMatches = false;
			
			const uint32 numCols = item->columnCount();		// check all columns
			for (uint32 i = 0; i < numCols && !folderMatches; ++i)
			{
				if (item->text(i).contains(filterString, Qt::CaseInsensitive))
					folderMatches = true;
			}

			// show all child items
			if (folderMatches == true)
			{
				for (uint32 i = 0; i < numChildren; ++i)
				{
					QTreeWidgetItem* child = item->child(i);
					if (child->childCount() == 0) // is file
					{
						child->setHidden(false);
						numVisible++;
					}
					else						 //  is folder
					{
						numVisible += ApplyFilter(filterString, child);
					}
				}
				
				return numVisible;
			} 


			// if the folder name didn't match we start recursive filtering on child items
			for (uint32 i = 0; i < numChildren; ++i)
				if (item->child(i) != NULL) // required?
					numVisible += ApplyFilter(filterString, item->child(i));

			// hide folder item if it (and its subfolders) don't contain any visible files
			item->setHidden(numVisible == 0);

			return numVisible;
		}
	}
}


// parse a given folder from JSON
QTreeWidgetItem* BackendFileSystemWidget::AddFolderItem(const Json::Item& jsonFolderItem, const Core::String& path, QTreeWidgetItem* parentItem)
{
	Json::Item nameItem			= jsonFolderItem.Find("name");
	Json::Item folderIdItem		= jsonFolderItem.Find("folderId");
	Json::Item attributesItem	= jsonFolderItem.Find("attributes");

	CORE_ASSERT(nameItem.IsNull() == false);
	CORE_ASSERT(folderIdItem.IsNull() == false);

	// create the tree widget item for the given folder
	QTreeWidgetItem* folderItem = NULL;
	if (parentItem == NULL)
		folderItem = new QTreeWidgetItem(mTreeWidget);
	else
		folderItem = new QTreeWidgetItem(parentItem);

	String folderUuid = folderIdItem.GetString();

//#ifdef NEUROMORE_PLATFORM_WINDOWS
	folderItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
//#else
	//folderItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//#endif

	// creud
	Creud creud;
	creud.ReadFromJson( jsonFolderItem );

	String iconPath = "Images/Icons/Folder.png";

	// sharing type
	if (attributesItem.IsNull() == false)
	{
		Json::Item sharingTypeItem = attributesItem.Find("sharingType");
		if (sharingTypeItem.IsNull() == false)
		{
			String sharingTypeString = sharingTypeItem.GetString();
			if (sharingTypeString.IsEqual("PRIVATE") == true)
			{
				iconPath = "Images/Icons/FolderApplications.png";

				// only do the very first time so that we don't pick a subfolder of the personal folder
				if (mPersonalFolderUuid.IsEmpty() == true)
					mPersonalFolderUuid = folderUuid;
			}
			else 
				iconPath = "Images/Icons/Users.png";
		}
	}

	folderItem->setIcon( 0, GetQtBaseManager()->FindIcon(iconPath.AsChar()) );

	folderItem->setText( 0, nameItem.GetString() );

	String folderPath = path + nameItem.GetString() + "/";
	
	folderItem->setData( 0, USERDATA_TYPE, FOLDER_TYPE );
	folderItem->setData( 0, USERDATA_PATH, folderPath.AsChar() );
	folderItem->setData( 0, USERDATA_ID, folderUuid.AsChar() );
	folderItem->setData( 0, USERDATA_CREUD, creud.ConvertToInt() );

	if (parentItem == NULL)
		mTreeWidget->addTopLevelItem( folderItem );
	else
		parentItem->addChild( folderItem );

	// item selection
	const bool isSelected = IsItemSelected( folderIdItem.GetString() );
	folderItem->setSelected( isSelected );

	// collapsed or expanded
	const bool isCollapsed = IsItemCollapsed(folderUuid.AsChar());
	folderItem->setExpanded( !isCollapsed );

	/////////////////////////////////

	// add all files within the current folder and add them
	Json::Item filesItem = jsonFolderItem.Find("files");
	if (filesItem.IsArray() == true)
	{
		const uint32 numFiles = filesItem.Size();
		for (uint32 i=0; i<numFiles; ++i)
			AddFileItem( filesItem[i], folderPath, folderItem );
	}

	// add all sub folders
	Json::Item foldersItem = jsonFolderItem.Find("folders");
	if (foldersItem.IsArray() == true)
	{
		const uint32 numFolders = foldersItem.Size();
		for (uint32 i=0; i<numFolders; ++i)
			AddFolderItem( foldersItem[i], folderPath, folderItem );
	}

	return folderItem;
}


// add a file tree widget item
QTreeWidgetItem* BackendFileSystemWidget::AddFileItem(const Json::Item& jsonFileItem, const Core::String& path, QTreeWidgetItem* parentItem)
{
	Json::Item nameItem		= jsonFileItem.Find("name");
	Json::Item typeItem		= jsonFileItem.Find("type");
	Json::Item fileIdItem	= jsonFileItem.Find("fileId");
	Json::Item revisionItem	= jsonFileItem.Find("revision");

	// create and add the tree widget item
	QTreeWidgetItem* fileItem = new QTreeWidgetItem(parentItem);
//#ifdef NEUROMORE_PLATFORM_WINDOWS
	fileItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
//#else
	//fileItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//#endif

	String typeString = typeItem.GetString();
	if (typeString.IsEqualNoCase(ITEM_TYPE_CLASSIFIER) == true)
		fileItem->setIcon( 0, GetQtBaseManager()->FindIcon("Images/Graph/ab16b7ee-bb6e-11e4-8dfc-aa07a5b093db.png") );
	else if (typeString.IsEqualNoCase(ITEM_TYPE_STATEMACHINE) == true)
		fileItem->setIcon( 0, GetQtBaseManager()->FindIcon("Images/Graph/532b918c-bb6c-11e4-8dfc-aa07a5b093db.png") );
	else if (typeString.IsEqualNoCase(ITEM_TYPE_EXPERIENCE) == true)
		fileItem->setIcon( 0, GetQtBaseManager()->FindIcon("Images/Graph/Experience.png") );

	// creud
	Creud creud;
	creud.ReadFromJson( jsonFileItem );

	fileItem->setText( 0, nameItem.GetString() );
	fileItem->setData( 0, USERDATA_PATH, path.AsChar() );
	fileItem->setData( 0, USERDATA_TYPE, typeString.AsChar() );
	fileItem->setData( 0, USERDATA_ID, fileIdItem.GetString() );
	fileItem->setData( 0, USERDATA_REVISION, revisionItem.GetInt() );
	fileItem->setData( 0, USERDATA_CREUD, creud.ConvertToInt() );

	// item selection
	const bool isSelected = IsItemSelected( fileIdItem.GetString() );
	fileItem->setSelected( isSelected );
		
	parentItem->addChild(fileItem);
	return fileItem;
}


// returns NULL in case nothing or multiple items are selected, returns the selection item if exactly one is selected
BackendFileSystemWidget::SelectionItem* BackendFileSystemWidget::GetSingleSelectionItem()
{
	// return NULL in case no item is selected
	if (mSelectedItems.IsEmpty() == true)
		return NULL;

	// return NULL in case multiple items are selected
	if (mSelectedItems.Size() > 1)
		return NULL;

	// return single selected item
	return &mSelectedItems[0];
}


void BackendFileSystemWidget::CollectSelectedItems()
{
	// clear the selected items array
	mSelectedItems.Clear();

	// get the selected items and the number of them
	QList<QTreeWidgetItem*> selectedTreeWidgetItems = mTreeWidget->selectedItems();
	
	// get the number of selected items and return directly in case nothing is selected
	const uint32 numSelectedItems = selectedTreeWidgetItems.count();
	if (numSelectedItems == 0)
		return;

	// iterate over the selected items in the tree widget and construct a selected item object for each
	for (uint32 i=0; i<numSelectedItems; ++i)
	{
		SelectionItem selectionItem = CreateSelectionItem( selectedTreeWidgetItems[i] );
		mSelectedItems.Add(selectionItem);
	}
}


bool BackendFileSystemWidget::IsItemSelected(const char* id)
{
	const uint32 numSelectedItems = mSelectedItems.Size();
	for (uint32 i=0; i<numSelectedItems; ++i)
	{
		if (mSelectedItems[i].GetUuidString().IsEqual(id) == true)
			return true;		
	}

	return false;
}


// called when an item in the list widget got selected
void BackendFileSystemWidget::OnSelectionChanged()
{
	CollectSelectedItems();

	UpdateInterface();
}

//
void BackendFileSystemWidget::OnItemClicked(QTreeWidgetItem* item, int column)
{
	CreateSelectionItem(item);
}


void BackendFileSystemWidget::OnItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	CreateSelectionItem(item);

	if (GetSession()->IsRunning() == true)
		QMessageBox::information( this, "Info", "Cannot load file while session is running.");
	else
		OnOpenItem();
}


void BackendFileSystemWidget::OnContextMenuRetrieveItemRevision()
{
	QAction*		revisionAction	= qobject_cast<QAction*>(sender());
	SelectionItem	selectedItem	= GetSelectedItem();
	int32				revision	= revisionAction->data().toInt();

	OpenFile( selectedItem, revision );
}


void BackendFileSystemWidget::OnContextMenu(const QPoint& point)
{
	QTreeWidgetItem* item = mTreeWidget->itemAt(point);
	if (item == NULL)
		return;

	SelectionItem* singleSelectedItem = GetSingleSelectionItem();

	QMenu menu(this);
	menu.setVisible(false);

	// in case we have write rights and only one item selected
	if (singleSelectedItem != NULL)
	{
		if (singleSelectedItem->IsFolder() == true)
		{
			if (singleSelectedItem->GetCreud().Update() == true)
			{
				// create folder
				QAction* addFolderAction = menu.addAction( "Create &Folder", this, SLOT(OnCreateFolder()) );
				addFolderAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/PlusFolder.png") );

				// create file
				QAction* addFileAction = menu.addAction( "&Create File", this, SLOT(OnCreateFile()) );
				addFileAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png") );

				// rename folder
				if (singleSelectedItem->HasParent() == true)
				{
					QAction* renameAction = menu.addAction( "&Rename Folder", this, SLOT(OnRenameItem()) );
					renameAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/EditText.png") );
				}
			}

			if (singleSelectedItem->HasParent() == true && singleSelectedItem->GetCreud().Delete() == true)
			{
				// remove folder or file
				QAction* removeAction = menu.addAction("&Delete Folder", this, SLOT(OnRemoveItem()) );
				removeAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Minus.png") );
			}
		}
		else
		{
			// open file
			if (singleSelectedItem->GetCreud().Update() == true)
			{
				QAction* openAction = menu.addAction("&Open", this, SLOT(OnOpenItem()));
				openAction->setIcon(GetQtBaseManager()->FindIcon("Images/Icons/Open.png"));
			}

			// rename folder or file
			if (singleSelectedItem->GetCreud().Update() == true)
			{
				QAction* renameAction = menu.addAction( "&Rename File", this, SLOT(OnRenameItem()) );
				renameAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/EditText.png") );
			}

			// copy file to personal folder
			QAction* copyFileAction = menu.addAction( "Copy To &Personal Folder", this, SLOT(OnCopyFileToPersonalFolder()) );
			copyFileAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Copy.png") );

			if (singleSelectedItem->GetCreud().Delete() == true)
			{
				// remove folder or file
				QAction* removeAction = menu.addAction("&Delete File", this, SLOT(OnRemoveItem()) );
				removeAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Minus.png") );
			}

#ifndef PRODUCTION_BUILD
//#ifdef NEUROMORE_PLATFORM_WINDOWS
			menu.addSeparator();

				// copy to clipboard
				QAction* copyJsonToClipboardAction = menu.addAction("Copy JSON to clipboard", this, SLOT(OnCopyJsonToClipboard()) );
				copyJsonToClipboardAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Copy.png") );

				// save JSON to disk
				QAction* saveToDiskAction = menu.addAction("Save To HDD", this, SLOT(OnSaveToDisk()) );
				saveToDiskAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/SaveAs.png") );

				// load JSON from disk and save it to the cloud
				QAction* loadFromDiskSaveToCloudAction = menu.addAction("Upload local file to cloud", this, SLOT(OnLoadFromDiskAndSaveToCloud()) );
				loadFromDiskSaveToCloudAction->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Cloud.png") );
			
			menu.addSeparator();
//#endif
#endif

			// revisions
			QMenu* revisionsMenu = menu.addMenu("Revisions");
			String revisionActionName;
		
			const int32 currentRevision = item->data(0, USERDATA_REVISION).toInt();
		
			int32 startRevision = 1;
			int32 numDisplayedRevisions = 32;
			if (currentRevision-numDisplayedRevisions > 1)
				startRevision = currentRevision-numDisplayedRevisions;

			for (int32 i=startRevision; i<=currentRevision; ++i)
			{
				if (i == currentRevision)
					revisionActionName.Format( "%i (current)", i );
				else
					revisionActionName.Format( "%i", i );

				QAction* revisionAction = revisionsMenu->addAction( revisionActionName.AsChar(), this, SLOT(OnContextMenuRetrieveItemRevision()) );
				revisionAction->setData( i );
			}
		}
	}

	if (menu.isEmpty() == false)
	{
		menu.exec(mTreeWidget->mapToGlobal(point));
		menu.setVisible(true);
	}
}


// called when the plus button got clicked
void BackendFileSystemWidget::OnCreateFolder()
{
	SelectionItem selectionItem = GetSelectedItem();

	// get the parent folder id in case we selected a file, and in case we selected a folder just its own id
	String parentFolderId;
	if (selectionItem.IsFolder() == true)
		parentFolderId = selectionItem.GetUuid();
	else
		parentFolderId = selectionItem.GetParentUuid();

	// open the dialog and return directly if the user canceled the process
	CreateFolderWindow createFolderWindow( "Name Folder", "New Folder", selectionItem.GetName(), this );
	if (createFolderWindow.exec() == QDialog::Rejected)
		return;

	GetFileSystem()->CreateFolder( GetUser()->GetToken(), createFolderWindow.GetLabelText().AsChar(), parentFolderId.AsChar() );
}


// called when the plus button got clicked
void BackendFileSystemWidget::OnCreateFile()
{
	SelectionItem selectionItem = GetSelectedItem();

	if (selectionItem.IsValid() == false)
	{
		QMessageBox::critical( this, "ERROR", "No item selected. Cannot create file." );
		return;
	}

	// get the parent folder id in case we selected a file, and in case we selected a folder just its own id
	String parentFolderId;
	if (selectionItem.IsFolder() == true)
		parentFolderId = selectionItem.GetUuid();
	else
		parentFolderId = selectionItem.GetParentUuid();

	// open the dialog and return directly if the user canceled the process
	CreateFileWindow createFileWindow( "Name File", "New File", this );
	if (createFileWindow.exec() == QDialog::Rejected)
		return;


	// 1. construct /files/create request
	FilesCreateRequest request( GetUser()->GetToken(), createFileWindow.GetLabelText().AsChar(), parentFolderId.AsChar(), createFileWindow.GetType().AsChar(), "{}" );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FilesCreateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		Refresh();
	});
}


void BackendFileSystemWidget::OnSearchFieldTextEdited(const QString & text)
{
	ApplyFilter(text);
}


void BackendFileSystemWidget::OpenFile(SelectionItem selectionItem, int32 revision)
{
	// dont open folders
	if (selectionItem.IsFolder() == true)
		return;

	// check for execute rights
	if (selectionItem.GetCreud().Execute() == false)
		return;
	
	// open classifier
	if (selectionItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_CLASSIFIER) == true)
		GetFileManager()->OpenClassifier( FileManager::LOCATION_BACKEND, selectionItem.GetUuid(), selectionItem.GetName(), revision );
	
	// open state machine
	if (selectionItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_STATEMACHINE) == true)
		GetFileManager()->OpenStateMachine( FileManager::LOCATION_BACKEND, selectionItem.GetUuid(), selectionItem.GetName(), revision );

	// open experience
	if (selectionItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_EXPERIENCE) == true)
		GetFileManager()->OpenExperience( FileManager::LOCATION_BACKEND, selectionItem.GetUuid(), selectionItem.GetName(), revision );
}


void BackendFileSystemWidget::OnOpenItem()
{
	if (GetSingleSelectionItem() == NULL)
		return;

	// get the single selected item
	// NOTE we create a copy here, as the selection item might change during successive calls to GetMainWindow()->CloseFile()
	SelectionItem singleSelectedItem = *GetSingleSelectionItem();

	OpenFile( singleSelectedItem );
}


void BackendFileSystemWidget::OnRenameItem()
{
	// get the selected item
	SelectionItem* singleSelectedItem = GetSingleSelectionItem();
	if (singleSelectedItem == NULL)
		return;

	// file
	if (singleSelectedItem->IsFolder() == false)
	{
		// open the dialog and return directly if the user canceled the process
		EnterLabelWindow nameWindow("Please enter a new file name", singleSelectedItem->GetName(), "/\\", this);
		if (nameWindow.exec() == QDialog::Rejected)
			return;

		String newName = nameWindow.GetLabelText();

		// rename file in backend
		GetFileSystem()->UpdateFile(GetUser()->GetToken(), singleSelectedItem->GetUuid(), NULL, newName.AsChar(), "");

		// TODO rename using file manager instead of directly!
	}
	else // folder
	{
		// open the dialog and return directly if the user canceled the process
		EnterLabelWindow nameWindow("Please enter a new folder name", singleSelectedItem->GetName(), "/\\", this);
		if (nameWindow.exec() == QDialog::Rejected)
			return;

		GetFileSystem()->UpdateFolder(GetUser()->GetToken(), singleSelectedItem->GetUuid(), "", nameWindow.GetLabelText().AsChar());
	}
}


void BackendFileSystemWidget::OnCopyFileToPersonalFolder()
{
	// check if the personal folder is valid
	if (mPersonalFolderUuid.IsEmpty() == true)
	{
		QMessageBox::critical( this, "ERROR", "Cannot copy file to personal folder. Personal folder not detected." );
		return;
	}

	// get the selected item
	SelectionItem selectedItem = GetSelectedItem();

	// find the graph based on the uuid
	Graph* graph = GetGraphManager()->FindGraphByUuid( selectedItem.GetUuid() );
	if (graph == NULL)
	{
		QMessageBox::critical( this, "ERROR", "Cannot copy file to personal folder. Different file selected than the shown graph in the graph window." );
		return;
	}

	String jsonContent;
	bool result = GraphExporter::Save( &jsonContent, graph );
	if (result == false)
	{
		String errorMsg;
		errorMsg.Format( "Cannot save file (filename='%s', UUID=%s).", selectedItem.GetName(), selectedItem.GetUuid() );
		LogError( errorMsg.AsChar() );
		QMessageBox::critical( this, "ERROR", "Cannot save file." );
		return;
	}

	String newItemName = selectedItem.GetNameString() + "_Copy";


	// 1. construct /files/create request
	FilesCreateRequest request( GetUser()->GetToken(), newItemName.AsChar(), mPersonalFolderUuid.AsChar(), selectedItem.GetType(), jsonContent.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// 3. construct and parse response
		FilesCreateResponse response(networkReply);
		if (response.HasError() == true)
			return;

		// 4. handle response
		Refresh();
	});
}


// save the selected graph as string and copy that string to the clipboard
void BackendFileSystemWidget::OnCopyJsonToClipboard()
{
	QClipboard* clipboard = QApplication::clipboard();
	String jsonContent;

	// get the selected item and return directly in case no single selected item is selected
	SelectionItem* selectedItem = GetSingleSelectionItem();
	if (selectedItem == NULL || selectedItem->IsFolder() == true)
	{
		clipboard->setText("");
		QMessageBox::critical( this, "ERROR", "Cannot copy graph JSON to clipboard. No or multiple files selected." );
		return;
	}

	// EXPERIENCE
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience != NULL && experience->GetUuidString().IsEqual(selectedItem->GetUuidString()) == true)
	{
		// save the experience
		Json jsonParser;
        Json::Item rootItem = jsonParser.GetRootItem();
		experience->Save(jsonParser, rootItem);	// Note: add parameters to export whole design incl. attached classifier/statemachine here
		jsonParser.WriteToString(jsonContent, true);
	}
	// GRAPH
	else
	{
		Graph* graph = GetGraphManager()->FindGraphByUuid( selectedItem->GetUuid() );
		if (graph == NULL)
		{
			clipboard->setText("");
			QMessageBox::critical( this, "ERROR", "Cannot copy graph JSON to clipboard. Different file selected than the shown graph in the graph window." );
			return;
		}

		// save the currently shown graph and save it as a string
		bool result = GraphExporter::Save( &jsonContent, graph );
		if (result == false)
		{
			clipboard->setText("");
			QMessageBox::critical( this, "ERROR", "Cannot copy graph JSON to clipboard. Saving the graph failed." );
			return;
		}
	}

	clipboard->setText( jsonContent.AsChar() );
}


// save JSON to a disk on the local hard drive
void BackendFileSystemWidget::OnSaveToDisk()
{
	// get the selected item and return directly in case no single selected item is selected
	SelectionItem* selectedItem = GetSingleSelectionItem();
	if (selectedItem == NULL || selectedItem->IsFolder() == true)
	{
		QMessageBox::critical( this, "ERROR", "Cannot save graph JSON to local hard disk. No or multiple files selected." );
		return;
	}

	
	// get the filename where to save it
	QString defaultFileName = mLastSelectedFileDialogFolder + "\\" + selectedItem->GetName();
	QString selectedFilter;
	QFileDialog::Options options;
	QString filename = QFileDialog::getSaveFileName(	this,						// parent
														"Save",						// caption
														defaultFileName,			// suggested file name and directory
														"JSON (*.json)",
														&selectedFilter,
														options );
	if (filename.isEmpty() == true)
		return;

	mLastSelectedFileDialogFolder = QFileInfo(filename).absolutePath();

	// save EXPERIENCE
	Experience* experience = GetEngine()->GetActiveExperience();
	if (experience != NULL && experience->GetUuidString().IsEqual(selectedItem->GetUuidString()) == true)
	{
		// save the experience
		Json jsonParser;
        Json::Item rootItem = jsonParser.GetRootItem();
		experience->Save(jsonParser, rootItem);	// Note: add parameters to export whole design incl. attached classifier/statemachine here
		bool result = jsonParser.WriteToFile(FromQtString(filename).AsChar(), true);
		if (result == false)
		{
			QMessageBox::critical( this, "ERROR", "Cannot save design JSON to local hard disk." );
			return;
		}
	} 
	// save GRAPH
	else 
	{
		// find the graph based on the uuid
		Graph* graph = GetGraphManager()->FindGraphByUuid( selectedItem->GetUuid() );
		if (graph == NULL)
		{
			QMessageBox::critical( this, "ERROR", "Cannot save graph JSON to local hard disk. Different file selected than the shown graph in the graph window." );
			return;
		}

		// save the currently shown graph and save it as a string
		bool result = GraphExporter::Save( FromQtString(filename).AsChar(), graph );
		if (result == false)
		{
			QMessageBox::critical( this, "ERROR", "Cannot save graph JSON to local hard disk. Saving the graph failed." );
			return;
		}
	}




	
}


// load a JSON graph from disk and save it back to the backend
void BackendFileSystemWidget::OnLoadFromDiskAndSaveToCloud()
{
	// get the filename where to save it
	QFileDialog::Options options;
	QString selectedFilter;
	QString qtFilename = QFileDialog::getOpenFileName(	this,								// parent
														"Open",								// caption
														"",									// directory
														"JSON (*.json)",
														&selectedFilter,
														options );

	if (qtFilename.isEmpty() == true)
		return;

	String filename = FromQtString(qtFilename);

	SelectionItem selectedItem = GetSelectedItem();

	// is experience
	if (selectedItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_EXPERIENCE) == true)
	{
		Json jsonParser;
		if (jsonParser.ParseFile(filename.AsChar()) == false)
		{
			LogError( "BackendFileSystemWidget::OnLoadFromDiskAndSaveToCloud(): Cannot open file '%s'.", filename.AsChar());
			return;
		}

		Experience* experience = new Experience();
		if (experience->Load(jsonParser, jsonParser.GetRootItem()) == false)
		{
			LogError( "BackendFileSystemWidget::OnLoadFromDiskAndSaveToCloud(): Cannot parse Json file '%s'.", filename.AsChar());
			return;
		}
		
		GetBackendInterface()->GetFileSystem()->SaveExperience( GetUser()->GetToken(), selectedItem.GetUuid(), experience );

		delete experience;
	}
	// is classifier/statemachine
	else
	{
		Graph* graph = NULL;
	
		if (selectedItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_CLASSIFIER) == true)
			graph = new Classifier();
		else if (selectedItem.GetTypeString().IsEqualNoCase(ITEM_TYPE_STATEMACHINE) == true)
			graph = new StateMachine();
		else 
			return;

		if (GraphImporter::LoadFromFile(filename.AsChar(), graph) == false)
		{
			LogError( "BackendFileSystemWidget::OnLoadFromDiskAndSaveToCloud(): Cannot parse Json file '%s'.", filename.AsChar());
			return;
		}
		
		GetBackendInterface()->GetFileSystem()->SaveGraph( GetUser()->GetToken(), selectedItem.GetUuid(), graph );

		delete graph;
	}
}


// called when the minus button got clicked
void BackendFileSystemWidget::OnRemoveItem()
{
	// get the selected item
	SelectionItem selectedItem = GetSelectedItem();

	// remove item
	if (selectedItem.IsFolder() == false)
	{
		String filename = selectedItem.GetName();

		// make sure we really want to remove it
		String msgText;
		msgText.Format( "Are you sure you want to delete <b>'%s'</b>?", filename.AsChar() );
		QMessageBox msgBox(	QMessageBox::Warning, "Delete File", msgText.AsChar(), QMessageBox::Yes | QMessageBox::No, this );
		msgBox.setDefaultButton( QMessageBox::No );

		msgBox.setTextFormat( Qt::RichText );
		if (msgBox.exec() != QMessageBox::Yes)
			 return;

		// close file, without save-changes dialog
		GetFileManager()->Close(selectedItem.GetUuid());
		
		// delete file in backend
		GetFileSystem()->DeleteFile( GetUser()->GetToken(), selectedItem.GetUuid() );
	}
	// remove folder
	else
	{
		String foldername = selectedItem.GetName();

		// make sure we really want to remove it
		String msgText;
		msgText.Format( "Are you sure you want to delete <b>'%s'</b>?", foldername.AsChar() );
		QMessageBox msgBox(	QMessageBox::Warning, "Delete Folder", msgText.AsChar(), QMessageBox::Yes | QMessageBox::No, this );

		msgBox.setTextFormat( Qt::RichText );
		if (msgBox.exec() != QMessageBox::Yes)
			 return;

		GetFileSystem()->DeleteFolder( GetUser()->GetToken(), selectedItem.GetUuid() );

		// TODO we should unload all loaded files that are inside this folder ... d'oh!
	}
}


BackendFileSystem* BackendFileSystemWidget::GetFileSystem() const	
{
	return GetQtBaseManager()->GetBackendInterface()->GetFileSystem();
}


void BackendFileSystemWidget::BackendFileSystemTreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
	mDraggedItem = SelectionItem();

	if (event->source() != this)
	{ 
		event->ignore();
		return; 
	}

	// get the current item (do NOT use itemAt(event->pos()) it's buggy)
	QTreeWidgetItem* item = this->currentItem();

	if (item == NULL)
	{
		event->ignore();
		return;
	}

#ifndef BACKEND_FILESYSTEM_ALLOWALL
	// only allow drag and drop in case the item has update privileges
	mDraggedItem = mParent->CreateSelectionItem(item);
	if (mDraggedItem.GetCreud().Update() == false)
	{
		event->ignore();
		return;
	}
#endif
	
	event->accept();
}


void BackendFileSystemWidget::BackendFileSystemTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{


	// only accept drop events from the same tree widget
	if (event->source() != this)
	{
		event->ignore();
		return;
	}

	// get the item at the current mouse position
	QTreeWidgetItem* item = itemAt( event->pos() );
	if (item == NULL)
	{
		event->ignore();
		return;
	}

	// only allow for folder items where we have update rights or file of equal type
	SelectionItem mouseOverSelectionItem = mParent->CreateSelectionItem(item);
#ifndef BACKEND_FILESYSTEM_ALLOWALL
	if (!mouseOverSelectionItem.GetCreud().Update() || (!mouseOverSelectionItem.IsFolder() && mouseOverSelectionItem.GetTypeString() != mDraggedItem.GetTypeString()))
	{
		event->ignore();
		return;
	}

	// don't allow dropping to the same folder
	if (mouseOverSelectionItem.IsFolder() && mDraggedItem.GetPathString().IsEqual(mouseOverSelectionItem.GetPath()) == true)
	{
		event->ignore();
		return;
	}
#endif

	// extract the class name
	String dropText = FromQtString( event->mimeData()->text() );
	Array<String> parts = dropText.Split( StringCharacter::semiColon );
	if (parts.Size() != 3)
	{
		LogError("BackendFileSystemTreeWidget::dropEvent() - Incorrect syntax using drop data '%s'", dropText.AsChar());
		event->accept();
		return;
	}

	String dragItemUuid = parts[0];

	// don't allow for the same item
	if (mouseOverSelectionItem.GetUuidString().IsEqual(dragItemUuid.AsChar()) == true)
	{
		event->ignore();
		return;
	}

	// accept the event
	event->accept();
}


void BackendFileSystemWidget::BackendFileSystemTreeWidget::dropEvent(QDropEvent* event)
{

	// only accept drop events from the same tree widget
	if (event->source() != this)
		return;

	QTreeWidgetItem* item = itemAt( event->pos() );
	if (item == NULL)
	{
		event->accept();
		return;
	}

	SelectionItem dropSelectionItem = mParent->CreateSelectionItem(item);

	// extract the class name
	String dropText = FromQtString( event->mimeData()->text() );
	Array<String> parts = dropText.Split( StringCharacter::semiColon );
	if (parts.Size() != 3)
	{
		LogError("BackendFileSystemTreeWidget::dropEvent() - Incorrect syntax using drop data '%s'", dropText.AsChar());
		event->accept();
		return;
	}

	String dragItemUuid = parts[0];
	bool   isValidFile	= parts[1].ToBool();
	String dragItemName	= parts[2];

   if (dropSelectionItem.IsFolder())
   {
      // get the parent folder id in case we selected a file, and in case we selected a folder just its own id
      String newFolderId;
      if (dropSelectionItem.IsFolder() == true)
         newFolderId = dropSelectionItem.GetUuid();
      else
         newFolderId = dropSelectionItem.GetParentUuid();

      String warningTitle;
      String warningText;
      if (isValidFile == true)
      {
         warningTitle = "Move File";
         warningText.Format("Moving this file will share it with everyone who can see the folder '%s'.", dropSelectionItem.GetName());
      }
      else
      {
         warningTitle = "Move Folder";
         warningText.Format("Moving this folder will share it with everyone who can see the folder '%s'.", dropSelectionItem.GetName());
      }

      if (QMessageBox::warning(this, warningTitle.AsChar(), warningText.AsChar(), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
      {
         if (isValidFile == true)
            mParent->GetFileSystem()->UpdateFile(GetUser()->GetToken(), dragItemUuid.AsChar(), NULL, "", newFolderId.AsChar());
         else
            mParent->GetFileSystem()->UpdateFolder(GetUser()->GetToken(), dragItemUuid.AsChar(), newFolderId.AsChar(), "");
      }
   }
   else
   {
      // source file values
      const String srcUuid = mDraggedItem.GetUuid();
      const String srcPath = mDraggedItem.GetPathString();
      const String srcName = mDraggedItem.GetNameString();

      // dest file values
      const String dstUuid = dropSelectionItem.GetUuid();
      const String dstPath = dropSelectionItem.GetPathString();
      const String dstName = dropSelectionItem.GetNameString();

      String warningText;
      warningText.Format("You are about to replace '%s%s' with '%s%s'.",
         dstPath.AsChar(), dstName.AsChar(),
         srcPath.AsChar(), srcName.AsChar());

      if (QMessageBox::warning(this, "Replacing File", warningText.AsChar(), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
      {
         // 1. Load the contents of the dragged item
         FilesGetRequest request(GetUser()->GetToken(), srcUuid);
         QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request);
         connect(reply, &QNetworkReply::finished, this, [reply, this, dstUuid]()
         {
            QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());
            FilesGetResponse response(networkReply);
            if (response.HasError())
            {
               QMessageBox::warning(this, "Error", "Operation failed", QMessageBox::Ok);
               return;
            }

            // 2. Update to the contents of the dropped item
            FilesUpdateRequest request(GetUser()->GetToken(), dstUuid, response.GetJsonContent());
            QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest(request);
            connect(reply, &QNetworkReply::finished, this, [reply, this]()
            {
               QNetworkReply* networkReply = qobject_cast<QNetworkReply*>(sender());
               FilesUpdateResponse response(networkReply);
               if (response.HasError())
               {
                  QMessageBox::warning(this, "Error", "Operation failed", QMessageBox::Ok);
                  return;
               }
            });
         });
      }
   }

	return;
}


// drop event on the tree widget
QMimeData* BackendFileSystemWidget::BackendFileSystemTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	if (items.count() != 1)
		return NULL;

	QTreeWidgetItem*	item			= items.at(0);
	SelectionItem		selectionItem	= mParent->CreateSelectionItem(item);
	
	QMimeData* data = new QMimeData();

	String textData;
	textData += selectionItem.GetUuid();
	textData.FormatAdd(";%i;%s", !selectionItem.IsFolder(), selectionItem.GetName());
	
	data->setText( textData.AsChar() );

	return data;
}


void BackendFileSystemWidget::OnItemCollapsed(QTreeWidgetItem* item)
{
	SelectionItem selectionItem = CreateSelectionItem(item);
	SetCollapseState( selectionItem.GetUuid(), true );
}


void BackendFileSystemWidget::OnItemExpanded(QTreeWidgetItem* item)
{
	SelectionItem selectionItem = CreateSelectionItem(item);
	SetCollapseState( selectionItem.GetUuid(), false );
}


BackendFileSystemWidget::CollapseState* BackendFileSystemWidget::FindCollapsedState(const char* uuid)
{
	const uint32 numCollapseStates = mFolderCollapseStates.Size();
	for (uint32 i=0; i<numCollapseStates; ++i)
	{
		if (mFolderCollapseStates[i].GetUuidString().IsEqual(uuid) == true)
			return &mFolderCollapseStates[i];
	}

	return NULL;
}


bool BackendFileSystemWidget::IsItemCollapsed(const char* uuid)
{
	CollapseState* state = FindCollapsedState(uuid);
	if (state == NULL)
		return false;

	return state->IsCollapsed();
}


void BackendFileSystemWidget::SetCollapseState(const char* uuid, bool isCollapsed)
{
	CollapseState* state = FindCollapsedState(uuid);
	if (state == NULL)
	{
		mFolderCollapseStates.Add( CollapseState(uuid, isCollapsed) );
		return;
	}

	state->SetIsCollapsed(isCollapsed);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
BackendFileSystemWidget::SelectionItem::SelectionItem()
{ 
	SetName("");
	SetPath("");
	SetType("");
	SetUuid("");
	mRevision		=-1;
	SetParentType("");
}


// constructor
BackendFileSystemWidget::SelectionItem::SelectionItem(const char* name, const char* path, const char* type, const char* id, const Creud& creud, int revision, const char* parentType, const char* parentUuid, const Creud& parentCrued)
{
	SetName(name);
	SetPath(path);
	SetType(type);
	SetUuid(id);
	mCreud = creud;
	mRevision=revision;

	// parent
	SetParentType(parentType);
	SetParentUuid(parentUuid);
	mParentCreud = parentCrued;
}
