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
#include "DeviceWidget.h"
#include "DevicesPlugin.h"

using namespace Core;

// constructor
DeviceWidget::DeviceWidget(Device* device, QWidget* parent) : QWidget(parent)
{
	CORE_ASSERT(device != NULL);

	mShowInfoWidget = false;

	mDeviceInfoTree = NULL;
	mDeviceInfoSensors = NULL;
	mDeviceTestWidget = NULL;
	
	mDevice = device;

	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
}


// destructor
DeviceWidget::~DeviceWidget()
{
}


// init widgets
void DeviceWidget::Init()
{
	// 1) left half of widget area (upper half of main layout)
	mLayout = new QGridLayout();
	mLayout->setMargin(0);
	//mLayout->setSpacing(0);

	//mLayout->addWidget(QLabel( mDevice->GetName().AsChar() ));

	// top: device icon (if one exists)
	String iconFilename;
	iconFilename.Format(":/Images/Devices/%s.png", mDevice->GetHardwareName());

	// in case the device icon doesn't exist, use the default one
	QFile existsFileTest(iconFilename.AsChar());
	if (existsFileTest.exists() == true)
	{
		mDeviceIcon = new QLabel();
		mDeviceIcon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

		mLayout->addWidget(mDeviceIcon, 0, 0);

		// scale and apply icon
		QPixmap icon(iconFilename.AsChar());
		icon = icon.scaledToWidth(130, Qt::TransformationMode::SmoothTransformation);
		mDeviceIcon->setPixmap(icon);
	}
	else
		mDeviceIcon = NULL;

	// middle: secondary device info widgets (battery, signal etc)
	mSecondaryDeviceInfoLayout = new QVBoxLayout();
	mSecondaryDeviceInfoLayout->setMargin(0);
	mBatteryStatusWidget = new BatteryStatusWidget(this);
	mBatteryStatusWidget->setVisible(mDevice->HasBatteryIndicator());
	mSecondaryDeviceInfoLayout->addWidget(mBatteryStatusWidget);
	mLayout->addLayout(mSecondaryDeviceInfoLayout, 1, 0);

	// bottom: info/test buttons
	QSize buttonSize = QSize(65, 25);
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	mDeviceInfoButton = new QPushButton("Info");
	mDeviceInfoButton->setCheckable(true);
	mDeviceInfoButton->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/Info.png"));
	//mDeviceInfoButton->setIconSize(QSize(25, 25));
	//mDeviceInfoButton->setContentsMargins(20, 2, 2, 2);
	mDeviceInfoButton->setFixedSize(buttonSize);
	connect(mDeviceInfoButton, SIGNAL(clicked()), this, SLOT(OnDeviceInfoButtonPressed()));
	buttonLayout->addWidget(mDeviceInfoButton);
	
	// create test button only if device has a test mode
	if (mDevice->HasTestMode() == true)
	{
		// "Test" Button
		mDeviceTestButton = new QPushButton("Test");
		mDeviceTestButton->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/Gauge.png"));
		//mDeviceTestButton->setIconSize(QSize(20, 20));
		//mDeviceTestButton->setContentsMargins(2, 2, 2, 2);
		mDeviceTestButton->setFixedSize(buttonSize);
		connect(mDeviceTestButton, SIGNAL(clicked()), this, SLOT(OnDeviceTestButtonPressed()));
		buttonLayout->addWidget(mDeviceTestButton);
		InitDeviceTestWidget();
	}
	else
	{
		mDeviceTestButton = NULL;
	}

	// add spacer widget
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
	buttonLayout->addWidget(spacerWidget);

	// add hor. button layout to main layout
	mLayout->addLayout(buttonLayout, 2, 0);

	// 2) right half of widget area (primary device info)
	mPrimaryDeviceInfoLayout = new QVBoxLayout();
	mPrimaryDeviceInfoLayout->setMargin(0);
	mLayout->addLayout(mPrimaryDeviceInfoLayout, 0, 1, 3, 1);

	// 3) device information tree (lower half of main layout)
	mDeviceInfoTree = new QTreeWidget();
	mDeviceInfoTree->setFixedHeight(70);
	mDeviceInfoTree->setSelectionMode( QAbstractItemView::SingleSelection);
	mDeviceInfoTree->setSortingEnabled(false);
	mDeviceInfoTree->setAlternatingRowColors(true);
	mDeviceInfoTree->setAnimated(true);
	
	// column and header settings
	mDeviceInfoTree->setColumnCount(2);
	mDeviceInfoTree->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
	mDeviceInfoTree->header()->setStretchLastSection(true);
	
	QStringList headerLabels;
	headerLabels << "Property" << "Value";
	mDeviceInfoTree->setHeaderLabels(headerLabels);

	// hide tree by default
	mDeviceInfoTree->hide();

	// add tree
	mLayout->addWidget(mDeviceInfoTree, 3, 0, 1, 2);
	
	setLayout(mLayout);
}


