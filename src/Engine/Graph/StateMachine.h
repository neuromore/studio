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

#ifndef __NEUROMORE_STATEMACHINE_H
#define __NEUROMORE_STATEMACHINE_H

// include the required headers
#include "../Core/Array.h"
#include "Graph.h"
#include "Action.h"


// forward declaration
class StateTransition;
class State;
class EntryState;
class ExitState;
class ActionState;

//
class ENGINE_API StateMachine : public Graph
{
	public:
		enum { TYPE_ID = 0x1002 };

		// attributes
		enum
		{
			ATTRIBUTE_MEDIAROOT = 0,
			NUM_STATEMACHINE_ATTRIBUTES
		};

		// constructor & destructor
		StateMachine(Graph* parentGraph=NULL);
		virtual ~StateMachine();

		// main functions
		virtual void Init() override;
		virtual void Reset() override;
		virtual void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		const char* GetTypeUuid() const override				{ return "532b918c-bb6c-11e4-8dfc-aa07a5b093db"; }
		uint32 GetType() const override							{ return TYPE_ID; }
		const char* GetReadableType() const override			{ return "State Machine"; }
		GraphObject* Clone(Graph* parentGraph) override			{ StateMachine* clone = new StateMachine(parentGraph); return clone; }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// start/resume/pause classifier execution
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		void Start();
		void Stop();
		
		// pause
		void Pause();
		void Continue();

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Assets
		///////////////////////////////////////////////////////////////////////////////////////////////////////
	
		// TODO: move into own asset manager class

		struct Asset
		{
			enum EActivity
			{
				INACTIVE,	// not played or invisible
				ACTIVE,		// playing or visible
				PAUSED		// paused
			};

			Asset() : mType(Action::ASSET_NONE), mActivity(INACTIVE), mAllowStreaming(false)		{}
			~Asset()																				{}
			Core::String		mLocation;
			Action::AssetType	mType;
			EActivity			mActivity;
			bool				mAllowStreaming;
		};

		uint32 CollectAssets(bool forceUpdate=false);
		Core::Array<Asset>& GetAssets()						{ return mAssets; }
		Asset* FindAssetByLocation(const char* location);
		void SetAssetActivity(const char* location, Asset::EActivity activity);
		void SetAssetGroupActivity(Action::AssetType type, Asset::EActivity activity);

		// type based asset helpers
		uint32 GetNumAssetsOfType(Action::AssetType type);
		const char* GetAssetLocationOfType(Action::AssetType type, uint32 index);
		bool GetAssetAllowStreamingOfType(Action::AssetType type, uint32 index);

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Child nodes (states)
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		
		bool RemoveNode(Node* node) override final;
		
		void CollectObjects() override final;
		
		// gather all important different classes of states
		void CollectStates();

		State* GetState(uint32 index);

		const Core::Array<State*>& GetActiveStates()		{ return mActiveStates; }
		uint32 GetNumActiveStates()	const					{ return mActiveStates.Size(); }
		State* GetActiveState(uint32 index)					{ return mActiveStates[index]; }

		const Core::Array<EntryState*>& GetEntryStates()	{ return mEntryStates; }
		uint32 GetNumEntryStates() const					{ return mEntryStates.Size(); }
		EntryState* GetEntryState(uint32 index)				{ return mEntryStates[index]; }

		const Core::Array<ExitState*>& GetExitStates()		{ return mExitStates; }
		uint32 GetNumExitStates() const						{ return mExitStates.Size(); }
		ExitState* GetExitState(uint32 index)				{ return mExitStates[index]; }
		
		bool ExitStateReached();
		uint32 FindExitStatus();
		uint32 GetExitStatus() const						{ return mExitStatus; }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Transitions
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		void CollectTransitions();

		Connection* AddConnection(Node* sourceNode, uint32 outputPortNr, Node* targetNode, uint32 inputPortNr, int32 startOffsetX, int32 startOffsetY, int32 endOffsetX, int32 endOffsetY) override;
		void RemoveConnection(Connection* connection) override;

		StateTransition* GetTransition(uint32 connectionIndex) const;
		bool IsTransitioning() const												{ return mActiveTransitions.Size() > 0; }

		uint32 GetNumActiveTransitions() const										{ return mActiveTransitions.Size(); }
		StateTransition* GetActiveTransition(uint32 index) const					{ return mActiveTransitions[index]; }
		
		uint32 FindNumOutTransitions(const State* sourceState, bool ignoreDisabled = false) const;
		StateTransition* FindOutTransition(const State* sourceState, uint32 index, bool ignoreDisabled = false) const;

		uint32 FindNumInTransitions(const State* targetState, bool ignoreDisabled = false) const;
		StateTransition* FindInTransition(const State* targetState, uint32 index, bool ignoreDisabled = false) const;

		void AddCondition(StateTransition* transition, StateTransitionCondition* condition);
		void RemoveCondition(StateTransition* transition, uint32 index);

		//----------------------------------------------------------------------------------------------------------------------------

		const char* GetMediaRoot()													{ return GetStringAttribute( ATTRIBUTE_MEDIAROOT ); }

		//----------------------------------------------------------------------------------------------------------------------------

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item) override;

	private:
		bool SaveTransitions(Core::Json& json, Core::Json::Item& item);

		Core::Array<State*>				mActiveStates;		
		Core::Array<StateTransition*>	mActiveTransitions;

		Core::Array<EntryState*>		mEntryStates;		
		Core::Array<ExitState*>			mExitStates;		
		Core::Array<ActionState*>		mActionStates;		

		Core::Array<Asset>				mAssets;
		bool							mAssetsInitialized;

		bool							mIsRunning;

		uint32							mExitStatus;

		void StartTransition(StateTransition* transition);
		void EndTransition(StateTransition* transition);

		void CheckConditions(State* sourceNode);
		void UpdateConditions(State* state, const Core::Time& elapsed, const Core::Time& delta);
};


#endif
