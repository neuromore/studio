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

#ifndef __NEUROMORE_PARAMETERNODE_H
#define __NEUROMORE_PARAMETERNODE_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "InputNode.h"


class ENGINE_API ParameterNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0003 };
		static const char* Uuid () { return "0a2f1004-bb6b-11e4-8dfc-aa07a5b093db"; }
		
		enum 
		{
			//INPUTPORT_VALUE = 0,
			INPUTPORT_SHOWCONTROL = 0,
			INPUTPORT_ENABLECONTROL,
			NUM_INPUTPORTS,
			OUTPUTPORT_VALUE = 0,
			NUM_OUTPUTPORTS
		};

		enum
		{
			ATTRIB_DESCRIPTION = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_DEFAULTVALUE,
			ATTRIB_SAMPLERATE,
			ATTRIB_NUMCHANNELS,
			ATTRIB_SHOWCONTROLS,
			ATTRIB_SHOWINPUTS,
			ATTRIB_CONTROLTYPE,
			ATTRIB_RANGEMIN,
			ATTRIB_RANGEMAX,
			ATTRIB_OPTIONS
		};

		enum EControlType
		{
			CONTROLTYPE_SEPARATOR,
			CONTROLTYPE_SLIDER,
			CONTROLTYPE_CHECKBOX,
			CONTROLTYPE_DROPDOWN,
			CONTROLTYPE_SELECTION_BUTTONS,
			CONTROLTYPE_EVENT_BUTTONS,
			NUM_CONTROLTYPES
		};

		// constructor & destructor
		ParameterNode(Graph* graph);
		~ParameterNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(211,122,255); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		const char* GetReadableType() const override							{ return "Parameter"; }
		const char* GetRuleName() const override final							{ return "NODE_Parameter"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ ParameterNode* clone = new ParameterNode(graph); return clone; }

		void UpdateSensors();
		uint32 GetNumSensors() const override									{ return mSensors.Size(); }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensors[index]; }

		void GenerateSamples() override;

		// set the parameter value manually (used by parameter widgets)
		uint32 GetNumChannels() const 											{ return GetNumSensors(); }
		void SetValue(double value, uint32 channelIndex);
		double GetValue(uint32 channelIndex = 0) const;

		// set alle values in the multichannel output
		void SetValue(double value);
		
		// accessors
		double GetDefaultValue() const											{ return GetFloatAttribute(ATTRIB_DEFAULTVALUE); }
		bool ShowControls() const												{ return GetBoolAttribute(ATTRIB_SHOWCONTROLS); }
		EControlType GetControlType() const										{ return (EControlType)GetInt32Attribute(ATTRIB_CONTROLTYPE); }
		const Core::Array<Core::String>& GetOptions() const						{ return GetStringArrayAttribute(ATTRIB_OPTIONS, Core::Array<Core::String>()); }
		const char* GetDescription() const										{ return GetStringAttribute(ATTRIB_DESCRIPTION); }

		bool IsWidgetVisible() const											{ return mShowWidget && ShowControls(); }
		bool IsWidgetEnabled() const											{ return mEnableWidget; }

		// for float range
		double GetRangeMin() const												{ return GetFloatAttribute(ATTRIB_RANGEMIN); }
		double GetRangeMax() const												{ return GetFloatAttribute(ATTRIB_RANGEMAX); }

	private:
		Core::Array<Sensor>	mSensors;			// holds output channel		
		Core::Array<double>	mValues;			// holds current values		
		ClockGenerator		mClock;				// output clock
		double				mSampleRate;		// for detecting changes in attribute
		double				mDefaultValue;		// for detecting changes in attribute
		bool				mShowInputs;		// for detecting changes in attribute
		bool				mShowControls;		// for detecting changes in attribute

		bool				mShowWidget;		// flags controled by the input
		bool				mEnableWidget;

		// set sensor / port name from parameter
		void UpdateNames();

		void UpdateInputPorts();
};


#endif
