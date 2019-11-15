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

// include the required headers
#include "StateMachine.h"
#include "State.h"
#include "EntryState.h"
#include "ExitState.h"
#include "ActionState.h"
#include "StateTransition.h"
#include "GraphImporter.h"
#include "GraphExporter.h"
#include "../EngineManager.h"
#include "../Core/EventManager.h"


using namespace Core;

// constructor
StateMachine::StateMachine(Graph* parentGraph) : Graph(parentGraph)
{
	SetName("State Machine");

	mIsDirty			= false;
	mIsRunning			= true;// FIXME: false; ??
	mExitStatus			= 0;
	mAssetsInitialized	= false;
}


// destructor
StateMachine::~StateMachine()
{
	// NOTE: base class automatically removes all child nodes (states) and connections (transitions)
}


// initialize the state machine
void StateMachine::Init()
{
	// ATTRIBUTES

	// media root attributelet
	AttributeSettings* mediaRootAttribute = RegisterAttribute("Media Root", "mediaRoot", "The root folder or base URL of all media assets in the state machine.", ATTRIBUTE_INTERFACETYPE_STRING);
	mediaRootAttribute->SetDefaultValue( AttributeString::Create("") );
}


// start statemachine: activate start states
void StateMachine::Start()
{
	// already running or no permissions
	if (mIsRunning == true || mCreud.Execute() == false)
		return;

	//Reset();

	// start entry states
	const uint32 numEntryStates = mEntryStates.Size();
	for (uint32 i = 0; i < numEntryStates; ++i)
	{
		EntryState* entryState = mEntryStates[i];
		entryState->ForceActivate();
	}

	mIsRunning = true;
	mExitStatus = 0;
}


void StateMachine::Stop()
{
	mIsRunning = false;
}


// pause execution
void StateMachine::Pause()
{
	// already paused or not running
	if (mIsRunning == false)
		return;

	mIsRunning = true;

	// TODO iterate over all child-statemachines and (un)pause them

	// TODO iterate over all playing media assets and issue the pause action
}


void StateMachine::Continue()
{
	// already unpaused
	if (mIsRunning == true)
		return;

	mIsRunning = false;

	// TODO iterate over all child-statemachines and (un)pause them

	// TODO iterate over all playing media assets and issue the continue action
}



// convert the given connection to a transition and return a pointer to it
StateTransition* StateMachine::GetTransition(uint32 connectionIndex) const
{
	Connection* connection = mConnections[connectionIndex];
	if (connection->GetType() != StateTransition::TYPE_ID)
	{
		LogError("StateMachine::GetTransition(): Connection is no valid state transition");
		CORE_ASSERT( false );
		return NULL;
	}

	// cast and return
	return static_cast<StateTransition*>(connection);
}


uint32 StateMachine::FindNumOutTransitions(const State* sourceState, bool ignoreDisabled) const
{
	const uint32 numTransitions = mConnections.Size();

	uint32 count = 0;
	for (uint32 i = 0; i < numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->GetSourceState() != sourceState)
			continue;

		if (ignoreDisabled == true && transition->IsDisabled() == true)
			continue;

		// found one
		count++;
	}

	return count;
}


StateTransition* StateMachine::FindOutTransition(const State* sourceState, uint32 index, bool ignoreDisabled) const
{
	const uint32 numTransitions = mConnections.Size();

	uint32 count = 0;
	for (uint32 i = 0; i < numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->GetSourceState() != sourceState)
			continue;

		if (ignoreDisabled == true && transition->IsDisabled() == true)
			continue;

		if (count == index)
			return transition;

		count++;
	}

	return NULL;
}


uint32 StateMachine::FindNumInTransitions(const State* targetState, bool ignoreDisabled) const
{
	const uint32 numTransitions = mConnections.Size();

	uint32 count = 0;
	for (uint32 i = 0; i < numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->GetTargetState() != targetState)
			continue;

		if (ignoreDisabled == true && transition->IsDisabled() == true)
			continue;

		// found one
		count++;
	}

	return count;
}


StateTransition* StateMachine::FindInTransition(const State* targetState, uint32 index, bool ignoreDisabled) const
{
	const uint32 numTransitions = mConnections.Size();

	uint32 count = 0;
	for (uint32 i = 0; i < numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->GetTargetState() != targetState)
			continue;

		if (ignoreDisabled == true && transition->IsDisabled() == true)
			continue;

		if (count == index)
			return transition;

		// found one
		count++;
	}

	return NULL;
}


