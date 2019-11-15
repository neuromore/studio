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

#ifndef __NEUROMORE_ACTION_H
#define __NEUROMORE_ACTION_H

// include the required headers
#include "../Config.h"
#include "../Core/String.h"
#include "../Core/EventSource.h"
#include "GraphObject.h"
#include "Graph.h"


// action class
class ENGINE_API Action : public GraphObject
{
	public:
		enum { BASE_TYPE = 0x005 };

		// constructor & destructor
		Action(Graph* graph);
		virtual ~Action();

		uint32 GetBaseType() const override final												{ return BASE_TYPE; }

		// action operation
		virtual void Execute() = 0;

		// linked asset
		enum AssetType
		{
			ASSET_NONE = 0,
			ASSET_AUDIO,
			ASSET_VIDEO,
			ASSET_IMAGE
		};
		virtual void GetAsset(Core::String* outLocation, AssetType* outType, bool* outAllowStreaming) const				{ *outLocation=""; *outType=ASSET_NONE; *outAllowStreaming=false; }

		// Serialization
		Core::Json::Item Save(Core::Json& json, Core::Json::Item& item) override;

	protected:
		StateMachine*		mParentStateMachine;
};


#endif
