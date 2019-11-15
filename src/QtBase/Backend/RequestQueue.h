/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_REQUESTQUEUE
#define __NEUROMORE_REQUESTQUEUE

// include required headers
#include <Core/Array.h>
#include "Request.h"


// request queue
class QTBASE_API RequestQueue
{
	public:
		// constructor & destructor
		RequestQueue();
		virtual ~RequestQueue();

		void Clear();
		uint32 Size();

		void Push(Request* request);
		void Pop();

		void Log();

	private:
		Core::Array<Request*> mQueue;
};


#endif
