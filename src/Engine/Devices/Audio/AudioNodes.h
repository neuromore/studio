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

#ifndef __NEUROMORE_AUDIONODES_H
#define __NEUROMORE_AUDIONODES_H

// include the required headers
#include "../../Config.h"
#include "AudioDevices.h"
#include "../../Graph/DeviceInputNode.h"
#include "../../Graph/DeviceOutputNode.h"
#include "../../Core/StandardHeaders.h"


// base class for all audio input nodes
class ENGINE_API AudioInputNodeBase : public DeviceInputNode
{
	public:

		enum {	ATTRIB_SAMPLERATE = NUM_DEVICENODEATTRIBUTES };

		// constructor & destructor
		AudioInputNodeBase(Graph* parentGraph, const uint32 deviceType) : DeviceInputNode(parentGraph, deviceType)	{}
		virtual ~AudioInputNodeBase();

		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;
		

		enum EError
		{
			ERROR_CANNOT_CHANGE_SAMPLERATE	= GraphObjectError::ERROR_RUNTIME		| 0x01,
			ERROR_DEVICE_NOT_FOUND			= GraphObjectError::ERROR_RUNTIME		| 0x02,
			ERROR_DEVICE_LOCKED				= GraphObjectError::ERROR_RUNTIME		| 0x03,
			ERROR_WRONG_SAMPLERATE			= GraphObjectError::ERROR_CONFIGURATION | 0x01,
		};
};

// base class for all audio output nodes (unfortunately we need both types)
class AudioOutputNodeBase : public DeviceOutputNode
{
	public:
		enum { ATTRIB_SAMPLERATE = NUM_DEVICENODEATTRIBUTES };

		// constructor & destructor
		AudioOutputNodeBase(Graph* parentGraph, const uint32 deviceType) : DeviceOutputNode(parentGraph, deviceType)	{}
		virtual ~AudioOutputNodeBase()																					{}

		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		enum EError
		{
			ERROR_CANNOT_CHANGE_SAMPLERATE	= GraphObjectError::ERROR_RUNTIME		| 0x01,
			ERROR_DEVICE_NOT_FOUND			= GraphObjectError::ERROR_RUNTIME		| 0x02,
			ERROR_DEVICE_LOCKED				= GraphObjectError::ERROR_RUNTIME		| 0x03,
			ERROR_WRONG_SAMPLERATE			= GraphObjectError::ERROR_CONFIGURATION | 0x01,
		};
		
		enum EWarning
		{
			WARNING_NO_INPUT				= GraphObjectWarning::WARNING_CUSTOM	| 0x01,
		};
};


//
// Audio Input, two channel
//

class AudioInputNode : public AudioInputNodeBase
{
	public:
		enum { TYPE_ID = 0xD00000 | AudioInputDevice::TYPE_ID };

		uint32 GetType() const override											{ return TYPE_ID; }
		static const char* Uuid()												{ return "c2d15dd6-a4f1-11e5-bf7f-feff819cdc9f"; }

		// constructor & destructor
		AudioInputNode(Graph* parentGraph) : AudioInputNodeBase(parentGraph, AudioInputDevice::TYPE_ID)			{}
		virtual ~AudioInputNode()																				{}

		Core::Color GetColor() const override									{ return Core::RGBA(30, 180, 158); } // one of the colors from the Muse logo
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Audio Input"; }
		GraphObject* Clone(Graph* parentObject) override						{ AudioInputNode* clone = new AudioInputNode(parentObject); return clone; }
};



//
// Audio Input, one channel
//

class MicrophoneNode : public AudioInputNodeBase
{
	public:
		enum { TYPE_ID = 0xD00000 | MicrophoneDevice::TYPE_ID };
		static const char* Uuid()												{ return "ef760c9c-a4f1-11e5-bf7f-feff819cdc9f"; }

		// constructor & destructor
		MicrophoneNode(Graph* parentGraph) : AudioInputNodeBase(parentGraph, MicrophoneDevice::TYPE_ID)			{}
		virtual ~MicrophoneNode()																				{}

		Core::Color GetColor() const override								{ return Core::RGBA(30, 180, 158); } // one of the colors from the Muse logo
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Microphone"; }
		GraphObject* Clone(Graph* parentObject) override						{ MicrophoneNode* clone = new MicrophoneNode(parentObject); return clone; }
};



//
// Audio Output, two channel
//

class AudioOutputNode : public AudioOutputNodeBase
{
	public:
		enum { TYPE_ID = 0xD00000 | AudioOutputDevice::TYPE_ID };
		static const char* Uuid()												{ return "a5fd8ecc-a50b-11e5-bf7f-feff819cdc9f"; }

		// constructor & destructor
		AudioOutputNode(Graph* parentGraph) : AudioOutputNodeBase(parentGraph, AudioOutputDevice::TYPE_ID)			{}
		virtual ~AudioOutputNode()																					{}

		Core::Color GetColor() const override								{ return Core::RGBA(30, 180, 158); } // one of the colors from the Muse logo
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Audio Output"; }
		GraphObject* Clone(Graph* parentObject) override						{ AudioOutputNode* clone = new AudioOutputNode(parentObject); return clone; }
};
#endif
