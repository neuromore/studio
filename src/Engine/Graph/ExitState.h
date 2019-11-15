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

#ifndef __NEUROMORE_EXITSTATE_H
#define __NEUROMORE_EXITSTATE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "State.h"


// state class
class ENGINE_API ExitState : public State
{
	public:
		enum { TYPE_ID = 0x1005 };
		static const char* Uuid () { return "6c3ee2bc-a060-11e5-8994-feff819cdc9f"; }

		enum
		{
			ATTRIB_EXITSTATUS	= NUM_STATE_ATTRIBUTES
		};

		// constructor & destructor
		ExitState(Graph* parentGraph);
		virtual ~ExitState();
		
		void Init() override final;
		void Reset() override final;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override final;
		void OnAttributesChanged() override final;

		Core::Color GetColor() const override									{ return Core::RGBA(234, 91, 33); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "End"; }
		const char* GetRuleName() const override final							{ return "STATE_End"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_GENERAL; }
		GraphObject* Clone(Graph* parentGraph) override							{ State* clone = new ExitState(parentGraph); return clone; }

		uint32 GetExitStatus() const											{ return (uint32)GetInt32Attribute(ATTRIB_EXITSTATUS); }

	protected:
		EActivationMode GetActivationMode() const override final				{ return ACTIVATE_FIRST; }
		EDeadEndMode GetDeadEndMode() const override final						{ return DEADEND_STAYACTIVE; }
	
		// an exit state can never exit
		bool CanExit(StateTransition* nextTransition) const override final		{ return false; }

};


#endif
