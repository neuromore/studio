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

#ifndef __NEUROMORE_NODE_H
#define __NEUROMORE_NODE_H

// include the required headers
#include "../Config.h"
#include "../Core/EventSource.h"
#include "GraphObject.h"
#include "Connection.h"
#include "StateTransition.h"
#include "Port.h"


// forward declaration
class Graph;

class ENGINE_API Node : public GraphObject
{
	friend Graph;
	public:
		enum { BASE_TYPE = 0x002 };
		enum { NODE_TYPE = 0x001 };

		// constructor & destructor
		Node(Graph* parentGraph=NULL);
		virtual ~Node();

		virtual void Reset() override;

		// reinitialize the node
		virtual void ReInit(const Core::Time& elapsed, const Core::Time& delta) { }

		// type management
		virtual uint32 GetBaseType() const override								{ return BASE_TYPE; }
		virtual uint32 GetNodeType() const										{ return NODE_TYPE; }
		virtual const char* GetRuleName() const									{ return ""; }

		// tree traversal flags
		inline bool IsUpdateReady() const										{ return mIsUpdateReady; }
		inline void SetUpdateReady(bool isReady)								{ mIsUpdateReady = isReady; }
		bool IsReInitReady() const												{ return mIsReInitReady; }
		void SetReInitReady(bool isReady)										{ mIsReInitReady = isReady; }

		bool IsInitialized() const												{ return mIsInitialized; }

		// Async reset forces a node reset during the next ReInit() call. Node will startup immediately, if it can.
		void ResetAsync()														{ mDoAsyncReset = true; }

		virtual Core::Color GetColor() const									{ return Core::Color(0, 159, 227); }
		virtual uint32 GetPaletteCategory() const								{ return CORE_INVALIDINDEX32; }

		// node name
		virtual void SetName(const char* name) override;

		// collapsed state
		enum ECollapsedState { COLLAPSE_NONE, COLLAPSE_VIP, COLLAPSE_ALL, NUM_COLLAPSED_STATES };		// VIP = Very Important Ports
		void SetCollapsedState(ECollapsedState state);
		ECollapsedState GetCollapsedState() const								{ return mCollapsedState;}
		
		// position
		void SetVisualPos(int32 x, int32 y);
		int32 GetVisualPosX() const												{ return mPosX; }
		int32 GetVisualPosY() const												{ return mPosY; }

		virtual Core::String& GetDebugString(Core::String& inout)				{ return inout; }

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Input ports
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		inline uint32 GetNumInputPorts() const									{ return mInputPorts.Size(); }
		inline InputPort& GetInputPort(uint32 index)							{ return mInputPorts[index]; }

		// adding input ports
		void InitInputPorts(uint32 numPorts);
		uint32 AddInputPort()													{ mInputPorts.AddEmpty(); return mInputPorts.Size()-1; }
		
		// removing input ports
		void RemoveInputPort(uint32 index);

		// input port search helpers
		uint32 FindInputPortIndex(const char* internalName) const;
		uint32 FindInputPortByID(uint32 portID) const;

		bool IsValidInputPortIndex(uint16 portIndex) const						{ return mInputPorts.IsValidIndex(portIndex); }


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Output ports
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		inline uint32 GetNumOutputPorts() const									{ return mOutputPorts.Size(); }
		inline OutputPort& GetOutputPort(uint32 index) 							{ return mOutputPorts[index]; }

		// adding output ports
		void InitOutputPorts(uint32 numPorts);
		uint32 AddOutputPort()													{ mOutputPorts.AddEmpty(); return mOutputPorts.Size()-1; }

		// removing output ports
		void RemoveOutputPort(uint32 index);

		// output port search helpers
		uint32 FindOutputPortIndex(const char* internalName) const;
		uint32 FindOutputPortByID(uint32 portID) const;

		bool IsValidOutputPortIndex(uint16 portIndex) const						{ return mOutputPorts.IsValidIndex(portIndex); }


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Connections
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		bool HasConnections() const;
		bool HasIncomingConnections() const;
		bool IsOutputNode() const												{ return mInputPorts.IsEmpty() == false && mOutputPorts.IsEmpty() == true; }
		bool IsInputNode() const												{ return mInputPorts.IsEmpty() == true && mOutputPorts.IsEmpty() == false; }

		// find if a node is a parent or child of another node (also true if node == this)
		bool FindNodeOnInputs(Node* node);
		bool FindNodeOnOutputs(Node* node);
		Node* GetSourceNode(uint32 inputPortIndex);
		
		uint32 GetNumChildNodes(uint32 outputPortIndex);
		Node* GetChildNode(uint32 outputPortIndex, uint32 childIndex);
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// Serialization
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;
		virtual bool Load(const Core::Json& json, const Core::Json::Item& item) override;

	protected:
		
		// update and reinit helpers
		void UpdateAllIncomingNodes(const Core::Time& elapsed, const Core::Time& delta);
		void ReInitAllIncomingNodes(const Core::Time& elapsed, const Core::Time& delta);
		bool BaseUpdate(const Core::Time& elapsed, const Core::Time& delta);
		bool BaseReInit(const Core::Time& elapsed, const Core::Time& delta);

		Core::Array<InputPort>	mInputPorts;
		Core::Array<OutputPort>	mOutputPorts;

		// visual information
		int32					mPosX;
		int32					mPosY;
		ECollapsedState			mCollapsedState;

		bool					mIsInitialized;
		
		// flag for asynchronous reset
		bool					mDoAsyncReset;

		// temporal tree traversal flag
		bool					mIsUpdateReady;
		bool					mIsReInitReady;
		bool					mIsFirstUpdateReady;
};


#endif
