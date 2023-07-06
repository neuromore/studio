/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include precompiled header
#include <QtBase/Precompiled.h>

// include required headers
#include "AttributeWidgets.h"
#include "../QtBaseManager.h"
#include "AttributeWidgetFactory.h"
#include <QHBoxLayout>


#define SPINNER_WIDTH 75

using namespace Core;

// constructor
AttributeWidget::AttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : QWidget()
{
	mReadOnly			= readOnly;
	mAttributeSettings	= attributeSettings;
	mAttributes			= attributes;
	mFirstAttribute		= NULL;
	mCustomData			= customData;
	mTabableWidget		= NULL;

	if (attributes.Size() > 0)
		mFirstAttribute = attributes[0];
}


// destructor
AttributeWidget::~AttributeWidget()
{
}


void AttributeWidget::CreateStandardLayout(QWidget* widget, const char* description)
{
	//QLabel*			label		= new QLabel( mNameString.AsChar() );
	QHBoxLayout*		layout		= new QHBoxLayout();
	
	//layout->addWidget(label);
	layout->addWidget(widget);
	layout->setAlignment(Qt::AlignLeft);

	setLayout(layout);

	//label->setMinimumWidth(ATTRIBUTEWIDGET_LABEL_WIDTH);
	//label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	//layout->setAlignment(Qt::AlignTop);

	// set tab-able widget
	mTabableWidget = widget;

	layout->setMargin(1);
	widget->setToolTip( description );
}


void AttributeWidget::CreateStandardLayout(QWidget* widget, Core::AttributeSettings* attributeSettings)
{
	assert(mAttributeSettings != NULL);
	CreateStandardLayout( widget, mAttributeSettings->GetDescription() );
}


void AttributeWidget::CreateStandardLayout(QWidget* widgetA, QWidget* widgetB, Core::AttributeSettings* attributeSettings)
{
	assert(mAttributeSettings != NULL);

	QWidget*		rightWidget = new QWidget();
	QHBoxLayout*	rightLayout = new QHBoxLayout();

	//rightLayout->setAlignment(Qt::AlignTop);

	rightLayout->addWidget(widgetA);
	rightLayout->addWidget(widgetB);
	rightLayout->setMargin(0);
	rightLayout->setAlignment(Qt::AlignLeft);

	rightWidget->setToolTip( mAttributeSettings->GetDescription() );
	rightWidget->setLayout(rightLayout);
	rightWidget->setObjectName("TransparentWidget");

	// set tab-able widget
	mTabableWidget = widgetA;

	CreateStandardLayout(rightWidget, attributeSettings);
}


void AttributeWidget::CreateStandardVLayout(QWidget* widgetA, QWidget* widgetB, Core::AttributeSettings* attributeSettings)
{
	assert(mAttributeSettings != NULL);

	QWidget*		rightWidget = new QWidget();
	QVBoxLayout*	rightLayout = new QVBoxLayout();

	rightLayout->addWidget(widgetA);
	rightLayout->addWidget(widgetB);
	rightLayout->setMargin(0);

	rightWidget->setToolTip( mAttributeSettings->GetDescription() );
	rightWidget->setLayout(rightLayout);
	rightWidget->setObjectName("TransparentWidget");

	CreateStandardLayout(rightWidget, attributeSettings);
}



// called when an attribute inside the widget got changed, this internally calls the callbacks from the attribute widget factory
void AttributeWidget::OnAttributeChanged(Attribute* attribute)
{
	AttributeWidgetFactory* widgetFactory = GetQtBaseManager()->GetAttributeWidgetFactory();
	widgetFactory->OnAttributeChanged(attribute);
}


void AttributeWidget::UpdateInterface()
{
	SetValue(mFirstAttribute);
	SetReadOnly( !mAttributeSettings->IsEnabled() );
	//SetVisible( mAttributeSettings->IsVisible() );
}

//-----------------------------------------------------------------------------------------------------------------

CheckBoxAttributeWidget::CheckBoxAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mCheckBox = new QCheckBox();
	CreateStandardLayout(mCheckBox, attributeSettings);

	SetValue(mFirstAttribute);

	mCheckBox->setEnabled( !readOnly );

	connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnCheckBox(int)));
}


