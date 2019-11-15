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

#ifndef __CORE_COLOR_H
#define __CORE_COLOR_H

// include the required headers
#include "StandardHeaders.h"
#include "String.h"


namespace Core
{

class ENGINE_API Color
{
	public:
		// constructors & destructor
		Color();
		Color(float value);
		Color(float inR, float inG, float inB, float inA=1.0);
		Color(const Color& color);
		Color(uint32 color);

		// mutators
		void Set(float inR, float inG, float inB, float inA=1.0);
		void Set(const Color& color);
		void Set(uint32 color);
		void Zero();
		Color& Clamp();

		// helpers
		uint32 ToDWORD() const;
		Color& FromHSV(float H, float S, float V);
		Color& FromHexString(const String& hexString);
		String ToHexString() const;

		Color& SetUniqueColor(uint32 index);

		// operators
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;
		const Color& operator*=(const Color& color);
		const Color& operator+=(const Color& color);
		const Color& operator-=(const Color& color);
		const Color& operator*=(float value);
		const Color& operator*=(double value);
		const Color& operator/=(float value);
		const Color& operator/=(double value);
		const Color& operator= (const Color& color);
		const Color& operator= (float colororValue);

	public:
		float r;
		float g;
		float b;
		float a;
};

// operators
ENGINE_API Color operator*(const Color& x, float value);
ENGINE_API Color operator*(const Color& x, double value);
ENGINE_API Color operator/(const Color& x, float value);
ENGINE_API Color operator/(const Color& x, double value);
ENGINE_API Color operator*(float value, const Color& y);
ENGINE_API Color operator*(double value, const Color& y);
ENGINE_API Color operator*(const Color& x, const Color& y);
ENGINE_API Color operator+(const Color& x, const Color& y);
ENGINE_API Color operator-(const Color& x, const Color& y);

// color component extraction
ENGINE_API uint8 ExtractRed(uint32 color);
ENGINE_API uint8 ExtractGreen(uint32 color);
ENGINE_API uint8 ExtractBlue(uint32 color);
ENGINE_API uint8 ExtractAlpha(uint32 color);

// 32-bit color construction
ENGINE_API uint32 RGBA(uint8 r, uint8 g, uint8 b, uint8 a=255);

} // namespace Core


#endif
