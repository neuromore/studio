/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "Slider.h"
#include <QHBoxLayout>


#define SPINBOX_WIDTH 75

using namespace Core;

// the constructor
IntSlider::IntSlider(QWidget* parent, Mode mode) : QWidget(parent)
{
	// create the layout
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->setMargin(0);

	// create the slider
	mSlider = new QSlider(Qt::Horizontal, this);
	mSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
	mSlider->setMinimumWidth(50);

	// create the spinbox
	mSpinBox = new IntSpinBox(this);
	mSpinBox->setMinimumWidth(SPINBOX_WIDTH);
	mSpinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	// create the label
	mLabel = new QLabel(this);

	// connect signals and slots
	connect( mSpinBox,	SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxChanged(double)) );
	connect( mSlider,	SIGNAL(valueChanged(int)), this, SLOT(OnSliderChanged(int)) );
	connect( mSlider,	SIGNAL(sliderReleased()),  this, SLOT(OnSliderReleased()) );

	SetMode(mode);

	hLayout->addWidget( mSlider );
	hLayout->addWidget( mSpinBox );
	hLayout->addWidget( mLabel );
}


// destructor
IntSlider::~IntSlider()
{
}


void IntSlider::SetMode(Mode mode)
{
	mMode = mode;

	if (mode == SPINNER)
	{
		mSpinBox->show();
		mLabel->hide();
	}
	else
	{
		mLabel->show();
		mSpinBox->hide();		
	}
}


// enable or disable the interface items
void IntSlider::SetEnabled(bool isEnabled)
{
	mSpinBox->setEnabled(isEnabled);
	mSlider->setEnabled(isEnabled);
	mLabel->setEnabled(isEnabled);
}


// set the range of the slider
void IntSlider::SetRange(int min, int max)
{
	mSpinBox->setRange(min, max);
	mSlider->setRange(min, max);

	// set step size
	int stepSize = (max-min) / 100;
	if (stepSize == 0)
		stepSize = 1;

	mSlider->setSingleStep(stepSize);
	mSpinBox->setSingleStep(stepSize);
}


// set the value for both the spinbox and the slider
void IntSlider::SetValue(int value)
{
	// set the new slider value
	mSlider->blockSignals(true);
	mSlider->setValue(value);
	mSlider->blockSignals(false);

	// set spinbox value
	if (mMode == SPINNER)
	{
		mSpinBox->blockSignals(true);
		mSpinBox->setValue(value);
		mSpinBox->blockSignals(false);
	}
	
	// set label text
	if (mMode == LABEL)
	{
		mLabel->blockSignals(true);

		QString text = QString::number(value);
		mLabel->setText(text);

		mLabel->blockSignals(false);
	}
}


// gets called when the spinbox value changes
void IntSlider::OnSpinBoxChanged(double value)
{
	SetValue(value);
	emit ValueChanged(value);
	emit ValueChanged();
	emit FinishedValueChange(value);
}


// gets called when the slider got moved
void IntSlider::OnSliderChanged(int value)
{
	SetValue(value);
	emit ValueChanged(value);
	emit ValueChanged();
}


// gets called when the slider was moved and then the mouse button got released
void IntSlider::OnSliderReleased()
{
	emit FinishedValueChange(mSlider->value());
}


// enable or disable signal emitting
void IntSlider::BlockSignals(bool flag)
{
	mSpinBox->blockSignals(flag);
	mSlider->blockSignals(flag);
}


//==============================================================================================================================

// the constructor
FloatSlider::FloatSlider(QWidget* parent, Mode mode) : QWidget(parent)
{
	mMode = mode;

	// create the layout
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->setMargin(0);

	// create the slider
	mSlider = new QSlider(Qt::Horizontal, this);
	mSlider->setMinimumWidth(50);
	mSlider->installEventFilter(new MouseWheelEventFilter());

	// create the spinbox
	mSpinBox = new SpinBox(this);
	mSpinBox->setMinimumWidth(SPINBOX_WIDTH);

	// create the label
	mLabel = new QLabel(this);

	// connect signals and slots
	connect( mSpinBox,	SIGNAL(valueChanged(double)), this, SLOT(OnSpinBoxChanged(double)) );
	connect( mSlider,	SIGNAL(valueChanged(int)), this, SLOT(OnSliderChanged(int)) );
	connect( mSlider,	SIGNAL(sliderReleased()),  this, SLOT(OnSliderReleased()) );

	SetMode(mode);

	hLayout->addWidget( mSlider );
	hLayout->addWidget( mSpinBox );
	hLayout->addWidget( mLabel );
}


// destructor
FloatSlider::~FloatSlider()
{
}


void FloatSlider::SetMode(Mode mode)
{
	mMode = mode;

	if (mode == SPINNER)
	{
		mSpinBox->show();
		mLabel->hide();
	}
	else
	{
		mLabel->show();
		mSpinBox->hide();		
	}
}


// enable or disable the interface items
void FloatSlider::SetEnabled(bool isEnabled)
{
	mSpinBox->setEnabled(isEnabled);
	mSlider->setEnabled(isEnabled);
	mLabel->setEnabled(isEnabled);
}


// set the range of slider
void FloatSlider::SetRange(double min, double max)
{
	mSpinBox->setRange(min, max);

	mSlider->setRange(min * 1000, max*1000);
	// set step size
	double stepSize = (max-min) / 100.0;
	if (stepSize < 0.001)
		stepSize = 0.001;

	mSpinBox->setSingleStep(stepSize);
	mSlider->setSingleStep(stepSize * 1000.0);
}


void FloatSlider::SetLabelValue(double value)
{
	// set label text
	if (mMode == LABEL)
	{
		mLabel->blockSignals(true);

		QString text = QString::number(value);
		mLabel->setText(text);

		mLabel->blockSignals(false);
	}
}


double FloatSlider::Round(double x)
{
	return roundf(x * 1000.0) / 1000.0;
}


// set the value for both the spinbox and the slider
void FloatSlider::SetValue(double value)
{
	value = Round(value);

	if (mMode == SPINNER)
	{
		mSpinBox->blockSignals(true);
		mSpinBox->setValue(value);
		mSpinBox->blockSignals(false);
	}

	mSlider->blockSignals(true);

	// remap range and set the value
	const double result = RemapRange(value, mSpinBox->minimum(), mSpinBox->maximum(), mSlider->minimum(), mSlider->maximum());
	mSlider->setValue((int)result);

	mSlider->blockSignals(false);

	// set label text
	SetLabelValue(value);
}


// gets called when the spinbox value changes
void FloatSlider::OnSpinBoxChanged(double value)
{
	SetValue(value);
	emit ValueChanged(value);
	emit FinishedValueChange(value);
}


// gets called when the slider got moved
void FloatSlider::OnSliderChanged(int value)
{
	const double realValue = RemapRange(value, mSlider->minimum(), mSlider->maximum(), mSpinBox->minimum(), mSpinBox->maximum());
	SetValue(realValue);
	emit ValueChanged(realValue);
}


// gets called when the slider was moved and then the mouse button got released
void FloatSlider::OnSliderReleased()
{
	emit FinishedValueChange(mSpinBox->value());
}


// enable or disable signal emitting
void FloatSlider::BlockSignals(bool flag)
{
	mSpinBox->blockSignals(flag);
	mSlider->blockSignals(flag);
}
