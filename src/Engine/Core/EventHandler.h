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

#ifndef __CORE_EVENTHANDLER_H
#define __CORE_EVENTHANDLER_H

// include the required headers
#include "Config.h"
#include "AttributeSet.h"
#include "../Graph/Node.h"
#include "../Graph/Graph.h"
#include "../Graph/Connection.h"
#include "../Graph/SPNode.h"
#include "../Graph/StateTransition.h"
#include "../Graph/StateTransitionCondition.h"
#include "../Device.h"
#include "../BciDevice.h"

// forward declaration (classifier is-of-type EventHandler)
class Classifier;
class Experience;

namespace Core
{

class ENGINE_API EventHandler
{
	public:
		EventHandler() : mEventSystemAcceptEvents(true) {}
		virtual ~EventHandler() {}

		void SetAcceptEvents(bool accept = true)		{ mEventSystemAcceptEvents = accept; }
		bool GetAcceptEvents() const					{ return mEventSystemAcceptEvents; }

		//
		// Core Events
		//  Frequently used events in the core are implemented with their own callback functions which reduces the calling overhead.
		//  Note: for each callback there must exists a calling function in the EventManager (create them there with the CREATE_EVENT_NOTIFY_FUNCTION_ macros)
		//

		// Progress View Events (from any to ProgressWindow)
		virtual void OnProgressStart(bool showProgressText=true, bool showProgressValue=true, bool showSubProgressText=true, bool showSubProgressValue=true) {}
		virtual void OnProgressEnd()							{}
		virtual void OnProgressText( const char* text )			{}
		virtual void OnProgressValue( float percentage )		{}
		virtual void OnSubProgressText( const char* text )		{}
		virtual void OnSubProgressValue( float percentage )		{}

		// session
		virtual void OnStartSession()							{}
		virtual void OnStopSession()							{}
		virtual void OnSessionUserChanged(const User& user)		{}

		// Visualization Tier Control Events (from any to NetworkServer)
		virtual void OnSwitchApplication( const char* name )	{}
		virtual void OnSwitchStage( uint32 index )				{}

		virtual void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute)	{}

		// Graph Node and connection Editing Events (from Graph to any)
		virtual void OnGraphReset(Graph* graph)															{}
		virtual void OnGraphModified(Graph* graph, GraphObject* object)									{}
		virtual void OnNodeRenamed(Graph* graph, Node* node, const Core::String& oldName)				{}
		virtual void OnNodeAdded(Graph* graph, Node* node)												{}
		virtual void OnRemoveNode(Graph* graph, Node* node)												{}
		virtual void OnNodeRemoved(Graph* graph, Node* node)											{}
		virtual void OnConnectionAdded(Graph* graph, Connection* connection)							{}
		virtual void OnRemoveConnection(Graph* graph, Connection* connection)							{}
		virtual void OnConnectionRemoved(Graph* graph, Connection* connection)							{}

		// Classifier Events
		virtual void OnNodeStarted(Graph* graph, SPNode* node)											{}
		virtual void OnNodeStopped(Graph* graph, SPNode* node)											{}

		// State Machine Events
		virtual void OnExitStateReached(uint32 status)													{}

		// Device Manager Events (from device manager to any)
		virtual void OnDeviceAdded(Device* device)														{}
		virtual void OnRemoveDevice(Device* device)														{}
		virtual void OnDeviceRemoved(Device* device)													{}

		// Misc Events (from engine to any)
		virtual void OnActiveBciChanged(BciDevice* device)												{}
		virtual void OnActiveClassifierChanged(Classifier* classifier)									{}
		virtual void OnActiveStateMachineChanged(StateMachine* stateMachine)							{}
		virtual void OnActiveExperienceChanged(Experience* experience)									{}

		// errors/warnings/infos
		virtual void OnError(uint32 errorId, const char* message, const char* description)				{}
		virtual void OnClearError(uint32 errorId)														{}
		virtual void OnWarning(uint32 warningId, const char* message, const char* description)			{}
		virtual void OnClearWarning(uint32 warningId)													{}
		virtual void OnInfo(uint32 infoId, const char* message, const char* description)				{}
		
		// Experience Events

		virtual void OnSetMasterVolume(double volume)													{}			// PROPOSED

		virtual void OnPlayAudio(const char* url, int32 numLoops, double beginAt, double volume, bool allowStreaming) {}
		virtual void OnSetAudioVolume(const char* url, double volume)									{}			// PROPOSED
		virtual void OnPauseAudio(const char* url, bool unpause)										{}			// PROPOSED
		virtual void OnStopAudio(const char* url)														{}
		virtual void OnSeekAudio(const char* url, uint32 millisecs)										{}			// PROPOSED

		virtual void OnPlayVideo(const char* url, int32 numLoops, double beginAt, double volume, bool allowStreaming)	{}
		virtual void OnSetVideoVolume(const char* url, double volume)									{}			// PROPOSED
		virtual void OnPauseVideo (const char* url, bool unpause)										{}			// PROPOSED
		virtual void OnStopVideo()																		{}
		virtual void OnSeekVideo(const char* url, uint32 millisecs)										{}			// PROPOSED

		virtual void OnShowImage(const char* url)														{}
		virtual void OnHideImage()																		{}

		virtual void OnShowText(const char* text, const Core::Color& color)								{}
		virtual void OnHideText()																		{}

		virtual void OnSetBackgroundColor(const Core::Color& color)										{}
		
		virtual void OnSetFourZoneAVEColors(const float* red, const float* green, const float* blue, const float* alpha) {}
		virtual void OnHideFourZoneAVE()																{}

		virtual void OnShowButton(const char* text, uint32 buttonId)									{}
		virtual void OnClearButtons()																	{}
		
		virtual void OnCommand(const char* command)														{}

		virtual void OnOpenUrl(const char* url) {}
		virtual void OnBrowserStartPlayer(double progress, bool fullscreen) { }
		virtual void OnBrowserStopPlayer() { }
		virtual void OnBrowserPausePlayer() { }

		virtual void OnShowTextInput(const char* text, uint32 inputId) { }
		virtual void OnHideTextInput() { }

	private:

		// enable/disable all events
		bool mEventSystemAcceptEvents;

};

} // namespace Core


#endif
