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

// include the required headers
#include "Actions.h"
#include "../Core/Math.h"
#include "../Core/EventManager.h"
#include "../EngineManager.h"


using namespace Core;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// play audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void PlayAudioAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Audio URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );

	// register volume slider attribute
	AttributeSettings* volumeAttribute = RegisterAttribute("Volume", "volume", "The audio volume in range [0.0, 1.0].", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	volumeAttribute->SetDefaultValue( AttributeFloat::Create(1.0) );
	volumeAttribute->SetMinValue( AttributeFloat::Create(0.0) );
	volumeAttribute->SetMaxValue( AttributeFloat::Create(1.0) );

	// create the is looping attribute
	const bool isLooping = false;
	AttributeSettings* loopingAttribute = RegisterAttribute("Loop Forever", "isLooping", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	loopingAttribute->SetDefaultValue( AttributeBool::Create(isLooping) );

	// create the num loops
	AttributeSettings* numLoopsAttribute = RegisterAttribute("Num Loops", "numLoops", "The number of loops the audio plays.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numLoopsAttribute->SetDefaultValue( AttributeInt32::Create(1) );
	numLoopsAttribute->SetMinValue( AttributeInt32::Create(0) );
	numLoopsAttribute->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );
	numLoopsAttribute->SetVisible( !isLooping );
	
	// create the allow streaming attribute
	AttributeSettings* streamingAttribute = RegisterAttribute("Allow Streaming", "streaming", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	streamingAttribute->SetDefaultValue( AttributeBool::Create(false) );
}


// attributes have changed
void PlayAudioAction::OnAttributesChanged()
{
	Action::OnAttributesChanged();

	// show/hide max number of loops attribute
	const bool isLooping = GetBoolAttribute( ATTRIBUTE_ISLOOPING );
	GetAttributeSettings(ATTRIBUTE_NUMLOOPS)->SetVisible( !isLooping );

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_NUMLOOPS)) );
}


// execute action
void PlayAudioAction::Execute()
{
	const char* url				= GetStringAttribute( ATTRIBUTE_URL );
	const double volume			= GetFloatAttribute( ATTRIBUTE_VOLUME );
	const bool isLooping		= GetBoolAttribute( ATTRIBUTE_ISLOOPING );
	const bool allowStream		= GetBoolAttribute( ATTRIBUTE_ALLOWSTREAM );

	int32 numLoops = -1;
	if (isLooping == false)
		numLoops			= GetInt32Attribute( ATTRIBUTE_NUMLOOPS );

	EMIT_EVENT(OnPlayAudio( url, numLoops, -1.0, volume, allowStream ));

	mParentStateMachine->SetAssetActivity(url,  StateMachine::Asset::ACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stop audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void StopAudioAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Audio URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );
}


// execute action
void StopAudioAction::Execute()
{
	const char* url = GetStringAttribute( ATTRIBUTE_URL );

	EMIT_EVENT(OnStopAudio( url ));

	mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::INACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pause audio action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void PauseAudioAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Audio URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );
	
	// create the allow unpause attribute
	AttributeSettings* unpauseAttribute = RegisterAttribute("Unpause", "unpause", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	unpauseAttribute->SetDefaultValue( AttributeBool::Create(false) );
}


// execute action
void PauseAudioAction::Execute()
{
	const char* url = GetStringAttribute( ATTRIBUTE_URL );
	const bool unpause = GetBoolAttribute( ATTRIBUTE_UNPAUSE );

	EMIT_EVENT(OnPauseAudio( url, unpause ));

	if (unpause == true)
		mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::ACTIVE);
	else	
		mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::INACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Seek Audio Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void SeekAudioAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Audio URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue(AttributeString::Create(""));

	// register seek position  attribute
	AttributeSettings* positionAttribute = RegisterAttribute("Position (s)", "position", "The position within the audio file to seek to.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	positionAttribute->SetDefaultValue(AttributeFloat::Create(1.0));
	positionAttribute->SetMinValue(AttributeFloat::Create(0.0));
	positionAttribute->SetMaxValue(AttributeFloat::Create(CORE_FLOAT_MAX));
}


