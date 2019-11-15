/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_COLORPALETTE_H
#define __NEUROMORE_COLORPALETTE_H

// include required headers
#include "QtBaseConfig.h"
#include <QColor>
#include <Core/Color.h>

namespace ColorPalette
{

struct Shared
{
	// text
	static Core::Color GetTextColor()				{ return GetCiColorWhiteDark(); }
	static QColor GetTextQColor()					{ return QColor(218,218,218); }

	// dark text
	static Core::Color GetDarkTextColor()			{ return GetCiColorGreyBrighter(); }
	static QColor GetDarkTextQColor()				{ return QColor(112,111,111); }

	// dark background
	static Core::Color GetDarkBackgroundColor()		{ return GetCiColorGreyDark(); }
	static QColor GetDarkBackgroundQColor()			{ return QColor(40,40,39); }

	// background
	static Core::Color GetBackgroundColor()			{ return GetCiColorGrey(); }
	static QColor GetBackgroundQColor()				{ return QColor(60,60,59); }

	// dark grid
	static Core::Color GetDarkGridColor()			{ return GetCiColorGrey(); }
	static QColor GetDarkGridQColor()				{ return QColor(75,75,74); }

	// dark sub grid
	static Core::Color GetDarkSubGridColor()		{ return GetDarkGridColor(); }
	static QColor GetDarkSubGridQColor()			{ return GetDarkGridQColor(); }

	// grid
	static Core::Color GetGridColor()				{ return GetCiColorGreyBright(); }
	static QColor GetGridQColor()					{ return QColor(87,87,86); }

	// sub grid
	static Core::Color GetSubGridColor()			{ return GetGridColor(); }
	static QColor GetSubGridQColor()				{ return GetGridQColor(); }

	// axis (on grids)
	static Core::Color GetAxisColor()				{ return GetCiColorGreyBrighter(); }
	static QColor GetAxisQColor()					{ return QColor(112,111,111); }

	// main CI colors
	static Core::Color GetCiColorWhite()			{ return Core::Color(0xFFFFFFFFu); } // RGB 255,255,255
	static Core::Color GetCiColorWhiteDark()		{ return Core::Color(0xFFDADADAu); } // RGB 218,218,218

	static Core::Color GetCiColorGrey()				{ return Core::Color(0xFF3C3C3Bu); } // RGB 60,60,59
	static Core::Color GetCiColorGreyBright()		{ return Core::Color(0xFF575756u); } // RGB 87,87,86
	static Core::Color GetCiColorGreyBrighter()		{ return Core::Color(0xFF706F6Fu); } // RGB 112,111,111
	static Core::Color GetCiColorGreyDark()			{ return Core::Color(0xFF282827u); } // RGB 40,40,39

	static Core::Color GetCiColorBlue()				{ return Core::Color(0xFF009EE3u); }

	static Core::Color GetCiColorGreen()			{ return Core::Color(0xFFBFEA21u); }

	static Core::Color GetCiColorBrown()			{ return Core::Color(0xFF462D16u); }

	static Core::Color GetCiColorRed()				{ return Core::Color(0xFFED002Eu); }

	static Core::Color GetCiColorOrange()			{ return Core::Color(0xFFEB7205u); }

	static Core::Color GetCiColorYellow()			{ return Core::Color(0xFFFFAB07u); }

};

}

#endif