void CheckBoxAttributeWidget::SetValue(Core::Attribute* attribute)
{
	if (attribute != NULL)
	{
		if (static_cast<Core::AttributeBool*>(attribute)->GetValue() == true)
			mCheckBox->setCheckState( Qt::Checked );
		else
			mCheckBox->setCheckState( Qt::Unchecked );
	}
	else
		mCheckBox->setCheckState( Qt::Unchecked );
}


void CheckBoxAttributeWidget::SetReadOnly(bool readOnly)
{
	mCheckBox->setDisabled(readOnly);
}


void CheckBoxAttributeWidget::OnCheckBox(int state)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		AttributeBool* attribute = static_cast<Core::AttributeBool*>(mAttributes[i]);

		// NOTE: new behaviour change; ignore if state is the same
		if ( (attribute->GetValue() == true && state == Qt::Checked) || (attribute->GetValue() == false && state == Qt::Unchecked) )
			continue;

		// set new state
		if (state == Qt::Checked)
			attribute->SetValue(true);
		else
			attribute->SetValue(false);

		// emit event
		OnAttributeChanged(attribute);
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

FloatSpinnerAttributeWidget::FloatSpinnerAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mSpinBox = new SpinBox();
	//mSpinBox->setMinimumWidth(SPINNER_WIDTH);
	//mSpinBox->setMaximumWidth(SPINNER_WIDTH);

	CreateStandardLayout(mSpinBox, attributeSettings);

	Core::Attribute* minAttribute = attributeSettings->GetMinValue();
	Core::Attribute* maxAttribute = attributeSettings->GetMaxValue();

	double minValue	= (minAttribute != NULL) ? static_cast<Core::AttributeFloat*>(minAttribute)->GetValue() : 0.0;
	double maxValue	= (maxAttribute != NULL) ? static_cast<Core::AttributeFloat*>(maxAttribute)->GetValue() : 1.0;
	double stepSize	= 0.1f;

	mSpinBox->setRange(minValue, maxValue);
	mSpinBox->setSingleStep( stepSize );
	mSpinBox->setDecimals(4);
	mSpinBox->setEnabled( !readOnly );

	// set tab-able widget
	mTabableWidget = mSpinBox->GetLineEdit();

	SetValue(mFirstAttribute);

	connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnDoubleSpinner(double)));
}


void FloatSpinnerAttributeWidget::SetValue(Core::Attribute* attribute)
{
	double value = (attribute != NULL) ? static_cast<Core::AttributeFloat*>(attribute)->GetValue() : 0.0;
	mSpinBox->setValue( value );
}


void FloatSpinnerAttributeWidget::SetReadOnly(bool readOnly)
{
	mSpinBox->setDisabled(readOnly);
}


// double spinner
void FloatSpinnerAttributeWidget::OnDoubleSpinner(double value)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeFloat* attribute = static_cast<Core::AttributeFloat*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
		
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

IntSpinnerAttributeWidget::IntSpinnerAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mSpinBox = new IntSpinBox();
	//mSpinBox->setMinimumWidth(SPINNER_WIDTH);
	//mSpinBox->setMaximumWidth(SPINNER_WIDTH);
	CreateStandardLayout(mSpinBox, attributeSettings);

	int value		= (mFirstAttribute != NULL) ? static_cast<Core::AttributeInt32*>(mFirstAttribute)->GetValue() : 0;
	int minValue	= static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->GetValue();
	int maxValue	= static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->GetValue();
	int stepSize	= 1;

	mSpinBox->setRange(minValue, maxValue);
	mSpinBox->setValue( value );
	mSpinBox->setSingleStep( stepSize );
	mSpinBox->setEnabled( !readOnly );

	// set tab-able widget
	mTabableWidget = mSpinBox->GetLineEdit();

	connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)));
}


void IntSpinnerAttributeWidget::SetValue(Core::Attribute* attribute)
{
	int32 value = (attribute != NULL) ? static_cast<Core::AttributeInt32*>(attribute)->GetValue() : 0;
	mSpinBox->setValue( value );
}


// int spinner
void IntSpinnerAttributeWidget::OnValueChanged(double value)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeInt32* attribute = static_cast<Core::AttributeInt32*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}
	
	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

