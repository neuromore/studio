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

#ifndef __NEUROMORE_CHANNELTRANSPOSERNODE_H
#define __NEUROMORE_CHANNELTRANSPOSERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API ChannelTransposerNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0056 };
		static const char* Uuid () { return "54b59de4-3272-11e6-ac61-9e71128cae77"; }

		enum
		{
			ATTRIB_NUMINPUTPORTS = 0,
			ATTRIB_NUMOUTPUTPORTS,
		};

		
		// constructor & destructor
		ChannelTransposerNode(Graph* graph);
		~ChannelTransposerNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
	
		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(150,79,77); }
		uint32 GetType() const override									{ return TYPE_ID; }
		const char* GetTypeUuid() const override final					{ return Uuid(); }
		const char* GetReadableType() const override					{ return "Transpose Channels"; }
		const char* GetRuleName() const override final					{ return "NODE_ChannelTransposer"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override						{ ChannelTransposerNode* clone = new ChannelTransposerNode(graph); return clone; }

	private:
		uint32 mSizeIn;			// number of  input multichannels = number of channels in each output multichannel
		uint32 mSizeOut;		// number of output multichannels = number of channels in each  input multichannel

		// inputs
		void EnsureFreeInputPort();
		void UpdateInputPorts();
		bool CheckAndCollectInputChannels();

		// outputs
		void ReInitOutputPorts();							// create numPorts output ports and connect the 
		void ReInitOutputChannels();						// create the output channels (one for each selected input channel) and connect them to the outputs
		void DeleteOutputChannels();

		Core::Array< Core::Array<uint32> >			mTransposedInputs;		// the input channels, transposed and adressed by their index in mInputChannels (same dimensions as output array, we just copy the values over 1:1)
		Core::Array< Core::Array<ChannelBase*> >	mOutputChannels;		// the output channels (swapped dimensions compared to inputs)

};


#endif
