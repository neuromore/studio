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

// include precompiled header
#include <Engine/Precompiled.h>

// include required files
#include "EncryptedJSONFile.h"
#include "LogManager.h"
#include "AES.h"


namespace Core
{

// load the license
Json* EncryptedJSONFile::Load(const char* filename)
{
	// load the license file
	FILE* file;
	file = fopen(filename, "rb\0");
	if (file == NULL)
	{
		LogError( "Cannot read '%s'.", filename );
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// read the encrypted data
	uint8* encryptedData = (uint8*)Allocate(fileSize);
	fread(encryptedData, fileSize, 1, file);
	fclose(file);

	// decrypt the data
	AES decryptor;
	ByteArray key;
	key.FromString(ENCRYPTEDJSONFILE_ENCRYPTIONKEY);
	ByteArray sourceData( encryptedData, fileSize );
	ByteArray decryptedData = decryptor.Decrypt(sourceData, key);

	// convert the decrypted data to a string
	char* stringData = (char*)Allocate(decryptedData.Size()+1);
	Core::MemCopy( stringData, decryptedData.Data(), decryptedData.Size());
	stringData[decryptedData.Size()] = '\0';

	// clear the memory again
	Core::Free( encryptedData );

	// parse the json data
	Json* json = new Json();
	if (json->Parse(stringData) == false)
	{
		LogError( "Cannot parse json for '%s'.", filename );
		delete json;
		Core::Free( stringData );
		return NULL;
	}

	// return success
	Core::Free( stringData );
	return json;
}


// save the license
bool EncryptedJSONFile::Save(const char* filename, Core::Json* json)
{
	// open the license file for writing
	// create the file on disk
	FILE* file;
	file = fopen(filename, "wb\0");
	if (file == NULL)
	{
		LogError("Cannot open '%s' for saving.", filename);
		return false;
	}

	// convert the json data to a string
	String jsonData;
	json->WriteToString( jsonData );

	// decrypt the data
	AES encryptor;
	ByteArray key;
	key.FromString(ENCRYPTEDJSONFILE_ENCRYPTIONKEY);
	ByteArray sourceData((uint8*)(jsonData.AsChar()), jsonData.GetLength() );
	ByteArray encryptedData = encryptor.Encrypt( sourceData, key );

	// write the encrypted data to the file
	if (fwrite(encryptedData.Data(), 1, encryptedData.Size(), file) < 0)
	{
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}

}
