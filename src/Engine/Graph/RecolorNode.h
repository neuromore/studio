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

#ifndef __NEUROMORE_RECOLORNODE_H
#define __NEUROMORE_RECOLORNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API RecolorNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0059 };
		static const char* Uuid () { return "8ec88b4c-6f7e-11e6-8b77-86f30ca893d3"; }

		enum { OUTPUTPORT = 0 };
		enum { INPUTPORT = 0 };
		enum 
		{ 
			ATTRIB_CHANNELCOLORS = 0,
			ATTRIB_COLORPICKER 
		}; 

		// constructor & destructor
		RecolorNode(Graph* graph);
		~RecolorNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return mMainColor; }
		uint32 GetType() const override									{ return TYPE_ID; }
		const char* GetTypeUuid() const override final					{ return Uuid(); }
		const char* GetReadableType() const override					{ return "Recolor"; }
		const char* GetRuleName() const override final					{ return "NODE_Recolor"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override						{ RecolorNode* clone = new RecolorNode(graph); return clone; }

		void UpdateOutputChannelColors() override;


	private:
		Core::Color mMainColor;
		bool		mLoadedAttributes;			// flag to detect the very first call to OnAttributesChanged while loading

		Core::Array<Channel<double>*> mChannels;
};


#endif
