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

// include precompiled header
#include <Engine/Precompiled.h>

// include required headers
#include "AnnotationNode.h"
#include "../Core/Math.h"
#include "../DSP/Spectrum.h"
#include "../EngineManager.h"


using namespace Core;

// constructor
AnnotationNode::AnnotationNode(Graph* graph) : SPNode(graph)
{
}


// destructor
AnnotationNode::~AnnotationNode()
{
}


// initialize the node
void AnnotationNode::Init()
{
	// init base class first
	SPNode::Init();

	// ATTRIBUTES

	// node text
	const char* defaultText = "Put some;meaningful;text here.";
	AttributeSettings* attributeText = RegisterAttribute("Text", "Text", "The text to show in the Annotation Node.", ATTRIBUTE_INTERFACETYPE_STRING);
	attributeText->SetDefaultValue(AttributeString::Create(defaultText));
	SetText(defaultText);

	// alignment
	AttributeSettings* attributeAlignHorizontal = RegisterAttribute("Horizontal Alignment", "AlignHorizontal", "How to align the text in the horizontal direction.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeAlignHorizontal->ResizeComboValues(3);
	attributeAlignHorizontal->SetComboValue(ALIGN_LEFT, "Left");
	attributeAlignHorizontal->SetComboValue(ALIGN_CENTER, "Center");
	attributeAlignHorizontal->SetComboValue(ALIGN_BOTTOM, "Right");
	attributeAlignHorizontal->SetDefaultValue(AttributeInt32::Create(ALIGN_CENTER));

	AttributeSettings* attributeAlignVertical = RegisterAttribute("Vertical Alignment", "AlignVertical", "How to align the text in the vertical direction.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	attributeAlignVertical->ResizeComboValues(3);
	attributeAlignVertical->SetComboValue(ALIGN_TOP, "Top");
	attributeAlignVertical->SetComboValue(ALIGN_CENTER, "Center");
	attributeAlignVertical->SetComboValue(ALIGN_BOTTOM, "Bottom");
	attributeAlignVertical->SetDefaultValue(AttributeInt32::Create(ALIGN_CENTER));

	// node sizes
	AttributeSettings* attributeNodeWidth = RegisterAttribute("Node Width", "NodeWidth", "Width of the Annotation Node, in pixels @ 96 dpi.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attributeNodeWidth->SetDefaultValue(AttributeInt32::Create(150));
	attributeNodeWidth->SetMinValue(AttributeInt32::Create(10));
	attributeNodeWidth->SetMaxValue(AttributeInt32::Create(CORE_INT16_MAX));
	//attributeNodeWidth->SetVisible(false);

	AttributeSettings* attributeNodeHeight = RegisterAttribute("Node Height", "NodeHeight", "Height of the Annotation Node, in pixels @ 96 dpi.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	attributeNodeHeight->SetDefaultValue(AttributeInt32::Create(100));
	attributeNodeHeight->SetMinValue(AttributeInt32::Create(10));
	attributeNodeHeight->SetMaxValue(AttributeInt32::Create(CORE_INT16_MAX));
	//attributeNodeHeight->SetVisible(false);

	// custom color
	AttributeSettings* attributeColorPick = RegisterAttribute("Node Color", "NodeColor", "The color of the node.", ATTRIBUTE_INTERFACETYPE_COLOR);
	attributeColorPick->SetDefaultValue(AttributeColor::Create(Color(0.5,0.5,0.5,1)));
}


void AnnotationNode::ReInit(const Time& elapsed, const Time& delta)
{
	if (BaseReInit(elapsed, delta) == false)
		return;

	// reinit baseclass
	SPNode::ReInit(elapsed, delta);

	PostReInit(elapsed, delta);
}


// update the node
void AnnotationNode::Update(const Time& elapsed, const Time& delta)
{
	if (BaseUpdate(elapsed, delta) == false)
		return;

	// update the baseclass
	SPNode::Update(elapsed, delta);
}


void AnnotationNode::OnAttributesChanged()
{
	// split the text into lines if it has changed
	const char* currentText = GetStringAttribute(ATTRIB_TEXT);
	if (mText.Compare(currentText) != 0)
		SetText(currentText);

}


void AnnotationNode::SetText(const char* text)
{
	mText = text;
	mTextLines = mText.Split(StringCharacter::semiColon);
}