FloatSliderAttributeWidget::FloatSliderAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mSlider = new FloatSlider();
	CreateStandardLayout(mSlider, attributeSettings);

	// set slider range
	Core::Attribute* minAttribute = attributeSettings->GetMinValue();
	Core::Attribute* maxAttribute = attributeSettings->GetMaxValue();

	double minValue	= (minAttribute != NULL) ? static_cast<Core::AttributeFloat*>(minAttribute)->GetValue() : 0.0;
	double maxValue	= (maxAttribute != NULL) ? static_cast<Core::AttributeFloat*>(maxAttribute)->GetValue() : 1.0;

	mSlider->SetRange(minValue, maxValue);
	mSlider->SetEnabled( !readOnly );

	SetValue(mFirstAttribute);
	//slider->setSingleStep( stepSize );

	// set tab-able widget
	mTabableWidget = mSlider->GetSpinBox()->GetLineEdit();

	connect(mSlider, SIGNAL(ValueChanged(double)), this, SLOT(OnFloatSlider(double)));
}


void FloatSliderAttributeWidget::SetValue(Core::Attribute* attribute)
{
	double value		= (attribute != NULL) ? static_cast<Core::AttributeFloat*>(attribute)->GetValue() : 0.0f;
	
	//double minValue	= static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMinValue())->GetValue();
	//double maxValue	= static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMaxValue())->GetValue();
	//const int32 sliderValue = ((value - minValue) / (maxValue - minValue)) * 1000;
	
	mSlider->SetValue(value);
}


void FloatSliderAttributeWidget::SetReadOnly(bool readOnly)
{
	mSlider->SetEnabled( !readOnly );
}


// float slider value change
void FloatSliderAttributeWidget::OnFloatSlider(double value)
{
	/*const float minValue = static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMinValue())->GetValue();
	const double maxValue = static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMaxValue())->GetValue();
	const double floatValue = minValue + (maxValue - minValue) * (value / 1000.0);*/

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeFloat* attribute = static_cast<Core::AttributeFloat*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}


//-----------------------------------------------------------------------------------------------------------------

FloatSliderLabelAttributeWidget::FloatSliderLabelAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mSlider = new FloatSlider(NULL, FloatSlider::LABEL);
	CreateStandardLayout(mSlider, attributeSettings);

	// set slider range
	Core::Attribute* minAttribute = attributeSettings->GetMinValue();
	Core::Attribute* maxAttribute = attributeSettings->GetMaxValue();

	double minValue	= (minAttribute != NULL) ? static_cast<Core::AttributeFloat*>(minAttribute)->GetValue() : 0.0;
	double maxValue	= (maxAttribute != NULL) ? static_cast<Core::AttributeFloat*>(maxAttribute)->GetValue() : 1.0;

	mSlider->SetRange(minValue, maxValue);


	mSlider->SetEnabled( !readOnly );

	SetValue(mFirstAttribute);
	//slider->setSingleStep( stepSize );

	// set tab-able widget
	mTabableWidget = mSlider->GetSpinBox()->GetLineEdit();

	connect(mSlider, SIGNAL(ValueChanged(double)), this, SLOT(OnFloatSlider(double)));
}


void FloatSliderLabelAttributeWidget::SetValue(Core::Attribute* attribute)
{
	double value		= (attribute != NULL) ? static_cast<Core::AttributeFloat*>(attribute)->GetValue() : 0.0;
	
	//double minValue	= static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMinValue())->GetValue();
	//double maxValue	= static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMaxValue())->GetValue();
	//const int32 sliderValue = ((value - minValue) / (maxValue - minValue)) * 1000;
	
	mSlider->SetValue(value);
}


void FloatSliderLabelAttributeWidget::SetReadOnly(bool readOnly)
{
	mSlider->SetEnabled( !readOnly );
}


// float slider value change
void FloatSliderLabelAttributeWidget::OnFloatSlider(double value)
{
	/*const float minValue = static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMinValue())->GetValue();
	const double maxValue = static_cast<Core::AttributeFloat*>(mAttributeSettings->GetMaxValue())->GetValue();
	const double floatValue = minValue + (maxValue - minValue) * (value / 1000.0);*/

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeFloat* attribute = static_cast<Core::AttributeFloat*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

IntSliderAttributeWidget::IntSliderAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mSlider = new IntSlider();
	CreateStandardLayout(mSlider, attributeSettings);

	int value		= (mFirstAttribute != NULL) ? static_cast<Core::AttributeInt32*>(mFirstAttribute)->GetValue() : 0;
	int minValue	= static_cast<Core::AttributeInt32*>(attributeSettings->GetMinValue())->GetValue();
	int maxValue	= static_cast<Core::AttributeInt32*>(attributeSettings->GetMaxValue())->GetValue();

	mSlider->SetRange(0, 1000);
	mSlider->SetEnabled( !readOnly );

	int stepSize = 1;
	const int32 sliderValue = ((value - minValue) / (float)(maxValue - minValue)) * 1000.0f;
	mSlider->SetValue( sliderValue );
	mSlider->SetSingleStep( stepSize );

	// set tab-able widget
	mTabableWidget = mSlider->GetSpinBox()->GetLineEdit();

	connect(mSlider, SIGNAL(ValueChanged(int)), this, SLOT(OnIntSlider(int)));
}


