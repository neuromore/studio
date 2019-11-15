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

#ifndef __NEUROMORE_AVECOLORNODE_H
#define __NEUROMORE_AVECOLORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API AVEColorNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0052 };
		static const char* Uuid () { return "095015d0-b7cc-11e5-9912-ba0be0483c18"; }

		enum
		{
			INPUTPORT_RED = 0,
			INPUTPORT_GREEN,
			INPUTPORT_BLUE,
		};

		enum
		{
			ATTRIB_OUTPUTTYPE = 0
		};

		enum EOutputType
		{
			OUTPUTTYPE_EXPERIENCE_BACKGROUND,
			OUTPUTTYPE_EXPERIENCE_AVE4,
			NUM_OUTPUTTYPES
		};

		// constructor & destructor
		AVEColorNode(Graph* graph);
		~AVEColorNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(197, 229, 146); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "AVE Color"; }
		const char* GetRuleName() const override final							{ return "NODE_AveColor"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_OUTPUT; }
		GraphObject* Clone(Graph* graph) override								{ AVEColorNode* clone = new AVEColorNode(graph); return clone; }

		EOutputType GetOutputType() const										{ return (EOutputType)GetInt32Attribute(ATTRIB_OUTPUTTYPE); }

		uint32 GetNumColorChannels() const;
		const Core::Color& GetColor(uint32 index) const							{ return mColors[index]; }
	

		enum EErrors
		{
			ERROR_WRONG_INPUT	= GraphObjectError::ERROR_CONFIGURATION		| 0x01,

		};

	private:
		Core::Array<Core::Color> mColors;

		// arrays for callback
		Core::Array<float> mRed;
		Core::Array<float> mGreen;
		Core::Array<float> mBlue;
		Core::Array<float> mAlpha;

};


#endif
