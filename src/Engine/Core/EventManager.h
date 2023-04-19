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

#ifndef __CORE_EVENTMANAGER_H
#define __CORE_EVENTMANAGER_H

// include the required headers
#include "Config.h"
#include "Array.h"
#include "String.h"
#include "EventHandler.h"
#include "EventLogger.h"
#include "EventManagerHelpers.h"

// forward declaration
class Graph;
class Experience;
class StateTransition;
class StateTransitionCondition;


namespace Core
{

class ENGINE_API EventManager
{
	public:
		EventManager();
		~EventManager();

		void AddEventHandler(EventHandler* eventHandler);
		bool RemoveEventHandler(EventHandler* eventHandler, bool delFromMem = false);
		void RemoveEventHandler(uint32 index, bool delFromMem = false);

		EventHandler* GetEventHandler(uint32 index) const		{ return mEventHandlers[index]; }
		uint32 GetNumEventHandlers() const						{ return mEventHandlers.Size(); }
		uint32 FindEventHandlerIndex(EventHandler* eventHandler) const;

		//---------------------------------------------------------------------

		//
		// Core Events
		//  Frequently used events in the core are implemented with their own callback functions which reduces the calling overhead.
		//  Use the EVENT_CREATE_NOTIFY_FUNCTION_ macros to generate a function that calls the same-named function in all registered event handlers.

		// Progress View Events
		void OnProgressStart( bool showProgressText, bool showProgressValue, bool showSubProgressText, bool showSubProgressValue )
		{
			const uint32 numEventHandlers = mEventHandlers.Size();
			for (uint32 i = 0; i < numEventHandlers; ++i)
			{
				mEventHandlers[i]->OnProgressStart( showProgressText, showProgressValue, showSubProgressText, showSubProgressValue );
				mEventHandlers[i]->OnProgressValue( 0.0f );
			}
		}

		void OnProgressEnd()
		{
			const uint32 numEventHandlers = mEventHandlers.Size();
			for (uint32 i = 0; i<numEventHandlers; ++i)
			{
				mEventHandlers[i]->OnProgressValue( 100.0f ); 
				mEventHandlers[i]->OnProgressEnd();
			}
		}

		// Progress Window
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnProgressText,  const char*, text );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnProgressValue, float, percentage );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnSubProgressText,  const char*, text );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnSubProgressValue, float, percentage );

		// session
		inline EVENT_CREATE_NOTIFY_FUNCTION_0( OnPrepareSession );
		inline EVENT_CREATE_NOTIFY_FUNCTION_0( OnStartSession );
		inline EVENT_CREATE_NOTIFY_FUNCTION_0( OnStopSession );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnSessionUserChanged, const User&, user );

		// Visualization Tier Control Events
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnSwitchApplication, const char*, name );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnSwitchStage, uint32, index );

		// Graph Editing Events
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnGraphReset,	Graph*, graph);
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnGraphModified, Graph*, graph, GraphObject*, object );							// POST event, object may be INVALID pointer
		inline EVENT_CREATE_NOTIFY_FUNCTION_3( OnNodeRenamed,	Graph*, graph, Node*, node, const Core::String&, oldName );
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnNodeAdded,		Graph*, graph, Node*, node );
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnRemoveNode,	Graph*, graph, Node*, node );
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnNodeRemoved,	Graph*, graph, Node*, node );									// POST event, node IS INVALID pointer	
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnConnectionAdded,	Graph*, graph, Connection*, connection );
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnRemoveConnection,  Graph*, graph, Connection*, connection );
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnConnectionRemoved, Graph*, graph, Connection*, connection );					// POST event, connection IS INVALID pointer	
		inline EVENT_CREATE_NOTIFY_FUNCTION_3( OnAttributeUpdated,  Graph*, graph, GraphObject*, object, Core::Attribute*, attribute);

		// classifier events
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnNodeStarted, Graph*, graph, SPNode*, node );									// POST event, SPNode::Start was executed already
		inline EVENT_CREATE_NOTIFY_FUNCTION_2( OnNodeStopped, Graph*, graph, SPNode*, node );									// POST event, SPNode::Start was executed already

		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnExitStateReached, uint32, exitStatus );

		// Device Manager events
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnDeviceAdded,	Device*, device );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnRemoveDevice,	Device*, device );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnDeviceRemoved, Device*, device );	// POST event, device IS INVALID pointer

		// Misc events
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnActiveBciChanged,			BciDevice*,		device );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnActiveClassifierChanged,	Classifier*,	classifier );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnActiveStateMachineChanged, StateMachine*,	stateMachine );
		inline EVENT_CREATE_NOTIFY_FUNCTION_1( OnActiveExperienceChanged,	Experience*,	experience );

		// experience events
		EVENT_CREATE_NOTIFY_FUNCTION_5( OnPlayAudio, const char*, url, int32, numLoops, double, beginAt, double, volume, bool, allowStream );
		EVENT_CREATE_NOTIFY_FUNCTION_1( OnStopAudio, const char*, url );
		EVENT_CREATE_NOTIFY_FUNCTION_2( OnPauseAudio, const char*, url, bool, unpause );
		EVENT_CREATE_NOTIFY_FUNCTION_2( OnSeekAudio, const char*, url, uint32, millisecs );

		EVENT_CREATE_NOTIFY_FUNCTION_5( OnPlayVideo, const char*, url, int32, numLoops, double, beginAt, double, volume, bool, allowStream );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnStopVideo );
		EVENT_CREATE_NOTIFY_FUNCTION_2( OnPauseVideo, const char*, url, bool, unpause );
		EVENT_CREATE_NOTIFY_FUNCTION_2( OnSeekVideo, const char*, url, uint32, millisecs);

		EVENT_CREATE_NOTIFY_FUNCTION_1( OnShowImage, const char*, url );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnHideImage );

		EVENT_CREATE_NOTIFY_FUNCTION_2( OnShowText, const char*, text, const Core::Color&, color );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnHideText );

		EVENT_CREATE_NOTIFY_FUNCTION_1( OnSetBackgroundColor, const Core::Color&, color );

		EVENT_CREATE_NOTIFY_FUNCTION_4( OnSetFourZoneAVEColors, const float*, red, const float*, green, const float*, blue, const float*, alpha );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnHideFourZoneAVE );

		EVENT_CREATE_NOTIFY_FUNCTION_2( OnShowButton, const char*, text, uint32, buttonId );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnClearButtons );

		EVENT_CREATE_NOTIFY_FUNCTION_1( OnCommand, const char*, command );

		EVENT_CREATE_NOTIFY_FUNCTION_1(OnOpenUrl, const char*, url);
		EVENT_CREATE_NOTIFY_FUNCTION_2(OnBrowserStartPlayer, double, progress, bool, fullscreen);
		EVENT_CREATE_NOTIFY_FUNCTION_0(OnBrowserStopPlayer);
		EVENT_CREATE_NOTIFY_FUNCTION_0(OnBrowserPausePlayer);

		EVENT_CREATE_NOTIFY_FUNCTION_2( OnShowTextInput, const char*, text, uint32, inputId );
		EVENT_CREATE_NOTIFY_FUNCTION_0( OnHideTextInput );

	private:
		Core::Array<EventHandler*>			mEventHandlers;
		EventLogger*						mEventLogger;
};

} // namespace Core


#endif