void IntSliderAttributeWidget::SetValue(Core::Attribute* attribute)
{
	int32 value = (attribute != NULL) ? static_cast<Core::AttributeInt32*>(attribute)->GetValue() : 0;
	mSlider->SetValue( value );
}



// int slider value change
void IntSliderAttributeWidget::OnIntSlider(int value)
{
	const int32 minValue = static_cast<Core::AttributeInt32*>(mAttributeSettings->GetMinValue())->GetValue();
	const int32 maxValue = static_cast<Core::AttributeInt32*>(mAttributeSettings->GetMaxValue())->GetValue();
	const int32 intValue = minValue + (maxValue - minValue) * (value / 1000.0f);

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeInt32* attribute = static_cast<Core::AttributeInt32*>(mAttributes[i]);

		if (attribute->GetValue() != intValue)
		{
			attribute->SetValue(intValue);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

ComboBoxAttributeWidget::ComboBoxAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mComboBox = new QComboBox();
	CreateStandardLayout(mComboBox, attributeSettings);

	int value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeInt32*>(mFirstAttribute)->GetValue() : 0;

	const uint32 numComboValues = attributeSettings->GetNumComboValues();
	for (uint32 i=0; i<numComboValues; ++i)
		mComboBox->addItem( attributeSettings->GetComboValueString(i).AsChar() );

	mComboBox->setCurrentIndex( value );
	mComboBox->setEditable( false );
	mComboBox->setEnabled( !readOnly );

	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBox(int)));
}


void ComboBoxAttributeWidget::SetValue(Core::Attribute* attribute)
{
	int32 value = (attribute != NULL) ? static_cast<Core::AttributeInt32*>(attribute)->GetValue() : 0;
	mComboBox->setCurrentIndex( value );
}


// adjust a combobox value
void ComboBoxAttributeWidget::OnComboBox(int value)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeInt32* attribute = static_cast<Core::AttributeInt32*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

StringAttributeWidget::StringAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mLineEdit = new QLineEdit();
	CreateStandardLayout(mLineEdit, attributeSettings);

	const char* value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeString*>(mFirstAttribute)->GetValue() : "";
	mLineEdit->setText( value );
	mLineEdit->setReadOnly( readOnly );
	mLineEdit->setEnabled( !readOnly );

	connect(mLineEdit, SIGNAL(editingFinished()), this, SLOT(OnStringChange()));
}


void StringAttributeWidget::SetValue(Core::Attribute* attribute)
{
	if (attribute == NULL)
		mLineEdit->clear();
	else
		mLineEdit->setText( static_cast<Core::AttributeString*>(attribute)->GetValue() );
}


// a string changed
void StringAttributeWidget::OnStringChange()
{
	assert( sender()->inherits("QLineEdit") == true );
	QLineEdit* widget = qobject_cast<QLineEdit*>( sender() );
	String widgetText;

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();

	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeString* attribute = static_cast<Core::AttributeString*>(mAttributes[i]);
		widgetText = FromQtString(widget->text());

		if (widgetText.Compare(attribute->GetValue()) != 0)
		{
			attribute->SetValue(widgetText);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}


//-----------------------------------------------------------------------------------------------------------------
// TODO: replace this with a real string array edit widget

StringArrayAttributeWidget::StringArrayAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mLineEdit = new QLineEdit();
	CreateStandardLayout(mLineEdit, attributeSettings);

	const char* value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeStringArray*>(mFirstAttribute)->AsString().AsChar() : "";
	mLineEdit->setText( value );
	mLineEdit->setReadOnly( readOnly );
	mLineEdit->setEnabled( !readOnly );

	connect(mLineEdit, SIGNAL(editingFinished()), this, SLOT(OnStringChange()));
}


