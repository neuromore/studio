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

#ifndef __NEUROMORE_INPUTNODE_H
#define __NEUROMORE_INPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"
#include "../Sensor.h"


class ENGINE_API InputNode : public SPNode
{
	public:
		enum { NODE_TYPE			= 0x003 };
		
		enum
		{
			ATTRIB_UPLOAD = 0,
			NUM_INPUTNODEATTRIBUTES
		};

		// constructor & destructor
		InputNode(Graph* graph);
		virtual ~InputNode();
		
		// type management
		virtual uint32 GetNodeType() const override							{ return NODE_TYPE; }

		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

	    // synchronize the the input channels (sensors) so the next sample that falls on this point in time
		void Sync(double syncTime) override;

		// override Start() and align output channels to absolute time, not relative to input
		virtual void Start(const Core::Time& elapsed) override;

		// get upload flag
		bool IsUploadEnabled() const										{ return GetBoolAttribute(ATTRIB_UPLOAD); }

		virtual uint32 GetNumSensors() const = 0;
		virtual Sensor* GetSensor(uint32 index) = 0;
		
		// implement this in case the node is the source of the input samples
		virtual void GenerateSamples()			{}

		double FindMaximumInputLatency();
		double FindMaximumLatency() override								{ return FindMaximumInputLatency(); }


		uint32 CalculateInputMemoryUsed();
};


#endif
