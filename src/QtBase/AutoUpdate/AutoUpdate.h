/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_AUTOUPDATE_H
#define __NEUROMORE_AUTOUPDATE_H

// include the required headers
#include <Core/StandardHeaders.h>
#include "../QtBaseConfig.h"


class AutoUpdate
{
	public:
		static bool IsUpdateAvailable(bool ignoreErrors=false);
		static void StartUpdateTool();
};


#endif