void StringArrayAttributeWidget::SetValue(Core::Attribute* attribute)
{
	if (attribute == NULL)
		mLineEdit->clear();
	else
		mLineEdit->setText( static_cast<Core::AttributeStringArray*>(attribute)->AsString().AsChar() );
}


// a string changed
void StringArrayAttributeWidget::OnStringChange()
{
	assert( sender()->inherits("QLineEdit") == true );
	QLineEdit* widget = qobject_cast<QLineEdit*>( sender() );
	String widgetText;
	String attribText;

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();

	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeStringArray* attribute = static_cast<Core::AttributeStringArray*>(mAttributes[i]);
		attribText = attribute->AsString();
		widgetText = FromQtString(widget->text());

		if (widgetText.Compare(attribText) != 0)
		{
			attribute->InitFromString(widgetText);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}


//-----------------------------------------------------------------------------------------------------------------


TextAttributeWidget::TextAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mPlainTextEdit = new QPlainTextEdit();
	CreateStandardLayout(mPlainTextEdit, attributeSettings);

	const char* value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeString*>(mFirstAttribute)->GetValue() : "";
	mPlainTextEdit->setPlainText( value );
	mPlainTextEdit->setReadOnly( readOnly );
	mPlainTextEdit->setEnabled( !readOnly );
	mPlainTextEdit->setFixedHeight(100);

	connect(mPlainTextEdit, SIGNAL(textChanged()), this, SLOT(OnStringChange()));
}


void TextAttributeWidget::SetValue(Core::Attribute* attribute)
{
	if (attribute == NULL)
		mPlainTextEdit->clear();
	else {
		QString s(static_cast<Core::AttributeString*>(attribute)->GetValue());
		if (mPlainTextEdit->toPlainText() != s)
			mPlainTextEdit->setPlainText(s);
	}
}


// a string changed
void TextAttributeWidget::OnStringChange()
{
	assert( sender()->inherits("QPlainTextEdit") == true );
	QPlainTextEdit* widget = qobject_cast<QPlainTextEdit*>( sender() );
	String widgetText;

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();

	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeText* attribute = static_cast<Core::AttributeText*>(mAttributes[i]);
		widgetText = FromQtString(widget->toPlainText());

		if (widgetText.Compare(attribute->GetValue()) != 0)
		{
			attribute->SetValue(widgetText);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();
}


//-----------------------------------------------------------------------------------------------------------------

ColorAttributeWidget::ColorAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	const Core::Color& color = (mFirstAttribute != NULL) ? static_cast<Core::AttributeColor*>(mFirstAttribute)->GetValue() : Core::Color( 0.0f, 0.0f, 0.0f, 0.0f );

	mColorLabel = new ColorLabel( this, ToQColor(color) );
	CreateStandardLayout(mColorLabel, attributeSettings);

	connect(mColorLabel, SIGNAL(ColorChangeEvent()), this, SLOT(OnColorChanged()));
}


void ColorAttributeWidget::SetValue(Core::Attribute* attribute)
{
	//const Core::Color& color = (attribute != NULL) ? static_cast<Core::AttributeColor*>(attribute)->GetValue() : Core::Color( 0.0f, 0.0f, 0.0f, 0.0f );
	//mColorLabel->GetColor(color);
}


// a string changed
void ColorAttributeWidget::OnColorChanged()
{
	ColorLabel* widget = qobject_cast<ColorLabel*>( sender() );

	const Core::Color& newColor = FromQtColor( widget->GetColor() );

	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeColor* attribute = static_cast<Core::AttributeColor*>(mAttributes[i]);

		if (attribute->GetValue() != newColor)
		{
			attribute->SetValue(newColor);
			OnAttributeChanged(attribute);
		}
	}

	FireValueChangedSignal();

}

//-----------------------------------------------------------------------------------------------------------------

ButtonAttributeWidget::ButtonAttributeWidget(const char* buttonText, const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mButton = new QPushButton();
	CreateStandardLayout( mButton, buttonText );
	mButton->setText(buttonText);

	connect(mButton, SIGNAL(released()), this, SLOT(OnReleased()));
}

void ButtonAttributeWidget::OnReleased()
{
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i = 0; i < numAttributes; ++i)
	{
		AttributeBool* attribute = static_cast<Core::AttributeBool*>(mAttributes[i]);
		attribute->SetValue(!attribute->GetValue());
		OnAttributeChanged(attribute);
	}

	FireValueChangedSignal();
}

