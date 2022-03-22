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

#ifndef __NEUROMORE_LORETAPLUGIN_H
#define __NEUROMORE_LORETAPLUGIN_H

// include required headers
#include "../../../Config.h"
#ifdef OPENCV_SUPPORT
#include <PluginSystem/Plugin.h>
#include "LoretaWidget.h"
#include <Core/EventHandler.h>
#include <AttributeWidgets/Property.h>

class LoretaPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		// constructor & destructor
		LoretaPlugin();
		virtual ~LoretaPlugin();

		// general
		const char* GetName() const override								{ return "LORETA Plugin"; }
		static const char* GetStaticTypeUuid()								{ return "dc5acfdd-e8f0-48f4-b7ca-ee9118935d7c"; }
		const char* GetRuleName() const override							{ return "PLUGIN_Loreta"; }
		Plugin* Clone() override											{ return new LoretaPlugin(); }

		// initialization
		void RegisterAttributes() override;
		bool Init() override;

		// reinit LORETA if input has changed
		void ReInitLoreta();

		
		// EVENTS
		// FIXME this is probably inefficient, we don't have to connect to all events
		// NOTE: this does not work! we cannot use e.g. OnConnectionAdded() to get the channels of the LoretaNode, because they are 
		// set in LoretaNode::Start() which is executed during the next update, so it happens after the event!!
		void OnNodeAdded(Graph* graph, Node* node) override						 { ReInitLoreta(); }
		void OnNodeRemoved(Graph* graph, Node* node) override					 { ReInitLoreta(); }
		void OnConnectionAdded(Graph* graph, Connection* connection) override	 { ReInitLoreta(); }
		void OnConnectionRemoved(Graph* graph, Connection* connection ) override { ReInitLoreta(); }
		void OnActiveClassifierChanged(Classifier* classifier) override			 { ReInitLoreta(); }
		void OnNodeStarted(Graph* graph, SPNode* node) override					 { ReInitLoreta(); }

		// access to loreta threadhandler for widget and rendercallback
		LoretaThreadHandler* GetLoretaThreadHandler()													{ return mLoretaThreadHandler; }

		// settings
		int32 GetNumVoxelSpinner()											{ return GetInt32AttributeByName("numVoxelSpinner"); }
	
		uint32 GetBrainPartSelection()										{ return GetInt32AttributeByName("brainComboBox"); }
		uint32 GetBrainColoringSelection()									{ return GetInt32AttributeByName("coloringComboBox"); }

		#ifdef LORETA_DEBUG
		bool GetDebugCheckbox()												{ return GetBoolAttributeByName("debugGridCheckBox"); }
		bool GetTIFFExportCheckBox()										{ return GetBoolAttributeByName("exportTIFFCheckBox"); }
		#endif

		bool GetRenderBrainCheckbox()										{ return GetBoolAttributeByName("renderBrainCheckbox"); }
		bool GetRenderHeadCheckbox()										{ return GetBoolAttributeByName("renderHeadCheckbox"); }

		float GetTransparency()												{ return GetFloatAttributeByName("numTransparency"); }
		float GetRegParameter()												{ return GetFloatAttributeByName("numRegParameter"); }
		float GetConductivity()												{ return GetFloatAttributeByName("numConductivity"); }

		ColorMapper GetColorMapper()										{ return mColorMapper; }

		// brain mesh
		void InitRenderMesh();
		const Mesh& GetBrainMesh() const									{ return mBrainMesh; }
		const Core::AABB& GetBrainMeshAABB() const							{ return mBrainMeshAABB; }

	private slots:
		void OnAttributeChanged(Property* property);
		
	private:
		LoretaWidget*	mLoretaWidget;

		Mesh			mBrainMesh;
		Core::AABB		mBrainMeshAABB;

		ColorMapper     mColorMapper;

		// thread
		Core::Thread*		  mThread;
		LoretaThreadHandler*  mLoretaThreadHandler;

};

#endif // OPENCV_SUPPORT
#endif
