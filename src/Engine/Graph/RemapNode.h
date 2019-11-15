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

#ifndef __NEUROMORE_REMAPNODE_H
#define __NEUROMORE_REMAPNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"


class ENGINE_API RemapNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0010 };
		static const char* Uuid () { return "01b36c94-bb6c-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			INPUTPORT_X			= 0,
			INPUTPORT_MAX_IN	= 1,
			INPUTPORT_MIN_IN	= 2,
			INPUTPORT_MAX_OUT	= 3,
			INPUTPORT_MIN_OUT	= 4,
			OUTPUTPORT_RESULT	= 0
		};

		/*enum 
		{
			PORTID_INPUT_X			= 0,
			PORTID_INPUT_MAX		= 2,
			PORTID_INPUT_MIN		= 3,
			PORTID_OUTPUT_RESULT	= 1
		};
*/
		enum
		{
			ATTRIB_INPUTMIN		= 0,
			ATTRIB_INPUTMAX		= 1,
			ATTRIB_OUTPUTMIN	= 2,
			ATTRIB_OUTPUTMAX	= 3,
			ATTRIB_CLAMP		= 4
		};
		
		// constructor & destructor
		RemapNode(Graph* graph);
		~RemapNode();

		// initialize
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override									{ return Core::RGBA(128,255,255); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Remap"; }
		const char* GetRuleName() const override final							{ return "NODE_Remap"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_MATH; }
		GraphObject* Clone(Graph* graph) override								{ RemapNode* clone = new RemapNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0010 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				double	mMinInput;
				double	mMaxInput;
				double	mMinOutput;
				double	mMaxOutput;
				bool	mClampOutput;
		};

		ProcessorSettings	mSettings;

		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x00010 };

			public:
				Processor() : ChannelProcessor()						{ Init();  }
				~Processor() { }

				uint32 GetType() const override							{ return TYPE_ID; }
				ChannelProcessor* Clone() override						{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				virtual const Settings& GetSettings() const override	{ return mSettings; }

				void Init() override
				{
					AddInput<double>();
					AddInput<double>();
					AddInput<double>();
					AddInput<double>();
					AddInput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings		mSettings;
		};
};


#endif
