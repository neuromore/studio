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

// include the required headers
#include <Backend/UsersGetRequest.h>
#include <Backend/UsersGetResponse.h>
#include "SelectUserWindow.h"
#include "InviteUserWindow.h"
#include "CreateUserWindow.h"

using namespace Core;

// constructor
SelectUserWindow::SelectUserWindow(const User& user, QWidget* parent, bool showSelf) : 
   mUser(user), 
   mFullAccess(user.FindRule("ROLE_ClinicAdmin") || user.FindRule("ROLE_ClinicClinician")),
   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	mShowSelf = showSelf;
	mNumTotalPages	= 0;
	mCurrentPage	= CORE_INVALIDINDEX32;

	// set the window title
	setWindowTitle( "Select User" );
	setWindowIcon( GetQtBaseManager()->FindIcon("Images/Icons/Users.png") );
	setMinimumWidth(600);
	setMinimumHeight(400);
	setModal(true);
	setWindowModality( Qt::ApplicationModal );
	 

	// add the main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);


	// top layout
	QHBoxLayout* topHLayout = new QHBoxLayout();
	topHLayout->setSpacing(6);
	topHLayout->setAlignment(Qt::AlignLeft);
	mainLayout->addLayout(topHLayout);

	// refresh button
	mRefreshButton = new ImageButton("/Images/Icons/Refresh.png", DEFAULT_ICONSIZE, "Refresh");
	mRefreshButton->setToolTip( "Update the user list." );
	topHLayout->addWidget(mRefreshButton);

	connect(mRefreshButton, &QPushButton::clicked, this, &SelectUserWindow::OnRefresh);

	// refresh label
	mRefreshLabel = new QLabel( "", this );
	topHLayout->addWidget(mRefreshLabel);

	// refresh timer
	mRefreshTimer = new QTimer(this);
	connect( mRefreshTimer, &QTimer::timeout, this, &SelectUserWindow::OnRefreshTimer );
	mRefreshTimer->setInterval(500);
	mRefreshTimer->start();

	// add spacer widget
	QWidget* spacerWidget = new QWidget(this);
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	topHLayout->addWidget(spacerWidget);

	// search edit box
	mSearchEdit = new SearchBoxWidget(this);
	connect(mSearchEdit, SIGNAL(TextChanged(const QString&)), this, SLOT(OnSearchEdited(const QString&)));
	connect(mSearchEdit, SIGNAL(TextCleared()), this, SLOT(OnSearchCleared()));
	topHLayout->addWidget(mSearchEdit);

	// search timer
	mSearchTimer = new QTimer(this);
	connect(mSearchTimer, &QTimer::timeout, this, &SelectUserWindow::OnRefresh );
	mSearchTimer->setInterval(750);

	// add table widget
	mTableWidget = new QTableWidget();
	mainLayout->addWidget(mTableWidget);
	mTableWidget->setEnabled(false);

 	// columns
	mTableWidget->setColumnCount(3);

	QTableWidgetItem* headerItem = new QTableWidgetItem("First/Middle Name");
	headerItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	mTableWidget->setHorizontalHeaderItem( 0, headerItem );

	headerItem = new QTableWidgetItem("Last Name");
	headerItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	mTableWidget->setHorizontalHeaderItem( 1, headerItem );

	headerItem = new QTableWidgetItem("Birthday");
	headerItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
	mTableWidget->setHorizontalHeaderItem( 2, headerItem );


	mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	mTableWidget->horizontalHeader()->show();

	// don't show the vertical header
	mTableWidget->verticalHeader()->hide();

	// complete row selection
	mTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableWidget->setSelectionMode( QAbstractItemView::SelectionMode::SingleSelection);
	mTableWidget->setFocusPolicy(Qt::NoFocus);	 // get rid of rectangular selection box around single cells
	mTableWidget->setAlternatingRowColors(true);

	connect( mTableWidget, &QTableWidget::cellDoubleClicked, this, &SelectUserWindow::OnCellDoubleClicked );
	connect( mTableWidget, &QTableWidget::itemSelectionChanged, this, &SelectUserWindow::OnSelectionChanged );


	// create protocol
	mCreateProtocolButton = new QPushButton("Create Protocol");
	mCreateProtocolButton->setToolTip( "Create a protocol for the selected user." );
	mCreateProtocolButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png") );
	mCreateProtocolButton->setEnabled(false);
	mainLayout->addWidget(mCreateProtocolButton);
	connect(mCreateProtocolButton, &QPushButton::clicked, this, &SelectUserWindow::OnCreateProtocolButtonClicked);

	// select user button
	mSelectUserButton = new QPushButton("Select User");
	mSelectUserButton->setToolTip( "Double-click the user that will do the session or select him or her and click this button." );
	mSelectUserButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/SuccessBlue.png") );
	mSelectUserButton->setEnabled(false);
	mainLayout->addWidget(mSelectUserButton);
	connect( mSelectUserButton, &QPushButton::clicked, this, &SelectUserWindow::OnSelectUserButtonClicked);

	// vertical bottom button row
	QHBoxLayout* hLayout = new QHBoxLayout();
	mainLayout->addLayout(hLayout);

	// create new user button
	mCreateUserButton = new QPushButton("Create User");
	mCreateUserButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png") );
	mCreateUserButton->setToolTip( "Create a new user." );
	mCreateUserButton->setEnabled(mFullAccess);
	hLayout->addWidget(mCreateUserButton);
	connect( mCreateUserButton, &QPushButton::clicked, this, &SelectUserWindow::OnCreateButtonClicked);

