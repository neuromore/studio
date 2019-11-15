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

#ifndef __NEUROMORE_BINSELECTORNODE_H
#define __NEUROMORE_BINSELECTORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/ChannelProcessor.h"
#include "../DSP/FrequencyBand.h"


class ENGINE_API BinSelectorNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0040 };
		static const char* Uuid () { return "44dec436-66c6-11e5-9d70-feff819cdc9f"; }

		enum
		{
			ATTRIB_USEMULTICHANNEL	= 0,
			ATTRIB_MINFREQ			= 1,
			ATTRIB_MAXFREQ			= 2,
			ATTRIB_NUMOUTPUTPORTS	= 3,
			ATTRIB_LOCK_PORTS		= 4,
		};
		
		enum
		{
			INPUTPORT_SPECTRUM		= 0,
		};

		enum 
		{
			PORTID_INPUT_SPECTRUM	= 0,
			PORTID_OUTPUT_CHANNEL   = 1
		};

		// constructor & destructor
		BinSelectorNode(Graph* graph);
		~BinSelectorNode();

		// initialize
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delt) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override								{ return Core::RGBA(255,213,141); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Select Bins"; }
		const char* GetRuleName() const override final							{ return "NODE_BinSelector"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override								{ BinSelectorNode* clone = new BinSelectorNode(graph); return clone; }
		bool IsUnstable() const override final									{ return true; }

	private:
		bool			mUseMultiChannel;
		FrequencyBand	mFrequencyBand;				// selected frequency band
		Spectrum		mConfigSpectrum;			// copy of first input spectrum, so we know the layout (don't care about the values)

		Core::Array<Channel<double>> mOutputChannels;		// one channel per bin
		
		void ReInitOutputPorts(const Spectrum& spectrum, const FrequencyBand& band);
		void ReInitOutputChannels(const Spectrum& spectrum, const FrequencyBand& band);
		void DeleteOutputChannels();

		// small bin index helpers
		uint32 GetMinBinIndex(const Spectrum& spectrum, const FrequencyBand& band)	const		{ return spectrum.CalcBinIndex(band.GetMinFrequency()); }
		uint32 GetMaxBinIndex(const Spectrum& spectrum, const FrequencyBand& band)	const		{ return spectrum.CalcBinIndex(band.GetMaxFrequency()); }
		int32 GetNumBins(const Spectrum& spectrum, const FrequencyBand& band)	const			{ int32 num = GetMaxBinIndex(spectrum, band) - GetMinBinIndex(spectrum, band) + 1; return (num > 0 ? num : 0); }

		// find number of valid input channels, falls back to 1 or 0, depending on case of failure
		uint32 FindNumInputChannels();
};


#endif
