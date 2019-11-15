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
#include "ParameterWidgets.h"
#include "../../AppManager.h"
#include <Graph/ParameterNode.h>
#include <Slider.h>


using namespace Core;

// constructor
ParameterWidget::ParameterWidget(ParameterNode* node, EWidgetPosition position, QWidget* parent) : QWidget(parent)
{
	CORE_ASSERT(node != NULL);
	mParameterNode = node;

	mLabel = NULL;
	mDefaultValue = 0.0;
	setToolTip( mParameterNode->GetDescription() );
}


// init widgets
void ParameterWidget::Init(QWidget* controlWidget, EWidgetPosition position)
{
	// create the vertical main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setMargin(3);
	mainLayout->setSpacing(1);
	mainLayout->setSizeConstraint(QLayout::SetNoConstraint );
	
	mLabel = new QLabel(mParameterNode->GetName(), this);
	mLabel->setVisible(mLabel->text().isEmpty() == false);
	mLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mLabel->setToolTip( mParameterNode->GetDescription() );
	
	controlWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	controlWidget->setToolTip( mParameterNode->GetDescription() );

	// create layout depending on the configured widget position
	switch (position)
	{
		case POSITION_LEFT:
		{
			QHBoxLayout* hLayout = new QHBoxLayout();
			hLayout->addWidget(controlWidget);
			hLayout->addWidget(mLabel);
			mainLayout->addLayout(hLayout);
		} 
		break;
	
		case POSITION_RIGHT:
		{
			QHBoxLayout* hLayout = new QHBoxLayout();
			hLayout->addWidget(mLabel);

			// a little hacky: add spacer only if label is invisible
			if (mLabel->isVisible() == true)
			{
				QWidget* spacer = new QWidget();
				spacer->setFixedWidth( 25 );
				hLayout->addWidget(spacer);
			}
			
			hLayout->addWidget(controlWidget);
			mainLayout->addLayout(hLayout);
		}
		break;
	
		case POSITION_BELOW:
		{
			mainLayout->addWidget(mLabel);
			mainLayout->addWidget(controlWidget);
		} 
		break;
	}

	setLayout(mainLayout);

	UpdateInterface();
	UpdateFromNode(mParameterNode);
}



// check if the node has changed since initialization
bool ParameterWidget::HasNodeChanged() const
{
	// compare name
	if (mLabel->text().compare(mParameterNode->GetName()) != 0)
		return true;

	// compare default value
	if (mDefaultValue != mParameterNode->GetDefaultValue())
		return true;

	return false;
}



// update from node
void ParameterWidget::UpdateFromNode(ParameterNode* node)
{
	// widget was not initialized yet???
	if (mLabel == NULL)
		return;

	if (node != NULL)
		mParameterNode = node;
	
	// take over new values
	mDefaultValue = mParameterNode->GetDefaultValue();
	 
	// update label
	mLabel->setText (mParameterNode->GetName());
	mLabel->setVisible(mLabel->text().isEmpty() == false);
}


// update displayed interface information
void ParameterWidget::UpdateInterface()
{
	if (HasNodeChanged() == true)
		UpdateFromNode(mParameterNode);
}




//
// Separator Widget
//

// init widgets
void ParameterSeparatorWidget::Init(EWidgetPosition position)
{
	mLineWidget = new QWidget(this);
	
	mLineWidget->setFixedHeight(2);
	mLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	mLineWidget->setStyleSheet(QString("background-color: #606060;"));

	ParameterWidget::Init(mLineWidget, position);

	QFont font = mLabel->font();
	font.setBold(true);
	mLabel->setFont(font);
}



//
// Slider Widget
//


// init widget
void ParameterSliderWidget::Init(EWidgetPosition position)
{
	mSlider = new FloatSlider(this);
	
	// connect slider
	connect(mSlider, &FloatSlider::ValueChanged, this, &ParameterSliderWidget::OnSliderValueChanged);
	
	// configure slider
	mSlider->SetMode(FloatSlider::SPINNER);

	ParameterWidget::Init(mSlider, position);
}


// update displayed interface information
void ParameterSliderWidget::UpdateInterface()
{
	// don't update interface while the slider is in use!
	if (mSlider->IsPressed() == false && mSlider->GetSpinBox()->hasFocus() == false)
		ParameterWidget::UpdateInterface();
}


// check if the node has changed since initialization
bool ParameterSliderWidget::HasNodeChanged() const
{
	if (ParameterWidget::HasNodeChanged() == true)
		return true;

	// compare range
	if (mRangeMin != mParameterNode->GetRangeMin() || mRangeMax != mParameterNode->GetRangeMax())
		return true;

	return false;
}


