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

// include required files
#include "GraphExporter.h"
#include "../Graph/Node.h"
#include "../Core/LogManager.h"
#include "../Core/Timer.h"


using namespace Core;

// save the given graph to the given string as JSON
bool GraphExporter::Save(Core::String* outString, Graph* graph)
{
	// make sure the graph to export is valid
	if (graph == NULL)
	{
		LogError("GraphExporter::Save(): Cannot save graph. Graph is invalid.");
		return false;
	}

	Timer saveTimer;

	// create our json parser
	Json json;
	Json::Item rootItem = json.GetRootItem();

	// fill the json parser
	graph->Save( json, rootItem );

	// save data to string
	json.WriteToString( *outString, false );

#ifdef CORE_DEBUG
	json.Log();
#endif

	// log the timing information
	const float saveTime = saveTimer.GetTime().InSeconds();
	LogInfo( "Saving %s took %.1f ms.", graph->GetReadableType(), saveTime * 1000.0f );
	return true;
}


// save the given graph to the file with the given filename
bool GraphExporter::Save(const char* filename, Graph* graph)
{
	// make sure the graph to export is valid
	if (graph == NULL)
	{
		LogError("GraphExporter::Save(): Cannot save graph. Graph is invalid.");
		return false;
	}

	Timer saveTimer;

	// create our json parser
	Json json;
	Json::Item rootItem = json.GetRootItem();

	// fill the json parser
	graph->Save( json, rootItem );

#ifdef CORE_DEBUG
	json.Log();
#endif

	// save data to disk
	bool result = json.WriteToFile( filename, false );

	// log the timing information
	const float saveTime = saveTimer.GetTime().InSeconds();
	LogInfo( "Saving %s '%s' took %.1f ms.", graph->GetReadableType(), filename, saveTime * 1000.0f );

	return result;
}
