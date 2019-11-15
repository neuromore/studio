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

#ifndef __NEUROMORE_RENAMENODE_H
#define __NEUROMORE_RENAMENODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API RenameNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0036 };
		static const char* Uuid () { return "81d10cd4-3d48-11e5-a151-feff819cdc9f"; }

		enum { OUTPUTPORT = 0 };
		enum { INPUTPORT = 0 };
		enum 
		{ 
			ATTRIB_CHANNELNAMES = 0,
			ATTRIB_MODE, 
		}; 

		enum EMode
		{
			MODE_RENAME, 
			MODE_PREFIX,
			MODE_POSTFIX,
		};
		
		// constructor & destructor
		RenameNode(Graph* graph);
		~RenameNode();

		// initialize & update
		void Init() override;
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;

		void OnAttributesChanged() override;

		Core::Color GetColor() const override							{ return Core::RGBA(116, 117, 242); }
		uint32 GetType() const override									{ return TYPE_ID; }
		const char* GetTypeUuid() const override final					{ return Uuid(); }
		const char* GetReadableType() const override					{ return "Rename"; }
		const char* GetRuleName() const override final					{ return "NODE_Rename"; }
		uint32 GetPaletteCategory() const override						{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override						{ RenameNode* clone = new RenameNode(graph); return clone; }

		void UpdateOutputChannelNames();


	private:
		Core::Array<Channel<double>*> mChannels;
};


#endif
