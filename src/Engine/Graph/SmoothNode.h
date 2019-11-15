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

#ifndef __NEUROMORE_SMOOTHINGNODE_H
#define __NEUROMORE_SMOOTHINGNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"


class ENGINE_API SmoothNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0013 };
		static const char* Uuid () { return "18994938-bb6c-11e4-8dfc-aa07a5b093db"; }
		
		enum 
		{
			ATTRIB_INTERPOLATIONSPEED	= 0,
			ATTRIB_STARTVALUE			= 1
		};

		enum
		{
			INPUTPORT					= 0,
			OUTPUTPORT					= 0
		};

		enum 
		{
			PORTID_INPUT				= 0,
			PORTID_OUTPUT				= 0
		};

		// constructor & destrucutor
		SmoothNode(Graph* graph);
		~SmoothNode();

		// initialize
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override									{ return Core::RGBA(103,188,219); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Smoothing"; }
		const char* GetRuleName() const override final							{ return "NODE_Smooth"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ SmoothNode* clone = new SmoothNode(graph); return clone; }
		
		const ChannelProcessor::Settings& GetSettings() override				{ return mSettings; }

	private:

		class ProcessorSettings : public ChannelProcessor::Settings
		{
			public:
				enum { TYPE_ID = 0x0013 };

				ProcessorSettings()			 {}
				virtual ~ProcessorSettings() {}
			
				uint32 GetType() const override		{ return TYPE_ID; }

				double mInterpolationSpeed;
				double mStartValue;
		};
		
		ProcessorSettings	mSettings;

		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x00013 };

			public:
				Processor() : ChannelProcessor()						{ Init();  mCurrentValue = 0; }
				~Processor() { }

				uint32 GetType() const override							{ return TYPE_ID; }
				ChannelProcessor* Clone() override						{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

				// settings
				void Setup(const ChannelProcessor::Settings& settings) override	{ mSettings = static_cast<const ProcessorSettings&>(settings); }
				virtual const Settings& GetSettings() const override	{ return mSettings; }

				void Init() override
				{
					AddInput<double>();
					AddOutput<double>();
				}

				void ReInit() override;
				void Update() override;

			private:
				ProcessorSettings	mSettings;
				double				mCurrentValue;
				bool				mIsFirstUpdate;
		};

};


#endif
