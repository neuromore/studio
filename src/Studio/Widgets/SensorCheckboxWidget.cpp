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
#include "SensorCheckboxWidget.h"
#include <Core/LogManager.h>


using namespace Core;

// constructor
SensorCheckboxWidget::SensorCheckboxWidget(QWidget* parent) : HMultiCheckboxWidget(parent)
{
	mBciDevice = NULL;
	CORE_EVENTMANAGER.AddEventHandler(this);
}


// destructor
SensorCheckboxWidget::~SensorCheckboxWidget()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


// check the corresponding checkbox for visibility information
bool SensorCheckboxWidget::IsVisible(EEGElectrodes::Electrode electrode) const
{
	// get the number of checkboxes and iterate through them
	const uint32 numCheckboxes = GetNumCheckboxes();
	for (uint32 i=0; i<numCheckboxes; ++i)
	{
		// check if the given id is the same as from the currently iterated sensor, if yes return the state of its checkbox
		if (mAvailableSensors[i].GetNameString().IsEqual(electrode.GetName()) == true)
			return IsChecked(i);
	}

	// the sensor has not been found
	return false;
}


// initialize for a new neuro headset
void SensorCheckboxWidget::ReInit(BciDevice* device)
{
	// get the available sensors for the given neuro headset
	mBciDevice							= device;
	mAvailableSensors					= mBciDevice->GetElectrodes();
	const uint32 numAvailableSensors	= mAvailableSensors.Size();

	// prepare the arrays for the reinitialization
	Array<String> names;
	Array<String> tooltips;
	Array<Color> colors;
	names.Resize( numAvailableSensors );
	tooltips.Resize( numAvailableSensors );
	colors.Resize( numAvailableSensors );

	// iterate through the available sensors
	for (uint32 i=0; i<numAvailableSensors; ++i)
	{
		EEGElectrodes::Electrode electrode = mAvailableSensors[i];

		// retrieve sensor information
		names[i] = electrode.GetName();
		colors[i].SetUniqueColor(i);
	}

	// reinit the multi checkbox widget
	HMultiCheckboxWidget::ReInit( names, tooltips, colors, "All Sensors" );
}
