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

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "GraphObjectFactory.h"

// Inputs
#include "ParameterNode.h"
#include "OscInputNode.h"
#include "SignalGeneratorNode.h"
#include "CloudInputNode.h"
#ifndef PRODUCTION_BUILD
  #include "MultiParameterNode.h"
#endif
#include "SessionInfoNode.h"
#include "EegDeviceNode.h"

// Outputs
#include "CustomFeedbackNode.h"
#include "BodyFeedbackNode.h"
#include "PointsNode.h"
#include "OscOutputNode.h"
#include "SessionTimeNode.h"
#include "CloudOutputNode.h"
#ifndef PRODUCTION_BUILD
  #include "AVEColorNode.h"
#endif

#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_OSX) || defined(NEUROMORE_PLATFORM_LINUX)
  #include "FileReaderNode.h"
  #include "FileWriterNode.h"
#endif

// Math
#include "CompareNode.h"
#include "LogicNode.h"
#include "RemapNode.h"
#include "Math1Node.h"
#include "Math2Node.h"
#include "StatisticsNode.h"
#include "ChannelMathNode.h"
#ifndef PRODUCTION_BUILD
  #include "PairwiseMathNode.h"
#endif
#include "IntegralNode.h"

// DSP
#include "FFTNode.h"
#include "LinearFilterNode.h"
#include "FrequencyBandNode.h"
#include "DominantFrequencyNode.h"
#include "BiquadFilterNode.h"
#include "BinSelectorNode.h"
#include "OscillatorNode.h"
#include "WaveformNode.h"

// Bio Analysis
#include "HrvNode.h"
#include "ThresholdNode.h"
#include "AutoThresholdNode.h"

// Utils
#include "SmoothNode.h"
#include "DelayNode.h"
#include "ResampleNode.h"
#include "FreezeNode.h"
#include "ChannelMergerNode.h"
#include "ChannelSelectorNode.h"
#include "ChannelTransposerNode.h"
#include "ViewNode.h"
#include "RenameNode.h"
#include "RecolorNode.h"
#include "ChannelInfoNode.h"
#include "MetaInfoNode.h"
#include "SampleGateNode.h"
#include "AnnotationNode.h"
#include "ColorWheelNode.h"
#include "SwitchNode.h"

// state machine
#include "EntryState.h"
#include "ExitState.h"
#include "ActionState.h"
#ifndef PRODUCTION_BUILD
  #include "SyncState.h"
#endif
#include "TimerState.h"
#include "StateTransition.h"

// state transition conditions
#include "StateTransitionTimeCondition.h"
#include "StateTransitionFeedbackCondition.h"
#include "StateTransitionButtonCondition.h"
#include "StateTransitionAudioCondition.h"
#include "StateTransitionVideoCondition.h"

// actions
#include "Actions.h"

// in development
#ifndef PRODUCTION_BUILD
	#include "LoretaNode.h"
#endif



using namespace Core;

