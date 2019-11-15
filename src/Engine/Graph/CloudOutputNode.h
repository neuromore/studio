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

#ifndef __NEUROMORE_CLOUDOUTPUTNODE_H
#define __NEUROMORE_CLOUDOUTPUTNODE_H

// include the required headers
#include "../Config.h"
#include "../CloudParameters.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "SPNode.h"


class ENGINE_API CloudOutputNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0043 };
		static const char* Uuid () { return "b8f5fc04-7cb0-11e5-8bcf-feff819cdc9f"; }
		
		enum { INPUTPORT_VALUE = 0 };

		enum { 
			ATTRIB_STORAGETYPE = 0,
			ATTRIB_STORAGEMODE,
		};

		enum EStorageType
		{
			STORAGETYPE_USERPARAMETER,
			STORAGETYPE_CLASSIFIERPARAMETER
		};

		enum EStorageMode
		{
			STORAGEMODE_OVERWRITE,
			STORAGEMODE_SAVEHISTORY
		};

		enum EError
		{
			ERROR_DUPLICATE_NAME	= GraphObjectError::ERROR_CONFIGURATION | 0x01,
			ERROR_WRONG_INPUT		= GraphObjectError::ERROR_CONFIGURATION | 0x02,
		};

		
		// constructor & destructor
		CloudOutputNode(Graph* graph);
		~CloudOutputNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(211, 211, 230); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Cloud Output"; }
		const char* GetRuleName() const override final							{ return "NODE_CloudOutput"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_OUTPUT; }
		GraphObject* Clone(Graph* graph) override								{ CloudOutputNode* clone = new CloudOutputNode(graph); return clone; }

		// access the parameter value
		bool HasOutputValue();
		double GetOutputValue();

		EStorageType GetStorageType() const										{ return (EStorageType)GetInt32Attribute(ATTRIB_STORAGETYPE); }
		EStorageMode GetStorageMode() const										{ return (EStorageMode)GetInt32Attribute(ATTRIB_STORAGEMODE); }

		uint32 SaveCloudParameters(CloudParameters& parameters);
	
	private:
		bool IsParameterNameUnique(const char* name);
};


#endif
