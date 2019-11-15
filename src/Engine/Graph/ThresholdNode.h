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

#ifndef __NEUROMORE_THRESHOLDNODE_H
#define __NEUROMORE_THRESHOLDNODE_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../Core/Array.h"
#include "../DSP/Channel.h"
#include "ProcessorNode.h"


class ENGINE_API ThresholdNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0037 };
		static const char* Uuid() { return "93eb63fc-5962-11e5-885d-feff819cdc9f"; }

		enum
		{
			ATTRIB_INTERVAL_LENGTH	= 0,
			ATTRIB_USE_RANGE,
			ATTRIB_LOW_THRESHOLD,
			ATTRIB_HIGH_THRESHOLD,
			ATTRIB_THRESHOLD_FUNCTION,
			ATTRIB_THRESHOLD_RANGE_FUNCTION,
		};

		enum
		{
			INPUTPORT_VALUE = 0,
			INPUTPORT_LOW_THRESHOLD,
			INPUTPORT_HIGH_THRESHOLD,
		};
		
		enum 
		{
			OUTPUTPORT_RATIO = 0,
			OUTPUTPORT_AUGMENT,
			OUTPUTPORT_INHIBIT,
		};

		// constructor & destructor
		ThresholdNode(Graph* graph);
		~ThresholdNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		Core::Color GetColor() const override								{ return Core::RGBA(204, 255, 51); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "Threshold"; }
		const char* GetRuleName() const override final						{ return "NODE_Threshold"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_BIO; }
		GraphObject* Clone(Graph* graph) override							{ ThresholdNode* clone = new ThresholdNode(graph); return clone; }

		virtual void OnAttributesChanged() override;

		void SetRangeModeEnabled(bool enabled);
		
		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:
		typedef bool (CORE_CDECL *CompareFunction)(double x, double y);

		// Single-value Compare Functions
		static bool CORE_CDECL CompareEqual(double x, double y);
		static bool CORE_CDECL CompareGreater(double x, double y);
		static bool CORE_CDECL CompareLess(double x, double y);
		static bool CORE_CDECL CompareGreaterOrEqual(double x, double y);
		static bool CORE_CDECL CompareLessOrEqual(double x, double y);
		static bool CORE_CDECL CompareInequal(double x, double y);

		enum ECompareFunction
		{
			FUNCTION_EQUAL = 0,
			FUNCTION_GREATER,
			FUNCTION_LESS,
			FUNCTION_GREATEROREQUAL,
			FUNCTION_LESSOREQUAL,
			FUNCTION_INEQUAL,
			NUM_FUNCTIONS
		};

		//////////////

		typedef bool (CORE_CDECL *RangeCompareFunction)(double x, double a, double b);

		// Single-value Compare Functions
		static bool CORE_CDECL RangeCompareWithinExclusive(double x, double a, double b);
		static bool CORE_CDECL RangeCompareWithinInclusive(double x, double a, double b);
		static bool CORE_CDECL RangeCompareOutsideExclusive(double x, double a, double b);
		static bool CORE_CDECL RangeCompareOutsideInclusive(double x, double a, double b);

		enum ERangeCompareFunction
		{
			FUNCTION_WITHIN_EXCLUSIVE = 0,
			FUNCTION_WITHIN_INCLUSIVE,
			FUNCTION_WITHOUT_EXCLUSIVE,
			FUNCTION_WITHOUT_INCLUSIVE,
			NUM_RANGE_FUNCTIONS
		};


		/////////////

	// Node Processor and settings
	protected:

		// settings fot the node processor
		class ProcessorSettings : public ChannelProcessor::Settings
		{
		public:
			enum { TYPE_ID = 0x0037 };

			ProcessorSettings()			 {}
			virtual ~ProcessorSettings() {}

			uint32 GetType() const override		{ return TYPE_ID; }
			
			// interval
			double	mIntervalDuration;			// length of the interval, expressed in seconds
			uint32	mNumSamples;				// length of the interval, expressed in samples

			// thresholds and range mode
			bool	mUseRange;
			double	mLowThreshold;		// also used for threshold in non-range mode
			double	mHighThreshold;	

			// compare modes
			ECompareFunction	mCompareFunction;
			CompareFunction		mCalculateCompareFunc;

			// compare modes
			ERangeCompareFunction	mRangeCompareFunction;
			RangeCompareFunction	mCalculateRangeCompareFunc;
		};

		ProcessorSettings	mSettings;

		// the node's channel processor
		class Processor : public ChannelProcessor
		{
			enum { TYPE_ID = 0x0037 };

		public:
			Processor() : ChannelProcessor()						{ Init(); }
			~Processor() { }

			uint32 GetType() const override							{ return TYPE_ID; }
			ChannelProcessor* Clone() override						{ Processor* clone = new Processor(); clone->Setup(mSettings); return clone; }

			// settings
			void Setup(const ChannelProcessor::Settings& settings) override;
			virtual const Settings& GetSettings() const override	{ return mSettings; }

			void Init() override
			{
				AddInput<double>();
				AddInput<double>();
				AddInput<double>();

				AddOutput<double>();	
				AddOutput<double>();
				AddOutput<double>();
			}

			void ReInit() override;
			void Update() override;

			// properties
			double GetLatency(uint32 inputPortIndex, uint32 outputPortIndex) const override		{ return 0.0; }
			uint32 GetNumEpochSamples(uint32 inputPortIndex) const override						{ return mSettings.mNumSamples; }

		private:
			void SetNumSamplesFromDuration();
			
			ProcessorSettings		mSettings;

			
		};
};


#endif
