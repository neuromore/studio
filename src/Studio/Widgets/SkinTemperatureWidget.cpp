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
#include "SkinTemperatureWidget.h"

using namespace Core;

// constructor
SkinTemperatureWidget::SkinTemperatureWidget(QWidget* parent) : QWidget(parent)
{
	// main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout( mainLayout );

	// add the header label
	QLabel* headerLabel = new QLabel( "Skin Temperature" );
	headerLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mainLayout->addWidget( headerLabel );

	// add the main widget
	QWidget* mainWidget = new QWidget();
	mainLayout->addWidget( mainWidget );


	// 
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setSpacing(0);
	mainWidget->setLayout( hLayout );

	mThermometerWidget = new ThermometerWidget();
	mThermometerWidget->setMinimumWidth( 50 );
	mThermometerWidget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Ignored );
	mThermometerWidget->SetMin( 31.0 );
	mThermometerWidget->SetMax( 39.0 );

	hLayout->addWidget( mThermometerWidget );

	// construct the degree celcius postfix
	QChar degreeSymbol(0x00b0);
	mDegreeCelciusPostfix = " ";
	mDegreeCelciusPostfix.append(degreeSymbol);
	mDegreeCelciusPostfix.append("C");


	mTemperatureLabel = new QLabel();
	mTemperatureLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
	mTemperatureLabel->setStyleSheet("QLabel { color : white; }");
	QFont temperatureFont = mTemperatureLabel->font();
	temperatureFont.setPixelSize(30);
	mTemperatureLabel->setFont(temperatureFont);
	hLayout->addWidget( mTemperatureLabel, 0, Qt::AlignCenter );

	SetTemperature(0.0f);
}


// destructor
SkinTemperatureWidget::~SkinTemperatureWidget()
{
}


// reset the average, min and max values
void SkinTemperatureWidget::Reset()
{}


// set the temperature
void SkinTemperatureWidget::SetTemperature(float temperature)
{
	// set the te
	mTempString.Format( "%.1f", temperature );
	mTemperatureString = mTempString.AsChar();

	if (temperature < 0.0f)
		mTemperatureString = "--.-";

	mTemperatureString += mDegreeCelciusPostfix;
	mTemperatureLabel->setText( mTemperatureString );

	// set the temperature for the thermometer
	mThermometerWidget->SetValue( temperature );
}
