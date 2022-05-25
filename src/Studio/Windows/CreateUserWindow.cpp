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
#include "CreateUserWindow.h"
#include "InviteUserWindow.h"
#include <Backend/UsersCreateRequest.h>
#include <Backend/UsersCreateResponse.h>
#include <Backend/UsersInviteRequest.h>
#include <Backend/UsersInviteResponse.h>

using namespace Core;

// constructor
CreateUserWindow::CreateUserWindow(QWidget* parent, const Core::String& email) : 
   QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
   mValidatorBirthday(QRegExp("[0-9]{4}-[0-9]{2}-[0-9]{2}"))
{
	// set the window title
	setWindowTitle( "Create User" );
	setWindowIcon( GetQtBaseManager()->FindIcon("Images/Icons/Users.png") );
	setMinimumWidth(400);
	setModal(true);
	setWindowModality( Qt::ApplicationModal );


	// add the main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);


	// helper
	if (brandingName != AntBrandingName) {
		mainLayout->addWidget( new QLabel("When providing an email address, a personal neuromore account will be created for the user.") );
	}
	// spacing widget
	mainLayout->addWidget( new QLabel("", this) );


	// name layout
	QGridLayout* nameLayout = new QGridLayout();
	nameLayout->setVerticalSpacing(0);
	mainLayout->addLayout(nameLayout);

	// first name
	QLabel* firstNameLabel = new QLabel( "First Name", this );
	nameLayout->addWidget( firstNameLabel, 0, 0 );

	mFirstNameEdit = new QLineEdit(this);
	nameLayout->addWidget( mFirstNameEdit, 1, 0 );
	connect( mFirstNameEdit, &QLineEdit::textEdited, this, &CreateUserWindow::OnFirstNameEdited );

	// mid name
	/*QLabel* midNameLabel = new QLabel( "Mid Name", this );
	nameLayout->addWidget( midNameLabel, 0, 1 );

	mMidNameEdit = new QLineEdit(this);
	nameLayout->addWidget( mMidNameEdit, 1, 1 );
	//connect( mMidNameEdit, &QLineEdit::textEdited, this, &CreateUserWindow::OnMidNameEdited );*/

	// last name
	QLabel* lastNameLabel = new QLabel( "Last Name", this );
	nameLayout->addWidget( lastNameLabel, 0, 2 );

	mLastNameEdit = new QLineEdit(this);
	nameLayout->addWidget( mLastNameEdit, 1, 2 );
	connect( mLastNameEdit, &QLineEdit::textEdited, this, &CreateUserWindow::OnLastNameEdited );


	// email layout
	QGridLayout* emailLayout = new QGridLayout();
	emailLayout->setVerticalSpacing(0);
	mainLayout->addLayout(emailLayout);

	// email
	QLabel* emailLabel = new QLabel( "Email (optional)", this );
	emailLayout->addWidget( emailLabel, 0, 0 );

	mEmailEdit = new QLineEdit( email.AsChar(), this );
	emailLayout->addWidget( mEmailEdit, 1, 0 );
	connect( mEmailEdit, &QLineEdit::textEdited, this, &CreateUserWindow::OnEmailEdited );

	// add email validator to line edit
	mValidatorEmail = new EmailValidator( true, mEmailEdit );
	//mLineEdit->setValidator(mValidator);

	// birthday layout
	QGridLayout* birthdayLayout = new QGridLayout();
	birthdayLayout->setVerticalSpacing(0);
	mainLayout->addLayout(birthdayLayout);

	// birthday
	QLabel* birthdayLabel = new QLabel("Birthday (optional, YYYY-MM-DD)", this);
	birthdayLayout->addWidget(birthdayLabel, 0, 0);

	mBirthdayEdit = new QLineEdit(this);
	birthdayLayout->addWidget(mBirthdayEdit, 1, 0);
	connect(mBirthdayEdit, &QLineEdit::textEdited, this, &CreateUserWindow::OnBirthdayEdited);

	// add birthday validator to line edit
	mBirthdayEdit->setValidator(&mValidatorBirthday);

	// spacing widget
	mainLayout->addWidget( new QLabel("", this) );


	// vertical bottom button row
	QHBoxLayout* hLayout = new QHBoxLayout();
	
	// create user button
	mCreateButton = new QPushButton(this);
	mCreateButton->setEnabled(false);
	hLayout->addWidget(mCreateButton);
	connect( mCreateButton, &QPushButton::clicked, this, &CreateUserWindow::OnCreateButtonClicked);

	// cancel button
	QPushButton* cancelButton = new QPushButton( "Cancel", this );
	hLayout->addWidget(cancelButton );
	connect( cancelButton, &QPushButton::clicked, this, &CreateUserWindow::OnCancelButtonClicked);

	mainLayout->addLayout(hLayout);

	// set tab order
	setTabOrder( mFirstNameEdit, mLastNameEdit );
	setTabOrder( mLastNameEdit, mEmailEdit );
	setTabOrder( mEmailEdit, mCreateButton );
	setTabOrder( mCreateButton, cancelButton );

	UpdateInterface();
	show();

	// don't allow resizing the window
	setSizeGripEnabled(false);
	setFixedSize( width(), height() );
}


