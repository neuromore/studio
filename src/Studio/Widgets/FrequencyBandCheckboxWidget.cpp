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
#include <Studio/Precompiled.h>

// include required headers
#include "FrequencyBandCheckboxWidget.h"

using namespace Core;

// constructor
FrequencyBandCheckboxWidget::FrequencyBandCheckboxWidget(QWidget* parent) : HMultiCheckboxWidget(parent)
{
	ReInit( GetEngine()->GetSpectrumAnalyzerSettings() );
}


// destructor
FrequencyBandCheckboxWidget::~FrequencyBandCheckboxWidget()
{
}


// reinitialize the horizontal checkbox widget
void FrequencyBandCheckboxWidget::ReInit(SpectrumAnalyzerSettings* settings)
{
	// get the number of available frequency bands
	const uint32 numBands = settings->GetNumFrequencyBands();

	// prepare the arrays for the reinitialization
	Array<String> names;
	Array<String> tooltips;
	Array<Color> colors;
	names.Resize( numBands );
	tooltips.Resize( numBands );
	colors.Resize( numBands );

	// iterate through the available frequency bands
	for (uint32 i=0; i<numBands; ++i)
	{
		FrequencyBand* band = settings->GetFrequencyBand(i);

		// retrieve frequency band information
		names[i]	= band->GetName();
		tooltips[i]	= band->GetDescription();
		colors[i]	= band->GetColor();
	}

	// reinit the multi checkbox widget
	HMultiCheckboxWidget::ReInit( names, tooltips, colors, "All Bands" );
}
