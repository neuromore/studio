/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "LayoutComboBox.h"
#include "QtBaseManager.h"


// constructor
LayoutComboBox::LayoutComboBox(QWidget* parent) : QComboBox(parent)
{
	// reset the application mode selection and connect it
	setCurrentIndex( -1 );
	connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentIndexChanged(int)) );
}


// destructor
LayoutComboBox::~LayoutComboBox()
{
}


void LayoutComboBox::ReInit()
{
	LayoutManager* layoutManager = GetQtBaseManager()->GetLayoutManager();

	// remember the currently selected layout text
	QString	selectedLayoutText = currentText();

	// remove all layouts
	blockSignals(true);
	clear();

	// get the number of layouts and iterate through them
	const uint32 numLayouts = layoutManager->GetNumLayouts();
	for (uint32 i=0; i<numLayouts; ++i)
	{
		Layout* layout = layoutManager->GetLayout(i);
		addItem( layout->GetName() );

		switch (layout->GetType())
		{
			case Layout::LOCAL:
			{
				setItemData( i, LayoutManager::GetHddColor(), Qt::BackgroundRole );
				setItemData( i, LayoutManager::GetHddIcon(), Qt::DecorationRole );
				break;
			}

			case Layout::BACKEND:
			{
				setItemData( i, LayoutManager::GetCloudColor(), Qt::BackgroundRole );
				setItemData( i, LayoutManager::GetCloudIcon(), Qt::DecorationRole );
				break;
			}

			case Layout::BUILTIN:
			{
				setItemData( i, LayoutManager::GetBoxColor(), Qt::BackgroundRole );
				setItemData( i, LayoutManager::GetBoxIcon(), Qt::DecorationRole );
				break;
			}
		}
	}

	blockSignals(false);

	setSizeAdjustPolicy( QComboBox::AdjustToContents );

	int layoutIndex = findText( selectedLayoutText );
	SilentChangeCurrentIndex(layoutIndex);
}


void LayoutComboBox::OnCurrentIndexChanged(int index)
{
	GetLayoutManager()->SwitchToLayoutByIndex( index );
}


void LayoutComboBox::SilentChangeCurrentIndex(int newIndex)
{
	blockSignals(true);
	setCurrentIndex( newIndex );
	blockSignals(false);
}
