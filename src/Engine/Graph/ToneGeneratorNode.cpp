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
#include "ToneGeneratorNode.h"

using namespace Core;

// constructor
ToneGeneratorNode::ToneGeneratorNode(Graph* graph) : SPNode(graph)
{
}

// destructor
ToneGeneratorNode::~ToneGeneratorNode()
{
}

// initialize the node
void ToneGeneratorNode::Init()
{
   // init base class first
   SPNode::Init();

   // CONFIG SPNODE
   RequireSyncedInput();

   // SETUP PORTS
   InitInputPorts(1);
   GetInputPort(INPUTPORT_FREQUENCY).Setup("Frequency", "freq", AttributeChannels<double>::TYPE_ID, INPUTPORT_FREQUENCY);
}


void ToneGeneratorNode::Start(const Time& elapsed)
{
   SPNode::Start(elapsed);
}

void ToneGeneratorNode::ReInit(const Time& elapsed, const Time& delta)
{
   if (BaseReInit(elapsed, delta) == false)
      return;

   // reinit baseclass
   SPNode::ReInit(elapsed, delta);

   PostReInit(elapsed, delta);
}


// update the node
void ToneGeneratorNode::Update(const Time& elapsed, const Time& delta)
{
   if (BaseUpdate(elapsed, delta) == false)
      return;

   // update the baseclass
   SPNode::Update(elapsed, delta);
}


void ToneGeneratorNode::OnAttributesChanged()
{
}