// update from node
void ParameterSliderWidget::UpdateFromNode(ParameterNode* node)
{
	ParameterWidget::UpdateFromNode(node);

	// widget was not initialized yet???
	if (mSlider == NULL)
		return;

	// take over new values
	mRangeMin = mParameterNode->GetRangeMin();
	mRangeMax = mParameterNode->GetRangeMax();
	 
	// update widgets, set default value
	mSlider->SetRange(mRangeMin, mRangeMax);
	mSlider->SetValue(mDefaultValue);
}




//
// Check Box Widget
//

// init widgets
void ParameterCheckBoxWidget::Init(EWidgetPosition position)
{
	QWidget* checkboxWidget = new QWidget(this);
	mCheckboxLayout = new QHBoxLayout();
	checkboxWidget->setLayout(mCheckboxLayout);
	
	ParameterWidget::Init(checkboxWidget, position);
}


void ParameterCheckBoxWidget::ApplyParameter()
{
	const uint32 numCheckboxes = mCheckboxes.Size();

	// Mode 1: one button per value
	if (mParameterNode->GetNumChannels() == numCheckboxes)
	{
		// find checked button
		for (uint32 i=0; i<numCheckboxes; ++i)
		{
			if (mCheckboxes[i]->isChecked() == true)
				mParameterNode->SetValue( 1.0, i);
			else
				mParameterNode->SetValue( 0.0, i);
		}
	}
	else // Mode 2: only on button can be selected
	{
		// find checked button
		int checkedIndex = CORE_INVALIDINDEX32;
		for (uint32 i=0; i<numCheckboxes && checkedIndex == CORE_INVALIDINDEX32; ++i)
			if (mCheckboxes[i]->isChecked() == true)
				checkedIndex = i;
		
		if (checkedIndex != CORE_INVALIDINDEX32)
		{
			// unselect all others to fix bad states
			for (uint32 i=0; i<numCheckboxes; ++i)
			{
				if (i != checkedIndex)
				{
					mCheckboxes[i]->blockSignals(true);
					mCheckboxes[i]->setChecked(false);
					mCheckboxes[i]->blockSignals(false);
				}
			}

			// output the value
			mParameterNode->SetValue( (double)checkedIndex );
		}
	}
}


// check if the node has changed since initialization
bool ParameterCheckBoxWidget::HasNodeChanged() const
{
	if (ParameterWidget::HasNodeChanged() == true)
		return true;

	const uint32 numButtonsEntries = mCheckboxes.Size();
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();

	if (options.Size() != numButtonsEntries)
		return true;
	
	// string-compare all button texts, return true if one does not match
	for (uint32 i=0; i<numButtonsEntries; ++i)
	{
		if (options[i].IsEqual( FromQtString(mCheckboxes[i]->text()) ) == false)
			return true;
	}

	// nothing changed
	return false;
}


// update from node
void ParameterCheckBoxWidget::UpdateFromNode(ParameterNode* node)
{
	ParameterWidget::UpdateFromNode(node);

	// widget was not initialized yet???
	if (mCheckboxLayout == NULL)
		return;
	
	// remove buttons
	const uint32 numCheckboxes = mCheckboxes.Size();
	for (uint32 i=0; i<numCheckboxes; ++i)
		mCheckboxes[i]->deleteLater();
	mCheckboxes.Clear();

	// add one checkbox per entry
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();
	const uint32 numEntries = options.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		QCheckBox* checkbox = new QCheckBox(options[i].AsChar());
		connect(checkbox, &QPushButton::clicked, this, &ParameterCheckBoxWidget::OnCheckBoxValueChanged);
		
		mCheckboxes.Add(checkbox);
		mCheckboxLayout->addWidget(checkbox);
	}

	const uint32 selectedIndex =  Min(numEntries-1, (uint32)mParameterNode->GetDefaultValue());
	mCheckboxes[selectedIndex]->setChecked(true);
}


void ParameterCheckBoxWidget::OnCheckBoxValueChanged(int state)
{
	QCheckBox* checkBox = static_cast<QCheckBox*>(sender());

	// find selected index
	const uint32 selectedCheckboxIndex = mCheckboxes.Find(checkBox);
	if (selectedCheckboxIndex == CORE_INVALIDINDEX32)
		return;

	const uint32 numCheckboxes = mCheckboxes.Size();

	// Mode 1: one checkbox per channel
	if (mParameterNode->GetNumChannels() == numCheckboxes)
	{
		// output the new value on every channel
		for (uint32 i=0; i<numCheckboxes; ++i)
		{
			const double stateValue = (mCheckboxes[i]->isChecked() ? 1.0 : 0.0);
			mParameterNode->SetValue( stateValue, i );
		}
	}
	else // Mode 2: single-selection mode
	{
		// don't allow unselect of an active element in this case
		if (state == false)
		{
			// TODO checkme
			checkBox->blockSignals(true);
			checkBox->setChecked(true);
			checkBox->blockSignals(false);
		}

		// unselect all other buttons
		const uint32 numCheckboxes = mCheckboxes.Size();
		for (uint32 i=0; i<numCheckboxes; ++i)
		{
			if (i != selectedCheckboxIndex)
			{ 
				mCheckboxes[i]->blockSignals(true);
				mCheckboxes[i]->setChecked(false);
				mCheckboxes[i]->blockSignals(false);
			}
		}

		// output the new value
		mParameterNode->SetValue( (double)selectedCheckboxIndex );
	}
}