// Check if the state machine has reached an exit state.
bool StateMachine::ExitStateReached()
{
	const uint32 numExitStates = mExitStates.Size();
	for (uint32 i = 0; i < numExitStates; ++i)
	{
		ExitState* state = mExitStates[i];
		if (state->IsActive() == true)
			return true;
	}

	return false;
}


// get highest exit status accross all exit states
uint32 StateMachine::FindExitStatus()
{
	// find the maximum status 
	uint32 maxStatus = 0;

	const uint32 numExitStates = mExitStates.Size();
	for (uint32 i = 0; i < numExitStates; ++i)
	{
		ExitState* state = mExitStates[i];
		if (state->IsActive() == true)
		{
			ExitState* exitState = static_cast<ExitState*>(state);
			maxStatus = Max(maxStatus, exitState->GetExitStatus());
		}
	}

	return maxStatus;
}


// transition is starting up
void StateMachine::StartTransition(StateTransition* transition)
{
	CORE_ASSERT(transition->IsTransitioning() == false);

	// get the source and the target node of the transition we need to start
	State* sourceState = transition->GetSourceState();
	State* targetState = transition->GetTargetState();

	sourceState->OnTryStateExit(transition);

	//  if target node is already in active or transition state, dont enter the state again the transition (so it behaves as if it was activated)
	if (targetState->OnTryStateEnter(sourceState, transition) == true)
		transition->OnStartTransition();
}



// transition has ended
void StateMachine::EndTransition(StateTransition* transition)
{
	CORE_ASSERT(transition->IsTransitioning() == true);
}


// check all outgoing transitions for the given node for if they are ready
void StateMachine::CheckConditions(State* sourceState)
{
	// skip calculations in case the source node is not valid
	if (sourceState == NULL)
		return;

	// num transitions total
	const uint32 numTransitions	= GetNumConnections();

	// FIXME optimize alloc
	// arrays to collect all ready transitions from this node
	Array<StateTransition*> readyTransitions;
	readyTransitions.Reserve(10);

	// find ready transitions
	for (uint32 i=0; i<numTransitions; ++i)
	{
		// get the current transition and skip it directly if in case it is disabled
		StateTransition* curTransition = GetTransition(i);
		if (curTransition->IsDisabled() == true)
			continue;

		// skip transitions that don't start from our given start node
		if (curTransition->GetSourceState() != sourceState)
			continue;

		// make sure source node can exit
		if (sourceState->CanExit(curTransition) == false)
			continue;

		// make sure target state can enter
		if (curTransition->GetTargetState()->CanEnter(sourceState, curTransition) == false)
			continue;

		// start transition if it is ready (if the conditions evaluate to true) and it is not already transitioning
		if (curTransition->IsReady() == true && curTransition->IsTransitioning() == false)
			readyTransitions.Add(curTransition);
	}


	// DEPRECATED: default to "highest priority" sometime in the future and disable everything else:
	const uint32 numReadyTransitions = readyTransitions.Size();
	if (numReadyTransitions > 0)
	{
		if (numReadyTransitions == 1)
			StartTransition(readyTransitions[0]);
		else
		{
			// trigger transitions depending on mode
			switch (sourceState->GetTransitionMode())
			{
				// trigger all transitions
				case State::TRANSITIONMODE_ALL:
				{
					for (uint32 i = 0; i < numReadyTransitions; ++i)
						StartTransition(readyTransitions[i]);
				} break;

				// trigger arandom transition
				case State::TRANSITIONMODE_RANDOM:
				{
					// choose random transition
					uint32 randomIndex = Math::RandIndex(numReadyTransitions);

					// make sure it is valid and start it
					if (randomIndex != CORE_INVALIDINDEX32 && randomIndex < numReadyTransitions)
						StartTransition(readyTransitions[randomIndex]);
				} break;

				// trigger the transitions with the highest priority
				case State::TRANSITIONMODE_PRIORITY:
				{
					// find max priority
					uint32 maxPriority = 0;
					for (uint32 i = 0; i < numReadyTransitions; ++i)
						maxPriority = Max(maxPriority, readyTransitions[i]->GetPriority());

					// trigger all transitions that have this priority
					for (uint32 i = 0; i < numReadyTransitions; ++i)
						if (readyTransitions[i]->GetPriority() == maxPriority)
							StartTransition(readyTransitions[i]);
				} break;

				default: break;
			}
		}
	}
}


