/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "Spinbox.h"
#include <Core/LogManager.h>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QKeyEvent>

#include "QtBaseManager.h"


using namespace Core;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// String validator
//////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
DoubleValidator::DoubleValidator(QObject* parent) : QValidator(parent)
{
	mValidator = new QDoubleValidator(parent);
	mValidator->setNotation( QDoubleValidator::StandardNotation );
}


// destructor
DoubleValidator::~DoubleValidator()
{
	delete mValidator;
}


// string validation function
QValidator::State DoubleValidator::validate(QString& input, int& pos) const
{
	input.replace(",", ".");
	return mValidator->validate(input, pos);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Spin button
//////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
SpinboxButton::SpinboxButton(QWidget* parent, SpinBox* spinbox, bool leftButton) : QToolButton(parent)
{
	mSpinbox	= spinbox;
	mLeftButton	= leftButton;
	
	if (mLeftButton == false)
	{
		setArrowType(Qt::RightArrow);
		setObjectName("SpinBoxRight");
	}
	else
	{
		setArrowType(Qt::LeftArrow);
		setObjectName("SpinBoxLeft");
	}

	// autorepeat
	setAutoRepeat(true);

	connect(this, SIGNAL(clicked()), this, SLOT(ClickButton()));
}


// destructor
SpinboxButton::~SpinboxButton()
{
}


// called when the spin up button has been clicked
void SpinboxButton::OnUpButtonClicked()
{
	// add a single step to the value
	double newValue = mSpinbox->value() + mSpinbox->singleStep();

	// check if the new value is out of range
	if (newValue > mSpinbox->maximum())
		newValue = mSpinbox->maximum();

	// fire value changed events
	mSpinbox->setValue(newValue);
	mSpinbox->EmitValueChangedSignal();
}


// called when the spin down button has been clicked
void SpinboxButton::OnDownButtonClicked()
{
	// remove a single step from the value
	double newValue = mSpinbox->value() - mSpinbox->singleStep();

	// check if the new value is out of range, don't emit any changes in this case
	if (newValue < mSpinbox->minimum())
		newValue = mSpinbox->minimum();

	// fire value changed events
	mSpinbox->setValue(newValue);
	mSpinbox->EmitValueChangedSignal();
}


// click the button
void SpinboxButton::ClickButton()
{
	if (mLeftButton == true)
		OnDownButtonClicked();
	else
		OnUpButtonClicked();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Specialized spinbox edit field
//////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
SpinboxLineEdit::SpinboxLineEdit(QWidget* parent, SpinBox* spinbox) : QLineEdit(parent)
{
	mSpinbox = spinbox;

	// center text
	setAlignment(Qt::AlignCenter);

	setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );
}


// destructor
SpinboxLineEdit::~SpinboxLineEdit()
{
}


// called when the line edit lost focus
void SpinboxLineEdit::focusOutEvent(QFocusEvent* event)
{
	FromQtString( text(), &mTemp );
	mTemp.Trim();
	mTemp.Replace( StringCharacter(','), StringCharacter('.') );

	if (mTemp.IsEmpty() == true)
		mSpinbox->Update();

	QLineEdit::focusOutEvent(event);
}


// called when the edit field got double clicked
void SpinboxLineEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
	QLineEdit::mouseDoubleClickEvent(event);
	//setCursorPosition(0); // doesn't work after selectAll(), as it resets the selection again
	selectAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Finally the spinbox itself
//////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
SpinBox::SpinBox(QWidget* parent, Type type) : QWidget(parent)
{
	mType						= type;
	mValue						= 0.0;

	// create the edit field
	mLineEdit = new SpinboxLineEdit(this, this);
	mLineEdit->setObjectName("SpinBoxEdit");

	setFocusProxy(mLineEdit);

	switch (type)
	{
		case DOUBLE:
		{
			mSingleStep					= 1.0;
			mMinimum					= 0.0;
			mMaximum					= 99.99;
			mNumDecimals				= 4;

			mLineEdit->setValidator( new DoubleValidator(mLineEdit) );

			break;
		}

		case INTEGER:
		{
			mSingleStep					= 1.0;
			mMinimum					= 0.0;
			mMaximum					= 100.0;
			mNumDecimals				= 0;

			mLineEdit->setValidator( new QIntValidator(mLineEdit) );

			break;
		}
	}

	connect(mLineEdit, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(mLineEdit, SIGNAL(returnPressed()), this, SLOT(OnEditingFinished()));
	connect(mLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnTextEdited(const QString&)));


	// create the up and down spin buttons
	mUpButton	= new SpinboxButton(this, this, false);
	mDownButton	= new SpinboxButton(this, this, true);

	// the main horizontal layout
	QHBoxLayout* layout = new QHBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(mDownButton, Qt::AlignLeft);
	layout->addWidget(mLineEdit, Qt::AlignLeft);
	layout->addWidget(mUpButton, Qt::AlignLeft);

	setLayout(layout);
	
	setFocusPolicy(Qt::StrongFocus);

	// FIXME this is the only thing that keeps the widget from shrinking too much? (e.g. in presession widget, if a client is connected)
	setMinimumHeight(21);

	// get the visuals up to date
	Update();
}


void SpinBox::resizeEvent(QResizeEvent* event)
{
	int height = mUpButton->height();
	mLineEdit->setMinimumHeight(height);
	mLineEdit->setMaximumHeight(height);
}


// destructor
SpinBox::~SpinBox()
{
}


// update the interface
void SpinBox::Update()
{
	switch (mType)
	{
		case DOUBLE:
		{
			mText.FromDouble(mValue);

			break;
		}

		case INTEGER:
		{
			mText.FromInt((int)mValue);

			break;
		}
	}

	//LogInfo("SetText: '%s'", mText.AsChar());

	// update the line edit
	mLineEdit->setText( mText.AsChar() );
	mLineEdit->setStyleSheet( "" );
	mLineEdit->setCursorPosition(0);

	// enable or disable the up and down buttons based on the value and its valid range
	mUpButton->setEnabled( mValue < mMaximum );
	mDownButton->setEnabled( mValue > mMinimum );
}


// called each time when a new character has been entered or removed, this doesn't update the value yet
void SpinBox::OnTextEdited(const QString& newText)
{
	// get the value string from the line edit
	FromQtString( mLineEdit->text(), &mTemp );
	mTemp.Trim();
	mTemp.Replace( StringCharacter(','), StringCharacter('.') );

	// check if the text is a valid value
	if (mTemp.IsValidFloat() == true)
	{
		// interpret the text and convert it to a value
		const double newValue = mTemp.ToDouble();

		// check if the value is in range
		if (newValue >= mMinimum && newValue <= mMaximum)
			mLineEdit->setStyleSheet( "" );
		else
			mLineEdit->setStyleSheet( "color: red;" );
	}
	else
		mLineEdit->setStyleSheet( "color: red;" );
}


// called when the user pressed enter, tab or we lost focus
void SpinBox::OnEditingFinished()
{
	// get the value string from the line edit
	FromQtString( mLineEdit->text(), &mText );
	mText.Trim();
	mTemp.Replace( StringCharacter(','), StringCharacter('.') );

	// check if the text is an invalid value
	if (mText.IsValidFloat() == false)
	{
		// reset the value to the last valid and used one
		setValue(mValue);
		emit valueChanged(mValue);
		return;
	}

	// interpret the text and convert it to a new value
	double newValue = mText.ToDouble();

	// in case the new value is out of range, use the last valid value
	if (newValue > mMaximum || newValue < mMinimum)
		newValue = mValue;

	// fire value changed events
	setValue(newValue);
	emit valueChanged(newValue);
}


// set a new value to the widget, this doesn't fire value changed events
void SpinBox::setValue(double value, bool rangeCheck, bool update)
{
	// check if we need to change if the given value is in range
	if (rangeCheck == true)
	{
		// if the value is too big, set it to the maximum
		if (value > mMaximum)
			value = mMaximum;

		// if the value is too small, set it to the minimum
		if (value < mMinimum)
			value = mMinimum;
	}

	// adjust the value
	mValue = value;
	//LogInfo("SetValue: '%g' String='%s'", mValue, String(mValue).AsChar());

	// show the new value in the interface
	if (update == true)
		Update();
}


// called when a keyboard button got pressed
void SpinBox::keyPressEvent(QKeyEvent* event)
{
	int32 key = event->key();

	// handle decreasing the value
	if (key == Qt::Key_Down)
		mDownButton->ClickButton();

	// handle increasing the value
	if (key == Qt::Key_Up)
		mUpButton->ClickButton();

	event->accept();
}


// just for completelyness and accepting the key events
void SpinBox::keyReleaseEvent(QKeyEvent* event)
{
	event->accept();
}
