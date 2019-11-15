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

#ifndef __NEUROMORE_MULTICHANNELCONTAINER_H
#define __NEUROMORE_MULTICHANNELCONTAINER_H

// include required headers
#include "../Config.h"
#include "../Core/StandardHeaders.h"
#include "ChannelBase.h"
#include <edflib/edflib.h>

// (de)serialize channels and multichannels
class ENGINE_API ChannelFileReader
{
	public:

		// the supported container formats
		enum EFormat
		{
			FORMAT_CSV_SIMPLE,
			FORMAT_CSV_TIMESTAMP,
			FORMAT_EDF_PLUS,
			//FORMAT_RAW_DOUBLE,
			NUM_FORMATS
		};


		// constructor & destructor
		ChannelFileReader()			{}
		~ChannelFileReader()		{}

		static const char* GetFormatName(EFormat format);
		static const char* GetFormatExtension(EFormat format);

		//
		// main methods
		//

		// reads an array of channels from a file in the specified format. If read fails false will be returned, the array will hold the data that was sucessfully read.
		bool Read(FILE* file, const char* filename, EFormat format, Core::Array<Channel<double>*>& channels);

	private:

		//
		// individual formats
		//


		// TODO if we want to stream files from disk instead of loading them at once, we have to split the read functions into a ReadHeader and ReadSamples (see ChanneFileWriter), and have to add getters like GetNumSampels and stuff..
		// NOTE don't do this until it its unavoidable.. requires a lot of additional stuff here 

		// CSV
		bool ReadCSV(FILE* inFile, bool useTimestamps, Core::Array<Channel<double>*>& outChannels);

		// .nmd
		bool ReadRawDouble(FILE* inFile, Core::Array<Channel<double>*>& outChannels);

		// .edf
		bool ReadEDF(FILE* inFile, const char* filename, Core::Array<Channel<double>*>& outChannels);

		// helpers
		bool SeekNumber(char* pos, const char* end);
};


#endif
