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
#include "UpgradeWindow.h"

using namespace Core;

// constructor
UpgradeWindow::UpgradeWindow(QWidget* parent, const char* upgradeReason) : QDialog(parent)
{
	// set the window title
	setWindowTitle( "Upgrade Needed" );

	setModal(true);
	setWindowModality( Qt::ApplicationModal );

	// add the main vertical layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	setLayout(mainLayout);

	// load the about dialog image and add it to the dialog
	QPixmap aboutImage( ":/Images/About.png" );
	const uint32 imageWidth = aboutImage.width();
	QLabel* aboutImageLabel = new QLabel(this);
	aboutImageLabel->setPixmap(aboutImage);
	mainLayout->addWidget( aboutImageLabel, 0, Qt::AlignTop );

	QLabel* upgradeReasonLabel = new QLabel(upgradeReason);
	upgradeReasonLabel->setAlignment(Qt::AlignCenter);
	//upgradeReasonLabel->setWordWrap(true);
	mainLayout->addWidget( upgradeReasonLabel, 0, Qt::AlignCenter );

	// button layout
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setMargin( 15 );
	mainLayout->addLayout(buttonLayout);

	// upgrade button
	mUpgrade = new QPushButton( "See Subscription Plans" );
	mUpgrade->setMinimumWidth( imageWidth / 4 );
	mUpgrade->setMaximumWidth( imageWidth / 4 );
	buttonLayout->addWidget( mUpgrade, 0, Qt::AlignHCenter );
	connect(mUpgrade, SIGNAL(clicked()), this, SLOT(OnUpgrade()));

	// cancel button
	mCancel = new QPushButton( "Cancel" );
	mCancel->setMinimumWidth( imageWidth / 4 );
	mCancel->setMaximumWidth( imageWidth / 4 );
	buttonLayout->addWidget( mCancel, 0, Qt::AlignHCenter );
	connect(mCancel, SIGNAL(clicked()), this, SLOT(OnCancel()));

	// avoid resizing
	setSizeGripEnabled(false);
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	setFixedSize( sizeHint() );

	show();
}


// destructor
UpgradeWindow::~UpgradeWindow()
{
}


void UpgradeWindow::OnUpgrade()
{
	QUrl url( GetManager()->GetStoreUrl() );
	QDesktopServices::openUrl(url);

	accept();
}


void UpgradeWindow::OnCancel()
{
	reject();
}
