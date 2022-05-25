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
#include "BciDeviceWidget.h"

using namespace Core;

#if defined(NEUROMORE_BRANDING_ANT) && defined(PRODUCTION_BUILD)
#define NEUROMORE_MINIMAL_DEVICE_STATS true
#else
#define NEUROMORE_MINIMAL_DEVICE_STATS false
#endif

// constructor
BciDeviceWidget::BciDeviceWidget(BciDevice* device, QWidget* parent) : DeviceWidget(device, parent)
{
	mBciDevice = device;
	mSignalQualityWidget	= NULL;
	mEEGElectrodeWidget		= NULL;
}


// destructor
BciDeviceWidget::~BciDeviceWidget()
{
}


// init after the parent dock window has been created
void BciDeviceWidget::Init()
{
	// init base
	DeviceWidget::Init();

	// add signal quality widget
	mSignalQualityWidget = new SignalQualityWidget(this);
	GetSecondaryDeviceInfoLayout()->addWidget(mSignalQualityWidget);

	// hide/show signal quality widget (if device is not wireless)
	if (mBciDevice->HasWirelessIndicator())
		mSignalQualityWidget->show();
	else
		mSignalQualityWidget->hide();

	// electrode widget
	mEEGElectrodeWidget = new EEGElectrodesWidget(this);

	// set minimuim size and expanding
	mEEGElectrodeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	mEEGElectrodeWidget->setFixedSize(180,180);

	// add it to primary layout
	QVBoxLayout* primaryLayout = GetPrimaryDeviceInfoLayout();
	primaryLayout ->addWidget(mEEGElectrodeWidget);

	mBciDevice = static_cast<BciDevice*>(mDevice);
	mEEGElectrodeWidget->SetDevice(mBciDevice);
}


void BciDeviceWidget::UpdateInterface()
{
	// call update of base class
	DeviceWidget::UpdateInterface();

	// update signal quality
	double signalQuality = mBciDevice->GetWirelessSignalQuality();
	mSignalQualityWidget->SetSignalQuality(signalQuality);

	// update eeg widget
	mEEGElectrodeWidget->update();

	// update impedance test widget
	if (mDeviceTestWidget != NULL && mDeviceTestWidget->isVisible())
		static_cast<ImpedanceTestWidget*>(mDeviceTestWidget)->UpdateInterface();
}


void BciDeviceWidget::AddSensorItems(QTreeWidgetItem* parent)
{
	// fill sensor list
	const uint32 numSensors = mBciDevice->GetNumSensors();
	const uint32 numNeuroSensors = mBciDevice->GetNumNeuroSensors();

	// only list stats once for neurosensors (element no. numNeuroSensors-1)
	for (uint32 i = numNeuroSensors-1; i<numSensors; ++i)
	{
		// add sensor item
		QTreeWidgetItem* sensorItem = new QTreeWidgetItem(parent);

		// find sensor
		Sensor* sensor = NULL;
		if (i == numNeuroSensors - 1)	// one index before all other non-eeg sensors
		{
			if (numNeuroSensors > 0)
			{
				sensor = mBciDevice->GetNeuroSensor(0);
				sensorItem->setText(0, "EEG Sensors");
				sensorItem->setExpanded(true);
			}
		}
		else
		{
			sensor = mBciDevice->GetSensor(i);
			sensorItem->setText(0, sensor->GetName());
			sensorItem->setExpanded(false);
		}
		
		CORE_ASSERT(sensor != NULL);
		
		QTreeWidgetItem* item;

		// add signal quality subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Signal Quality");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);

		// add current value subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Current Value");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);

		// add sample rate subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Sample Rate");

		// add drift correction subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Drift (Corrected)");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);

		// add burst size subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Max/Avg Burst Size");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);

		// add burst size subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Max Latency");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);

		// add sample counter subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Received");

		// add lost sample counter subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Lost");

		// add total memory subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0, "Memory Used");
		item->setHidden(NEUROMORE_MINIMAL_DEVICE_STATS);
	}
}