// constructor
GraphObjectFactory::GraphObjectFactory()
{
	// reserve space for the registered object types
	mRegisteredObjects.Reserve( 128 );

	// register default object types
	
	// graph nodes

		// Inputs
		RegisterObjectType( new ParameterNode(NULL) );
		RegisterObjectType( new SignalGeneratorNode(NULL) );
		RegisterObjectType( new OscInputNode(NULL) );
		RegisterObjectType( new SessionTimeNode(NULL) );
		RegisterObjectType( new CloudInputNode(NULL) );
#ifndef PRODUCTION_BUILD
		RegisterObjectType( new MultiParameterNode(NULL) );
#endif
		RegisterObjectType( new SessionInfoNode(NULL) );
		RegisterObjectType( new EegDeviceNode(NULL) );

		// Outputs
		RegisterObjectType( new CustomFeedbackNode(NULL) );
		RegisterObjectType( new BodyFeedbackNode(NULL) );
		RegisterObjectType( new OscOutputNode(NULL) );
		RegisterObjectType( new PointsNode(NULL) );
		RegisterObjectType( new CloudOutputNode(NULL) );

#ifndef PRODUCTION_BUILD
		RegisterObjectType( new AVEColorNode(NULL) );
#endif

#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_OSX) || defined(NEUROMORE_PLATFORM_LINUX)
		RegisterObjectType( new FileReaderNode(NULL) );
		RegisterObjectType( new FileWriterNode(NULL) );
#endif

		// Math
		RegisterObjectType( new Math1Node(NULL) );
		RegisterObjectType( new Math2Node(NULL) );
		RegisterObjectType( new CompareNode(NULL) );
		RegisterObjectType( new LogicNode(NULL) );
		RegisterObjectType( new RemapNode(NULL) );
		RegisterObjectType( new ChannelMathNode(NULL) );
		RegisterObjectType( new StatisticsNode(NULL) );
#ifndef PRODUCTION_BUILD
		RegisterObjectType( new PairwiseMathNode(NULL) );
#endif
		RegisterObjectType( new IntegralNode(NULL) );

		// DSP nodes
		RegisterObjectType( new FFTNode(NULL) );
		RegisterObjectType( new LinearFilterNode(NULL) );
		RegisterObjectType( new FrequencyBandNode(NULL) );
		RegisterObjectType( new DominantFrequencyNode(NULL) );
		RegisterObjectType( new BiquadFilterNode(NULL) );
		RegisterObjectType( new BinSelectorNode(NULL) );
		RegisterObjectType( new OscillatorNode(NULL) );
		RegisterObjectType( new WaveformNode(NULL) );
		
		// Bio Analysis
		RegisterObjectType( new HrvNode(NULL) );
		RegisterObjectType( new ThresholdNode(NULL) );
		RegisterObjectType( new AutoThresholdNode(NULL) );

		// Utils
		RegisterObjectType( new SmoothNode(NULL) );
		RegisterObjectType( new DelayNode(NULL) );
		RegisterObjectType( new ResampleNode(NULL) );
		RegisterObjectType( new FreezeNode(NULL) );
		RegisterObjectType( new ChannelMergerNode(NULL) );
		RegisterObjectType( new ChannelSelectorNode(NULL) );
		RegisterObjectType( new ChannelTransposerNode(NULL) );
		RegisterObjectType( new ViewNode(NULL) );
		RegisterObjectType( new RenameNode(NULL) );
		RegisterObjectType( new RecolorNode(NULL) );
		RegisterObjectType( new ChannelInfoNode(NULL) );
		RegisterObjectType( new MetaInfoNode(NULL) );
		RegisterObjectType( new SampleGateNode(NULL) );
		RegisterObjectType( new AnnotationNode(NULL) );
		RegisterObjectType( new ColorWheelNode(NULL) );
		RegisterObjectType( new SwitchNode(NULL) );

	// state machine

		// states
		RegisterObjectType( new EntryState(NULL) );
		RegisterObjectType( new ExitState(NULL) );
		RegisterObjectType( new ActionState(NULL) );
#ifndef PRODUCTION_BUILD
		RegisterObjectType( new SyncState(NULL) );
#endif
		RegisterObjectType( new TimerState(NULL) );

		// state transition
		RegisterObjectType( new StateTransition(NULL) );

		// transition conditions
		RegisterObjectType( new StateTransitionTimeCondition(NULL) );
		RegisterObjectType( new StateTransitionFeedbackCondition(NULL) );
		RegisterObjectType( new StateTransitionButtonCondition(NULL) );
		RegisterObjectType( new StateTransitionAudioCondition(NULL) );
		RegisterObjectType( new StateTransitionVideoCondition(NULL) );

		// actions
		RegisterObjectType( new PlayAudioAction(NULL) );
		RegisterObjectType( new PauseAudioAction(NULL) );
		RegisterObjectType( new SeekAudioAction(NULL) );
		RegisterObjectType( new StopAudioAction(NULL) );

		RegisterObjectType( new PlayVideoAction(NULL) );
		RegisterObjectType( new StopVideoAction(NULL) );
		RegisterObjectType( new PauseVideoAction(NULL) );
		RegisterObjectType( new SeekVideoAction(NULL) );

		RegisterObjectType( new ShowImageAction(NULL) );
		RegisterObjectType( new HideImageAction(NULL) );

		RegisterObjectType( new ShowTextAction(NULL) );
		RegisterObjectType( new HideTextAction(NULL) );

		RegisterObjectType( new SetBackgroundColorAction(NULL) );

		RegisterObjectType( new CommandAction(NULL) );
		RegisterObjectType( new ParameterAction(NULL) );
		RegisterObjectType( new ClearButtonsAction(NULL) );

#ifndef PRODUCTION_BUILD
		// LORETA
		RegisterObjectType( new LoretaNode(NULL) );
#endif
}