#ifndef NEUROMORE_BRANDING_ANT
	// invite user button
	mInviteUserButton = new QPushButton("Invite Existing User");
	mInviteUserButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/SendFeedback.png") );
	mInviteUserButton->setToolTip( "Invite users that already have a neuromore account." );
	mInviteUserButton->setEnabled(mFullAccess);
	hLayout->addWidget(mInviteUserButton);
	connect( mInviteUserButton, &QPushButton::clicked, this, &SelectUserWindow::OnInviteButtonClicked);
#endif
	// cancel button
	QPushButton* cancelButton = new QPushButton("Cancel");
	cancelButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Clear.png") );
	mainLayout->addWidget(cancelButton );
	connect( cancelButton, &QPushButton::clicked, this, &SelectUserWindow::OnCancelButtonClicked);


	// retrieve callbacks when we scroll
	QScrollBar* vScrollBar = mTableWidget->verticalScrollBar();
	connect( vScrollBar, &QScrollBar::valueChanged, this, &SelectUserWindow::OnScrolled );

	// reset our refresh timer after initializing the window
	mLastRefreshTimer.Reset();

	OnRefresh();

	// position window in the screen center
	GetQtBaseManager()->CenterToScreen(this);

	show();
}


// destructor
SelectUserWindow::~SelectUserWindow()
{
}


// double click on cell selects user immediately
void SelectUserWindow::OnCellDoubleClicked(int row, int column)
{
	const User& user = mListedUsers[row];

	// trigger the button event
	OnSelectUserButtonClicked();
}


void SelectUserWindow::OnSelectionChanged()
{
	QList<QTableWidgetItem*> selectedItems = mTableWidget->selectedItems();

	// is selection empty?
	if (selectedItems.isEmpty() == true)
	{
		mSelectUserButton->setEnabled(false);
		mCreateProtocolButton->setEnabled(false);
		return;
	}

	// change the selected user
	const User& user = mListedUsers[ selectedItems[0]->row() ];

	//LogDebug("Selected User '%s'", user.GetId());

	// enable selectuser button
	mSelectUserButton->setEnabled(true);
	mCreateProtocolButton->setEnabled(mFullAccess);
}


