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
#include "ImpedanceTestWidget.h"

using namespace Core;

// impedance quality profiles
ImpedanceTestWidget::Threshold ImpedanceTestWidget::Thresholds[] = {
   ImpedanceTestWidget::Threshold("Research",        5.0,  10.0,  20.0,  80.0), 
   ImpedanceTestWidget::Threshold("Neurofeedback",  10.0,  20.0,  40.0,  80.0), 
   ImpedanceTestWidget::Threshold("Testing",       100.0, 200.0, 300.0, 400.0)
};

// constructor
ImpedanceTestWidget::ImpedanceTestWidget(BciDevice* device, QWidget* parent) : QWidget(parent), 
	mImpedanceThreshold(&ImpedanceTestWidget::Thresholds[DEFAULTPROFILE]),
	mPixmapPass(GetQtBaseManager()->FindIcon("Images/Icons/SuccessCircled.png").pixmap(20, 20)),
	mPixmapFail(GetQtBaseManager()->FindIcon("Images/Icons/FailCircled.png").pixmap(20, 20))
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
	vMainLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Main Grid Layout
	// add the main widget w/ grid layout
	QGridLayout* mainLayout = new QGridLayout();
	mainLayout->setMargin(2);
	mainWidget->setLayout(mainLayout);
	
	// Row 1
	QLabel* label = new QLabel("<b>Impedance Test</b>");
	mainLayout->addWidget(label, 0, 0, 1, 2);

	// Row 2
	label = new QLabel("Threshold:");
	mainLayout->addWidget(label, 1, 0, 1, 1);

	mThresholdComboBox = new QComboBox();
	mThresholdComboBox->setEditable(false);
	mThresholdComboBox->setMaxCount(NUMPROFILES);
	for (size_t i = 0; i < NUMPROFILES; i++)
		mThresholdComboBox->addItem(Thresholds[i].name);

	connect(mThresholdComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnThresholdSelected(int)));
	connect(mThresholdComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(OnThresholdEdited(const QString&)));
	mThresholdComboBox->setFixedWidth(100);
	mainLayout->addWidget(mThresholdComboBox, 1, 1, 1, 1);
	// select default threshold
	mThresholdComboBox->setCurrentIndex(DEFAULTPROFILE);
	OnThresholdSelected(DEFAULTPROFILE);

	// Row 3
	QLabel* passedlbl = new QLabel("Passed:");
	mainLayout->addWidget(passedlbl, 2, 0, 1, 1);

	// Pass/Fail icon
	mTestLabel = new QLabel();
	mTestLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//mTestLabel->setAlignment(Qt::AlignCenter);
	//mTestLabel->setStyleSheet("color: white;  background-color: black;");
	mainLayout->addWidget(mTestLabel, 2, 1, 1, 1);
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
	constexpr double electrodeActiveDetectionThreshold = 400; // 200 kiloohms
	
	// note: impedance values are transported via the EEG sensors
	double minImpedance = DBL_MAX;
	double maxImpedance = -DBL_MAX;
	double avgImpedance = 0.0;
	uint32 avgCount = 0;

	// 0.0 (or close to it) is unconnected/no impedance
	constexpr double minthreshold = 0.5;

	bool testPassed = true;

	const uint32 numSensors = mDevice->GetNumNeuroSensors();
	for (uint32 i = 0; i < numSensors; ++i)
	{
		const double impedance = mDevice->GetImpedance(i);

		if (impedance > electrodeActiveDetectionThreshold || impedance < minthreshold)
			continue;

		// skip 
		if (activeClassifier && !activeClassifier->IsSensorUsed(mDevice->GetNeuroSensor(i)))
			continue;

		
		minImpedance = Min(minImpedance, impedance);
		maxImpedance = Max(maxImpedance, impedance);
		avgImpedance += impedance;
		avgCount++;
	}


	// no active electrode: test failed
	if (avgCount == 0)
	{
		testPassed = false;

		// set sensor contact quality to no-signal
		for (uint32 i = 0; i < numSensors; ++i)
			mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
	}
	else
	{
		avgImpedance /= (double)avgCount;

		// test passed?
		testPassed = minImpedance >= minthreshold && maxImpedance >= minthreshold && maxImpedance <= mImpedanceThreshold->yellow;

		// set sensor contact quality (reuse electrode widget) to indicate pass/fail of individual electrodes
		for (uint32 i = 0; i < numSensors; ++i)
		{
			const double impedance = mDevice->GetImpedance(i);
			
			if (impedance < minthreshold) // == 0.0 case
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);

			else if (impedance <= mImpedanceThreshold->green)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_GOOD);

 			else if (impedance <= mImpedanceThreshold->yellow)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_FAIR);

			else if (impedance <= mImpedanceThreshold->orange)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_POOR);

			else if (impedance <= mImpedanceThreshold->red)
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_VERY_BAD);

			else
				mDevice->GetNeuroSensor(i)->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
		}
	}

	// update fail/passed icon
	mTestLabel->setPixmap(testPassed ? mPixmapPass : mPixmapFail);
}


// threshold combobox selection changed
void ImpedanceTestWidget::OnThresholdSelected(int index)
{
	if (index >= NUMPROFILES)
		return;

	mImpedanceThreshold = &Thresholds[index];

	String s;
	s.Format("%s THRESHOLDS\n GREEN\t\t< %i kOhm\n YELLOW\t< %i kOhm\n ORANGE\t< %i kOhm\n RED\t\t< %i kOhm",
		mImpedanceThreshold->name,
		(int)mImpedanceThreshold->green,
		(int)mImpedanceThreshold->yellow,
		(int)mImpedanceThreshold->orange,
		(int)mImpedanceThreshold->red);

	mThresholdComboBox->setToolTip(s.AsChar());
}


// text was edited
void ImpedanceTestWidget::OnThresholdEdited(const QString& text)
{
	SetThresholdFromText(text);
}


void ImpedanceTestWidget::SetThresholdFromText(const QString& text)
{
	for (size_t i = 0; i < NUMPROFILES; i++) 
	{
		if (text == Thresholds[i].name)
		{
			mImpedanceThreshold = &Thresholds[i];
			return;
		}
	}
}
