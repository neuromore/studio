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
#include "CreateFolderWindow.h"

using namespace Core;

// constructor
CreateFolderWindow::CreateFolderWindow(const char* windowTitle, const char* defaultLabelText, Core::String  parentFolderId, QWidget* parent) : QDialog(parent)
{
	// update title of the about dialog
	setWindowTitle( windowTitle );

	// create the about dialog's layout
	QVBoxLayout* layout = new QVBoxLayout(this);

	// add the label
	mLineEdit = new QLineEdit(defaultLabelText);
	connect( mLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnTextChanged(const QString&)) );
	layout->addWidget(mLineEdit);

	// set the cursor to the beginning and select all
	mLineEdit->setCursorPosition(0);
	mLineEdit->selectAll();

	// add the cancel and cancel buttons
	QHBoxLayout* hlayout = new QHBoxLayout();
	mOkButton = new QPushButton("Ok");
	mCancelButton = new QPushButton("Cancel");
	hlayout->addWidget(mOkButton);
	hlayout->addWidget(mCancelButton);
	layout->addLayout(hlayout);

	connect( mOkButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( mCancelButton, SIGNAL(clicked()), this, SLOT(reject()) );

	OnTextChanged(defaultLabelText);

	setMinimumWidth( 300 );
}


// destructor
CreateFolderWindow::~CreateFolderWindow()
{
}


void CreateFolderWindow::OnTextChanged(const QString& newText)
{
	if (newText.isEmpty() == true || newText.indexOf('/') != -1 || newText.indexOf('\\') != -1)
	{
		QtBaseManager::SetWidgetAsInvalidInput( mLineEdit, false );
		mOkButton->setEnabled(false);
	}
	else
	{
		QtBaseManager::SetWidgetAsInvalidInput( mLineEdit, true );
		mOkButton->setEnabled(true);
	}
}
