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
#include "LicenseAgreementWindow.h"
#include <Backend/UsersAgreementRequest.h>
#include <Backend/UsersAgreementResponse.h>

#ifdef NEUROMORE_BRANDING_ANT
#define ABOUTIMAGE ":/Images/About-ANT.png"
#else
#define ABOUTIMAGE ":/Images/About-neuromore.png"
#endif

using namespace Core;

// constructor
LicenseAgreementWindow::LicenseAgreementWindow(bool showAgreeAndCancelButtons, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
	setWindowTitle("Licenses");

	// create the about dialog's layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	setLayout(mainLayout);

	// load the about dialog image and add it to the dialog
	QPixmap aboutImage(ABOUTIMAGE);
	const uint32 imageWidth = aboutImage.width();
	QLabel* aboutImageLabel = new QLabel(this);
	aboutImageLabel->setPixmap(aboutImage);
	mainLayout->addWidget( aboutImageLabel, 0, Qt::AlignTop );


	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	mainLayout->addLayout( layout );


	// add tab bar
	mTabBar = new QTabBar(this);
	connect(mTabBar, SIGNAL(currentChanged(int)), this, SLOT(OnChangeCategoryTab(int)));
	layout->addWidget( mTabBar );


	// text edit
	mTextEdit = new QTextEdit(this);
	mTextEdit->append("Loading license ...");
	mTextEdit->setMinimumHeight( 400 );
	layout->addWidget( mTextEdit );

	// agree & cancel buttons
	if (showAgreeAndCancelButtons)
	{
		QHBoxLayout* buttonsLayout = new QHBoxLayout();
		buttonsLayout->setMargin(5);
		layout->addLayout(buttonsLayout);

		// license i agree button
		mAgreeLicenseButton = new QPushButton( "I Agree", this );
		//mAgreeLicenseButton->setEnabled(false);
		mAgreeLicenseButton->setMinimumWidth( imageWidth / 4 );
		mAgreeLicenseButton->setMaximumWidth( imageWidth / 4 );
		buttonsLayout->addWidget( mAgreeLicenseButton );

		connect( mAgreeLicenseButton, SIGNAL(clicked()), this, SLOT(OnAgreedLicense()) );

		// cancel button
		QPushButton* cancelButton = new QPushButton( "Cancel", this );
		cancelButton->setMinimumWidth( imageWidth / 4 );
		cancelButton->setMaximumWidth( imageWidth / 4 );
		buttonsLayout->addWidget( cancelButton );

		connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
	}
	else
	{
		mAgreeLicenseButton = NULL;
	}

	// avoid resizing
	setSizeGripEnabled(false);
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

#if defined(NEUROMORE_BRANDING_ANT)
	AddLicense("License Terms", GetManager()->GetLicenseUrl());
	AddLicense("Privacy Policy", GetManager()->GetPrivacyPolicyUrl());
#else
	AddLicense("Licensing", GetManager()->GetLicenseUrl());
	AddLicense("Terms and Conditions", GetManager()->GetCloudTermsUrl());
	AddLicense("Privacy Policy", GetManager()->GetPrivacyPolicyUrl());
#endif

	// position window in the screen center
	GetQtBaseManager()->CenterToScreen(this);
}


// destructor
LicenseAgreementWindow::~LicenseAgreementWindow()
{
}


void LicenseAgreementWindow::AddLicense(const char* name, const char* url)
{
	LicenseAgreement* license = new LicenseAgreement(QUrl(url), this);
	connect( license, &LicenseAgreement::TextChanged, this, &LicenseAgreementWindow::OnTextChanged );

	mLicenses.Add( license );
	mTabBar->addTab(name);
}


void LicenseAgreementWindow::SetText(const char* text)
{
	mTextEdit->clear();
	mTextEdit->append( text );
	
	// set the cursor to the beginning
	QTextCursor textCursor = mTextEdit->textCursor();
	textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	mTextEdit->setTextCursor( textCursor );

	mTextEdit->setReadOnly(true);
	mTextEdit->setStyleSheet("color: rgb(0,159,227);");
}


void LicenseAgreementWindow::OnTextChanged()
{
	// get the sender license
	LicenseAgreement* senderLicense = qobject_cast<LicenseAgreement*>( sender() );
	
	// get the currently visible license
	int tabIndex = mTabBar->currentIndex();
	if (tabIndex < 0)
		return;
	LicenseAgreement* currentLicense = mLicenses[tabIndex];
	
	// return directly if we're viewing a different license
	if (senderLicense != currentLicense)
		return;

	String text = currentLicense->GetText();
	SetText(text.AsChar());
}


void LicenseAgreementWindow::OnChangeCategoryTab(int index)
{
	LicenseAgreement* license = mLicenses[index];
	SetText( license->GetText() );
}


void LicenseAgreementWindow::OnAgreedLicense()
{
	// construct /users/agreement request
	UsersAgreementRequest request( GetUser()->GetToken(), GetUser()->GetId(), "license_agreement" );

	// process request and connect to the reply
	QNetworkReply* reply = GetBackendInterface()->GetNetworkAccessManager()->ProcessRequest( request );

	// lambda slot for processing reply
	connect(reply, &QNetworkReply::finished, this, [reply, this]()
	{
		// translate it to our data model
		UsersAgreementResponse response(reply);
		if (response.HasError() == true)
		{
			// TODO error message
			return;
		}

		// accept and close the window
		accept();
	});
}
