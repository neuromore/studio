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

#ifndef __NEUROMORE_VISUALIZATION_H
#define __NEUROMORE_VISUALIZATION_H

// include required headers
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "Config.h"


class Visualization
{
	public:
		Visualization();
		virtual ~Visualization();

		// start the visualization
		void Start();

		bool ParseFromJsonFile(const char* filename);

		// name
		void SetName(const char* name)								{ mName = name; }
		const char* GetName() const									{ return mName; }

		// description
		void SetDescription(const char* desc)						{ mDescription = desc; }
		const char* GetDescription() const							{ return mDescription; }

		// executable filename
		void SetExecutableFilename(const char* filename);
		const char* GetExecutableFilename() const					{ return mExecutableFilename; }

		// name
		void SetImageFilename(const char* filename)					{ mImageFilename = filename; }
		const char* GetImageFilename() const						{ return mImageFilename; }

	private:
		Core::String	mName;
		Core::String	mDescription;
		Core::String	mExecutableFilename;
		Core::String	mImageFilename;
};


#endif
