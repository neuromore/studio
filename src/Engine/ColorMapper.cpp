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
#include "ColorMapper.h"
#include "Core/Math.h"
#include "Core/LogManager.h"


using namespace Core;

// constructor
ColorMapper::ColorMapper()
{
	SetColorMapping( COLORMAPPING_RAINBOW );
}


// destructor
ColorMapper::~ColorMapper()
{
}


// add a color to the color mapping gradient
void ColorMapper::Add(const Core::Color& color, float value)
{
	value = Clamp<float>( value, 0.0, 1.0 );

	Keyframe key;
	key.mColor = color;
	key.mValue = value;
	mColors.Add(key);
}


// clear the color mapping
void ColorMapper::Clear()
{
	mColors.Clear();
}


// adjust the color mapping to use
void ColorMapper::SetColorMapping(EColorMapping colorMapping)
{
	// get rid of the old color mapping
	Clear();

	// assign the new color mapping
	mColorMapping = colorMapping;
	switch(mColorMapping)
	{

		// blue to green to yellow to red
		case COLORMAPPING_RAINBOW:
		{
			Add( Color( 0.0, 1.0, 0.0 ), 0.0 );
			Add( Color( 1.0, 1.0, 0.0 ), 0.5 );
			Add( Color( 1.0, 0.0, 0.0 ), 1.0 );
			
			break;
		}

		case COLORMAPPING_BLUEPINKRED:
		{
			Add( Color( 0.0, 0.0, 1.0), 0.0 );
			Add( Color( 1.0, 0.0, 1.0), 0.5 );
			Add( Color( 1.0, 0.0, 0.0), 1.0 );
		}

		// black to white
		case COLORMAPPING_GREYSCALE:
		{
			Add( Color( 0.0, 0.0, 0.0 ), 0.0 );
			Add( Color( 1.0, 1.0, 1.0 ), 1.0 );
			break;
		}

		// black to orange to white
		case COLORMAPPING_BLACKBODYRADIATION:
		{
			Add( Color( 0.0, 0.0, 0.0 ), 0.0 );
			Add( Color( 1.0, 0.5, 0.0 ), 0.5 );
			Add( Color( 1.0, 1.0, 1.0 ), 1.0 );
			break;
		}

		// green to red
		case COLORMAPPING_ISOLUMINANTGREENRED:
		{
			Add( Color( 0.0, 1.0, 0.0 ), 0.0 );
			Add( Color( 1.0, 0.0, 0.0 ), 1.0 );
			break;
		}

		case COLORMAPPING_ISOLUMINANTBLUERED:
		{
			Add( Color( 0.0, 0.0, 1.0), 0.0 );
			Add( Color( 1.0, 0.0, 0.0), 1.0 );
		}

		case COLORMAPPING_REDYELLOWGREENBLUE:
		{
			Add( Color( 0.0, 0.0, 1.0), 0.0  );
			Add( Color( 0.0, 1.0, 0.5), 0.33f );
			Add( Color( 1.0, 1.0, 0.0), 0.66f );
			Add( Color( 1.0, 0.0, 0.0), 1.0  );
			
		}
        
        default: { break; }
	}

	// update the number of colors
	mNumColors = mColors.Size();
}


// calculate the color
Core::Color ColorMapper::CalcColor(float normalizedValue)
{
	// assume we have at least two colors, else a gradient color mapping doesn't make any sense
	CORE_ASSERT( mNumColors > 1 );

	float value = 0.0;
	Color colorA, colorB;

	// go either to the optimized two color path or the keyframe way
	if (mNumColors != 2)
	{
		// iterate through the colors and find the two correct ones
		for (uint32 i=1; i<mNumColors; ++i)
		{
			if (mColors[i].mValue >= normalizedValue)
			{
				colorA	= mColors[i-1].mColor;
				colorB	= mColors[i  ].mColor;

				float valueA = mColors[i-1].mValue;
				float valueB = mColors[i  ].mValue;
				float rangedValue = (normalizedValue - valueA) / (valueB - valueA);
				value = rangedValue;

				break;
			}
		}
	}
	else // we are dealing with a simple two colored gradient
	{
		colorA	= mColors[0].mColor;
		colorB	= mColors[1].mColor;
		value	= Clamp<float>(normalizedValue, 0.0, 1.0);
	}

	// interpolate and return the color
	return Core::LinearInterpolate<Color>( colorA, colorB, value );
}


// get the color mapping name
const char* ColorMapper::GetColorMappingName(EColorMapping colorMapping)
{
	switch(colorMapping)
	{
		case COLORMAPPING_RAINBOW:				{ return "Rainbow"; }
		case COLORMAPPING_BLUEPINKRED:			{ return "Blue-Pink-Red"; }
		case COLORMAPPING_GREYSCALE:			{ return "Grey-Scale"; }
		case COLORMAPPING_BLACKBODYRADIATION:	{ return "Black-Body-Radiation"; }
		case COLORMAPPING_ISOLUMINANTGREENRED:	{ return "Isoluminant Green-Red"; }
		case COLORMAPPING_ISOLUMINANTBLUERED:   { return "Isoluminant Blue-Red"; }
		case COLORMAPPING_REDYELLOWGREENBLUE:	{ return "Red-Yellow-Green-Blue"; }
        default:                                { return ""; }
	}
}
