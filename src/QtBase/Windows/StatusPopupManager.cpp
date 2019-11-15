/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "StatusPopupManager.h"
#include <Core/LogManager.h>
#include "../QtBaseManager.h"


// constructor
StatusPopupManager::StatusPopupManager() : QObject()
{
}


// destructor
StatusPopupManager::~StatusPopupManager()
{
	const int32 numActivePopups = mActivePopups.Size();
	if (numActivePopups <= 0)
		return;

	// destruct our popups back to front
	for (int32 i=numActivePopups-1; i>=0; i--)
		delete mActivePopups[i];
}


int32 StatusPopupManager::CalcYPos(uint32 index)
{
	const QRect mainWindowRect = GetQtBaseManager()->GetMainWindow()->rect();
	//QPoint mainWindowCenter = mainWindowRect.center();
	int32 halfHeight = StatusPopupWindow::GetHalfHeight();
	int32 height = halfHeight*2;

	return mainWindowRect.height() - halfHeight - (index * height);
}


void StatusPopupManager::UpdatePositions()
{
	const uint32 numActivePopups = mActivePopups.Size();
	for (uint32 i=0; i<numActivePopups; ++i)
	{
		StatusPopupWindow* popup = mActivePopups[i];

		popup->MoveTo( CalcYPos(i) );
	}
}


//
StatusPopupWindow* StatusPopupManager::Create()
{
	uint32 newIndex = mActivePopups.Size();
	int32 yPos = CalcYPos(newIndex);

	StatusPopupWindow* statusPopup = new StatusPopupWindow( this, GetQtBaseManager()->GetMainWindow(), yPos );
	mActivePopups.Add(statusPopup);

	UpdatePositions();

	return statusPopup;
}


void StatusPopupManager::Remove(StatusPopupWindow* statusPopup)
{
	if (mActivePopups.Find(statusPopup) != CORE_INVALIDINDEX32)
		statusPopup->FadeOut();
}
