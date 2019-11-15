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

#ifndef __NEUROMORE_STATEMACHINETRANSITION_H
#define __NEUROMORE_STATEMACHINETRANSITION_H

// include the required headers
#include "../Core/Array.h"
#include "GraphObject.h"
#include "Connection.h"
#include "StateTransitionCondition.h"


// forward declaration
class Node;
class Graph;
class State;
class StateMachine;

//
class ENGINE_API StateTransition : public Connection
{
	public:
		enum { TYPE_ID = 0x00600 };
		static const char* Uuid () { return "5ec683a8-bb6c-11e4-8dfc-aa07a5b093db"; }
		
		enum
		{
			ATTRIB_DISABLED						= 0,
			ATTRIB_PRIORITY						= 1,
			ATTRIB_TRANSITIONTIME				= 2
		};

		// constructor & destructor
		StateTransition(StateMachine* stateMachine);
		virtual ~StateTransition();

		void Init() override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		uint32 GetType() const override final											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final									{ return Uuid(); }
		virtual const char* GetReadableType() const override final						{ return "State Transition"; }
	
		void OnAttributesChanged() override;
		bool ContainsAttribute(Core::Attribute* attribute) const override;

		inline StateMachine* GetStateMachine() const									{ return mStateMachine; }
		State* GetSourceState() const;
		State* GetTargetState() const;

		bool IsReady() const;
		bool IsTransitioning() const													{ return mIsActive; }
		inline bool IsDone() const														{ return mIsDone; }
		inline double GetProgress() const												{ return mProgress; }

		inline double GetBlendTime() const												{ return GetFloatAttribute(ATTRIB_TRANSITIONTIME); }
		inline uint32 GetPriority()														{ return GetInt32Attribute(ATTRIB_PRIORITY); }
		inline bool IsDisabled()														{ return GetBoolAttribute(ATTRIB_DISABLED); }
		
		// callbacks
		void OnStartTransition();
		void OnEndTransition();

		// visual offset
		inline int32 GetVisualStartOffsetX() const										{ return mStartOffsetX; }
		inline int32 GetVisualStartOffsetY() const										{ return mStartOffsetY; }
		inline int32 GetVisualEndOffsetX() const										{ return mEndOffsetX; }
		inline int32 GetVisualEndOffsetY() const										{ return mEndOffsetY; }
		void SetVisualOffsets(int32 startX, int32 startY, int32 endX, int32 endY)		{ mStartOffsetX=startX; mStartOffsetY=startY; mEndOffsetX=endX; mEndOffsetY=endY; }
		
		inline uint32 GetNumConditions() const											{ return mConditions.Size(); }
		inline StateTransitionCondition* GetCondition(uint32 index) const				{ return mConditions[index]; }
		inline uint32 FindConditionIndex(StateTransitionCondition* condition) const		{ return mConditions.Find(condition); }

        // conditions
        void AddCondition(StateTransitionCondition* condition);
        void RemoveCondition(uint32 index, bool delFromMem=true)						{ if (delFromMem) delete mConditions[index]; mConditions.Remove( index ); }
        void RemoveAllConditions(bool delFromMem=true);
        void ResetConditions();

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override final;
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item) override final;

	protected:
		bool SaveConditions(Core::Json& json, Core::Json::Item& item);
		
	private:
		StateMachine*								mStateMachine;
		Core::Array<StateTransitionCondition*>		mConditions;
		
		int32										mStartOffsetX;
		int32										mStartOffsetY;
		int32										mEndOffsetX;
		int32										mEndOffsetY;

		double										mProgress;
		Core::Time									mTotalSeconds;
		bool										mIsDone;
		bool										mIsActive;
};


#endif
