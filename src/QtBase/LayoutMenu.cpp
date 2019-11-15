/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "LayoutMenu.h"
#include "QtBaseManager.h"


// constructor
LayoutMenu::LayoutMenu(QWidget* parent) : QMenu("&Layouts", GetQtBaseManager()->GetMainWindow())
{
	mSaveAction			= NULL;
	mCustomizingEnabled	= false;
}


// destructor
LayoutMenu::~LayoutMenu()
{
}


// update the layouts menu
void LayoutMenu::ReInit()
{
	// clear the menu
	mSaveAction = NULL;
	clear();

	LayoutManager*	layoutManager	= GetQtBaseManager()->GetLayoutManager();
	//Layout*			activeLayout	= layoutManager->GetActiveLayout();

	// get the number of layouts and iterate through them
	const uint32 numLayouts = layoutManager->GetNumLayouts();
	for (uint32 i=0; i<numLayouts; ++i)
	{
		Layout*		layout		= layoutManager->GetLayout(i);
		QAction*	layoutAction= NULL;

		switch (layout->GetType())
		{
			case Layout::LOCAL:
			{
				layoutAction = addAction( LayoutManager::GetHddIcon(), layout->GetName() );
				break;
			}

			case Layout::BACKEND:
			{
				layoutAction = addAction( LayoutManager::GetCloudIcon(), layout->GetName() );
				break;
			}

			case Layout::BUILTIN:
			{
				layoutAction = addAction( LayoutManager::GetBoxIcon(), layout->GetName() );
				break;
			}
		}

		layoutAction->setData( i );
		connect(layoutAction, SIGNAL(triggered()), this, SLOT(OnLayoutClicked()));
	}

	if (mCustomizingEnabled == true)
	{
		// add a separator
		addSeparator();

		// add the save current layout menu item
		QAction* layoutSave = addAction( GetQtBaseManager()->FindIcon("Images/Icons/Save.png"), tr("Save") );
		connect(layoutSave, SIGNAL(triggered()), GetLayoutManager(), SLOT(OnSaveLayout()));
		mSaveAction = layoutSave;

		// add the save as menu item
		QAction* layoutsSaveAs = addAction( GetQtBaseManager()->FindIcon("Images/Icons/SaveAs.png"), tr("Save As") );
		connect(layoutsSaveAs, SIGNAL(triggered()), GetLayoutManager(), SLOT(OnSaveLayoutAs()));

		// add all layout file presets to the remove menu as well
		QMenu* removeMenu = addMenu( GetQtBaseManager()->FindIcon("Images/Icons/Minus.png"), "Remove" );
		for (uint32 i=0; i<numLayouts; ++i)
		{
			Layout* layout = layoutManager->GetLayout(i);

			if (layout->GetType() != Layout::LOCAL)
				continue;

			QAction* removeAction = removeMenu->addAction( layout->GetName() );
			removeAction->setData( i );
			connect(removeAction, SIGNAL(triggered()), this, SLOT(OnRemoveLayout()));
		}
	}
}


// called when a given layout menu action 
void LayoutMenu::OnLayoutClicked()
{
	QAction* action = qobject_cast<QAction*>( sender() );
	int layoutIndex = action->data().toInt();

	GetLayoutManager()->SwitchToLayoutByIndex( layoutIndex );
}


// remove a given layout
void LayoutMenu::OnRemoveLayout()
{
	if (mCustomizingEnabled == false)
		return;

	QAction* action = qobject_cast<QAction*>( sender() );
	int layoutIndex = action->data().toInt();

	GetLayoutManager()->OnRemoveLayout( layoutIndex );
}


void LayoutMenu::UpdateInterface()
{
	if (mSaveAction != NULL)
	{
		Layout* activeLayout = GetLayoutManager()->GetActiveLayout();
		if (activeLayout != NULL && activeLayout->GetType() != Layout::BUILTIN)
			mSaveAction->setEnabled(true);
		else
			mSaveAction->setEnabled(false);
	}
}


void LayoutMenu::EnableCustomizing(bool enableCustomizing)
{
	mCustomizingEnabled = enableCustomizing;
	ReInit();
	UpdateInterface();
}