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

#ifndef __NEUROMORE_ANNOTATIONNODE_H
#define __NEUROMORE_ANNOTATIONNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "SPNode.h"


class ENGINE_API AnnotationNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0050 };
		static const char* Uuid () { return "5cd752ee-b313-11e5-9f22-ba0be0483c18"; }
		
		enum
		{
			ATTRIB_TEXT		= 0,
			ATTRIB_ALIGN_HORIZONTAL,
			ATTRIB_ALIGN_VERTICAL,
			ATTRIB_NODE_WIDTH,
			ATTRIB_NODE_HEIGHT,
			ATTRIB_COLORPICK,
		};

		enum Alignment
		{
			ALIGN_LEFT		= 0,
			ALIGN_CENTER	= 1,
			ALIGN_RIGHT		= 2,
			ALIGN_TOP		= 0,
			ALIGN_BOTTOM	= 2,
		};

		// constructor & destructor
		AnnotationNode(Graph* graph);
		~AnnotationNode();

		// initialize & update
		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void OnAttributesChanged() override;

		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Note"; }
		const char* GetRuleName() const override final							{ return "NODE_Annotation"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_UTILS; }
		GraphObject* Clone(Graph* graph) override								{ AnnotationNode* clone = new AnnotationNode(graph); return clone; }
		
		Core::Color GetColor() const override								{ return GetColorAttribute(ATTRIB_COLORPICK); }
		
		const char* GetText()													{ return GetStringAttribute(ATTRIB_TEXT); }
		void SetText(const char* text);

		// line-wise text for rendering
		uint32 GetNumLines() const												{ return mTextLines.Size(); }
		const Core::String& GetLine(uint32 index)								{ return mTextLines[index]; }

		// text alignment
		Alignment GetHorizontalAlignment()										{ return (Alignment)GetInt32Attribute(ATTRIB_ALIGN_HORIZONTAL); }
		Alignment GetVerticalAlignment()										{ return (Alignment)GetInt32Attribute(ATTRIB_ALIGN_VERTICAL); }

		// node size
		uint32 GetNodeWidth() const												{ return GetInt32Attribute(ATTRIB_NODE_WIDTH); }
		uint32 GetNodeHeight() const											{ return GetInt32Attribute(ATTRIB_NODE_HEIGHT); }
		
		
	private:
		Core::String						mText;
		Core::Array<Core::String>			mTextLines;

};


#endif