// destructor
GraphObjectFactory::~GraphObjectFactory()
{
	// for all registered objects
	for (uint32 i=0; i<mRegisteredObjects.Size(); ++i)
	{
		GraphObject* object = mRegisteredObjects[i];
		delete object;
	}

	mRegisteredObjects.Clear();
}


// create new object
GraphObject* GraphObjectFactory::CreateObjectByTypeUuid(Graph* graph, const char* typeUuid)
{
	const uint32 index = FindRegisteredObjectByTypeUuid( typeUuid );
	if (index == CORE_INVALIDINDEX32)
	{
		LogWarning("GraphObjectFactory::CreateObject(): Failed to create object");
		return NULL;
	}

	// check if t

	// create object
	GraphObject* result = mRegisteredObjects[index]->Clone(graph);
	result->Init();

	// create default attribute values
	result->CreateDefaultAttributeValues();

	return result;
}


// create new object
GraphObject* GraphObjectFactory::CreateObjectByTypeID(Graph* graph, uint32 typeID)
{
	const uint32 index = FindRegisteredObjectByTypeID( typeID );
	if (index == CORE_INVALIDINDEX32)
	{
		LogWarning("GraphObjectFactory::CreateObject(): Failed to create object");
		return NULL;
	}

	// create object
	GraphObject* result = mRegisteredObjects[index]->Clone(graph);
	if (result == NULL)
	{
		LogWarning("GraphObjectFactory::CreateObject(): Failed to create object");
		return NULL;
	}

	// init it
	result->Init();

	// create default attribute values
	result->CreateDefaultAttributeValues();

	return result;
}


// create new object
GraphObject* GraphObjectFactory::CreateObject(Graph* graph, GraphObject* registeredObject)
{
	CORE_ASSERT( registeredObject != NULL );

	const uint32 index = FindRegisteredObject( registeredObject );
	if (index == CORE_INVALIDINDEX32)
	{
		LogWarning("GraphObjectFactory::CreateObject(): Failed to create object");
		return NULL;
	}

	// clone object
	GraphObject* result = mRegisteredObjects[index]->Clone(graph);

	// init it
	result->Init();

	// create default attribute values
	result->CreateDefaultAttributeValues();

	return result;
}


// register object
bool GraphObjectFactory::RegisterObjectType(GraphObject* object)
{
	if (FindRegisteredObject(object) != CORE_INVALIDINDEX32 ||
		FindRegisteredObjectByTypeID(object->GetType()) != CORE_INVALIDINDEX32 ||
		FindRegisteredObjectByTypeUuid(object->GetTypeUuid()) != CORE_INVALIDINDEX32)
	{
		CORE_ASSERT(false);
		LogWarning( "GraphObjectFactory::RegisterObjectType(): Already exists" );
		return false;
	}

	// register new object
	mRegisteredObjects.Add( object );
	return true;
}
