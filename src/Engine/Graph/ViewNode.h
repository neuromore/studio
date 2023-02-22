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

#ifndef __NEUROMORE_VIEWNODE_H
#define __NEUROMORE_VIEWNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API ViewNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0033 };
		static const char* Uuid () { return "8b55d922-3200-11e5-a151-feff819cdc9f"; }
		
		static const constexpr uint32_t VIEWDURATION    = 30;   // 30s
		static const constexpr uint32_t VIEWDURATIONMAX = 1200; // 20min
		
		//
		enum
		{
			INPUTPORT_DOUBLE	= 0,
			INPUTPORT_SPECTRUM,
			INPUTPORT_ENABLE,
		};

		enum
		{
			ATTRIB_SCALING		= 0,
			ATTRIB_RANGEMIN,
			ATTRIB_RANGEMAX,
			ATTRIB_USECOLOR,
			ATTRIB_COLORPICK,
			ATTRIB_COMBINE,
		};

		enum EScalingMode
		{
			SCALING_PEAK		= 0,
			SCALING_STDDEV,
			SCALING_CUSTOM
		};

		// constructor & destructor
		ViewNode(Graph* graph);
		~ViewNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;
		
		Core::Color GetColor() const override									{ return Core::RGBA(0,229,189); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "View"; }
		const char* GetRuleName() const override final							{ return "NODE_View"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_OUTPUT; }
		GraphObject* Clone(Graph* graph) override								{ ViewNode* clone = new ViewNode(graph); return clone; }

		// get visual configuration
		EScalingMode GetScalingMode() const										{ return (EScalingMode)GetInt32Attribute(ATTRIB_SCALING); }

		// per channel visual configuration
		bool CustomColor() const												{ return GetBoolAttribute(ATTRIB_USECOLOR); }
		Core::Color GetCustomColor() const										{ return GetColorAttribute(ATTRIB_COLORPICK); }
		bool CombinedView() const												{ return GetBoolAttribute(ATTRIB_COMBINE); }
		
		double GetRangeMin(uint32 index) const									{ if (index < mRangesMin.Size()) return mRangesMin[index]; return 0.0; }
		double GetRangeMax(uint32 index) const									{ if (index < mRangesMax.Size()) return mRangesMax[index]; return 0.0; }

		// get the enable flag
		bool GetEnableValue() const												{ return mEnableValue; }
		
		// get input channels
		uint32 GetNumDoubleChannels();
		MultiChannel* GetDoubleChannels();
		Channel<double>* GetDoubleChannel(uint32 index);

		// get input from spectrum port
		uint32 GetNumSpectrumChannels();
		MultiChannel* GetSpectrumChannels();
		Channel<Spectrum>* GetSpectrumChannel(uint32 index);

		inline void SetViewDuration(double seconds)									
		{ 
			mViewDuration = seconds;
			SyncBufferSize(false);
		}
		
		virtual uint32 GetNumEpochSamples(uint32 inputPortIndex) const override;

	private:
		void CalculateScalingRange();
		void SyncBufferSize(bool discard = false);

		double				mViewDuration;
		double				mMaxViewDuration;

		Core::Array<double>	mRangesMin;
		Core::Array<double>	mRangesMax;

		bool				mEnableValue;


};


#endif
