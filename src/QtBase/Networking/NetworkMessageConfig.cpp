/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include required headers
#include "NetworkMessageConfig.h"
#include <Core/LogManager.h>
#include <QHostAddress>


using namespace Core;

// constructor
NetworkMessageConfig::NetworkMessageConfig(int clientType, Core::String name) : NetworkMessage(new QByteArray())
{
	mClientType = clientType;
	mName = name;
	Write();
}

// create a message from raw data
NetworkMessageConfig::NetworkMessageConfig(QByteArray* data) : NetworkMessage(data)
{
	Read();
}


// destructor
NetworkMessageConfig::~NetworkMessageConfig()
{
}


// write information to byte array
void NetworkMessageConfig::Write()
{
	// construct json object
	Json json;
	Json::Item rootItem = json.GetRootItem();

	// add basic items
	rootItem.AddInt("clientType", mClientType);
	rootItem.AddString("name", mName);

	json.WriteToString(mMessage, false);
		
	// calculate the size of the message and resize the byte array
	SetPayloadSize(mMessage.GetLength());

	// get access to the byte array data and copy the memory over to it
	char* data = GetPayload();

	// copy the message
	memcpy(data, mMessage.AsChar(), mMessage.GetLength());

	// write header
	WriteHeader();
}


// read information from byte array
void NetworkMessageConfig::Read()
{
	// parse header
	ReadHeader();

	// calculate the size of the data and resize message string length
	mMessage.Resize(GetPayloadSize());

	// get access to the byte array data
	char* data = GetPayload();

	// copy the message
	memcpy(mMessage.AsChar(), data, mMessage.GetLength());

	// parse json string
	Json json;
	if (json.Parse(mMessage) == false)
		return;

	Json::Item clientType = json.Find("clientType");
	if (clientType.IsInt() == true) 
		mClientType = clientType.GetInt();

	Json::Item nameItem = json.Find("name");
	if (nameItem.IsString() == true)
		mName = nameItem.GetString();
}