// update displayed interface information
void DeviceWidget::UpdateInterface()
{

	// update battery status widget
	if (mDevice->GetPowerSupplyType() == Device::POWERSUPPLY_BATTERY)
	{
		if (mDevice->HasBatteryIndicator() == true)
			mBatteryStatusWidget->SetStatus(mDevice->GetBatteryChargeLevel(), mDevice->GetCriticalBatteryLevel());
		else
			mBatteryStatusWidget->SetStatus(0.0, BatteryStatusWidget::STATUS_UNKNOWN);

		mBatteryStatusWidget->show();
	}
	else
	{
		// dont show a widget
		mBatteryStatusWidget->hide();
	}

	// update device widegets
	UpdateDeviceItems();

	// update sensor list (if device info is shown)
	if (mDeviceInfoTree->isHidden() == false && mDeviceInfoSensors != NULL)
		UpdateSensorItems(mDeviceInfoSensors);

	// disable impedance test while session is running
	if (mDeviceTestButton != NULL)
		mDeviceTestButton->setEnabled(!GetSession()->IsRunning());
}


void DeviceWidget::InitDeviceInfoWidget()
{
	// clear tree
	mDeviceInfoTree->clear();

	// fill device info tree
	AddDeviceItems();

	// sensor subitem
	mDeviceInfoSensors = new QTreeWidgetItem();
	mDeviceInfoSensors->setText(0, "Sensors");
	mDeviceInfoTree->addTopLevelItem(mDeviceInfoSensors);
	mDeviceInfoSensors->setExpanded(true);

	// num sensors
	mTempString.Format("%i", mDevice->GetNumSensors());
	mDeviceInfoSensors->setText(1, mTempString.AsChar());

	// fill sensors list
	AddSensorItems(mDeviceInfoSensors);
}


void DeviceWidget::InitDeviceTestWidget()
{
	if (mDevice->HasTestMode() == false || mDeviceTestWidget != NULL)
		return;

	mDeviceTestWidget = CreateDeviceTestWidget();
	mLayout->addWidget(mDeviceTestWidget, 4, 0, 1, 2);

	if (mDevice->IsTestRunning())
	{
		mDeviceTestWidget->setVisible(true);
		mDeviceTestButton->setText("Close");
	}
	else
	{
		mDeviceTestWidget->setVisible(false);
		mDeviceTestButton->setText("Test");
	}
}


void DeviceWidget::AddDeviceItems()
{
	// Name
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, "Device Name");
	item->setText(1, mDevice->GetName().AsChar());
	mDeviceInfoTree->addTopLevelItem(item);

	// Memory usage
	item = new QTreeWidgetItem();
	item->setText(0, "Memory Used");
	item->setText(1, "0 KB");
	mDeviceInfoTree->addTopLevelItem(item);
}


void DeviceWidget::UpdateDeviceItems()
{
	// Memory usage
	QTreeWidgetItem* item = mDeviceInfoTree->topLevelItem(1);
	mTempString.Format("%.2f KB", mDevice->CalculateSensorMemoryUsage() / 1024.0);
    if (item != NULL)
        item->setText(1,mTempString.AsChar());
}


void DeviceWidget::AddSensorItems(QTreeWidgetItem* parent)
{
	// fill sensor list
	const uint32 numSensors = mDevice->GetNumSensors();
	for (uint32 i=0; i<numSensors; ++i)
	{
		Sensor* sensor = mDevice->GetSensor(i);
		
		// add sensor item
		QTreeWidgetItem* sensorItem = new QTreeWidgetItem(parent);
		sensorItem->setExpanded(false);
		sensorItem->setText(0,sensor->GetName());

		QTreeWidgetItem* item;

		// add signal quality subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Signal Quality");

		// add signal quality subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Current Value");

		// add sample rate subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Sample Rate");

		// add drift correction subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Drift (Corrected)");

		// add burst size subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Max/Avg Burst Size");
		
		// add burst size subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Max Latency");

		// add sample counter subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Received");

		// add lost sample counter subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Lost");

		// add total memory subitem
		item = new QTreeWidgetItem(sensorItem);
		item->setText(0,"Memory Used");
	}
}


