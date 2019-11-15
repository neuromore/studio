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

#ifndef __CORE_EVENTLOGGER_H
#define __CORE_EVENTLOGGER_H

#include "Config.h"
#include "EventHandler.h"
#include "../Graph/Classifier.h"
#include "../Graph/StateMachine.h"
#include "../Experience.h"
#include "../Device.h"

namespace Core
{

class ENGINE_API EventLogger : public Core::EventHandler
{
	public:
		EventLogger() : EventHandler()	{}
		virtual ~EventLogger()			{}

		// Progress View Events (from any to ProgressWindow)
		virtual void OnProgressStart(bool showProgressText = true, bool showProgressValue = true, bool showSubProgressText = true, bool showSubProgressValue = true) override final
		{
			LogDebug("Event: OnProgressStart( showProgressText='%s', showProgressValue='%s', bool showSubProgressText='%s', showSubProgressValue='%s' )", (showProgressText ? "true" : "false"), (showProgressValue ? "true" : "false"), (showSubProgressText ? "true" : "false"), (showSubProgressValue ? "true" : "false") );
		}

		virtual void OnProgressEnd() override final
		{
			LogDebug("Event: OnProgressEnd()");
		}

		virtual void OnProgressText( const char* text ) override final
		{
			LogDebug("Event: OnProgressText( text='%s' )", text);
		}
		virtual void OnProgressValue( float percentage ) override final
		{
			LogDebug("Event: OnProgressValue( percentage='%.3f' )", percentage);
		}
		virtual void OnSubProgressText( const char* text ) override final
		{
			LogDebug("Event: OnSubProgressText( text='%s' )", text);
		}

		virtual void OnSubProgressValue( float percentage ) override final
		{
			LogDebug("Event: OnSubProgressValue( percentage='%.3f' )", percentage);
		}

		// Visualization Tier Control Events (from any to NetworkServer)
		virtual void OnSwitchApplication( const char* name ) override final
		{
			LogDebug("Event: OnSwitchApplication( name='%s' )", name);
		}

		virtual void OnStartSession() override final
		{
			LogDebug("Event: OnStartSession()");
		}

		virtual void OnStopSession() override final
		{
			LogDebug("Event: OnStopSession()");
		}

		virtual void OnSwitchStage( uint32 index ) override final
		{
			LogDebug("Event: OnSwitchStage( index='%i' )", index);
		}

		// Note: not logged
		// Graph Node and connection Editing Events (from Graph to any)
		/*virtual void OnGraphModified(Graph* graph, GraphObject* object)						
		virtual void OnNodeRenamed(Graph* graph, Node* node, const Core::String& oldName)	
		virtual void OnNodeAdded(Graph* graph, Node* node)									
		virtual void OnRemoveNode(Graph* graph, Node* node)									
		virtual void OnNodeRemoved(Graph* graph, Node* node)								
		virtual void OnConnectionAdded(Graph* graph, Connection* connection)				
		virtual void OnRemoveConnection(Graph* graph, Connection* connection)				
		virtual void OnConnectionRemoved(Graph* graph, Connection* connection)				
		*/

		virtual void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute) override final
		{
			LogDebug("Event: OnAttributeUpdated( graph='%s', object='%s', attribute='%x' ) ", (graph != NULL ? graph->GetName() : "NULL"), (object != NULL ? object->GetName() : "NULL"), attribute);
		}

		// Classifier Events

		virtual void OnNodeStarted(Graph* graph, SPNode* node) override final
		{
			LogDebug("Event: OnNodeStarted( graph='%s', node='%s' ) ", (graph != NULL ? graph->GetName() : "NULL"), (node != NULL ? node->GetName() : "NULL"));
		}	

		virtual void OnNodeStopped(Graph* graph, SPNode* node) override final
		{
			LogDebug("Event: OnNodeStopped( graph='%s', node='%s' ) ", (graph != NULL ? graph->GetName() : "NULL"), (node != NULL ? node->GetName() : "NULL"));
		}	

		// State Machine Events
		
		virtual void OnExitStateReached(uint32 status) override final
		{
			LogDebug("Event: OnExitStateReached( status='%i' ) ", status);
		}

		// Device Manager Events (from device manager to any)
		virtual void OnDeviceAdded(Device* device) override final
		{
			LogDebug("Event: OnDeviceAdded( device='%s' ) ", (device != NULL ? device->GetHardwareName() : "NULL"));
		}		

		virtual void OnRemoveDevice(Device* device) override final
		{
			LogDebug("Event: OnRemoveDevice( device='%s' ) ", (device != NULL ? device->GetHardwareName() : "NULL"));
		}							
					
