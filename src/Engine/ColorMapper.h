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

#ifndef __NEUROMORE_COLORMAPPER_H
#define __NEUROMORE_COLORMAPPER_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/Array.h"
#include "Core/Color.h"


// the color mapper class
class ENGINE_API ColorMapper
{
	public:
		enum EColorMapping
		{
			COLORMAPPING_RAINBOW				= 0,
			COLORMAPPING_BLUEPINKRED			= 1,
			COLORMAPPING_GREYSCALE				= 2,
			COLORMAPPING_BLACKBODYRADIATION		= 3,
			COLORMAPPING_ISOLUMINANTGREENRED	= 4,
			COLORMAPPING_ISOLUMINANTBLUERED     = 5,
			COLORMAPPING_REDYELLOWGREENBLUE		= 6,
			COLORMAPPING_NUM					= 7
		};

		// constructor & destructor
		ColorMapper();
		virtual ~ColorMapper();

		// calculate the gradient color
		Core::Color CalcColor(float normalizedValue);

		// automatic color mapping
		void SetColorMapping(EColorMapping colorMapping);
		EColorMapping GetColorMapping() const											{ return mColorMapping; }
		static const char* GetColorMappingName(EColorMapping colorMapping);

		// access to internals
		uint32 GetNumColors() const														{ return mColors.Size(); }
		const Core::Color& GetColor(uint32 index) const								{ return mColors[index].mColor; }
		float GetColorPos(uint32 index) const											{ return mColors[index].mValue; }

		// manual control
		void Add(const Core::Color& color, float value);
		void Clear();

	private:
		struct Keyframe
		{
			Core::Color		mColor;
			float				mValue;
		};

		Core::Array<Keyframe>	mColors;
		uint32					mNumColors;
		EColorMapping			mColorMapping;
};


#endif
