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

#ifndef __NEUROMORE_FILEREADERNODE_H
#define __NEUROMORE_FILEREADERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "../DSP/ChannelFileReader.h"
#include "InputNode.h"


class ENGINE_API FileReaderNode : public InputNode
{
	public:
		enum { TYPE_ID = 0x0038 };
		static const char* Uuid () { return "93d9c13a-629d-11e5-9d70-feff819cdc9f"; }
		
		enum { OUTPUTPORT_VALUE = 0 };
		enum
		{
			ATTRIB_URL = NUM_INPUTNODEATTRIBUTES,
			ATTRIB_FORMAT,
			ATTRIB_SAMPLERATE,
			NUM_ATTRIBUTES
		};

		enum EError
		{
			ERROR_FILE_NOT_READABLE		= GraphObjectError::ERROR_RUNTIME | 0x01,
			ERROR_FORMAT_NOT_READABLE	= GraphObjectError::ERROR_RUNTIME | 0x02,
		};

		enum EWarning
		{
			WARNING_FILE_EMPTY	= GraphObjectWarning::WARNING_RUNTIME | 0x01,
		};

		// constructor & destructor
		FileReaderNode(Graph* graph);
		~FileReaderNode();

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
		const char* GetReadableType() const override							{ return "File Reader"; }
		const char* GetRuleName() const override final							{ return "NODE_FileReader"; }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_INPUT; }
		GraphObject* Clone(Graph* graph) override								{ FileReaderNode* clone = new FileReaderNode(graph); return clone; }

		uint32 GetNumSensors() const override									{ return mSensors.Size(); }
		Sensor* GetSensor(uint32 index)  override								{ return &mSensors[index]; }

		void GenerateSamples() override;

		bool IsUploadEnabled() const											{ return false; }

	private:
		Core::Array<Sensor>				mSensors;		// output sensors
		ClockGenerator					mClock;			// main sample output clock 

		ChannelFileReader				mFileReader;	// for reading files
		double							mNumSamples;	// maxmimum number of samples that can be read from all channels
		Core::Array<Channel<double>*>	mFileChannels;	// the loaded samples from file

		double							mSampleRate;	// output sample rate (same for all outputs)
		Core::String					mFileName;		// for detecting attribute changes
		uint32							mFileFormat;	// for detecting attribute changes

		bool							mHasLoadError;  // additional error state so it donesn't try to parse a bad file more than once
		bool							mHasData;		// if node has data it can output


};


#endif