//
// Combo Box Widget
//

// init widgets
void ParameterComboBoxWidget::Init(EWidgetPosition position)
{
	mComboBox = new QComboBox(this);
	
	// connect slider
	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxSelectionChanged(int)));
	
	ParameterWidget::Init(mComboBox, position);
}



// check if the node has changed since initialization
bool ParameterComboBoxWidget::HasNodeChanged() const
{
	if (ParameterWidget::HasNodeChanged() == true)
		return true;

	// compare entries
	const uint32 numEntries = mComboBox->count();
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();
	if (options.Size() != numEntries)
		return true;
	
	// string-compare all entries, return true if one does not match
	for (uint32 i=0; i<numEntries; ++i)
	{
		if (options[i].IsEqual( FromQtString(mComboBox->itemText(i)) ) == false)
			return true;
	}

	// nothing changed
	return false;
}


// update from node
void ParameterComboBoxWidget::UpdateFromNode(ParameterNode* node)
{
	ParameterWidget::UpdateFromNode(node);

	// widget was not initialized yet???
	if (mComboBox == NULL)
		return;

	
	// clear box
	mComboBox->clear();

	// add all entries
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();
	const uint32 numEntries = options.Size();
	for (uint32 i=0; i<numEntries; ++i)
		mComboBox->addItem(options[i].AsChar());

	// select the default value
	const uint32 selectedItemIndex = (uint32)mParameterNode->GetDefaultValue();
	mComboBox->setCurrentIndex( Min(numEntries-1, selectedItemIndex) );
}


//
// Selection Buttons Widget
//

// init widgets
void ParameterSelectionButtonWidget::Init(EWidgetPosition position)
{
	QWidget* buttonWidget = new QWidget(this);
	mButtonLayout = new QHBoxLayout();
	buttonWidget->setLayout(mButtonLayout);
	
	ParameterWidget::Init(buttonWidget, position);
}


void ParameterSelectionButtonWidget::ApplyParameter()
{
	const uint32 numButtons = mButtons.Size();

	// Mode 1: one button per value
	if (mParameterNode->GetNumChannels() == numButtons)
	{
		// find checked button
		for (uint32 i=0; i<numButtons; ++i)
		{
			if (mButtons[i]->isChecked() == true)
				mParameterNode->SetValue( 1.0, i);
			else
				mParameterNode->SetValue( 0.0, i);
		}
	}
	else // Mode 2: only on button can be selected
	{
		uint32 checkedIndex = CORE_INVALIDINDEX32;
		for (uint32 i=0; i<numButtons; ++i)
		{
			if (mButtons[i]->isChecked() == true)
			{
				checkedIndex = i;
				break;
			}
		}
		
		if (checkedIndex != CORE_INVALIDINDEX32)
		{
			// unselect all others to fix bad states
			for (uint32 i=0; i<numButtons; ++i)
			{
				if (i != checkedIndex)
				{
					mButtons[i]->blockSignals(true);
					mButtons[i]->setChecked(false);
					mButtons[i]->blockSignals(false);
				}
			}

			// output the value
			mParameterNode->SetValue( (double)checkedIndex );
		}
	}
}


// check if the node has changed since initialization
bool ParameterSelectionButtonWidget::HasNodeChanged() const
{
	if (ParameterWidget::HasNodeChanged() == true)
		return true;

	const uint32 numButtonsEntries = mButtons.Size();
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();

	if (options.Size() != numButtonsEntries)
		return true;
	
	// string-compare all button texts, return true if one does not match
	for (uint32 i=0; i<numButtonsEntries; ++i)
	{
		if (options[i].IsEqual( FromQtString(mButtons[i]->text()) ) == false)
			return true;
	}

	// nothing changed
	return false;
}


