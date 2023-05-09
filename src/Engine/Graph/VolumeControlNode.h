/****************************************************************************
**
** Copyright 2022 neuromore co
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

#ifndef __NEUROMORE_VOLUMECONTROLNODE_H
#define __NEUROMORE_VOLUMECONTROLNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "CustomFeedbackNode.h"

// forward declaration
class Classifier;

class ENGINE_API VolumeControlNode : public CustomFeedbackNode
{
	public:
		enum { TYPE_ID				= 0x0060 };
        enum { OUTPUTNODE_TYPE		= 0x004 };

        enum { INPUTPORT_VALUE		= 0 };

        enum
        {
            ATTRIB_MODE = NUM_FEEDBACKBASEATTRIBUTES,
            ATTRIB_FILENAME,
            ATTRIB_NODENAME,
        };

        enum Mode
        {
            STUDIO_VOLUME           = 0,
            OVERALL_SYSTEM_VOLUME   = 1,
            SINGLE_MEDIA_FILE       = 2,
            TONE                    = 3,
            VISUALIZATION           = 4,
            MODE_NUM                = 5
        };

        enum EError
		{
			ERROR_VALUE_RANGE			= GraphObjectError::ERROR_RUNTIME		| 0x02,
		};

		static const char* Uuid () { return "150e3357-e4ed-4e67-bbb1-c7cc731153da"; }

		VolumeControlNode(Graph* parentGraph);
		virtual ~VolumeControlNode();

        bool IsEmpty(uint32 channelIndex = 0) const;

        // get signal resolution
		Mode GetMode() const										{ return (Mode)GetInt32Attribute(ATTRIB_MODE); }
        const char* GetFileUrl() const                              { return GetStringAttribute(ATTRIB_FILENAME); }
        const char* GetToneName() const                             { return GetStringAttribute(ATTRIB_NODENAME); }

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		Core::Color GetColor() const override                                   { return Core::RGBA(0,229,189); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Volume Control"; }
		const char* GetRuleName() const override final							{ return "NODE_VOLUMECONTROL"; }
        uint32 GetOutputNodeType() const override								{ return OUTPUTNODE_TYPE; }
		GraphObject* Clone(Graph* graph) override								{ VolumeControlNode* clone = new VolumeControlNode(graph); return clone; }

        double GetRangeMin() const override                                     { return 0.0; }
        double GetRangeMax() const override                                     { return 1.0; }

        void WriteOscMessage(OscPacketParser::OutStream* outStream) override;

    private:
        void ShowAttributesForMode(Mode type);
        const char* GetModeString(Mode mode) const;
        Mode mCurrentMode;
        Core::String mOscAddress;
};

#endif
