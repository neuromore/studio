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

#ifndef __CORE_EVENTMANAGERHELPERS_H
#define __CORE_EVENTMANAGERHELPERS_H

/**
 * Helper macros for implementing the notify functions (contain loops that call all registered EventHandlers) in the EventManager. 
 * They allow us to implement event callbacks with zero overhead which is useful for frequently used core events,
 * but on the flipside disallow us to selectively disable/block them.
 */
#define EVENT_CREATE_NOTIFY_FUNCTION_0(FNAME)					\
	void FNAME() {												\
		const uint32 size = mEventHandlers.Size();			\
		for (uint32 i = 0; i<size; ++i)							\
			if (mEventHandlers[i]->GetAcceptEvents() == true)	\
				mEventHandlers[i]->FNAME(); }

#define EVENT_CREATE_NOTIFY_FUNCTION_1(FNAME, TYPE1, VNAME1)	\
	void FNAME(TYPE1 VNAME1) {									\
		const uint32 size = mEventHandlers.Size();			\
		for (uint32 i = 0; i<size; ++i)							\
			if (mEventHandlers[i]->GetAcceptEvents() == true)	\
				mEventHandlers[i]->FNAME( VNAME1 );		  }

#define EVENT_CREATE_NOTIFY_FUNCTION_2(FNAME, TYPE1, VNAME1, TYPE2, VNAME2)	\
	void FNAME(TYPE1 VNAME1, TYPE2 VNAME2) {								\
		const uint32 size = mEventHandlers.Size();						\
		for (uint32 i = 0; i<size; ++i)										\
			if (mEventHandlers[i]->GetAcceptEvents() == true)				\
				mEventHandlers[i]->FNAME( VNAME1, VNAME2 );			}

#define EVENT_CREATE_NOTIFY_FUNCTION_3(FNAME, TYPE1, VNAME1, TYPE2, VNAME2, TYPE3, VNAME3)	\
	void FNAME(TYPE1 VNAME1, TYPE2 VNAME2, TYPE3 VNAME3)	{								\
		const uint32 size = mEventHandlers.Size();										\
		for (uint32 i = 0; i<size; ++i)														\
			if (mEventHandlers[i]->GetAcceptEvents() == true)								\
				mEventHandlers[i]->FNAME( VNAME1, VNAME2, VNAME3 );		}

#define EVENT_CREATE_NOTIFY_FUNCTION_4(FNAME, TYPE1, VNAME1, TYPE2, VNAME2, TYPE3, VNAME3, TYPE4, VNAME4)	\
	void FNAME(TYPE1 VNAME1, TYPE2 VNAME2, TYPE3 VNAME3, TYPE4 VNAME4)	{									\
		const uint32 size = mEventHandlers.Size();														\
		for (uint32 i = 0; i<size; ++i)																		\
			if (mEventHandlers[i]->GetAcceptEvents() == true)												\
				mEventHandlers[i]->FNAME( VNAME1, VNAME2, VNAME3, VNAME4 ); }

#define EVENT_CREATE_NOTIFY_FUNCTION_5(FNAME, TYPE1, VNAME1, TYPE2, VNAME2, TYPE3, VNAME3, TYPE4, VNAME4, TYPE5, VNAME5)	\
	void FNAME(TYPE1 VNAME1, TYPE2 VNAME2, TYPE3 VNAME3, TYPE4 VNAME4, TYPE5 VNAME5)	{									\
		const uint32 size = mEventHandlers.Size();																		\
		for (uint32 i = 0; i<size; ++i)																						\
			if (mEventHandlers[i]->GetAcceptEvents() == true)																\
				mEventHandlers[i]->FNAME( VNAME1, VNAME2, VNAME3, VNAME4, VNAME5 ); }


#endif
