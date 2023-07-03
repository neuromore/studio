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
	setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum);


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
   constexpr double minthreshold = 0.1;

   Classifier*  classifier = GetEngine()->GetActiveClassifier();
   const uint32 numSensors = mDevice->GetNumNeuroSensors();
   const uint32 numUsedSensors = classifier ? classifier->GetNumUsedSensors() : 0U;

   bool testok = true;

   for (uint32 i = 0; i < numSensors; ++i)
   {
      Sensor* sensor = mDevice->GetNeuroSensor(i);
      double  impedance = mDevice->GetImpedance(i);
      bool    isused = classifier && classifier->IsSensorUsed(sensor);

      // update contact quality on sensor based on selected thresholds
      if      (impedance <  minthreshold)                sensor->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);
      else if (impedance <= mImpedanceThreshold->green)  sensor->SetContactQuality(Sensor::CONTACTQUALITY_GOOD);
      else if (impedance <= mImpedanceThreshold->yellow) sensor->SetContactQuality(Sensor::CONTACTQUALITY_FAIR);
      else if (impedance <= mImpedanceThreshold->orange) sensor->SetContactQuality(Sensor::CONTACTQUALITY_POOR);
      else if (impedance <= mImpedanceThreshold->red)    sensor->SetContactQuality(Sensor::CONTACTQUALITY_VERY_BAD);
      else                                               sensor->SetContactQuality(Sensor::CONTACTQUALITY_NO_SIGNAL);

      // test fails if either a used electrode has poor/bad/no signal
      // or (if none is used), any electrode has poor/bad/no signal
      if (isused || numUsedSensors == 0)
      {
         switch (sensor->GetContactQuality())
         {
         case Sensor::CONTACTQUALITY_POOR:
         case Sensor::CONTACTQUALITY_VERY_BAD:
         case Sensor::CONTACTQUALITY_NO_SIGNAL:
            testok = false;
            break;
         default: 
            break;
         }
      }
   }

   // update fail/passed icon
   mTestLabel->setPixmap(testok ? mPixmapPass : mPixmapFail);
}


// threshold combobox selection changed
void ImpedanceTestWidget::OnThresholdSelected(int index)
{
	if (index >= NUMPROFILES)
		return;

	mImpedanceThreshold = &ImpedanceTestWidget::Thresholds[index];

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
