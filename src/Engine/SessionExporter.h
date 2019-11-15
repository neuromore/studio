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

#ifndef __NEUROMORE_SESSIONEXPORTER_H
#define __NEUROMORE_SESSIONEXPORTER_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/MemoryFile.h"
#include "Device.h"
#include "Session.h"
#include "Graph/Classifier.h"


class ENGINE_API SessionExporter
{
	public:
		static bool Save(const char* folderPath, const char* userId, const char* dataChunkId);

		// data chunk
		static bool GenerateDataChunkJson(Core::Json& json, Core::Json::Item& item, const char* userId, const char* supervisorId, const char* debitorId, const char* classifierId, uint32 classifierRevision, const Core::String& stateMachineId, uint32 stateMachineRevision, const Core::String& experienceId, uint32 experienceRevision, const Core::String& startDateTime, const Core::String& stopDateTime);

		// channel json
		static bool GenerateChannelJson(Core::Json& json, Core::Json::Item& item, const char* userId, const char* dataChunkId, const char* classifierUuid, const char* nodeUuid, Channel<double>* channel);
		static bool SaveChannelJsonToDisk(const char* filename, const char* userId, const char* dataChunkId, const char* classifierUuid, const char* nodeUuid, Channel<double>* channel);

		// sample helpers
		static bool SaveSamplesToDisk(const char* filename, Channel<double>* channel);
		static bool SaveSamplesToMemoryFile(Core::MemoryFile* outFile, Channel<double>* channel);
		static void SaveSamples(Core::MemoryFile* file, Channel<double>* channel);
};


#endif
