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

#ifndef __NEUROMORE_DEPRECATEDGRAPHOBJECTS_H
#define __NEUROMORE_DEPRECATEDGRAPHOBJECTS_H

// include the required headers
#include "../Config.h"

//#include "FilterNode.h"

using namespace Core;

class ENGINE_API DeprecatedGraphObjects
{
	public:

	// translate node type strings
	static bool TranslateNodeTypeItem(Json::Item& nodeItem)
	{
		
		// get the node type uuid 
		Json::Item typeItem = nodeItem.Find( "type" );
		if (typeItem.IsString() == false)
		{
			LogError("Graph Object Translator: Error translating node type, it is not a string.");
			return false;
		}

		String type = typeItem.GetString();

		// Pre 2015 stuff
		/*
		if (type.Compare("GraphParameterNode") == 0)
			typeItem.SetString(ParameterNode::Uuid());

		else if (type.Compare("GraphOscNode") == 0)
			typeItem.SetString(OscInputNode::Uuid());

		else if (type.Compare("GraphSignalGeneratorNode") == 0 || type.Compare("GraphSinusOscillatorNode") == 0)
			typeItem.SetString(SignalGeneratorNode::Uuid());

		else if (type.Compare("GraphFeedbackScoreNode") == 0)
			typeItem.SetString(CustomFeedbackNode::Uuid());

		else if (type.Compare("GraphPointNode") == 0)
			typeItem.SetString(PointsNode::Uuid());

		else if (type.Compare("GraphSessionTimeNode") == 0)
			typeItem.SetString(SessionTimeNode::Uuid());

		else if (type.Compare("GraphFloatConditionNode") == 0)
			typeItem.SetString(CompareNode::Uuid());

		else if (type.Compare("GraphBoolLogicNode") == 0)
			typeItem.SetString(LogicNode::Uuid());

		else if (type.Compare("GraphSmoothingNode") == 0)
			typeItem.SetString(SmoothNode::Uuid());

		else if (type.Compare("GraphRangeRemapperNode") == 0)
			typeItem.SetString(RemapNode::Uuid());

		else if (type.Compare("GraphFloatMath1Node") == 0)
			typeItem.SetString(Math1Node::Uuid());

		else if (type.Compare("GraphFloatMath2Node") == 0 || type.Compare("81318ae8-bb6a-11e4-8dfc-aa07a5b093db") == 0 || type.Compare("GraphMathNode") == 0)
			typeItem.SetString(Math2Node::Uuid());

		else if (type.Compare("GraphStatisticsNode") == 0)
			typeItem.SetString(StatisticsNode::Uuid());

		else if (type.Compare("63915e72-bb6b-11e4-8dfc-aa07a5b093db") == 0 || type.Compare("GraphAverageNode") == 0)
			typeItem.SetString(ChannelMathNode::Uuid());

		else if (type.Compare("GraphFFTNode") == 0)
			typeItem.SetString(FFTNode::Uuid());

		else if (type.Compare("GraphFilterNode") == 0)
			typeItem.SetString(FilterProcessor::Uuid());
				
		else if (type.Compare("GraphFrequencyBandNode") == 0)
			typeItem.SetString(FrequencyBandNode::Uuid());

		else if (type.Compare("GraphDominantFrequencyNode") == 0)
			typeItem.SetString(DominantFrequencyNode::Uuid());
		
		else if (type.Compare("StateTransition") == 0)
			typeItem.SetString(StateTransition::Uuid());
		
		else if (type.Compare("StateTransitionTimeCondition") == 0)
			typeItem.SetString(StateTransitionTimeCondition::Uuid());
		
		// sensor type strings must be hardcoded, because core knows nothing about them..
		
		else if (type.Compare("GraphBioHarnessNode") == 0)
			typeItem.SetString("33df5804-845e-4890-9ef6-ad3a28fed68c");
		
		else if (type.Compare("GraphEmotivInsightNode") == 0)
			typeItem.SetString("bfa15a54-bb6c-11e4-8dfc-aa07a5b093db");

		else if (type.Compare("GraphMuseNode") == 0)
			typeItem.SetString("cdbd7ec4-bb6c-11e4-8dfc-aa07a5b093db");

		else if (type.Compare("GraphNeuroSkyNode") == 0)
			typeItem.SetString("d676273c-bb6c-11e4-8dfc-aa07a5b093db");

		else if (type.Compare("GraphVersusNode") == 0)
			typeItem.SetString("e8f30376-bb6c-11e4-8dfc-aa07a5b093db");

		else if (type.Compare("TestSystemGraphNode") == 0)
			typeItem.SetString("f6708456-bb6c-11e4-8dfc-aa07a5b093db");
		*/

		// 

		return true;
	}


	static bool TranslateNodeAttribute(String uuid, Json::Item& nameItem, Json::Item& valueItem)
	{
		// value must always be a string
		if (nameItem.IsString() == false)
		{
			LogError("Graph Object Translator: Error translating node '%s': attribute has no name", uuid.AsChar());
			return false;
		}

		String name = nameItem.GetString();

		bool success = true;

		// filter node  // 24.10.2016 - spuce filter implementation requires cleanup of note attribute names
	/*	if (uuid.IsEqual(FilterNode::Uuid()) == true)
		{
			if (name.IsEqual("FilterType") == true)			
			{
				nameItem.SetString("Configuration");		// "FilterType" -> "configuration"
			}
			else if (name.IsEqual("FilterMethod") == true)	
			{
				nameItem.SetString("Type");					// "FilterMethod" -> "type"
				if (valueItem.IsInt() == false)
					success = false;
				else
					valueItem.SetInt(Filter::BUTTERWORTH);		// assume only butterworth was selected (others were not implemented, only selectable) -> always set it to the new butterworth index
			}


		} */
		// other nodes/attributes here

		return success;
	}


	// translate node attributes
	static bool TranslateNodeAttributes(const Json::Item& nodeItem)
	{
		// get the node type uuid 
		Json::Item typeItem = nodeItem.Find( "type" );
		if (typeItem.IsString() == false)
		{
			LogError("Graph Object Translator: Error translating attribute, node type is not a string");
			return false;
		}
		
		String uuid = typeItem.GetString();

		// go through all attributes
		Json::Item attrArray = nodeItem.Find("attributes");
		if (attrArray.IsArray() == false)
		{
			LogError("Graph Object Translator: Error translating node '%s': no attributes found", uuid.AsChar());
			return false;
		}

		bool success = true;

		const uint32 size = attrArray.Size();
		for (uint32 i=0; i<size; ++i)
		{
			Json::Item attribute = attrArray[i];
			if (attribute.IsObject() == false)
			{
				success = false;
				continue;
			}

			// get attribute name and value
			Json::Item name = attribute.Find("internalName");
			Json::Item value = attribute.Find("value");

			// now translate the attribute
			if (TranslateNodeAttribute(uuid, name, value) == false)
			{
				success = false;
				continue;
			}
		}

		return success;
	}


	// translate the json of a deprecated node; returns false if it cannot be translated
	static bool TranslateNode(Json::Item& nodeItem)
	{
		// make sure the given item is valid
		if (nodeItem.IsNull() == true)
			return false;

		bool success = false;

		// translate type string first
		success |= TranslateNodeTypeItem(nodeItem);

		// translate attributes
		success |= TranslateNodeAttributes(nodeItem);

		return success;
	}

};
#endif
