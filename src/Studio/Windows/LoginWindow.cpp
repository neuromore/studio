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

// include the required headers
#include "LoginWindow.h"
#include "../AppManager.h"
#include <Core/LogManager.h>
#include <QtBaseManager.h>
#include <QLabel>
#include <QSizePolicy>
#include <QPixmap>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDesktopServices>

using namespace Core;

// constructor
LoginWindow::LoginWindow(QWidget* parent, const char* title, const char* firstLineEdit, const char* firstLineEditText, const char* secondLineEdit, const char* secondLineEditText, const char* buttonText, bool passwordHidden) : QDialog(parent, Qt::Tool)
{
	mRememberMe = NULL;

	// set the window title
	setWindowTitle( title );

	setModal(true);
	setWindowModality( Qt::ApplicationModal );

	// add the main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);


	// load the about dialog image and add it to the dialog
	QPixmap aboutImage(GetManager()->GetLoginImageName());
	const uint32 imageWidth = aboutImage.width();
	QLabel* aboutImageLabel = new QLabel(this);
	aboutImageLabel->setPixmap(aboutImage);
	mainLayout->addWidget( aboutImageLabel, 0, Qt::AlignTop );
	

	// grouping widget contains login fields and the login button
	mGroupingWidget = new QWidget();
	mGroupingWidget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
	mainLayout->addWidget(mGroupingWidget);

	// create the about dialog's layout
	QVBoxLayout* innerLayout = new QVBoxLayout();
	innerLayout->setMargin( 15 );
	mGroupingWidget->setLayout(innerLayout);
	
	QGridLayout* loginLayout = new QGridLayout();
	innerLayout->addLayout(loginLayout);

	// user name
	loginLayout->addWidget( new QLabel(firstLineEdit), 0, 0 );
	mUserId = new QLineEdit();
	mUserId->setText( firstLineEditText );
	loginLayout->addWidget( mUserId, 0, 1 );
	connect( mUserId, &QLineEdit::textEdited, this, &LoginWindow::OnUserIdEdited );

	// password
	loginLayout->addWidget( new QLabel(secondLineEdit), 1, 0 );
	mPassword = new QLineEdit();

	if (passwordHidden == true)
		mPassword->setEchoMode(QLineEdit::Password);

	mPassword->setText( secondLineEditText );

	loginLayout->addWidget( mPassword, 1, 1 );
	connect( mPassword, &QLineEdit::textEdited, this, &LoginWindow::OnPasswordEdited );

	const char* forgotPasswordText = "Forgot password?";

	// remember me checkbox and forgot password
	loginLayout->addWidget( new QLabel("Remember Me"), 2, 0 );

	QHBoxLayout* hLayout = new QHBoxLayout();

	mRememberMe = new QCheckBox();

	mRememberMe->setChecked(GetManager()->IsLoginRemberMePrechecked());

	// forgot password
	mForgotPassword = new LinkWidget(forgotPasswordText);
	connect(mForgotPassword, &LinkWidget::Clicked, this, []()
	{
		QUrl url( GetManager()->GetForgotPasswordUrl() );
		QDesktopServices::openUrl(url);
	});

	// add toolbar spacer widget
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	hLayout->addWidget( mRememberMe );
	hLayout->addWidget( spacerWidget );
	hLayout->addWidget( mForgotPassword );

	loginLayout->addLayout( hLayout, 2, 1 );


	// login button
	mLogin = new QPushButton( buttonText );
	mLogin->setMinimumWidth( imageWidth / 4 );
	mLogin->setMaximumWidth( imageWidth / 4 );
	innerLayout->addWidget( mLogin, 0, Qt::AlignHCenter );
	//spacingLayout->addWidget( mLogin );
	connect(mLogin, SIGNAL(clicked()), this, SLOT(OnLogin()));

	// avoid resizing
	setSizeGripEnabled(false);
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	setFixedSize( sizeHint() );

	mLogin->setEnabled( false );
	if (mUserId->text().isEmpty() == false && mPassword->text().isEmpty() == false)
			mLogin->setEnabled( true );

	// set tab order
	setTabOrder( mUserId, mPassword );
	setTabOrder( mPassword, mLogin );

	show();
	activateWindow();
	raise();
	setFocus();
	mUserId->setFocus();
}


// destructor
LoginWindow::~LoginWindow()
{
}


void LoginWindow::OnLogin()
{
	mGroupingWidget->setVisible(false);
	emit Login();
	accept();
}


void LoginWindow::OnUserIdEdited(const QString& text)
{
	mLogin->setEnabled( false );
	if (text.isEmpty() == false && mPassword->text().isEmpty() == false)
		mLogin->setEnabled( true );
}


void LoginWindow::OnPasswordEdited(const QString& text)
{
	mLogin->setEnabled( false );
	if (text.isEmpty() == false && mUserId->text().isEmpty() == false)
		mLogin->setEnabled( true );
}
