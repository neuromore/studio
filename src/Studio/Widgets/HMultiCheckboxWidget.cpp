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
#include "HMultiCheckboxWidget.h"
#include <Core/LogManager.h>
#include <QScrollArea>


using namespace Core;

// constructor
HMultiCheckboxWidget::HMultiCheckboxWidget(QWidget* parent) : QWidget(parent)
{
	mHighlightedIndex	= CORE_INVALIDINDEX32;
	mMainWidget			= NULL;
	mCheckboxAll		= NULL;

	// create the main layout
	mLayout = new QVBoxLayout();
	mLayout->setMargin(0);
	mLayout->setSpacing(0);
	setLayout(mLayout);
}


// destructor
HMultiCheckboxWidget::~HMultiCheckboxWidget()
{
}


// initialize for a new neuro headset
void HMultiCheckboxWidget::ReInit(const Array<String>& names, const Array<String>& tooltips, const Array<Color>& colors, const char* allCheckboxName)
{
	// put the delete of the main widget into the message queue and reset it
	if (mMainWidget != NULL)
		mMainWidget->deleteLater();
	mMainWidget = NULL;

	// create a new main widget
	mMainWidget = new MainWidget(this);

	// get the number of checkboxes to create
	const uint32 numCheckboxes = names.Size();
	CORE_ASSERT( numCheckboxes == tooltips.Size() && numCheckboxes == colors.Size() );

	// layout for the checkboxes
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setMargin(3);
	hLayout->setSpacing(10);
	hLayout->setAlignment( Qt::AlignLeft );

	// if there are more than one checkbox, add the "All" checkboxes on the left
	if (numCheckboxes > 1)
	{
		// all checkbox
		mCheckboxAll = new QCheckBox( allCheckboxName );
		mCheckboxAll->setToolTip( "Enable/disable all with a single click" );
		mCheckboxAll->setChecked( true );
		hLayout->addWidget( mCheckboxAll );
		connect( mCheckboxAll, SIGNAL(stateChanged(int)), this, SLOT(OnAllCheckbox(int)) );
	} else
	{
		mCheckboxAll = NULL;
	}

	// prepare the checkbox array
	mCheckboxes.Resize( numCheckboxes );

	// iterate through the available checkboxes
	Core::String widgetColorText;
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		// create the style sheet used to set the checkbox's color
		widgetColorText.Format( "QCheckBox { color: rgb(%.0f, %.0f, %.0f) }", colors[i].r*255.0f, colors[i].g*255.0f, colors[i].b*255.0f );

		// create the widget and add it to the layout
		mCheckboxes[i] = new QCheckBox( names[i].AsChar() );
		mCheckboxes[i]->setStyleSheet( widgetColorText.AsChar() );
		mCheckboxes[i]->setToolTip( tooltips[i].AsChar() );
		mCheckboxes[i]->setChecked( true );

		// connect and add it
		connect( mCheckboxes[i], SIGNAL(stateChanged(int)), this, SLOT(OnCheckbox(int)) );
		hLayout->addWidget( mCheckboxes[i] );
	}

	mMainWidget->setLayout(hLayout);
	mMainWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// at the very end, add the new widget to the layout
	mLayout->addWidget(mMainWidget);

	/////////////////////////////////////////////////////////////////////////////
	// scrollable??
	/*QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setAlignment(Qt::AlignLeft);
	scrollArea->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	scrollArea->setWidget(mMainWidget);
	mLayout->addWidget(scrollArea);*/
}


// get the number of checked checkboxes
uint32 HMultiCheckboxWidget::CalcNumChecked() const
{
	uint32 result = 0;

	// get the number of checkboxes and iterate through them
	const uint32 numCheckboxes = mCheckboxes.Size();
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		if (mCheckboxes[i]->isChecked() == true)
			result++;
	}

	return result;
}


void HMultiCheckboxWidget::SetChecked(uint32 index, bool checked)
{
	const uint32 numCheckboxes = mCheckboxes.Size();
	if (index >= numCheckboxes)
		return;

	mCheckboxes[index]->setChecked(checked);
}


// check the given number of checkboxes, uncheck the other ones
void HMultiCheckboxWidget::CheckXCheckboxes(uint32 numChecked)
{
	uint32 counter = 0;
	
	this->blockSignals(true);

	// get the number of checkboxes and iterate through them
	const uint32 numCheckboxes = mCheckboxes.Size();
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		if (counter < numChecked)
		{
			mCheckboxes[i]->setChecked(true);
			counter++;
		}
		else
			mCheckboxes[i]->setChecked(false);
	}
	this->blockSignals(false);

	emit SelectionChanged();
}


// called when the "All" checkbox got clicked
void HMultiCheckboxWidget::OnAllCheckbox(int state)
{
	bool enable = false;
	if (state == Qt::Checked)
		enable = true;

	bool changed = false;

	this->blockSignals(true);

	// get the number of checkboxes and iterate through them
	const uint32 numCheckboxes = mCheckboxes.Size();
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		if (mCheckboxes[i]->isChecked() != enable)
		{
			mCheckboxes[i]->setChecked(enable);
			changed = true;
		}
	}
	this->blockSignals(false);

	// emit signal if at least one box was changed
	if (changed)
		emit SelectionChanged();
}


// called when any of the other checkboxes got clicked
void HMultiCheckboxWidget::OnCheckbox(int state)
{
	if (mCheckboxAll != NULL)
	{
		mCheckboxAll->blockSignals(true);

		const uint32 numCheckboxes = mCheckboxes.Size();
		if (CalcNumChecked() == numCheckboxes)
			mCheckboxAll->setChecked(true);
		else
			mCheckboxAll->setChecked(false);

		mCheckboxAll->blockSignals(false);
	}

	emit SelectionChanged();
}


// called when the mouse moved
void HMultiCheckboxWidget::mouseMoveEvent(QMouseEvent* event)
{
	// check if any of the checkboxes is on mouse overed
	const uint32 numCheckboxes = mCheckboxes.Size();
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		if (mCheckboxes[i]->underMouse() == true)
		{
			if (i != mHighlightedIndex)
			{
				mHighlightedIndex = i;
				emit HighlightedIndexChanged(mHighlightedIndex);
			}
			return;
		}
	}

	// if no checkbox is hovered
	if (mHighlightedIndex != CORE_INVALIDINDEX32)
	{
		mHighlightedIndex = CORE_INVALIDINDEX32;
	
		// signal that a previously highlighted checkbox is no longer highlighted
		emit HighlightedIndexChanged(-1);
	}
}


// get the highlighted checkbox index
uint32 HMultiCheckboxWidget::GetHighlightedIndex() const
{
	// get the current local cursor pos
	QPoint localCursorPos = mapFromGlobal( QCursor::pos() );

	// make sure no checkbox is highlighted in case the mouse is not inside this widget at all
	if (localCursorPos.x() < 0 || localCursorPos.x() > width() ||
		localCursorPos.y() < 0 || localCursorPos.y() > height())
		return CORE_INVALIDINDEX32;

	return mHighlightedIndex;
}
