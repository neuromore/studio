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

	// layouts from base class
	QHBoxLayout* primaryLayout = GetPrimaryDeviceInfoLayout();
	QHBoxLayout* secondaryLayout = GetSecondaryDeviceInfoLayout();

	// add signal quality widget
	mSignalQualityWidget = new SignalQualityWidget(this);
	secondaryLayout->addWidget(mSignalQualityWidget);

	// hide/show signal quality widget (if device is not wireless)
	if (mBciDevice->HasWirelessIndicator())
		mSignalQualityWidget->show();
	else
		mSignalQualityWidget->hide();

	// impedance grid widget (container)
	mImpedanceGridWidget = new QWidget(this);

	// impedance grid
	mImpedanceGrid = new QGridLayout(mImpedanceGridWidget);
	mImpedanceGrid->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	mImpedanceGrid->setSizeConstraint(QLayout::SetMinimumSize);
	mImpedanceGrid->setContentsMargins(QMargins(16, 0, 0, 0));

	const QSize lblsize(30, 20);
	const QSize valsize(50, 20);

	QLabel* headlbl = new QLabel();
	headlbl->setFixedSize(lblsize);
	headlbl->setText("CH");
	headlbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	headlbl->setStyleSheet("background-color: black;");
	headlbl->setAlignment(Qt::AlignCenter);

	QLabel* headval = new QLabel();
	headval->setFixedSize(valsize);
	headval->setText("kOhm");
	headval->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	headval->setStyleSheet("background-color: black;");
	headval->setAlignment(Qt::AlignCenter);

	mImpedanceGrid->addWidget(headlbl, 0, 0);
	mImpedanceGrid->addWidget(headval, 0, 1);

	if (mBciDevice)
	{
		const uint32 numSensors = std::min(8U, mBciDevice->GetNumNeuroSensors());
		for (uint32_t i = 0; i < numSensors; i++)
		{
			Sensor* sensor = mBciDevice->GetNeuroSensor(i);

			// name label
			QLabel* lbl = new QLabel();
			lbl->setFixedSize(lblsize);
			lbl->setText(sensor->GetName());
			lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			lbl->setAlignment(Qt::AlignCenter);

			if (Branding::HasColorCodesForChannels)
			{
				switch (i)
				{
				case 0:  lbl->setStyleSheet("color: brown;  background-color: black;"); break;
				case 1:  lbl->setStyleSheet("color: red;    background-color: black;"); break;
				case 2:  lbl->setStyleSheet("color: orange; background-color: black;"); break;
				case 3:  lbl->setStyleSheet("color: yellow; background-color: black;"); break;
				case 4:  lbl->setStyleSheet("color: green;  background-color: black;"); break;
				case 5:  lbl->setStyleSheet("color: blue;   background-color: black;"); break;
				case 6:  lbl->setStyleSheet("color: purple; background-color: black;"); break;
				case 7:  lbl->setStyleSheet("color: grey;   background-color: black;"); break;
				default: lbl->setStyleSheet("color: white;  background-color: black;"); break;
				}
			}
			else
				lbl->setStyleSheet("color: white;  background-color: black;");

			// value label
			QLabel* val = new QLabel();
			val->setFixedSize(valsize);
			val->setText(QString().sprintf("%5.1f", mBciDevice->GetImpedance(i)));
			val->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			val->setAlignment(Qt::AlignCenter);

			mImpedanceGrid->addWidget(lbl, i+1, 0);
			mImpedanceGrid->addWidget(val, i+1, 1);
		}

		for (uint32_t i = 0; i < 3; i++)
		{
			// name label
			QLabel* lbl = new QLabel();
			lbl->setFixedSize(lblsize);
			lbl->setText("AVG");
			lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			lbl->setAlignment(Qt::AlignCenter);
			lbl->setStyleSheet("color: white;  background-color: black;");

			// name
			switch (i)
			{
			case 0: lbl->setText("MIN"); break;
			case 1: lbl->setText("AVG"); break;
			case 2: lbl->setText("MAX"); break;
			default: break;
			}

			// value label
			QLabel* val = new QLabel();
			val->setFixedSize(valsize);
			val->setText(QString().sprintf("%5.1f", 0.0));
			val->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			val->setAlignment(Qt::AlignCenter);
			val->setStyleSheet("background-color: black;");

			mImpedanceGrid->addWidget(lbl, i+numSensors+1, 0);
			mImpedanceGrid->addWidget(val, i+numSensors+1, 1);
		}
	}

	primaryLayout->addWidget(mImpedanceGridWidget);

	// electrode widget
	mEEGElectrodeWidget = new EEGElectrodesWidget(this);

	// set minimuim size and expanding
	mEEGElectrodeWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	mEEGElectrodeWidget->setFixedSize(256,256);

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

	// update impedance grid
	if (mBciDevice && mBciDevice->IsTestRunning() && mImpedanceGridWidget && mImpedanceGrid && mImpedanceGrid->rowCount() > 0)
	{
		Classifier* classifier = GetEngine()->GetActiveClassifier();
		mImpedanceGridWidget->setVisible(mBciDevice->HasEegContactQualityIndicator());
		const uint32 numSensors = std::min(
			(uint32)mImpedanceGrid->rowCount()-1U, 
			mBciDevice->GetNumNeuroSensors());
		const uint32 numUsedSensors = classifier ? classifier->GetNumUsedSensors() : 0U;
		double max = DBL_MIN;
		double avg = 0.0;
		double min = DBL_MAX;
		uint32 cnt = 0;
		for (uint32_t i = 0; i < numSensors; i++)
		{
			Sensor* sensor = mBciDevice->GetNeuroSensor(i);
			double impedance = mBciDevice->GetImpedance(i);
			Color color = sensor->GetContactQualityColor();
			const bool isused = classifier && classifier->IsSensorUsed(sensor);

			// value label in grid
			QLabel* item = (QLabel*)mImpedanceGrid->itemAtPosition(i + 1, 1)->widget();

			if (impedance >= 0.1)
				item->setText(QString().sprintf("%5.1f", impedance));
			else
				item->setText("");

			if (numUsedSensors == 0 || isused)
			{
				if (impedance > 0.1)
				{
					max = impedance > max ? impedance : max;
					min = impedance < min ? impedance : min;
					avg += impedance;
					cnt++;
				}
			}
			else
				color *= 0.4; // same factor as in EEGElectrodesWidget.cpp

			item->setStyleSheet(QString("color: black; background-color: %1;").arg(
				color.ToHexString().AsChar()));
		}

		if (cnt) avg /= cnt;

		QLabel* lblmin = (QLabel*)mImpedanceGrid->itemAtPosition(numSensors + 1, 1)->widget();
		if (min >= 0.1 && min < DBL_MAX) lblmin->setText(QString().sprintf("%5.1f", min));
		else lblmin->setText("");

		QLabel* lblavg = (QLabel*)mImpedanceGrid->itemAtPosition(numSensors + 2, 1)->widget();
		if (avg >= 0.1) lblavg->setText(QString().sprintf("%5.1f", avg));
		else lblavg->setText("");

		QLabel* lblmax = (QLabel*)mImpedanceGrid->itemAtPosition(numSensors + 3, 1)->widget();
		if (max >= 0.1) lblmax->setText(QString().sprintf("%5.1f", max));
		else lblmax->setText("");
	}
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