// emit OnUserSelected with user
void SelectUserWindow::OnSelectUserButtonClicked()
{
	QModelIndexList selectedRows = mTableWidget->selectionModel()->selectedRows();
	if (selectedRows.count() != 1)
		return;

	// get the array index from the mime data of the first item in the row
	const uint32 selectedRow = selectedRows.at(0).row();
	const User& user = mListedUsers[selectedRow];

	// emit signal
	emit(OnUserSelected(user));

	// close window
	close();
}

// emit OnCreateProtocol with user
void SelectUserWindow::OnCreateProtocolButtonClicked()
{
   QModelIndexList selectedRows = mTableWidget->selectionModel()->selectedRows();
   if (selectedRows.count() != 1)
      return;

   // get the array index from the mime data of the first item in the row
   const uint32 selectedRow = selectedRows.at(0).row();
   const User& user = mListedUsers[selectedRow];

   // emit signal
   emit(OnCreateProtocol(user));

   // close window
   close();
}

// invite user
void SelectUserWindow::InviteUser(const String& email)
{
	InviteUserWindow inviteUserWindow( this, email );
	int exitCode = inviteUserWindow.exec();

	switch (exitCode)
	{
		// success
		case QDialog::Accepted:
		{
			OnRefresh();
			break;
		}

		// create user
		case INVITEWINDOW_EXITCODE_CREATEUSER:
		{
			CreateUser( inviteUserWindow.GetEmail() );
			break;
		}

		// default
		default:
		{
			break;
		}
	}
}


// create new user
void SelectUserWindow::CreateUser(const String& email)
{
	CreateUserWindow createUserWindow( this, email );
	int exitCode = createUserWindow.exec();

	switch (exitCode)
	{
		// success
		case QDialog::Accepted:
		{
			OnRefresh();
			break;
		}

		// invite user
		case CREATEUSERWINDOW_EXITCODE_INVITEUSER:
		{
			InviteUser( createUserWindow.GetEmail() );
			break;
		}

		// default
		default:
		{
			break;
		}
	}
}


// emit cancel signal on button click
void SelectUserWindow::OnCancelButtonClicked()
{
	// close window
	close();
}


// called when refresh button got clicked
void SelectUserWindow::OnRefresh()
{
	mCurrentPage	= CORE_INVALIDINDEX32;
	mNumTotalPages	= CORE_INVALIDINDEX32;

	mListedUsers.Clear();
	mTableWidget->setRowCount(0);

	RequestNextPage(true);
}


// on scrolled
void SelectUserWindow::OnScrolled()
{
	QScrollBar* vScrollBar = mTableWidget->verticalScrollBar();

	int value		= vScrollBar->value();
	int min			= vScrollBar->minimum();
	int max			= vScrollBar->maximum();
	int pageStep	= vScrollBar->pageStep();
	
	if (value > max-pageStep)
		RequestNextPage();
}


// request the next page
void SelectUserWindow::RequestNextPage(bool force)
{
	// do the first request
	if (mCurrentPage == CORE_INVALIDINDEX32)
	{
		RequestPage( 0, force );
		return;
	}

	// request next page
	if (mCurrentPage < mNumTotalPages)
		RequestPage( mCurrentPage+1, force );
}


// request the next page of users
void SelectUserWindow::RequestPage(uint32 pageIndex, bool force)
{
	// stop timer
	mSearchTimer->stop();

	// avoid double calls
	if (mTableWidget->isEnabled() == false && force == false)
		return;

	mTableWidget->setEnabled(false);

	// pick a user rule filter (just patients by default, everyone for admin)
	// backend checks for "%RULE%", so "ROLE_Clinic" matches all roles with that prefix
	const char* RULE = GetUser()->FindRule("ROLE_ClinicAdmin") ? "ROLE_Clinic" : "ROLE_ClinicPatient";

	// construct /users/get request
	UsersGetRequest request( mUser.GetToken(), pageIndex, 100, mSearchEdit->GetText().toUtf8().data(), RULE);

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );

		// translate it to our data model
		UsersGetResponse response(networkReply);
		if (response.HasError() == true)
			return;

		mCurrentPage	= response.GetPageIndex();
		mNumTotalPages	= response.GetTotalPages();

		// process all users
		const uint32 numUsers = response.GetUsers().Size();
		for (uint32 i=0; i<numUsers; ++i)
		{
			const User& user = response.GetUsers()[i];
			AddUser(user);
		}

		// reset the refresh timer as we just updated the user list
		mLastRefreshTimer.Reset();

		// enable the table widget as we now filled it with data
		mTableWidget->setEnabled(true);
	});
}


