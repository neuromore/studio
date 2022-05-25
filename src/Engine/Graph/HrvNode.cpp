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
#include "HrvNode.h"


using namespace Core;

// constructor
HrvNode::HrvNode(Graph* graph) : ProcessorNode( graph, new HrvProcessor() )
{
	mSettings.mOutputSampleRate	= 128;
	mSettings.mTimeDomainMethod	= HrvTimeDomain::METHOD_RMSSD;
	mSettings.mNumRRIntervals	= 10;
}


// destructor
HrvNode::~HrvNode()
{
}


// initialize the node
void HrvNode::Init()
{
	// init base class first
	ProcessorNode::Init();

	// SETUP PORTS

	GetInputPort(INPUTPORT_CHANNEL).Setup("RR", "x", AttributeChannels<double>::TYPE_ID, PORTID_INPUT);
	GetOutputPort(OUTPUTPORT_CHANNEL).Setup("Out", "y", AttributeChannels<double>::TYPE_ID, PORTID_OUTPUT);
	GetOutputPort(OUTPUTPORT_CHANNEL).SetName( HrvTimeDomain::GetName(mSettings.mTimeDomainMethod) );
	
	// ATTRIBUTES

	// analysis method
	const uint32 defaultMode = (uint32)mSettings.mTimeDomainMethod;
	AttributeSettings* methodAttr = RegisterAttribute("Method", "Method", "The calculated HRV metric.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	methodAttr->ResizeComboValues(HrvTimeDomain::NUM_TIME_DOMAIN_METHODS);
	for (uint32 i = 0; i < HrvTimeDomain::NUM_TIME_DOMAIN_METHODS; i++)
		methodAttr->SetComboValue(i, HrvTimeDomain::GetName((HrvTimeDomain::EMethod)i));
	methodAttr->SetDefaultValue(AttributeInt32::Create(defaultMode));

	// num RR intervals
	AttributeSettings* numRRAttr = RegisterAttribute("Number of RR Intervals", "NumRRIntervals", "How many RR intervals are used in the analysis.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numRRAttr->SetDefaultValue( AttributeInt32::Create(mSettings.mNumRRIntervals) );
	numRRAttr->SetMinValue( AttributeInt32::Create(2) );
	numRRAttr->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );
}


// re-initialize the node
void HrvNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	ProcessorNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// start the node
void HrvNode::Start(const Time& elapsed)
{
	// create and reinit processors etc
	ProcessorNode::Start(elapsed);

	// update output port names
	GetOutputPort(OUTPUTPORT_CHANNEL).SetName(HrvTimeDomain::GetName(mSettings.mTimeDomainMethod));
}


// update the node
void HrvNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update baseclass
	ProcessorNode::Update(elapsed, delta);
}


// attributes have changed
void HrvNode::OnAttributesChanged()
{
	// check if attributes have changed
	const uint32 methodID = GetInt32Attribute(ATTRIB_METHOD);
	const uint32 numRRIntervals = GetInt32Attribute(ATTRIB_NUM_RR_INTERVALS);

	// do not reinit if nothing has changed
	if (mSettings.mNumRRIntervals == numRRIntervals && methodID == (uint32)mSettings.mTimeDomainMethod)
		return;

	mSettings.mTimeDomainMethod = (HrvTimeDomain::EMethod)methodID;
	mSettings.mNumRRIntervals	= numRRIntervals;
	
	GetOutputPort(OUTPUTPORT_CHANNEL).SetName(HrvTimeDomain::GetName(mSettings.mTimeDomainMethod));

	ResetAsync();
}
