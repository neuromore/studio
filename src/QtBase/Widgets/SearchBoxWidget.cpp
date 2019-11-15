/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "SearchBoxWidget.h"
#include "../QtBaseManager.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>


using namespace Core;

// constructor
SearchBoxWidget::SearchBoxWidget(QWidget* parent) : QWidget(parent)
{
	mLayout = new QHBoxLayout(this);
	mLayout->setMargin(0);
	mLayout->setSpacing(0);

	// search icon button
	QPushButton* searchIconButton = new QPushButton("", this);
	searchIconButton->setObjectName("SearchButton");
	mLayout->addWidget( searchIconButton );

	// edit field
	mSearchEdit = new SearchBoxEdit(this);
	mLayout->addWidget( mSearchEdit );
	setFocusProxy( mSearchEdit ); 

	// clear button
	QPushButton* clearButton = new QPushButton("", this);
	clearButton->setObjectName("SearchClearButton");
	mLayout->addWidget( clearButton );

	connect( mSearchEdit, SIGNAL(textChanged(const QString&)), this, SLOT(OnTextChanged(const QString&)) );
	connect( clearButton, SIGNAL(pressed()), this, SLOT(ClearSearch()) );

	ClearSearch();
}


// destructor
SearchBoxWidget::~SearchBoxWidget()
{
}


const QString& SearchBoxWidget::GetText()
{
	return mText;
}


void SearchBoxWidget::OnTextChanged(const QString & text)
{
	mText = text;
	emit TextChanged(text);
}


void SearchBoxWidget::ClearSearch()
{
	mSearchEdit->Reset();
	mText.clear();
	emit TextCleared();
}