// execute action
void SeekAudioAction::Execute()
{
	const char* url = GetStringAttribute(ATTRIBUTE_URL);
	const float seconds = GetFloatAttribute(ATTRIBUTE_POSITION);

	const uint32 millisecs = (uint32)(seconds * 1000.0f);

	EMIT_EVENT( OnSeekAudio(url, millisecs) );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// show image action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void ShowImageAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Image URL", "url", "The URL or local filename of the image file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );
}


// execute action
void ShowImageAction::Execute()
{
	const char* url = GetStringAttribute( ATTRIBUTE_URL );

	EMIT_EVENT(OnShowImage( url ));

	// TODO use asset manager
	mParentStateMachine->SetAssetGroupActivity(Action::ASSET_IMAGE, StateMachine::Asset::INACTIVE);
	mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::ACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hide image action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void HideImageAction::Init()
{
}


// execute action
void HideImageAction::Execute()
{
	EMIT_EVENT(OnHideImage());

	// TODO use asset manager
	mParentStateMachine->SetAssetGroupActivity(Action::ASSET_IMAGE, StateMachine::Asset::INACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// show text action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void ShowTextAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Text", "test", "The test to show.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("Example text") );

	// custom color
	AttributeSettings* colorAttribute = RegisterAttribute("Color", "color", "The text color.", ATTRIBUTE_INTERFACETYPE_COLOR);
	colorAttribute->SetDefaultValue( AttributeColor::Create(RGBA(0,159,227)) );
}


// execute action
void ShowTextAction::Execute()
{
	mText					= GetStringAttribute( ATTRIBUTE_TEXT );
	const Color	color	= GetColorAttribute( ATTRIBUTE_COLOR );

	// check if there are any placeholders in the string
	if (mText.Find("${") != CORE_INVALIDINDEX32)
	{
		// get the active classifier
		Classifier* classifier = GetEngine()->GetActiveClassifier();
		if (classifier == NULL)
		{
			LogWarning( "ShowTextAction::Execute(): Cannot replace placeholder for text action '%s'. No active classifier.", mText.AsChar() );
			return;
		}

		// get the number of feedback nodes and iterate through them
		const uint32 numFeedbackNodes = classifier->GetNumCustomFeedbackNodes();
		for (uint32 i=0; i<numFeedbackNodes; ++i)
		{
			CustomFeedbackNode* feedbackNode = classifier->GetCustomFeedbackNode(i);

			mTempPlaceholder.Format( "${%s}", feedbackNode->GetName() );
			mTempValue.Format( "%.2f", feedbackNode->GetCurrentValue() );
			mText.Replace( mTempPlaceholder.AsChar(), mTempValue.AsChar() );
		}
	}

	EMIT_EVENT(OnShowText( mText.AsChar(), color ));

	// TODO should text be managed like an asset? i think so!
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hide text action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void HideTextAction::Init()
{
}


// execute action
void HideTextAction::Execute()
{
	EMIT_EVENT(OnHideText());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// set background color action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void SetBackgroundColorAction::Init()
{
	// custom color
	AttributeSettings* colorAttribute = RegisterAttribute("Color", "color", "The background color.", ATTRIBUTE_INTERFACETYPE_COLOR);
	colorAttribute->SetDefaultValue(AttributeColor::Create(RGBA(0, 159, 227)));
}


// execute action
void SetBackgroundColorAction::Execute()
{
	const Color	color = GetColorAttribute(ATTRIBUTE_COLOR);

	EMIT_EVENT(OnSetBackgroundColor(color));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// command action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void CommandAction::Init()
{
	// command attribute
	AttributeSettings* commandAttribute = RegisterAttribute("Command", "cmd", "The command to call.", ATTRIBUTE_INTERFACETYPE_STRING);
	commandAttribute->SetDefaultValue( AttributeString::Create("") );
}


// execute action
void CommandAction::Execute()
{
	String command = GetStringAttribute( ATTRIBUTE_COMMAND );

	command.Trim();
	if (command.IsEmpty() == true)
		return;

	// DEPRECATED: map stage control commands to the current OnSwitchStage() event, not the command event
	// this can be removed later

	const uint32 numArgs = command.CalcNumWords();

	if (numArgs > 1)
	{
		String arg1 = command.ExtractWord(0);
		if (arg1.CompareNoCase("doSwitchStage") == 0)
		{
			int arg2 = command.ExtractWord(1).ToInt();
			EMIT_EVENT( OnSwitchStage(arg2) );
		}
		else if (arg1.CompareNoCase("doSwitchApplication") == 0)
		{
			String arg2 = command.ExtractWord(1);
			EMIT_EVENT( OnSwitchApplication(arg2) );
		}
	}

	// emit command event
	EMIT_EVENT( OnCommand(command.AsChar()) );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// play video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void PlayVideoAction::Init()
{
	// register video url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Video URL", "url", "The URL or local filename of the video file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );

	// register volume slider attribute
	AttributeSettings* volumeAttribute = RegisterAttribute("Volume", "volume", "The video volume in range [0.0, 1.0].", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	volumeAttribute->SetDefaultValue( AttributeFloat::Create(1.0) );
	volumeAttribute->SetMinValue( AttributeFloat::Create(0.0) );
	volumeAttribute->SetMaxValue( AttributeFloat::Create(1.0) );

	// create the is looping attribute
	AttributeSettings* loopingAttribute = RegisterAttribute("Loop Forever", "isLooping", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	loopingAttribute->SetDefaultValue( AttributeBool::Create(false) );

	// create the num loops
	AttributeSettings* numLoopsAttribute = RegisterAttribute("Num Loops", "numLoops", "The number of loops the video plays.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	numLoopsAttribute->SetDefaultValue( AttributeInt32::Create(1) );
	numLoopsAttribute->SetMinValue( AttributeInt32::Create(0) );
	numLoopsAttribute->SetMaxValue( AttributeInt32::Create(CORE_INT32_MAX) );

	// create the allow streaming attribute
	AttributeSettings* streamingAttribute = RegisterAttribute("Allow Streaming", "streaming", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	streamingAttribute->SetDefaultValue( AttributeBool::Create(false) );
}


// attributes have changed
void PlayVideoAction::OnAttributesChanged()
{
	Action::OnAttributesChanged();

	// show/hide max number of loops attribute
	const bool isLooping = GetBoolAttribute( ATTRIBUTE_ISLOOPING );
	GetAttributeSettings(ATTRIBUTE_NUMLOOPS)->SetVisible( !isLooping );

	// fire event
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_NUMLOOPS)) );
}


// execute action
void PlayVideoAction::Execute()
{
	const char* url			= GetStringAttribute( ATTRIBUTE_URL );
	const double volume		= GetFloatAttribute( ATTRIBUTE_VOLUME );
	const bool isLooping	= GetBoolAttribute( ATTRIBUTE_ISLOOPING );
	const bool allowStream	= GetBoolAttribute( ATTRIBUTE_ALLOWSTREAM );

	int32 numLoops = -1;
	if (isLooping == false)
		numLoops			= GetInt32Attribute( ATTRIBUTE_NUMLOOPS );

	EMIT_EVENT(OnPlayVideo( url, numLoops, -1.0, volume, allowStream ));

	// TODO use asset manager
	mParentStateMachine->SetAssetGroupActivity(Action::ASSET_VIDEO, StateMachine::Asset::INACTIVE);
	mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::ACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stop video action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void StopVideoAction::Init()
{
}


// execute action
void StopVideoAction::Execute()
{
	EMIT_EVENT(OnStopVideo());

	// TODO use asset manager
	mParentStateMachine->SetAssetGroupActivity(Action::ASSET_VIDEO, StateMachine::Asset::INACTIVE);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pause Video Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void PauseVideoAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Video URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue( AttributeString::Create("") );
	
	// create the allow unpause attribute
	AttributeSettings* unpauseAttribute = RegisterAttribute("Unpause", "unpause", "", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	unpauseAttribute->SetDefaultValue( AttributeBool::Create(false) );
}


// execute action
void PauseVideoAction::Execute()
{
	const char* url = GetStringAttribute( ATTRIBUTE_URL );
	const bool unpause = GetBoolAttribute( ATTRIBUTE_UNPAUSE );

	EMIT_EVENT(OnPauseVideo( url, unpause ));

	if (unpause == true)
		mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::ACTIVE);
	else	
		mParentStateMachine->SetAssetActivity(url, StateMachine::Asset::INACTIVE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Seek Video Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void SeekVideoAction::Init()
{
	// register audio url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Video URL", "url", "The URL or local filename of the audio file.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue(AttributeString::Create(""));

	// register seek position  attribute
	AttributeSettings* positionAttribute = RegisterAttribute("Position (s)", "position", "The position within the audio file to seek to.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	positionAttribute->SetDefaultValue(AttributeFloat::Create(1.0));
	positionAttribute->SetMinValue(AttributeFloat::Create(0.0));
	positionAttribute->SetMaxValue(AttributeFloat::Create(CORE_FLOAT_MAX));
}


// execute action
void SeekVideoAction::Execute()
{
	const char* url = GetStringAttribute(ATTRIBUTE_URL);
	const float seconds = GetFloatAttribute(ATTRIBUTE_POSITION);

	const uint32 millisecs = (uint32)(seconds * 1000.0f);

	EMIT_EVENT(OnSeekVideo(url, millisecs));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// parameter action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void ParameterAction::Init()
{
	// register parameter name attribute
	AttributeSettings* parameterAttribute = RegisterAttribute("Parameter Name", "parameterName", "Name of the parameter node of the bound classifier.", ATTRIBUTE_INTERFACETYPE_STRING);
	parameterAttribute->SetDefaultValue( AttributeString::Create("") );

	// parameter channel index
	AttributeSettings* parameterChannelAttribute = RegisterAttribute("Parameter Channel Index", "parameterChannelIndex", "Select which parameter value to use if the parameter node has more than one output channel.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	parameterChannelAttribute->SetDefaultValue(AttributeInt32::Create(0));
	parameterChannelAttribute->SetMinValue(AttributeInt32::Create(0));
	parameterChannelAttribute->SetMaxValue(AttributeInt32::Create(CORE_INT32_MAX));

	// function combobox
	Core::AttributeSettings* functionParam = RegisterAttribute("Function", "function", "The function to use.", ATTRIBUTE_INTERFACETYPE_COMBOBOX);
	functionParam->ResizeComboValues( (uint32)FUNCTION_NUMFUNCTIONS );
	for (uint32 i=0; i<(uint32)FUNCTION_NUMFUNCTIONS; ++i)
		functionParam->SetComboValue( i, GetFunctionString((EFunction)i) );
	functionParam->SetDefaultValue( Core::AttributeInt32::Create(FUNCTION_SET) );

	// register value attribute
	AttributeSettings* valueAttribute = RegisterAttribute("Value", "value", "The value will be used by the function (e.g. increase by value, scale by value, set value).", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	valueAttribute->SetDefaultValue( AttributeFloat::Create(1.0) );
	valueAttribute->SetMinValue( AttributeFloat::Create(-CORE_FLOAT_MAX) );
	valueAttribute->SetMaxValue( AttributeFloat::Create(+CORE_FLOAT_MAX) );

	// feedback node name
	AttributeSettings* feedbackNameAttribute = RegisterAttribute("Feedback Name", "feedbackName", "Name of the feedback node.", ATTRIBUTE_INTERFACETYPE_STRING);
	feedbackNameAttribute->SetDefaultValue(AttributeString::Create(""));
	feedbackNameAttribute->SetVisible( false );

	// feedback channel index
	AttributeSettings* feedbackChannelAttribute = RegisterAttribute("Feedback Channel Index", "feedbackChannelIndex", "Select which feedback value to use if the feedback node has more than one input channel.", ATTRIBUTE_INTERFACETYPE_INTSPINNER);
	feedbackChannelAttribute->SetDefaultValue(AttributeInt32::Create(0));
	feedbackChannelAttribute->SetMinValue(AttributeInt32::Create(0));
	feedbackChannelAttribute->SetMaxValue(AttributeInt32::Create(CORE_INT32_MAX));
	feedbackChannelAttribute->SetVisible( false );

	// register min random value float spinner
	AttributeSettings* minValueAttribute = RegisterAttribute("Random Min Value", "randMin", "The minimum randomized value.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	minValueAttribute->SetDefaultValue( AttributeFloat::Create(0.0f) );
	minValueAttribute->SetMinValue( AttributeFloat::Create(-CORE_FLOAT_MAX) );
	minValueAttribute->SetMaxValue( AttributeFloat::Create(+CORE_FLOAT_MAX) );
	minValueAttribute->SetVisible( false );

	// register max random value float spinner
	AttributeSettings* maxValueAttribute = RegisterAttribute("Random Max Value", "randMax", "The maximum randomized value.", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	maxValueAttribute->SetDefaultValue( AttributeFloat::Create(1.0f) );
	maxValueAttribute->SetMinValue( AttributeFloat::Create(-CORE_FLOAT_MAX) );
	maxValueAttribute->SetMaxValue( AttributeFloat::Create(+CORE_FLOAT_MAX) );
	maxValueAttribute->SetVisible( false );
}


// attributes have changed
void ParameterAction::OnAttributesChanged()
{
	Action::OnAttributesChanged();

	const EFunction function = (EFunction)GetInt32Attribute(ATTRIBUTE_FUNCTION);

	// show/hide the randomization attributes
	GetAttributeSettings(ATTRIBUTE_VALUE)->SetVisible(function != FUNCTION_RANDOM && function != FUNCTION_FEEDBACK && function != FUNCTION_MULTIFEEDBACK);
	GetAttributeSettings(ATTRIBUTE_MINVALUE)->SetVisible(function == FUNCTION_RANDOM);
	GetAttributeSettings(ATTRIBUTE_MAXVALUE)->SetVisible(function == FUNCTION_RANDOM);

	// show/hide feedback name and channel index
	GetAttributeSettings(ATTRIBUTE_FEEDBACKNAME)->SetVisible(function == FUNCTION_FEEDBACK || function == FUNCTION_MULTIFEEDBACK);
	GetAttributeSettings(ATTRIBUTE_FEEDBACKCHANNELINDEX)->SetVisible(function == FUNCTION_FEEDBACK);

	// show/hide parameter channel index
	GetAttributeSettings(ATTRIBUTE_PARAMETERCHANNELINDEX)->SetVisible(function != FUNCTION_MULTIFEEDBACK);

	// fire events
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_VALUE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_MINVALUE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_MAXVALUE)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_FEEDBACKNAME)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_FEEDBACKCHANNELINDEX)) );
	EMIT_EVENT( OnAttributeUpdated(mParentGraph, this, GetAttributeValue(ATTRIBUTE_PARAMETERCHANNELINDEX)) );
}


// execute action
void ParameterAction::Execute()
{
	const char* parameterName			= GetStringAttribute( ATTRIBUTE_PARAMETERNAME );
	const uint32 parameterChannelIndex	= GetInt32Attribute( ATTRIBUTE_PARAMETERCHANNELINDEX );
	const double value					= GetFloatAttribute( ATTRIBUTE_VALUE );
	const char* feedbackName			= GetStringAttribute( ATTRIBUTE_FEEDBACKNAME );
	const uint32 feedbackChannelIndex	= GetInt32Attribute( ATTRIBUTE_FEEDBACKCHANNELINDEX );
	const EFunction function			= (EFunction)GetInt32Attribute( ATTRIBUTE_FUNCTION );
	const double minValue				= GetFloatAttribute( ATTRIBUTE_MINVALUE );
	const double maxValue				= GetFloatAttribute( ATTRIBUTE_MAXVALUE );

	// get the active classifier
	Classifier* classifier = GetEngine()->GetActiveClassifier();
	if (classifier == NULL)
	{
		LogWarning( "ParameterAction: Cannot set parameter '%s'. No active classifier.", parameterName );
		return;
	}

	// find bound parameter node
	ParameterNode* parameterNode = NULL;
	const uint32 numParameterNodes = classifier->GetNumParameterNodes();
	for (uint32 i=0; i<numParameterNodes; ++i)
	{
		ParameterNode* curNode = classifier->GetParameterNode(i);

		if (curNode->GetNameString().IsEqual(parameterName) == true)
		{
			parameterNode = curNode;
			break;
		}
	}

	// check if we found the bound parameter node
	if (parameterNode == NULL)
	{
		LogWarning( "ParameterAction: Cannot set parameter '%s'. No parameter node with the given name.", parameterName );
		return;
	}

	// make sure the specified parameter channel exists
	if (parameterChannelIndex >= parameterNode->GetNumSensors())
	{
		LogWarning("ParameterAction: Cannot set parameter '%s' : invalid channel index '%i' - node has only '%i' channels.", parameterName, parameterChannelIndex, parameterNode->GetNumSensors());
		return;
	}

	// try to find feedback node, if set-by-feedback function is used
	FeedbackNode* feedbackNode = NULL;
	if (function == FUNCTION_FEEDBACK || function == FUNCTION_MULTIFEEDBACK)
	{
		const uint32 numFeedbackNodes = classifier->GetNumFeedbackNodes();
		for (uint32 i = 0; i < numFeedbackNodes; ++i)
		{
			FeedbackNode* curNode = classifier->GetFeedbackNode(i);

			if (curNode->GetNameString().IsEqual(feedbackName) == true)
			{
				feedbackNode = curNode;
				break;
			}
		}

		// feedback node not found
		if (feedbackNode == NULL)
		{
			LogWarning("ParameterAction: Cannot set parameter '%s' from FeedbackNode '%s': no node with the given name found.", parameterName, feedbackName);
			return;
		}

		// make sure the specified feedback channel exists
		if (feedbackChannelIndex >= feedbackNode->GetNumCurrentValues())
		{
			LogWarning("ParameterAction: Cannot set parameter '%s' from FeedbackNode '%s': invalid channel index '%i' - node has only '%i' channels.", parameterName, feedbackName, feedbackChannelIndex, feedbackNode->GetNumOutputChannels());
			return;
		}
	}

	// execute the action: calculate the new value
	switch (function)
	{
		case FUNCTION_SET:				{ parameterNode->SetValue( value, parameterChannelIndex); break; }
		case FUNCTION_INCREASE:			{ parameterNode->SetValue( parameterNode->GetValue() + value, parameterChannelIndex );  break; }
		case FUNCTION_DECREASE:			{ parameterNode->SetValue( parameterNode->GetValue() - value, parameterChannelIndex );  break; }
		case FUNCTION_SCALE:			{ parameterNode->SetValue( parameterNode->GetValue() * value, parameterChannelIndex );  break; }
		case FUNCTION_RANDOM:			{ parameterNode->SetValue( Math::RandD(minValue, maxValue), parameterChannelIndex );  break; }
	
		case FUNCTION_FEEDBACK:			{ parameterNode->SetValue(feedbackNode->GetCurrentValue(feedbackChannelIndex), parameterChannelIndex); break; }
		case FUNCTION_MULTIFEEDBACK:	
		{ 
			// set all feedback values if they exist (no warning if the sizes don't match)
			const uint32 numChannels = Core::Min(feedbackNode->GetNumCurrentValues(), parameterNode->GetNumSensors());
			for (uint32 i = 0; i < numChannels; ++i)
			{
				const double feedbackValue = feedbackNode->GetCurrentValue(i);
				parameterNode->SetValue(feedbackValue, i);
				LogDebug("Set Parameter %s index %i from Feedbackscore %s to %.2f", parameterNode->GetName(), i, feedbackNode->GetName(), feedbackValue);
			}
			
		} break;

		default:						{}
	}
}


const char* ParameterAction::GetFunctionString(EFunction function)
{
	switch (function)
	{
		case FUNCTION_SET:				{ return "Set to Value";		}
		case FUNCTION_INCREASE:			{ return "Increase by Value";	}
		case FUNCTION_DECREASE:			{ return "Decrease by Value";	}
		case FUNCTION_SCALE:			{ return "Scale by Value";		}
		case FUNCTION_RANDOM:			{ return "Set Random [MinValue, MaxValue]";}
		case FUNCTION_FEEDBACK:			{ return "Set to Feedback Value ";	}
		case FUNCTION_MULTIFEEDBACK:	{ return "Set to Feedback Value (all channels)";	}
		default:						{ return "Undefined";			}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clear buttons action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void ClearButtonsAction::Init()
{
}


// execute action
void ClearButtonsAction::Execute()
{
	EMIT_EVENT(OnClearButtons());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// open url action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void OpenUrlAction::Init()
{
	// register url attribute
	AttributeSettings* urlAttribute = RegisterAttribute("URL", "url", "The URL to navigate to in browser.", ATTRIBUTE_INTERFACETYPE_STRING);
	urlAttribute->SetDefaultValue(AttributeString::Create(""));
}

// execute action
void OpenUrlAction::Execute()
{
	EMIT_EVENT(OnOpenUrl(GetStringAttribute(ATTRIBUTE_URL)));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Browser Start Player Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void BrowserStartPlayerAction::Init()
{
	// register progress attribute
	AttributeSettings* positionAttribute = RegisterAttribute("Progress (s)", "progress", "The relative position to seek to [0.0-1.0].", ATTRIBUTE_INTERFACETYPE_FLOATSPINNER);
	positionAttribute->SetDefaultValue(AttributeFloat::Create(1.0));
	positionAttribute->SetMinValue(AttributeFloat::Create(0.0));
	positionAttribute->SetMaxValue(AttributeFloat::Create(1.0));

	// register fullscreen attribute
	AttributeSettings* urlAttribute = RegisterAttribute("Fullscreen", "fullscreen", "Check if playback should start in fullscreen.", ATTRIBUTE_INTERFACETYPE_CHECKBOX);
	urlAttribute->SetDefaultValue(AttributeBool::Create(false));
}

// execute action
void BrowserStartPlayerAction::Execute()
{
	const double progress = GetFloatAttribute(ATTRIBUTE_PROGRESS);
	const bool fullscreen = GetBoolAttribute(ATTRIBUTE_FULLSCREEN);

	EMIT_EVENT(OnBrowserStartPlayer(progress, fullscreen));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Browser Stop Player Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void BrowserStopPlayerAction::Init()
{
}

// execute action
void BrowserStopPlayerAction::Execute()
{
	EMIT_EVENT(OnBrowserStopPlayer());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Browser Pause Player Action
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize attributes
void BrowserPausePlayerAction::Init()
{
}

// execute action
void BrowserPausePlayerAction::Execute()
{
	EMIT_EVENT(OnBrowserPausePlayer());
}