void DeviceWidget::UpdateSensorItems(QTreeWidgetItem* parent)
{
	const uint32 numSensors = mDevice->GetNumSensors();

	CORE_ASSERT(numSensors == parent->childCount());
	for (uint32 i=0; i<numSensors; ++i)
	{
		QTreeWidgetItem* sensorItem = parent->child(i);

		// skip update if item is collapsed
		if (sensorItem->isExpanded() == false)
			continue;

		Sensor* sensor = mDevice->GetSensor(i);
		Channel<double>* channel = sensor->GetChannel();

		QTreeWidgetItem* item;

		// signal quality subitem
		item = sensorItem->child(0);
		item->setHidden(sensor->HasContactQuality() == false);
		if (sensor->HasContactQuality() == true)
		{
			mTempString.Format("%s", sensor->GetContactQualityAsString());
			item->setText(1, mTempString.AsChar());
		}

		
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
			item->setText(1,"variable");
		else
		{
			const double actualSampleRate = sensor->GetRealSampleRate();
			const double diffPercentage = 100.0 * (actualSampleRate - sampleRate) / sampleRate;
			mTempString.Format("%.2fHz (%.2fHz %.2f%%)", actualSampleRate, sampleRate, diffPercentage);
			item->setText(1,mTempString.AsChar());
		}

		// drift correction samples
		item = sensorItem->child(3);
		item->setHidden(sensor->GetDriftCorrectionEnabled() == false);
		if (sensor->GetDriftCorrectionEnabled() == true)
		{
			const int32		driftDistance = sensor->CalculateDrift();
			const uint32	numSamplesAdded = sensor->GetNumDriftSamplesAdded();
			const uint32	numSamplesRemoved = sensor->GetNumDriftSamplesRemoved();
			mTempString.Format("%i smpl (+%i -%i)", -driftDistance, numSamplesAdded, numSamplesRemoved);
			item->setText(1, mTempString.AsChar());
		}
		
		// max/avg burst size subitem
		item = sensorItem->child(4);
		const uint32 maxBurstSize = sensor->FindMaxBurstSize();
		const double averageBurstSize = sensor->CalculateAverageBurstSize();
		mTempString.Format("%i / %.1f smpl", maxBurstSize, averageBurstSize);
		item->setText(1,mTempString.AsChar());

		// max latency
		item = sensorItem->child(5);
		const double maxLatencyInMilliseconds = mDevice->FindMaxLatency() * 1000.0;
		mTempString.Format("%.3f ms", maxLatencyInMilliseconds);
		item->setText(1,mTempString.AsChar());

		// sample counter subitem
		item = sensorItem->child(6);
		mTempString.Format("%i smpl", channel->GetSampleCounter());
		item->setText(1,mTempString.AsChar());
		
		// lost samples subitem
		item = sensorItem->child(7);
		mTempString.Format("%i smpl", sensor->GetNumLostSamples());
		item->setText(1,mTempString.AsChar());

		// sample memory used
		item = sensorItem->child(8);
		const double kiloByte = (double)sensor->GetChannel()->CalculateMemoryAllocated() / 1024.0;
		mTempString.Format("%.2f KB", kiloByte);
		item->setText(1,mTempString.AsChar());
	}
}


void DeviceWidget::AddDeviceInfo(const char* name, QWidget* value)
{
	CORE_ASSERT(value != NULL);

	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0,name);
	mDeviceInfoTree->addTopLevelItem(item);
	mDeviceInfoTree->setItemWidget(item, 1, value);
}


void DeviceWidget::OnDeviceInfoButtonPressed()
{
	// tree was not initialized yet
	if (mDeviceInfoTree->topLevelItemCount() == 0)
		InitDeviceInfoWidget();

	// Hack: hide this widget while hiding its children to prevent flicker
	setVisible(false);
	
	// toggle widget visibility
	mShowInfoWidget = !mShowInfoWidget;
	mDeviceInfoTree->setVisible(mShowInfoWidget);

	// update Button text
	if (mShowInfoWidget == true) 
	{
		if (mDevice->IsTestRunning())
			OnDeviceTestButtonPressed();
		mDeviceInfoButton->setText("Close");
	}
	else
		mDeviceInfoButton->setText("Info");
	
	//// hide other widgets
	//if (mShowTestWidget == true)
	//{
	//	mShowTestWidget = false;
	//	mDeviceTestWidget->hide();
	//	mDevice->StopTest();
	//}

	setVisible(true);
}


void DeviceWidget::OnDeviceTestButtonPressed()
{
	// test widget was not initialized yet
	if (mDeviceTestWidget == NULL)
		InitDeviceTestWidget();

	// start test 
	if (!mDevice->IsTestRunning())
		mDevice->StartTest();

	// stop test
	else
	{
		mDevice->StopTest();

		// reset sensor signal quality  that was used to display impedances
		const uint32 numSensors = mDevice->GetNumSensors();
		for (uint32 i = 0; i < numSensors; ++i)	
			mDevice->GetSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NOT_AVAILABLE);
		
	}

	// Hack: hide this widget while hiding its children to prevent flicker
	setVisible(false);
	
	if (mDevice->IsTestRunning())
	{
		if (mShowInfoWidget)
			OnDeviceInfoButtonPressed();

		mDeviceTestWidget->setVisible(true);
		mDeviceTestButton->setText("Stop");
	}
	else
	{
		mDeviceTestWidget->setVisible(false);
		mDeviceTestButton->setText("Test");
	}

	setVisible(true);
}