// update from node
void ParameterSelectionButtonWidget::UpdateFromNode(ParameterNode* node)
{
	ParameterWidget::UpdateFromNode(node);

	// widget was not initialized yet???
	if (mButtonLayout == NULL)
		return;
	
	// remove buttons
	const uint32 numButtons = mButtons.Size();
	for (uint32 i=0; i<numButtons; ++i)
		mButtons[i]->deleteLater();
	mButtons.Clear();

	// add one button per entrie
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();
	const uint32 numEntries = options.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		QPushButton* button = new QPushButton(options[i].AsChar());
		connect(button, &QPushButton::clicked, this, &ParameterSelectionButtonWidget::OnButtonClicked);
		button->setCheckable(true);

		mButtons.Add(button);
		mButtonLayout->addWidget(button);
	}

	const uint32 selectedIndex =  Min(numEntries-1, (uint32)mParameterNode->GetDefaultValue());
	mButtons[selectedIndex]->setChecked(true);
}


void ParameterSelectionButtonWidget::OnButtonClicked()
{
	QPushButton* button = static_cast<QPushButton*>(sender());

	// find out what button was clicked
	const uint32 selectedButtonIndex = mButtons.Find(button);
	if (selectedButtonIndex == CORE_INVALIDINDEX32)
		return;
	
	// don't deselect an already checked button
	if (button->isChecked() == false)
	{
		button->blockSignals(true);
		button->setChecked(true);
		button->blockSignals(false);
	}

	const uint32 numButtons = mButtons.Size();
	
	// Mode 1: one button per channel
	if (mParameterNode->GetNumChannels() == numButtons)
	{
		// output the new value on every channel
		for (uint32 i=0; i<numButtons; ++i)
		{
			const double stateValue = (mButtons[i]->isChecked() ? 1.0 : 0.0);
			mParameterNode->SetValue( stateValue, i );
		}
	}
	else // Mode 2: single-selection mode
	{
		// unselect all other buttons
		for (uint32 i=0; i<numButtons; ++i)
		{
			if (i != selectedButtonIndex)
			{ 
				mButtons[i]->blockSignals(true);
				mButtons[i]->setChecked(false);
				mButtons[i]->blockSignals(false);
			}
		}

		// output the new value
		mParameterNode->SetValue( (double)selectedButtonIndex );
	}
}



//
// Event Buttons Widget
//

// init widgets
void ParameterEventButtonWidget::Init(EWidgetPosition position)
{
	QWidget* buttonWidget = new QWidget(this);
	mButtonLayout = new QHBoxLayout();
	buttonWidget->setLayout(mButtonLayout);
	
	ParameterWidget::Init(buttonWidget, position);
}


// check if the node has changed since initialization
bool ParameterEventButtonWidget::HasNodeChanged() const
{
	if (ParameterWidget::HasNodeChanged() == true)
		return true;

	const uint32 numButtonsEntries = mButtons.Size();
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();

	if (options.Size() != numButtonsEntries)
		return true;
	
	// string-compare all button texts, return true if one does not match
	for (uint32 i=0; i<numButtonsEntries; ++i)
	{
		if (options[i].IsEqual( FromQtString(mButtons[i]->text()) ) == false)
			return true;
	}

	// nothing changed
	return false;
}


// update from node
void ParameterEventButtonWidget::UpdateFromNode(ParameterNode* node)
{
	ParameterWidget::UpdateFromNode(node);

	// widget was not initialized yet???
	if (mButtonLayout == NULL)
		return;
	
	// remove buttons
	const uint32 numButtons = mButtons.Size();
	for (uint32 i=0; i<numButtons; ++i)
		mButtons[i]->deleteLater();
	mButtons.Clear();

	// add one button per entry
	const Core::Array<Core::String>& options = mParameterNode->GetOptions();
	const uint32 numEntries = options.Size();
	for (uint32 i=0; i<numEntries; ++i)
	{
		QPushButton* button = new QPushButton(options[i].AsChar());
		connect(button, &QPushButton::clicked, this, &ParameterEventButtonWidget::OnButtonClicked);

		mButtons.Add(button);
		mButtonLayout->addWidget(button);
	}
}


void ParameterEventButtonWidget::OnButtonClicked()
{
	// find out what button was clicked
	QPushButton* button = static_cast<QPushButton*>(sender());

	const uint32 selectedButtonIndex = mButtons.Find(button);
	if (selectedButtonIndex == CORE_INVALIDINDEX32)
		return;

	const uint32 numButtons = mButtons.Size();

	// Mode 1: one button per channel
	if (mParameterNode->GetNumChannels() == numButtons)
	{
		// trigger the respective channel
		mParameterNode->SetValue( 1.0, selectedButtonIndex );
	}
	else // Mode 2: single-value mode
	{
		// output the new value
		mParameterNode->SetValue( (double)(selectedButtonIndex + 1) );
	}
}

