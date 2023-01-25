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

#ifndef __NEUROMORE_FFTNODE_H
#define __NEUROMORE_FFTNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ProcessorNode.h"
#include "../DSP/FFTProcessor.h"


class ENGINE_API FFTNode : public ProcessorNode
{
	public:
		enum { TYPE_ID = 0x0016 };
		static const char* Uuid () { return "316ff94a-bb6a-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_FFTORDER			= 0,
			ATTRIB_WINDOWFUNCTION	= 1,
			ATTRIB_SHIFTSAMPLES		= 2,
			ATTRIB_ZEROPADDING		= 3

		};

		enum
		{
			INPUTPORT_CHANNEL		= 0,
			OUTPUTPORT_SPECTRUM		= 0
		};

		enum 
		{
			PORTID_INPUT_SAMPLE		= 0,
			PORTID_OUTPUT_SPECTRUM	= 1
		};

		
		// constructor & destructor
		FFTNode(Graph* graph);
		~FFTNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(128,22,255); }
		uint32 GetType() const override										{ return TYPE_ID; }
		const char* GetTypeUuid() const override final						{ return Uuid(); }
		const char* GetReadableType() const override						{ return "FFT"; }
		const char* GetRuleName() const override final						{ return "NODE_FFT"; }
		uint32 GetPaletteCategory() const override							{ return CATEGORY_DSP; }
		GraphObject* Clone(Graph* graph) override							{ FFTNode* clone = new FFTNode(graph); return clone; }

		const ChannelProcessor::Settings& GetSettings() override			{ return mSettings; }

	private:
		FFTProcessor::FFTSettings  mSettings;

};


#endif