// update conditions for all transitions that start from the given state and all wild card transitions
void StateMachine::UpdateConditions(State* state, const Time& elapsed, const Time& delta)
{
	// skip calculations in case the node is not valid
	if (state == NULL)
		return;

	// get the number of transitions and iterate through them
	const uint32 numTransitions = GetNumConnections();
	for (uint32 i=0; i<numTransitions; ++i)
	{
		// get the current transition and skip it directly if in case it is disabled
		StateTransition* transition = GetTransition(i);
		if (transition->IsDisabled() == true)
			continue;

		// skip transitions that don't start from our given current node
		if (transition->GetSourceNode() != state)
			continue;

		// skip transitions that are not made for interrupting when we are currently transitioning
		if (transition->IsTransitioning() == true)
			continue;

		// get the number of conditions assigned to the transition and iterate through them
		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 j=0; j<numConditions; ++j)
		{
			// update the transition condition
			StateTransitionCondition* condition = transition->GetCondition(j);
			condition->Update(elapsed, delta);
		}
	}
}


// update
void StateMachine::Update(const Time& elapsed, const Time& delta)
{
	if (mIsRunning == false ||  mCreud.Execute() == false)
		return;

	// PHASE 1: check all conditions and collect all states and Transitions
	
	// update all conditions from the transitions that start from an active state state
	const uint32 numActiveStates = mActiveStates.Size();
	for (uint32 i = 0; i < numActiveStates; ++i)
	{
		State* activeState = mActiveStates[i];

		if (activeState->IsTransitioning() == true)
			continue;

		UpdateConditions(activeState, elapsed, delta);

		// check all transition conditions starting from the current node
		CheckConditions(activeState);
	}

	CollectStates();
	CollectTransitions();

	// PHASE 2: Check if a transition has finished
	const uint32 numTransitions = mActiveTransitions.Size();
	for (uint32 i = 0; i < numTransitions; ++i)
	{
		StateTransition* transition = mActiveTransitions[i];

		// update transition
		transition->Update(elapsed, delta);

		// if the transition has finished with transitioning
		if (transition->IsDone() == true)
		{
			transition->OnEndTransition();
				
			State* sourceState = transition->GetSourceState();
			State* targetState = transition->GetTargetState();

			// exit source state
			if (sourceState != NULL)
				sourceState->OnTryStateExited(transition);
				
			// enter source state
			if (targetState != NULL)
				targetState->OnTryStateEntered(sourceState, transition);

			// reset the transition
			transition->Reset();
		}
	}

	// collect states again, the active ones might have changed
	CollectStates();
	
	// PHASE 3: Check if exit state was reached
	if (ExitStateReached() == true)
	{
		// stop state machine
		mIsRunning = false;
		mExitStatus = FindExitStatus();

		EMIT_EVENT(OnExitStateReached(mExitStatus));
		return;
	}

	// PHASE 4: Update all states
	
	const uint32 numStates = mNodes.Size();
	for (uint32 i = 0; i<numStates; ++i)
	{
		State* state = GetState(i);
		state->Update(elapsed, delta);
	}

	// TODO I don't think this is necessary here? check again!
	// collects states again, states may have disabled themselfs
	CollectStates();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Json::Item StateMachine::Save(Json& json, Json::Item& item)
{
	// TODO FIXME this has serialized by graph object, but this is not permitted by the structure right now (root object is not written correctly)
	//Write(jsonParser, parentItem, false);

	// 1: save states
	if (SaveNodes(json, item) == false)
		return json.NullItem();

	// 2: save all state transitions
	if (SaveTransitions(json, item) == false)
		return json.NullItem();

	// reset the dirty flag
	SetIsDirty(false);

	return item;
}


// save all state transitions
bool StateMachine::SaveTransitions(Json& json, Json::Item& item)
{
	// get the number of transitions and check if we have to save any, if not skip directly
	const uint32 numTransitions = GetNumConnections();
	if (numTransitions == 0)
		return true;

	// add the transitions array item
	Json::Item transitionsItem = item.AddArray("transitions");

	// iterate through and save the transitions
	for (uint32 i=0; i<numTransitions; ++i)
	{
		Connection* connection = GetConnection(i);
		connection->Save( json, transitionsItem );
	}

	return true;
}


bool StateMachine::Load(const Json& json, const Json::Item& item)
{
	bool success = true;

	// TODO FIXME this has serialized by graph object, but this is not permitted by the structure right now (root object is not written correctly)
	//Read(jsonParser, parentItem, true);

	// 1: load all nodes
	if (GraphImporter::LoadNodes(json, item, this) == false)
		success = false;

	// 2: load all transitions
	if (GraphImporter::LoadTransitions(json, item, this) == false)
		success = false;

	// TODO nested statemachine loading

	// iterate over the classifiers and preload them (only for the root state machine)
/*	if (GetParent() == NULL) // NOTE: important that we call the function, because the parameter parentNode is the given state machine itself and NOT its parent
	{
		const uint32 numClassifiers = mNodes.Size();
		for (uint32 i=0; i<numClassifiers; ++i)
		{
			// get the root state machine classifier state
			GraphNode* childNode = mNodes[i];
			if (childNode->GetType() != StateClassifier::TYPE_ID)
			{
				LogError( "State at root level needs to be a classifier state." );
				CORE_ASSERT(false);
				continue;
			}

			// down-cast it to a classifier state
			StateClassifier* classifierState = static_cast<StateClassifier*>( childNode );

			// get the uuid for the classifier and put it into the loading queue
			String classifierUuid = classifierState->GetClassifierUuid();
			GetCoreManager()->GetGraphManager()->LoadGraph( classifierUuid.AsChar(), GetSession()->GetToken() );
		}
	}*/

	// reset the dirty flag
	SetIsDirty(false);
	
	return success;
}

void StateMachine::CollectObjects()
{
	// collect 
	Graph::CollectObjects();

	
	CollectStates();
	CollectTransitions();
}


// remove a given node
bool StateMachine::RemoveNode(Node* node)
{
	// find the index of the given node in the node array and remove it in case the index is valid
	const uint32 index = FindNodeIndex(node);
	if (index == CORE_INVALIDINDEX32)
	{
		LogError( "StateMachine::RemoveNode(): Cannot remove state '%s' from state machine '%s'. State is not part of the state machine.", node->GetName(), GetName() );
		return false;
	}

	// base class remove node
	const bool success = Graph::RemoveNode(node);
	
	CollectStates();
	
	return success;
}


// rewind the nodes in the state machine
void StateMachine::Reset()
{
	// reset all states and transitions
	Graph::Reset();

	mIsRunning = true;// FIXME: false; should be default
	mExitStatus = 0;  

	// TODO test if we can remove this
	// start entry states
	const uint32 numEntryStates = mEntryStates.Size();
	for (uint32 i = 0; i < numEntryStates; ++i)
	{
		EntryState* entryState = mEntryStates[i];
		entryState->ForceActivate();
	}
}


// gather all important different classes of states
void StateMachine::CollectStates()
{
	// get the number of nodes inside the classifier
	const uint32 numStates = mNodes.Size();

	// calculate the number of output nodes
	uint32 numActiveStates = 0;
	uint32 numEntryStates = 0;
	uint32 numExitStates = 0;
	uint32 numActionStates = 0;

	for (uint32 i = 0; i<numStates; ++i)
	{
		State* state = GetState(i);

		if (state->IsActive() == true || state->IsTransitioning())
			numActiveStates++;

		if (state->GetType() == EntryState::TYPE_ID)
			numEntryStates++;

		if (state->GetType() == ExitState::TYPE_ID)
			numExitStates++;

		if (state->GetType() == ActionState::TYPE_ID)
			numActionStates++;
	}

	// make sure our arrays have the correct size
	if (mActiveStates.Size() != numActiveStates)	mActiveStates.Resize(numActiveStates);
	if (mEntryStates.Size() != numEntryStates)		mEntryStates.Resize(numEntryStates);
	if (mExitStates.Size() != numExitStates)		mExitStates.Resize(numExitStates);
	if (mActionStates.Size() != numActionStates)	mActionStates.Resize(numActionStates);

	uint32 activeStateIndex = 0;
	uint32 entryStateIndex = 0;
	uint32 exitStateIndex = 0;
	uint32 actionStateIndex = 0;

	for (uint32 i = 0; i<numStates; ++i)
	{
		State* state = GetState(i);

		if (state->IsActive() == true || state->IsTransitioning())
		{
			mActiveStates[activeStateIndex] = state;
			activeStateIndex++;
		}

		if (state->GetType() == EntryState::TYPE_ID)
		{
			mEntryStates[entryStateIndex] = static_cast<EntryState*>(state);
			entryStateIndex++;
		}

		if (state->GetType() == ExitState::TYPE_ID)
		{
			mExitStates[exitStateIndex] = static_cast<ExitState*>(state);
			exitStateIndex++;
		}

		if (state->GetType() == ActionState::TYPE_ID)
		{
			mActionStates[actionStateIndex] = static_cast<ActionState*>(state);
			actionStateIndex++;
		}
	}
}


// gather all important different classes of states
void StateMachine::CollectTransitions()
{
	// Active Transitions

	// get the number of nodes inside the classifier
	const uint32 numTransitions = mConnections.Size();

	// calculate the number of output nodes
	uint32 numActiveTransitions = 0;

	for (uint32 i = 0; i<numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->IsTransitioning() == true)
			numActiveTransitions++;
	}

	// make sure our arrays have the correct size
	if (mActiveTransitions.Size() != numActiveTransitions)	
		mActiveTransitions.Resize(numActiveTransitions);

	uint32 activeTransitionsIndex = 0;

	for (uint32 i = 0; i<numTransitions; ++i)
	{
		StateTransition* transition = GetTransition(i);

		if (transition->IsTransitioning() == true)
		{
			mActiveTransitions[activeTransitionsIndex] = transition;
			activeTransitionsIndex++;
		}
	}
}