//-----------------------------------------------------------------------------------------------------------------

// constructor
ColorMappingAttributeWidget::ColorMappingAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mColorMapper = new ColorMapper();
	mColorMappingWidget = new ColorMappingWidget( mColorMapper, this );
	mColorMappingWidget->hide();

	mComboBox = new QComboBox();

	CreateStandardVLayout(mComboBox, mColorMappingWidget, attributeSettings);

	mColorMappingWidget->update();

	int value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeInt32*>(mFirstAttribute)->GetValue() : 0;

	const uint32 numComboValues = ColorMapper::COLORMAPPING_NUM;
	for (uint32 i=0; i<numComboValues; ++i)
		mComboBox->addItem( mColorMapper->GetColorMappingName((ColorMapper::EColorMapping)i) );

	mComboBox->setCurrentIndex( value );
	mColorMapper->SetColorMapping( (ColorMapper::EColorMapping)value );
	mColorMappingWidget->update();

	mComboBox->setEditable( false );
	mComboBox->setEnabled( !readOnly );

	mColorMappingWidget->show();

	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBox(int)));
}


// destructor
ColorMappingAttributeWidget::~ColorMappingAttributeWidget()
{
	delete mColorMapper;
}


void ColorMappingAttributeWidget::SetValue(Core::Attribute* attribute)
{
	int32 value = (attribute != NULL) ? static_cast<Core::AttributeInt32*>(attribute)->GetValue() : 0;
	mComboBox->setCurrentIndex( value );
}


// adjust a combobox value
void ColorMappingAttributeWidget::OnComboBox(int value)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeInt32* attribute = static_cast<Core::AttributeInt32*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}

	// update the color mapping widget
	mColorMapper->SetColorMapping( (ColorMapper::EColorMapping)value );
	mColorMappingWidget->update();

	FireValueChangedSignal();
}


//-----------------------------------------------------------------------------------------------------------------

WindowFunctionAttributeWidget::WindowFunctionAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode) : AttributeWidget(attributes, attributeSettings, customData, readOnly, creationMode)
{
	mComboBox = new QComboBox();

	mWindowFunction = new WindowFunction();
	mWindowFunction->SetType( WindowFunction::WINDOWFUNCTION_GAUSSIAN );

	mWindowFunctionWidget = new WindowFunctionWidget( mWindowFunction, WindowFunctionWidget::DOMAIN_TIME, this );

	CreateStandardVLayout(mComboBox, mWindowFunctionWidget, attributeSettings);

	int value = (mFirstAttribute != NULL) ? static_cast<Core::AttributeInt32*>(mFirstAttribute)->GetValue() : 0;

	const uint32 numComboValues = attributeSettings->GetNumComboValues();
	for (uint32 i=0; i<numComboValues; ++i)
		mComboBox->addItem( attributeSettings->GetComboValueString(i).AsChar() );

	mComboBox->setCurrentIndex( value );
	mComboBox->setEditable( false );
	mComboBox->setEnabled( !readOnly );

	mWindowFunction->SetType( (WindowFunction::EWindowFunction)value );
	mWindowFunctionWidget->update();

	// set tab-able widget
	mTabableWidget = mComboBox;

	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBox(int)));
}


// destructor
WindowFunctionAttributeWidget::~WindowFunctionAttributeWidget()
{
	delete mWindowFunction;
}


void WindowFunctionAttributeWidget::SetValue(Core::Attribute* attribute)
{
	int32 value = (attribute != NULL) ? static_cast<Core::AttributeInt32*>(attribute)->GetValue() : 0;
	mComboBox->setCurrentIndex( value );
}


// adjust a combobox value
void WindowFunctionAttributeWidget::OnComboBox(int value)
{
	// get the number of attributes and iterate through them
	const uint32 numAttributes = mAttributes.Size();
	for (uint32 i=0; i<numAttributes; ++i)
	{
		Core::AttributeInt32* attribute = static_cast<Core::AttributeInt32*>(mAttributes[i]);

		if (attribute->GetValue() != value)
		{
			attribute->SetValue(value);
			OnAttributeChanged(attribute);
		}
	}

	mWindowFunction->SetType( (WindowFunction::EWindowFunction)value );
	mWindowFunctionWidget->update();

	FireValueChangedSignal();
}