// destructor
CreateUserWindow::~CreateUserWindow()
{
}


// called whenever any of the line edits got edited
void CreateUserWindow::UpdateInterface()
{
	bool createButtonEnabled = true;
	String buttonText;

	QString emailString = mEmailEdit->text();

	bool validEmail = false;
	if (emailString.isEmpty() == false)
	{
		int pos;
		QValidator::State state = mValidatorEmail->validate(emailString, pos);

		if (state == QValidator::Acceptable)
		{
			validEmail = true;
			QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, true );
		}
		else if (state == QValidator::State::Intermediate)
		{
			createButtonEnabled = false;
			buttonText = "Not a valid email";
			QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, true );
		}
		else
		{
			createButtonEnabled = false;
			buttonText = "Not a valid email";
			QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, false );
		}
	}

	// last name
	if (mLastNameEdit->text().isEmpty() == true)
	{
		createButtonEnabled = false;
		buttonText = "Last name is empty";
	}

	// first name
	if (mFirstNameEdit->text().isEmpty() == true)
	{
		createButtonEnabled = false;
		buttonText = "First name is empty";
	}

	// birthday
	if (!mBirthdayEdit->text().isEmpty())
	{
		QDate tmpDate = QDate::fromString(mBirthdayEdit->text(), "yyyy-MM-dd");
		if (!tmpDate.isValid())
		{
			createButtonEnabled = false;
			buttonText = "Birthday is invalid";
 		}
	}

	mCreateButton->setEnabled( createButtonEnabled );

	if (createButtonEnabled == true)
	{
		if (validEmail == true)
			mCreateButton->setText( "Create User + Send Email" );
		else
			mCreateButton->setText( "Create User" );

		mCreateButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/Plus.png") );
	}
	else
	{
		mCreateButton->setText( buttonText.AsChar() );
		mCreateButton->setIcon( QIcon() );
	}
}


// create user button clicked
void CreateUserWindow::OnCreateButtonClicked()
{
	setEnabled(false);

	String firstName	= mFirstNameEdit->text().toUtf8().data();
	String lastName		= mLastNameEdit->text().toUtf8().data();
	String email		= mEmailEdit->text().toUtf8().data();
	String birthday		= mBirthdayEdit->text().toUtf8().data();

	Array<String> parentIds;
	const uint32 numCompanies = GetUser()->GetNumParentCompanyIds();
	for (uint32 i = 0; i < numCompanies; i++)
		parentIds.Add( GetUser()->GetParentCompanyId(i) );

	// 1. construct invite request
	UsersCreateRequest request( GetUser()->GetToken(), email, firstName, lastName, birthday, parentIds, 3004 );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
	connect(reply, &QNetworkReply::finished, this, [reply, this, email]()
	{
		// 3. construct and parse response
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
		UsersCreateResponse response(networkReply);

		// 4. success handling
		if (response.HasError() == false)
		{
			// user created successfully, close the window
			accept();
		}
		// 5. error handling
		else if	(response.HasErrorOfType("CREATE_USER_FAILED") == true)
		{
         // TODO: This is hacky, not every respone is a fail due to an existing user
			String msg;
			msg.Format( "The user with the given email already has a neuromore account. Do you want to invite '%s'?", email.AsChar() );
			if (QMessageBox::information(this, "User Already Exists", msg.AsChar(), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
				done(CREATEUSERWINDOW_EXITCODE_INVITEUSER);
			else
				reject();
		}
		// unknown error
		else
		{
			response.ShowErrorDialog(this);
			reject();
			return;
		}
	});
}


// emit cancel signal on button click
void CreateUserWindow::OnCancelButtonClicked()
{
	// close window
	close();
}
