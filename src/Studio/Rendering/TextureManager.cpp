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

// include the required headers
//#define STB_IMAGE_IMPLEMENTATION
#include "TextureManager.h"
#include <Core/LogManager.h>
#include <QFile>


using namespace Core;

// constructor
TextureManager::TextureManager()
{
	LogDetailedInfo("Constructing texture manager ...");

}


// destructor
TextureManager::~TextureManager()
{
	LogDetailedInfo("Destructing texture manager ...");

/*	Clear();

	// get rid of the OpenGL items
	const uint32 numItems = mOpenGLItems.Size();
	for (uint32 i=0; i<numItems; ++i)
		delete mOpenGLItems[i];
	mOpenGLItems.Clear();*/
}


bool TextureManager::LoadTextureFromQtResource(const char* filename)
{
	// load the file
/*	QFile imageFile(filename);
	if (imageFile.open(QIODevice::ReadOnly) == false)
	{
		LogError( "TextureManager::LoadTextureFromQtResource(): Cannot read image file from Qt resource path '%s'.", filename);
		return false;
	}

	// read the file size
	int dataSize = imageFile.size();

	// allocate memory
	mImageData.Resize( dataSize );

	// copy over data
	Core::MemCopy( mImageData.GetPtr(), imageFile.readAll().data(), dataSize );

	mImage = stbi_load_from_memory(mImageData.GetPtr(), mImageData.Size(), &mStbiWidth, &mStbiHeight, &mStbiComp, STBI_rgb_alpha);
	*/
	return true;
}
