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

#ifndef __NEUROMORE_ACTIONSTATE_H
#define __NEUROMORE_ACTIONSTATE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "State.h"
#include "ActionSet.h"


// state class
class ENGINE_API ActionState : public State
{
	public:
		enum { TYPE_ID = 0x1003 };
		static const char* Uuid () { return "ef7d598a-261a-11e5-b345-feff819cdc9f"; }

		// constructor & destructor
		ActionState(Graph* parentGraph);
		virtual ~ActionState();
		
		void Init() override final;
		void Reset() override final;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override final;
		void OnAttributesChanged() override final;
		bool ContainsAttribute(Core::Attribute* attribute) const override;

		Core::Color GetColor() const override									{ return Core::RGBA(191,234,33); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Action"; }
		const char* GetRuleName() const override final							{ return "STATE_Action"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_GENERAL; }
		GraphObject* Clone(Graph* parentGraph) override							{ State* clone = new ActionState(parentGraph); return clone; }

		// actions
		ActionSet& GetOnEnterActions()											{ return mOnEnterActions; }
		ActionSet& GetOnExitActions()											{ return mOnExitActions; }

	protected:
		EActivationMode GetActivationMode() const override final				{ return ACTIVATE_FIRST; }
		EDeadEndMode GetDeadEndMode() const override final						{ return DEADEND_DEACTIVATE; }

		void OnActivate() override final;
		void OnDeactivate() override final;

		// serialization
		Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override final;
		bool Load(const Core::Json& json, const Core::Json::Item& item) override final;
		bool LoadActionSet(const Core::Json& json, const Core::Json::Item& item, ActionSet* actionSet);

	private:
		ActionSet	mOnEnterActions;
		ActionSet	mOnExitActions;
};


#endif