void SelectUserWindow::AddUser(const User& user)
{
	// avoid duplicates in the list
	if (IsUserListed(user.GetId()) == true)
		return;

	// is this the parent user? show it or not, dependign on setting
	if (user.GetIdString().IsEqual(mUser.GetId()) == true)
		if (mShowSelf == false)
			return;

	// add the user
	const uint32 row = mTableWidget->rowCount();

	mTableWidget->insertRow(row);

	// Col 1: first+middle name
	mTempString = user.GetFirstName();
	if (user.GetMiddleNameString().IsEmpty() == false)
		mTempString.FormatAdd(" %s", user.GetMiddleName());	// add middle name

	QTableWidgetItem* firstItem = new QTableWidgetItem(mTempString.AsChar());
	firstItem->setFlags(firstItem->flags() ^ Qt::ItemIsEditable);
	mTableWidget->setItem( row, 0, firstItem );

	//  Col 2: last name
	QTableWidgetItem* secondItem = new QTableWidgetItem(user.GetLastName());
	secondItem->setFlags(secondItem->flags() ^ Qt::ItemIsEditable);
	mTableWidget->setItem( row, 1, secondItem );

	// tooltip: user Id
	//mTempString.Format("User ID: '%s'", user.GetId());
	//firstItem->setToolTip(mTempString.AsChar());
	//secondItem->setToolTip(mTempString.AsChar());

	//  Col 3: birthday name
	QTableWidgetItem* birthdayItem = new QTableWidgetItem(user.GetBirthday());
	birthdayItem->setFlags(secondItem->flags() ^ Qt::ItemIsEditable);
	mTableWidget->setItem( row, 2, birthdayItem);

	// add user to user array
	mListedUsers.Add(user);
}


// check if the user is already listed?
bool SelectUserWindow::IsUserListed(const char* userId) const
{
	const uint32 numUsers = mListedUsers.Size();
	for (uint32 i=0; i<numUsers; ++i)
	{
		const User& user = mListedUsers[i];
		if (user.GetIdString().IsEqual(userId) == true)
			return true;
	}

	// user with the given user id not in yet
	return false;
}


void SelectUserWindow::OnRefreshTimer()
{
	uint32 timeInSecs = (uint32)mLastRefreshTimer.GetTime().InSeconds();

	// first 10 seconds
	if (timeInSecs < 10)
		mTempString.Format( "Updated just now" );
	// below one minute
	else if (timeInSecs < 60)
	{
		uint32 timeInSecDecades = timeInSecs / 10;
		mTempString.Format( "Updated %i seconds ago", timeInSecDecades*10 );
	}
	// after one minute
	else
	{
		uint32 timeInMins = (uint32)mLastRefreshTimer.GetTime().InMinutes();

		if (timeInMins == 1)
			mTempString.Format( "Updated 1 minute ago" );
		else
			mTempString.Format( "Updated %i minutes ago", timeInMins );
	}

	mRefreshLabel->setText( mTempString.AsChar() );
}

// called when search text changes
void SelectUserWindow::OnSearchEdited(const QString& text)
{
   mSearchTimer->stop();
   mSearchTimer->start();
}

void SelectUserWindow::OnSearchCleared()
{
   mSearchTimer->stop();
   mSearchTimer->start();
}