		virtual void OnDeviceRemoved(Device* device) override final
		{
			// Note: device is always invalid pointer due to the nature of this event
			LogDebug("Event: OnDeviceRemoved( device='%x' ) ", device);
		}				

		virtual void OnActiveBciChanged(BciDevice* device) override final
		{
			LogDebug("Event: OnActiveBciChanged( device='%s' ) ", (device != NULL ? device->GetHardwareName() : "NULL"));
		}		

		virtual void OnActiveClassifierChanged(Classifier* classifier) override final
		{
			LogDebug("Event: OnActiveClassifierChanged( classifier='%s' ) ", (classifier != NULL ? classifier->GetUuid() : "NULL"));
		}		
				
		virtual void OnActiveStateMachineChanged(StateMachine* stateMachine) override final
		{
			LogDebug("Event: OnActiveStateMachineChanged( stateMachine='%s' ) ", (stateMachine != NULL ? stateMachine->GetUuid() : "NULL"));
		}		

		virtual void OnActiveExperienceChanged(Experience* experience) override final
		{
			LogDebug("Event: OnActiveExperienceChanged( experience='%s' ) ", (experience != NULL ? experience->GetUuid() : "NULL"));
		}		

		// Experience Events
		virtual void OnSetMasterVolume(double volume) override final
		{
			LogDebug("Event: OnSetMasterVolume( volume='%.3f' ) ", volume);
		}		

		virtual void OnPlayAudio(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override final
		{
			LogDebug("Event: OnPlayAudio( url='%s', numLoops='%i', beginAt='%.3f', volume='%.3f', allowStream='%s' )", url, numLoops, beginAt, volume, (allowStream ? "true" : "false"));
		}		

		virtual void OnSetAudioVolume(const char* url, double volume) override final
		{
			LogDebug("Event: OnSetAudioVolume( url='%s', volume='%.3f' ) ", url, volume);
		}

		virtual void OnPauseAudio(const char* url, bool unpause) override final
		{
			LogDebug("Event: OnPauseAudio( url='%s', unpause='%s' ) ", url, (unpause ? "true" : "false"));
		}

		virtual void OnStopAudio(const char* url) override final
		{
			LogDebug("Event: OnStopAudio( url='%s' ) ", url);
		}

		virtual void OnSeekAudio(const char* url, uint32 millisecs) override final
		{
			LogDebug("Event: OnSeekAudio( url='%s', millisecs='%i') ", url, millisecs);
		}

		virtual void OnPlayVideo(const char* url, int32 numLoops, double beginAt, double volume, bool allowStream) override final
		{
			LogDebug("Event: OnPlayVideo( url='%s', numLoops='%i', beginAt='%.3f', volume='%.3f', allowStream='%s' )", url, numLoops, beginAt, volume, (allowStream ? "true" : "false"));
		}				

		virtual void OnPauseVideo (const char* url, bool unpause) override final
		{
			LogDebug("Event: OnPauseVideo( url='%s', unpause='%s')", url, (unpause ? "true" : "false"));
		}

		virtual void OnStopVideo() override final
		{
			LogDebug("Event: OnStopVideo()");
		}

		virtual void OnSeekVideo(const char* url, uint32 millisecs) override final
		{
			LogDebug("Event: OnSeekVideo( url='%s', millisecs='%i' )", url, millisecs);
		}

		virtual void OnShowImage(const char* url) override final
		{
			LogDebug("Event: OnShowImage( url='%s')", url);
		}

		virtual void OnHideImage() override final
		{
			LogDebug("Event: OnHideImage()");
		}

		virtual void OnShowText(const char* text, const Core::Color& color) override final
		{
			LogDebug("Event: OnShowText( text='%s', color='(%.3f, %.3f, %.3f)'", text, color.r, color.g, color.b);
		}

		virtual void OnHideText() override final
		{
			LogDebug("Event: OnHideText()");
		}

		virtual void OnSetBackgroundColor(const Core::Color& color) override final
		{
			LogDebug("Event: OnSetBackgroundColor( color='(%.3f, %.3f, %.3f)'", color.r, color.g, color.b);
		}
		
		virtual void OnShowButton(const char* text, uint32 buttonId) override final
		{
			LogDebug("Event: OnShowButton( text='%s', buttonId='%i')", text, buttonId);
		}

		virtual void OnClearButtons() override final
		{
			LogDebug("Event: OnClearButtons()");
		}
		
		virtual void OnCommand(const char* command) override final
		{
			LogDebug("Event: OnCommand( command='%s')", command);
		}
		
};

}



#endif