State* StateMachine::GetState(uint32 index)
{
	Node* node = mNodes[index];
	if (node->GetBaseType() != State::BASE_TYPE)
		return NULL;

	State* state = static_cast<State*> (node);
	return state;
}


// add the given new transition
Connection* StateMachine::AddConnection(Node* sourceNode, uint32 sourcePortNr, Node* targetNode, uint32 targetPortNr, int32 startOffsetX, int32 startOffsetY, int32 endOffsetX, int32 endOffsetY)
{
	if (sourceNode == targetNode)
	{
		LogWarning( "Source and target node are equal. Cannot create looping transition." );
		return NULL;
	}

	// create a new state transition
	StateTransition* transition = new StateTransition(this);

	// set transition info
	transition->SetSourceNode( sourceNode );
	transition->SetSourcePort( sourcePortNr );
	transition->SetTargetNode( targetNode );
	transition->SetTargetPort( targetPortNr );
	transition->SetVisualOffsets( startOffsetX, startOffsetY, endOffsetX, endOffsetY );

	// add it to the connections array
	mConnections.Add( transition );

	// 1. initialize the transition
	transition->Init();

	// 2. create default attribute values
	transition->CreateDefaultAttributeValues();

	// 3. reset the transition
	transition->Reset();

	// set the dirty flag
	SetIsDirty(true);

	return transition;
}


