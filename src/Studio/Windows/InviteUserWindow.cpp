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
#include "InviteUserWindow.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <Backend/UsersInviteRequest.h>
#include <Backend/UsersInviteResponse.h>
#include <QMessageBox>
#include <QVBoxLayout>


using namespace Core;

// constructor
InviteUserWindow::InviteUserWindow(QWidget* parent, const Core::String& email) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	// set the window title
	setWindowTitle( "Invite Existing User" );
	setWindowIcon( GetQtBaseManager()->FindIcon("Images/Icons/Users.png") );
	setModal(true);
	setWindowModality( Qt::ApplicationModal );

	// add the main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	// helper
	mainLayout->addWidget( new QLabel("Enter the email address of the existing neuromore user you want to invite (Example: john.doe@gmail.com).\nThe user will receive an invitation email. After accepting the invitation, you will be informed by mail and see the user in your list.") );

	// add line edit
	mEmailEdit = new QLineEdit( email.AsChar(), this );
	mainLayout->addWidget(mEmailEdit);
	connect( mEmailEdit, &QLineEdit::textEdited, this, &InviteUserWindow::OnTextEdited );

	// add email validator to line edit
	mValidator = new EmailValidator( false, mEmailEdit );
	//mLineEdit->setValidator(mValidator);


	// vertical bottom button row
	QHBoxLayout* hLayout = new QHBoxLayout();
	
	// select user button
	mInviteButton = new QPushButton();
	hLayout->addWidget(mInviteButton);
	connect( mInviteButton, &QPushButton::clicked, this, &InviteUserWindow::OnInviteButtonClicked);

	// cancel button
	QPushButton* cancelButton = new QPushButton("Cancel");
	hLayout->addWidget(cancelButton );
	connect( cancelButton, &QPushButton::clicked, this, &InviteUserWindow::OnCancelButtonClicked);

	mainLayout->addLayout(hLayout);

	show();

	OnTextEdited();

	// don't allow resizing the window
	setSizeGripEnabled(false);
	setFixedSize( width(), height() );

	// automatically start invitation in case the email was preset already
	//if (email.IsEmpty() == false)
	//	OnInviteButtonClicked();
}


// destructor
InviteUserWindow::~InviteUserWindow()
{
}


// called whenever the line edit text got changed
void InviteUserWindow::OnTextEdited()
{
	bool inviteButtonEnabled = true;
	String buttonText;
	QString text = mEmailEdit->text();

	int pos;
	QValidator::State state = mValidator->validate(text, pos);

	if (state == QValidator::Acceptable)
	{
		inviteButtonEnabled = true;
		QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, true );
	}
	else if (state == QValidator::State::Intermediate)
	{
		buttonText = "Not a valid email";
		inviteButtonEnabled = false;
		QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, true );
	}
	else
	{
		buttonText = "Not a valid email";
		inviteButtonEnabled = false;
		QtBaseManager::SetWidgetAsInvalidInput( mEmailEdit, false );
	}

	mInviteButton->setEnabled( inviteButtonEnabled );

	if (inviteButtonEnabled == true)
	{
		mInviteButton->setText( "Send Invitation Email" );
		mInviteButton->setIcon( GetQtBaseManager()->FindIcon("Images/Icons/SendFeedback.png") );
	}
	else
	{
		mInviteButton->setText( buttonText.AsChar() );
		mInviteButton->setIcon( QIcon() );
	}
}


void InviteUserWindow::ShowSuccessDialog(QWidget* parent, const char* email)
{
	String title;
	String msg;
	//const uint32 numEmails = emails.Size();
	//if (numEmails == 1)
	{
		// title
		title = "User Invited";

		// message
		msg.Format( "We've succeffully sent an invitation email to '%s'. ", email );
		msg += "As soon as the invitation got accepted, you will receive an email.\n\nPlease click the OK button after the user accepted your invitation or press the refresh button to update your user list afterwards.";
	}
	/*else
	{
		// title
		title.Format("%ix Users Invited", numEmails);

		// message
		msg.Format( "We've succeffully sent %ix invitation emails.\n", numEmails );
		msg += "As soon as the invitation got accepted, you will receive an email. Please click the refresh button to update your user list afterwards.\n\n";

		for (uint32 i=0; i<numEmails; ++i)
		{
			msg += emails[i];

			if (i != numEmails-1)
				msg += "\n";
		}
	}*/
		
	QMessageBox::information( parent, title.AsChar(), msg.AsChar() );
}


// invite button clicked
void InviteUserWindow::OnInviteButtonClicked()
{
	setEnabled(false);

	// 1. construct invite request
	String email = mEmailEdit->text().toUtf8().data();
	UsersInviteRequest request( GetUser()->GetToken(), GetUser()->GetId(), email.AsChar() );

	// 2. process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request, Request::UIMODE_SILENT );
	connect(reply, &QNetworkReply::finished, this, [reply, this, email]()
	{
		// 3. construct and parse response
		QNetworkReply* networkReply = qobject_cast<QNetworkReply*>( sender() );
		UsersInviteResponse response(networkReply);

		// 4. success handling
		if (response.HasError() == false)
		{
			ShowSuccessDialog( this, email.AsChar() );
			accept();
		}
		// 5. error handling
		else if	(response.HasErrorOfType("INVITED_USER_HAS_NO_ACCOUNT") == true)
		{
			String msg;
			msg.Format( "The user with the given email doesn't have a neuromore account yet. Do you want to create an account for '%s'?", email.AsChar() );
			if (QMessageBox::information(this, "No Account Found", msg.AsChar(), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
				done(INVITEWINDOW_EXITCODE_CREATEUSER);
			else
				reject();
		}
		else if	(response.HasErrorOfType("INVITED_USER_ALREADY_ADDED") == true)
		{
			QMessageBox::information(this, "User Already Added", response.GetErrorMessage(), QMessageBox::Ok);
			reject();
		}
		// unknown error
		else
		{
			QMessageBox::warning( this, "Error", response.GetErrorMessage(), QMessageBox::Ok );
			setEnabled(true);
			return;
		}
	});
}


// emit cancel signal on button click
void InviteUserWindow::OnCancelButtonClicked()
{
	// close window
	close();
}