void BciDeviceWidget::UpdateSensorItems(QTreeWidgetItem* parent)
{
	const uint32 numSensors = mDevice->GetNumSensors();
	const uint32 numNeuroSensors = mBciDevice->GetNumNeuroSensors();

	const uint32 numItems = parent->childCount();
	CORE_ASSERT(numItems == numSensors - numNeuroSensors + 1);

	// note: first element (numNeuroSensors-1) is special (all eeg sensors)
	for (uint32 i = 0; i<numItems; ++i)
	{
		QTreeWidgetItem* sensorItem = parent->child(i);

		// skip update if item is collapsed
		if (sensorItem->isExpanded() == false)
			continue;

		QTreeWidgetItem* item;
		item = sensorItem->child(0);

		// find sensor
		Sensor* sensor = NULL;
		if (i == 0)	// one index before all other non-eeg sensors
		{
			const uint32 numNeuroSensors = mBciDevice->GetNumNeuroSensors();
			Sensor::EContactQuality worstQuality = Sensor::CONTACTQUALITY_GOOD;
			for (uint32 s = 0; s < numNeuroSensors; s++)
				worstQuality = Min<Sensor::EContactQuality>(worstQuality, mBciDevice->GetNeuroSensor(s)->GetContactQuality());
			mTempString.Format("%s", sensor->GetContactQualityDescription(worstQuality));
			item->setText(1, mTempString.AsChar());

			if (numNeuroSensors > 0)
				sensor = mBciDevice->GetNeuroSensor(0);	// use first neurosensor

		}
		else
		{
			sensor = mBciDevice->GetSensor(i - 1 + numNeuroSensors);

			// signal quality subitem 
			item = sensorItem->child(0);
			item->setHidden(sensor->HasContactQuality() == false);
			if (sensor->HasContactQuality() == true)
			{
				mTempString.Format("%s", sensor->GetContactQualityAsString());
				item->setText(1, mTempString.AsChar());
			}
		}
	
		CORE_ASSERT(sensor != NULL);

		Channel<double>* channel = sensor->GetChannel();

		// current value subitem
		item = sensorItem->child(1);
		if (sensor->GetChannel()->GetNumSamples() > 0)
		{
			const double currentValue = sensor->GetChannel()->GetLastSample();
			mTempString.Format("%.6f", currentValue);
			item->setText(1, mTempString.AsChar());
		}
		else
		{
			item->setText(1, "-");
		}

		// sample rate subitem
		item = sensorItem->child(2);
		const double sampleRate = channel->GetSampleRate();
		if (sampleRate == 0.0)	// float compare is OK here
			item->setText(1, "variable");
		else
		{
			const double actualSampleRate = sensor->GetRealSampleRate();
			const double diffPercentage = 100.0 * (actualSampleRate - sampleRate) / sampleRate;
			mTempString.Format("%.2fHz (%.2fHz %.2f%%)", actualSampleRate, sampleRate, diffPercentage);
			item->setText(1, mTempString.AsChar());
		}

		// drift correction samples
		item = sensorItem->child(3);
		const int32		driftDistance = sensor->CalculateDrift();
		const uint32	numSamplesAdded = sensor->GetNumDriftSamplesAdded();
		const uint32	numSamplesRemoved = sensor->GetNumDriftSamplesRemoved();
		mTempString.Format("%i smpl (+%i -%i)", -driftDistance, numSamplesAdded, numSamplesRemoved);
		item->setText(1, mTempString.AsChar());

		// max/avg burst size subitem
		item = sensorItem->child(4);
		const uint32 maxBurstSize = sensor->FindMaxBurstSize();
		const double averageBurstSize = sensor->CalculateAverageBurstSize();
		mTempString.Format("%i / %.1f smpl", maxBurstSize, averageBurstSize);
		item->setText(1, mTempString.AsChar());

		// max latency
		item = sensorItem->child(5);
		const double maxLatencyInMilliseconds = mDevice->FindMaxLatency() * 1000.0;
		mTempString.Format("%.3f ms", maxLatencyInMilliseconds);
		item->setText(1, mTempString.AsChar());

		// sample counter subitem
		item = sensorItem->child(6);
		mTempString.Format("%i smpl", channel->GetSampleCounter());
		item->setText(1, mTempString.AsChar());

		// lost samples subitem
		item = sensorItem->child(7);
		mTempString.Format("%i smpl", sensor->GetNumLostSamples());
		item->setText(1, mTempString.AsChar());

		// sample memory used
		item = sensorItem->child(8);
		const double kiloByte = (double)sensor->GetChannel()->CalculateMemoryAllocated() / 1024.0;
		mTempString.Format("%.2f KB", kiloByte);
		item->setText(1, mTempString.AsChar());
	}
}
