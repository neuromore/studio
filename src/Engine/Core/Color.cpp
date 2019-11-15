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
#include "Color.h"
#include "Math.h"


namespace Core
{

// constructor
Color::Color()
{
	r = 0.0;
	g = 0.0;
	b = 0.0;
	a = 1.0;
}


// constructor
Color::Color(float value)
{
	r = value;
	g = value;
	b = value;
	a = value;
}


// constructor
Color::Color(float inR, float inG, float inB, float inA)
{
	r = inR;
	g = inG;
	b = inB;
	a = inA;
}


// constructor
Color::Color(const Color& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
}


// constructor
Color::Color(uint32 color)
{
	Set(color);
}


void Color::Set(float inR, float inG, float inB, float inA)
{
	r = inR;
	g = inG;
	b = inB;
	a = inA;
}


void Color::Set(uint32 color)
{
	r = ExtractRed(color)  / 255.0;
	g = ExtractGreen(color) / 255.0;
	b = ExtractBlue(color) / 255.0;
	a = ExtractAlpha(color) / 255.0;
}


void Color::Set(const Color& color)
{
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
}


void Color::Zero()
{
	r = 0.0;
	g = 0.0;
	b = 0.0;
	a = 0.0;
}


Color& Color::Clamp()
{
	r = Core::Clamp<float>(r, 0.0, 1.0);
	g = Core::Clamp<float>(g, 0.0, 1.0);
	b = Core::Clamp<float>(b, 0.0, 1.0);
	a = Core::Clamp<float>(a, 0.0, 1.0);
	
	return *this;
}


uint32 Color::ToDWORD() const
{
	return Core::RGBA((uint8)(r*255), (uint8)(g*255), (uint8)(b*255), (uint8)(a*255));
}


/** 
 * Set RGB Values using the HSV color space
 * @param H Hue, range is (0.0, 1.0)
 * @param S Saturation, range is (0.0, 1.0)
 * @param V Value, range is (0, 1.0)
 * @result Returns itself, with changed color values
 */
Color& Color::FromHSV(float H, float S, float V)
{
	const float h = H * 360.0f / 60.0f;		// normalized hue (60 degrees)
	const float C = S * V;          // chroma
    const float X = C * (1.0f - Math::Abs(Math::FMod(h, 2.0f) - 1.0f));
	const float M = V - C;

	if (0 <= h && h < 1)		{ r = C; g = X; b = 0; }
	else if (1 <= h && h < 2)	{ r = X; g = C; b = 0; }
	else if (2 <= h && h < 3)	{ r = 0; g = C; b = X; }
	else if (3 <= h && h < 4)	{ r = 0; g = X; b = C; }
	else if (4 <= h && h < 5)	{ r = X; g = 0; b = C; }
	else if (5 <= h && h < 6)	{ r = C; g = 0; b = X; }
	else						{ r = 0; g = 0; b = 0; }

	r += M;
	g += M;
	b += M;

	Clamp();

	return *this;
}


Color& Color::FromHexString(const String& hexString)
{
	// must be 7 chars
	if (hexString.GetLength() == 7)
	{
		uint32 color;

		if (sscanf(hexString.AsChar(), "#%x", &color) == 1)
		{
			r = ExtractRed(color)  / 255.0;
			g = ExtractGreen(color) / 255.0;
			b = ExtractBlue(color) / 255.0;
			a = 1.0;

			Clamp();
		}
	}

	return *this;
}


String Color::ToHexString() const
{
	//					            vvvv clamping vvvv
	unsigned char red =   (r < 0.0f ? 0.0f : (r > 1.0f ? 1.0f : r) ) * 255;
	unsigned char green = (g < 0.0f ? 0.0f : (g > 1.0f ? 1.0f : g) ) * 255;
	unsigned char blue =  (b < 0.0f ? 0.0f : (b > 1.0f ? 1.0f : b) )  * 255;

	String hexString;
	hexString.Format("#%02X%02X%02X", red, green, blue);
	return hexString;
}


/** 
 * Pick a unique, nice contrasting bright color. Works based on golden-ratio sampling of the Color Hue space
 * The algorithm assigns a unique color to each number. For each increment in the number, the algorithm always picks the color that is farthest
 * away from all previously picked colors (Golden-ratio sampling, see http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/)
 * @param n The index of the color to pick
 * @result Returns itself, with changed color values
 */
Color& Color::SetUniqueColor(uint32 index)
{
	uint32 maxIndex = index;
	const float goldenRatioConjugate = 0.6180339887498f;
			
	// well-selected starting point
	float hue = 0.3f;

	// generate hue iteratively
	for (uint32 i=0; i<maxIndex; ++i)
	{
		hue += goldenRatioConjugate;
		hue = Math::FMod(hue, 1.0f);
	}

	FromHSV(hue, 0.85f, 1.0f);

	return *this;
}

// operators
bool Color::operator==(const Color& color) const			{ return ((r==color.r) && (g==color.g) && (b==color.b) && (a==color.a)); }
bool Color::operator!=(const Color& color) const			{ return ((r!=color.r) || (g!=color.g) || (b!=color.b) || (a!=color.a)); }
const Color& Color::operator*=(const Color& color)			{ r*=color.r; g*=color.g; b*=color.b; a*=color.a; return *this; }
const Color& Color::operator+=(const Color& color)			{ r+=color.r; g+=color.g; b+=color.b; a+=color.a; return *this; }
const Color& Color::operator-=(const Color& color)			{ r-=color.r; g-=color.g; b-=color.b; a-=color.a; return *this; }
const Color& Color::operator*=(float value)					{ r*=value; g*=value; b*=value; a*=value; return *this; }
const Color& Color::operator*=(double value)				{ r*=value; g*=value; b*=value; a*=value; return *this; }
const Color& Color::operator/=(float value)					{ float inv=1.0/value; r*=inv; g*=inv; b*=inv; a*=inv; return *this; }
const Color& Color::operator/=(double value)				{ float inv=1.0/value; r*=inv; g*=inv; b*=inv; a*=inv; return *this; }
const Color& Color::operator= (const Color& color)			{ r=color.r; g=color.g; b=color.b; a=color.a; return *this; }
const Color& Color::operator= (float colororValue)			{ r=colororValue; g=colororValue; b=colororValue; a=colororValue; return *this; }

Color operator*(const Color& x, float value)				{ return Color(x.r*value, x.g*value, x.b*value, x.a*value); }
Color operator*(const Color& x, double value)				{ return Color(x.r*value, x.g*value, x.b*value, x.a*value); }
Color operator/(const Color& x, float value)				{ float inv=1.0/value; return Color(x.r*inv, x.g*inv, x.b*inv, x.a*inv); }
Color operator/(const Color& x, double value)				{ float inv=1.0/value; return Color(x.r*inv, x.g*inv, x.b*inv, x.a*inv); }
Color operator*(float value, const Color& y)				{ return Color(value*y.r, value*y.g, value*y.b, value*y.a); }
Color operator*(double value, const Color& y)				{ return Color(value*y.r, value*y.g, value*y.b, value*y.a); }
Color operator*(const Color& x, const Color& y)				{ return Color(x.r*y.r, x.g*y.g, x.b*y.b, x.a*y.a); }
Color operator+(const Color& x, const Color& y)				{ return Color(x.r+y.r, x.g+y.g, x.b+y.b, x.a+y.a); }
Color operator-(const Color& x, const Color& y)				{ return Color(x.r-y.r, x.g-y.g, x.b-y.b, x.b-y.b); }

// color component extraction
uint8 ExtractRed(uint32 color)								{ return ((color>>16) & 0xff); }
uint8 ExtractGreen(uint32 color)							{ return ((color>>8)  & 0xff); }
uint8 ExtractBlue(uint32 color)								{ return (color & 0xff); }
uint8 ExtractAlpha(uint32 color)							{ return (color>>24); }

// 32-bit color construction
uint32 RGBA(uint8 r, uint8 g, uint8 b, uint8 a)				{ return ((uint32)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))); }

} // namespace Core
