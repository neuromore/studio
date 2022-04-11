/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "EnterLabelWindow.h"
#include "../QtBaseManager.h"
#include <QLabel>
#include <QSizePolicy>
#include <QPixmap>
#include <QVBoxLayout>
#include <QPushButton>


using namespace Core;

// constructor
EnterLabelWindow::EnterLabelWindow(const char* windowTitle, const char* defaultLabelText, const String& invalidChars, QWidget* parent) : QDialog(parent)
{
	// update title of the about dialog
	setWindowTitle( windowTitle );

	mInvalidChars = invalidChars;

	// create the about dialog's layout
	QVBoxLayout* layout = new QVBoxLayout(this);

	// add the label
	mLineEdit = new QLineEdit(defaultLabelText);
	connect( mLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnTextChanged(const QString&)) );
	layout->addWidget(mLineEdit);

	// add the cancel and cancel buttons
	QHBoxLayout* hlayout = new QHBoxLayout();
	mOkButton = new QPushButton("Ok");
	mCancelButton = new QPushButton("Cancel");
	hlayout->addWidget(mOkButton);
	hlayout->addWidget(mCancelButton);
	layout->addLayout(hlayout);

	connect( mOkButton, SIGNAL(clicked()), this, SLOT(OnOkButtonPressed()) );
	connect( mCancelButton, SIGNAL(clicked()), this, SLOT(OnCancelButtonPressed()) );

	setMinimumWidth( 300 );
	OnTextChanged(defaultLabelText);
}


// destructor
EnterLabelWindow::~EnterLabelWindow()
{
}


// 
void EnterLabelWindow::OnTextChanged(const QString& newText)
{
	bool containsInvalidChar = false;
	const uint32 numInvalidChars = mInvalidChars.GetLength();
	for (uint32 i=0; i<numInvalidChars; ++i)
	{
		char invalidChar = mInvalidChars[i];

		if (newText.indexOf(invalidChar) != -1)
		{
			containsInvalidChar = true;
			break;
		}
	}

	if (newText.isEmpty() == true || containsInvalidChar == true)
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


void EnterLabelWindow::OnOkButtonPressed()
{
	accept();
}


void EnterLabelWindow::OnCancelButtonPressed()
{
	reject();
}
