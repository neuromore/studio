/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_STATUSPOPUPMANAGER_H
#define __NEUROMORE_STATUSPOPUPMANAGER_H

// include required headers
#include "../QtBaseConfig.h"
#include "StatusPopupWindow.h"
#include <Core/Array.h>


class QTBASE_API StatusPopupManager : public QObject
{
	Q_OBJECT

	public:
		// constructor & destructor
		StatusPopupManager();
		virtual ~StatusPopupManager();

		StatusPopupWindow* Create();
		void Remove(StatusPopupWindow* statusPopup);

	private:
		friend class StatusPopupWindow;

		void UpdatePositions();
		int32 CalcYPos(uint32 index);

		Core::Array<StatusPopupWindow*> mActivePopups;
};


#endif