void StateMachine::RemoveConnection(Connection* connection)
{
	// make sure we're dealing with a transition
	if (connection->GetType() == StateTransition::TYPE_ID)
	{
		StateTransition* transition = static_cast<StateTransition*>(connection);

		const uint32 numConditions = transition->GetNumConditions();
		for (uint32 i=0; i<numConditions; ++i)
		{
			StateTransitionCondition* condition = transition->GetCondition(i);
			mObjects.RemoveByValue(condition);
		}
	}

	Graph::RemoveConnection( connection );
}


void StateMachine::AddCondition(StateTransition* transition, StateTransitionCondition* condition)
{
	mObjects.Add(condition);
	transition->AddCondition(condition);

	// set the dirty flag
	SetIsDirty(true);
}


void StateMachine::RemoveCondition(StateTransition* transition, uint32 index)
{
	StateTransitionCondition* condition = transition->GetCondition(index);
	if (condition != NULL)
	{
		mObjects.RemoveByValue(condition);
		transition->RemoveCondition(index);

		// set the dirty flag
		SetIsDirty(true);
	}
}

// collect all assets that the state machine uses
uint32 StateMachine::CollectAssets(bool forceUpdate)
{
	// skip in case we already collected the assets and we don't want to force an update
	if (mAssetsInitialized == true && forceUpdate == false)
		return mAssets.Size();

	mAssets.Clear();

	//
	// collect assets from action states
	//
	const uint32 numStates = mActionStates.Size();
	for (uint32 i=0; i<numStates; ++i)
	{
		ActionState* state = mActionStates[i];

		// add on-enter action assets
		uint32 numActions = state->GetOnEnterActions().GetNumActions();
		for (uint32 j=0; j<numActions; ++j)
		{
			Action* action = state->GetOnEnterActions().GetAction(j);
			
			Asset asset;
			action->GetAsset( &asset.mLocation, &asset.mType, &asset.mAllowStreaming );
			if (asset.mType == Action::ASSET_NONE)
				continue;

			mAssets.Add( asset );
		}

		// add on-exit action assets
		numActions = state->GetOnExitActions().GetNumActions();
		for (uint32 j=0; j<numActions; ++j)
		{
			Action* action = state->GetOnExitActions().GetAction(j);

			Asset asset;
			action->GetAsset( &asset.mLocation, &asset.mType, &asset.mAllowStreaming );
			if (asset.mType == Action::ASSET_NONE)
				continue;

			mAssets.Add( asset );
		}
	}

	// TODO recursively collect the assets of all child statemachines

	mAssetsInitialized = true;
	return mAssets.Size();
}


