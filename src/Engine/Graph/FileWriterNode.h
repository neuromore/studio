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

#ifndef __NEUROMORE_FILEWRITERNODE_H
#define __NEUROMORE_FILEWRITERNODE_H

// include the required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "../DSP/ClockGenerator.h"
#include "../DSP/ChannelFileWriter.h"
#include "InputNode.h"


class ENGINE_API FileWriterNode : public SPNode
{
	public:
		enum { TYPE_ID = 0x0041 };
		enum { OUTPUTNODE_TYPE = 0x004 };
		static const char* Uuid () { return "edca7a3e-7519-11e5-8bcf-feff819cdc9f"; }
		
		enum { INPUTPORT_VALUE = 0 };
		enum
		{
			ATTRIB_FILE,
			ATTRIB_FORMAT,
			ATTRIB_WRITEMODE,
			NUM_ATTRIBUTES
		};

		enum EError
		{
			ERROR_FILE_NOT_WRITEABLE	= GraphObjectError::ERROR_RUNTIME | 0x01,
			ERROR_FILE_ALREADY_EXISTS	= GraphObjectError::ERROR_RUNTIME | 0x02,
		};


		// constructor & destructor
		FileWriterNode(Graph* graph);
		~FileWriterNode();

		// initialize & update
		void Init() override; 
		void Reset() override;
		void ReInit(const Core::Time& elapsed, const Core::Time& delta) override;
		void Start(const Core::Time& elapsed) override;
		void Update(const Core::Time& elapsed, const Core::Time& delta) override;
		
		void OnAttributesChanged() override;

		// node information & helpers
		Core::Color GetColor() const override									{ return Core::RGBA(122,211,255); }
		uint32 GetType() const override											{ return TYPE_ID; }
		const char* GetTypeUuid() const override final							{ return Uuid(); }
		uint32 GetPaletteCategory() const override								{ return CATEGORY_OUTPUT; }
		const char* GetReadableType() const override							{ return "File Writer"; }
		const char* GetRuleName() const override final							{ return "NODE_FileWriter"; }
		GraphObject* Clone(Graph* graph) override								{ FileWriterNode* clone = new FileWriterNode(graph); return clone; }

		bool closeFile();

	private:
		ClockGenerator					mClock;				// clock for regular writing

		Core::Array<Channel<double>*>	mWriteChannels;		// the write channels

		FILE*							mFile;				// the filehandle
		Core::String					mFileName;			// the final filename
		Core::String					mFileNameUnchanged;	// the unchanged filename (same as attribute)
		
		ChannelFileWriter				mFileWriter;		// writes channels to files
		ChannelFileWriter::EFormat		mFileFormat;		// the selected file format

		int								mHandle;			// the handle of the edf plus file

		bool							mHasWriteError;		// remember file write errors
		bool							mIsWriting;			// file write state (true after header was written)

		enum EWriteMode
		{
			WRITEMODE_KEEP = 0, 
			// WRITEMODE_OVERWRITE_ALWAYS,
			WRITEMODE_OVERWRITE
			// WRITEMODE_APPEND
		};

		EWriteMode					mWriteMode;			// overwrite/append modes


};


#endif
