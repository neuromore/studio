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

// include required headers
#include "ImpedanceTestWidget.h"
//#include "../../AppManager.h"
//#include <Device.h>
#include <BciDevice.h>
#include <QtBaseManager.h>
#include <Core/LogManager.h>
#include <Core/Math.h>
#include <QHBoxLayout>
#include <QSpacerItem>


using namespace Core;

// constructor
ImpedanceTestWidget::ImpedanceTestWidget(BciDevice* device, QWidget* parent) : QWidget(parent)
{
	mDevice = device;

	// expand width but not height
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);


	// add the main widget
	QWidget* mainWidget = new QWidget();
	mainWidget->setObjectName("TransparentWidget");
	QVBoxLayout* vMainLayout = new QVBoxLayout();
	vMainLayout->setMargin(4);
	vMainLayout->addWidget(mainWidget);
	setLayout(vMainLayout);
	vMainLayout->setAlignment(Qt::AlignLeft);

  // Main Grid Layout
	// add the main widget w/ grid layout
	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setMargin(2);
	mainWidget->setLayout(mainLayout);
	
  // Column 1
	QLabel* label = new QLabel("<b>Impedance Test</b>");
	mainLayout->addWidget(label, 0, 0, 1, 2);

	label = new QLabel("Threshold:");
	mainLayout->addWidget(label, 1, 0, 2, 1);

	mThresholdComboBox = new QComboBox();
	const uint32 numPresets = 4;
	mThresholdComboBox->setEditable(true);
	mThresholdComboBox->setMaxCount(numPresets);
	mThresholdComboBox->addItem("5 kOhm");
	mThresholdComboBox->addItem("10 kOhm");
	mThresholdComboBox->addItem("20 kOhm");
	mThresholdComboBox->addItem("40 kOhm");
	connect(mThresholdComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnThresholdSelected(int)));
	connect(mThresholdComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(OnThresholdEdited(const QString&)));
	mThresholdComboBox->setFixedWidth(75);
	mainLayout->addWidget(mThresholdComboBox, 1, 1, 2, 1);
	// select default threshold
	mThresholdComboBox->setCurrentIndex(3);
	OnThresholdSelected(3);

  // Column 2
	// expanding spacer widget 
	QWidget* spacerWidget = new QWidget();
	spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	mainLayout->addWidget(spacerWidget, 0, 2, 3, 1);

  // Column 3
	// min/max/average
	label = new QLabel("Min:");				mainLayout->addWidget(label, 0, 3);
	label = new QLabel("Max:");				mainLayout->addWidget(label, 1, 3);
	label = new QLabel("Avg:");				mainLayout->addWidget(label, 2, 3);

	mMinImpedanceLabel = new QLabel("-");	mainLayout->addWidget(mMinImpedanceLabel, 0, 4);
	mMaxImpedanceLabel = new QLabel("-");	mainLayout->addWidget(mMaxImpedanceLabel, 1, 4);
	mAvgImpedanceLabel = new QLabel("-");	mainLayout->addWidget(mAvgImpedanceLabel, 2, 4);

  // Column 4
	// fixed size spacer widget 
	spacerWidget = new QWidget();
	spacerWidget->setFixedWidth(15);
	mainLayout->addWidget(spacerWidget, 0, 5, 3, 1);

  // Column 5
	// Pass/Fail icon
	mPassIconLabel = new QLabel();
	mFailIconLabel = new QLabel();
	const int pixmapSize = 50;
	mPassIconLabel->setPixmap(GetQtBaseManager()->FindIcon("Images/Icons/SuccessCircled.png").pixmap(pixmapSize, pixmapSize));
	mFailIconLabel->setPixmap(GetQtBaseManager()->FindIcon("Images/Icons/FailCircled.png").pixmap(pixmapSize, pixmapSize));
	mPassIconLabel->setVisible(false);
	mFailIconLabel->setVisible(false);
	mPassIconLabel->setToolTip("Passed");
	mFailIconLabel->setVisible("Failed");

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mPassIconLabel);
	hLayout->addWidget(mFailIconLabel);
	mainLayout->addLayout(hLayout, 0, 6, 3, 1);
}


// destructor
ImpedanceTestWidget::~ImpedanceTestWidget()
{
}


void ImpedanceTestWidget::UpdateInterface()
{
	Classifier* activeClassifier = GetEngine()->GetActiveClassifier();

	// calc min/max/avg impedance accross sensors
	
	// max impedance, if the values is larger we assume the lead is unconnected and ignore the electrode
	const double electrodeActiveDetectionThreshold = 200; // 200 kiloohms
	
	// note: impedance values are transported via the EEG sensors
	double minImpedance = DBL_MAX;
	double maxImpedance = -DBL_MAX;
	double avgImpedance = 0.0;
	uint32 avgCount = 0;

	bool testPassed = true;

	const uint32 numSensors = mDevice->GetNumNeuroSensors();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		const double impedance = mDevice->GetImpedance(i);

		// skip invalid values (Eccept exactly 0.0), dont skip used electrodes
		if (activeClassifier == NULL || (activeClassifier != NULL && activeClassifier->IsSensorUsed(mDevice->GetSensor(i)) == false))
			if (impedance > electrodeActiveDetectionThreshold || impedance < 0.0)
				continue;
		
		minImpedance = Min(minImpedance, impedance);
		maxImpedance = Max(maxImpedance, impedance);
		avgImpedance += impedance;
		avgCount++;
	}


	// no active electrode: test failed
	if (avgCount == 0)
	{
		mMinImpedanceLabel->setText("-");
		mMaxImpedanceLabel->setText("-");
		mAvgImpedanceLabel->setText("-");

		testPassed = false;

		// set sensor contact quality to no-signal
		for (uint32 i = 0; i < numSensors; ++i)
			mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
	}
	else
	{
		avgImpedance /= (double)avgCount;

		// update min/max/avg labels
		mTempString.Format("%.2f k", minImpedance);
		mMinImpedanceLabel->setText(mTempString.AsChar());
		mTempString.Format("%.2f k", maxImpedance);
		mMaxImpedanceLabel->setText(mTempString.AsChar());
		mTempString.Format("%.2f k", avgImpedance);
		mAvgImpedanceLabel->setText(mTempString.AsChar());

		// test passed?
		testPassed = (maxImpedance <= mImpedanceThreshold);

		// set sensor contact quality (reuse electrode widget) to indicate pass/fail of individual electrodes
		for (uint32 i = 0; i < numSensors; ++i)
		{
			const double impedance = mDevice->GetImpedance(i);
			if (impedance > electrodeActiveDetectionThreshold)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
			else if (impedance <= mImpedanceThreshold)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_GOOD);
			else
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_VERY_BAD);
		}
	}

	// update fail/passed icon
	mPassIconLabel->setVisible(testPassed);
	mFailIconLabel->setVisible(!testPassed);
}


// threshold combobox selection changed
void ImpedanceTestWidget::OnThresholdSelected(int index)
{
	SetThresholdFromText(mThresholdComboBox->itemText(index));
}


// text was edited
void ImpedanceTestWidget::OnThresholdEdited(const QString& text)
{
	SetThresholdFromText(text);
}


void ImpedanceTestWidget::SetThresholdFromText(const QString& text)
{
	mImpedanceThreshold = text.split(' ')[0].toDouble();
}