// collect the number of assets of the given type
uint32 StateMachine::GetNumAssetsOfType(Action::AssetType type)
{
	CollectAssets();

	uint32 result = 0;

	const uint32 numAssets = mAssets.Size();
	for (uint32 i=0; i<numAssets; ++i)
	{
		if (mAssets[i].mType == type)
			result++;
	}

	return result;
}


// get the asset location with the given index of the given type
const char* StateMachine::GetAssetLocationOfType(Action::AssetType type, uint32 index)
{
	CollectAssets();

	uint32 counter = 0;

	// get the number of assets and iterate through them all
	const uint32 numAssets = mAssets.Size();
	for (uint32 i=0; i<numAssets; ++i)
	{
		// skip all other than the given type
		const StateMachine::Asset& asset = mAssets[i];
		if (asset.mType != type)
			continue;

		// compare the current asset counter with the given index
		if (index == counter)
			return asset.mLocation.AsChar();

		// increase the counter as we found another sound file
		counter++;
	}

	return "";
}


// get the asset allow streaming flag with the given index of the given type
bool StateMachine::GetAssetAllowStreamingOfType(Action::AssetType type, uint32 index)
{
	CollectAssets();

	uint32 counter = 0;

	// get the number of assets and iterate through them all
	const uint32 numAssets = mAssets.Size();
	for (uint32 i=0; i<numAssets; ++i)
	{
		// skip all other than the given type
		const StateMachine::Asset& asset = mAssets[i];
		if (asset.mType != type)
			continue;

		// compare the current asset counter with the given index
		if (index == counter)
			return asset.mAllowStreaming;

		// increase the counter as we found another sound file
		counter++;
	}

	return "";
}


StateMachine::Asset* StateMachine::FindAssetByLocation(const char* location)
{
	const uint32 numAssets = mAssets.Size();
	for (uint32 i = 0; i < numAssets; ++i)
	{
		if (mAssets[i].mLocation.IsEqual(location) == true)
			return &mAssets[i];
	}

	return NULL;
}


void StateMachine::SetAssetActivity(const char* location, Asset::EActivity activity)
{
	Asset* asset = FindAssetByLocation(location);

	// asset not found
	if (asset == NULL)
		return;

	asset->mActivity = activity;
}


void StateMachine::SetAssetGroupActivity(Action::AssetType type, Asset::EActivity activity)
{
	// set activity of all assets of this type
	const uint32 numAssets = mAssets.Size();
	for (uint32 i = 0; i < numAssets; ++i)
	{
		if (mAssets[i].mType == type)
			mAssets[i].mActivity = activity;
	}
}
