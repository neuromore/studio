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

#ifndef __NEUROMORE_POINTSNODE_H
#define __NEUROMORE_POINTSNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "OutputNode.h"


class ENGINE_API PointsNode : public OutputNode
{
	public:
		enum { TYPE_ID				= 0x0015 };
		enum { OUTPUTNODE_TYPE		= 0x003 };
		static const char* Uuid () { return "30589b50-bb6c-11e4-8dfc-aa07a5b093db"; }

		enum
		{
			ATTRIB_POINTSMULTIPLIER = NUM_BASEATTRIBUTES + 0
		};

		enum { INPUTPORT_VALUE		= 0 };

		PointsNode(Graph* parentNode);
		~PointsNode();

		void Init() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Reset() override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;

		Core::Color GetColor() const override								{ return Core::RGBA(112,52,255); }
		uint32 GetType() const override											{ return TYPE_ID; }
		uint32 GetOutputNodeType() const override								{ return OUTPUTNODE_TYPE; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Points"; }
		const char* GetRuleName() const override final							{ return "NODE_Points"; }
		GraphObject* Clone(Graph* parentGraph) override							{ PointsNode* clone = new PointsNode(parentGraph); return clone; }

		double GetPoints() const												{ return mPoints; }

		// DEPRECATE
		virtual Channel<double>* GetRawInputChannel(uint32 portIndex) override	{ return &mIntegratedInput; }
		void DEPRECATED_Update(const Core::Time& elapsed, const Core::Time& delta);

	private:
		// DEPRECATE
		Channel<double> mIntegratedInput;

		double mPoints;
};


#endif
