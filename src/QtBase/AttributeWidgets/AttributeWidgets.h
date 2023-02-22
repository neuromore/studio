/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_ATTRIBUTEWIDGETS_H
#define __NEUROMORE_ATTRIBUTEWIDGETS_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/AttributeBool.h>
#include <Core/AttributeInt32.h>
#include <Core/AttributeFloat.h>
#include <Core/AttributeString.h>
#include <Core/AttributeStringArray.h>

#include <Core/AttributeText.h>

#include <Core/AttributeColor.h>
#include <Core/AttributeSettings.h>
#include <Core/AttributeSet.h>
#include <ColorMapper.h>
#include "../Widgets/WindowFunctionWidget.h"
#include "../Widgets/ColorMappingWidget.h"
#include "../Spinbox.h"
#include "../Slider.h"
#include "../ColorLabel.h"
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>


class QTBASE_API AttributeWidget : public QWidget
{
	Q_OBJECT
	public:
		AttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		virtual ~AttributeWidget();

		// called when an attribute inside the widget got changed, this internally calls the callbacks from the attribute widget factory
		void OnAttributeChanged(Core::Attribute* attribute);

		void CreateStandardLayout(QWidget* widget, Core::AttributeSettings* attributeSettings);
		void CreateStandardLayout(QWidget* widget, const char* description);
		void CreateStandardLayout(QWidget* widgetA, QWidget* widgetB, Core::AttributeSettings* attributeSettings);
		void CreateStandardVLayout(QWidget* widgetA, QWidget* widgetB, Core::AttributeSettings* attributeSettings);

		void SetCreationMode(bool creationMode)					{ mCreationMode = creationMode; }
		bool GetCreationMode()									{ return mCreationMode; }
		Core::AttributeSettings* GetAttributeInfo()				{ return mAttributeSettings; }

		virtual void SetReadOnly(bool readOnly)					{}
		virtual void EnableWidgets(bool enabled)				{ setDisabled(!enabled); }

		// visibility
		virtual void SetVisible(bool isVisible)					{ setVisible(isVisible); }
		virtual bool IsVisible() const							{ return isVisible(); }

		QWidget* GetTablableWidget() const						{ return mTabableWidget; }

		// reads out the attribute value and updates the interface elements accordingly
		void UpdateInterface();
		virtual void SetValue(Core::Attribute* attribute) = 0;

	signals:
		void ValueChanged();

	protected:
		void FireValueChangedSignal()							{ emit ValueChanged(); }
		Core::Array<Core::Attribute*>			mAttributes;
		Core::Attribute*						mFirstAttribute;
		Core::AttributeSettings*				mAttributeSettings;
		QWidget*								mTabableWidget;
		bool									mReadOnly;
		bool									mCreationMode;
		void*									mCustomData;
};


// checkbox attribute widget
class QTBASE_API CheckBoxAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		CheckBoxAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
		void SetReadOnly(bool readOnly) override;

	public slots:
		void OnCheckBox(int state);
	private:
		QCheckBox* mCheckBox;
};


// float spinner attribute widget
class QTBASE_API FloatSpinnerAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		FloatSpinnerAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
		void SetReadOnly(bool readOnly) override;

	protected slots:
		void OnDoubleSpinner(double value);
	private:
		SpinBox* mSpinBox;
};


// int spinner attribute widget
class QTBASE_API IntSpinnerAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		IntSpinnerAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnValueChanged(double value);
	private:
		IntSpinBox* mSpinBox;
};


// float slider attribute widget
class QTBASE_API FloatSliderAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		FloatSliderAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
		void SetReadOnly(bool readOnly) override;
	protected slots:
		void OnFloatSlider(double value);
	private:
		FloatSlider* mSlider;
};


// float slider (with label) attribute widget
class QTBASE_API FloatSliderLabelAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		FloatSliderLabelAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
		void SetReadOnly(bool readOnly) override;
	protected slots:
		void OnFloatSlider(double value);
	private:
		FloatSlider* mSlider;
};


// int slider attribute widget
class QTBASE_API IntSliderAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		IntSliderAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnIntSlider(int value);
	private:
		IntSlider* mSlider;
};


// combo box attribute widget
class QTBASE_API ComboBoxAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		ComboBoxAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnComboBox(int value);
	private:
		QComboBox* mComboBox;
};


// string attribute widget
class QTBASE_API StringAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		StringAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnStringChange();
	private:
		QLineEdit* mLineEdit;
};



// TODO create pop-upable textbox where the user enteres and removes lines
// string array attribute widget
class QTBASE_API StringArrayAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		StringArrayAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnStringChange();
	private:
		QLineEdit* mLineEdit;
};


// text attribute widget
class QTBASE_API TextAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		TextAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnStringChange();
	private:
		QPlainTextEdit* mPlainTextEdit;
};


//
// TODO add channel and electrode selection widgets that uses AttributeStringArray
//

// color attribute widget
class QTBASE_API ColorAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		ColorAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnColorChanged();
	private:
		ColorLabel* mColorLabel;
};


// button attribute widget
class QTBASE_API ButtonAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		ButtonAttributeWidget(const char* buttonText, const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		void SetValue(Core::Attribute* attribute) override	{}
		QPushButton* GetButton() const						{ return mButton; }
	protected slots:
		void OnReleased ();
	private:
		QPushButton*		mButton;
};


// color mapping attribute widget
class QTBASE_API ColorMappingAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		ColorMappingAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		~ColorMappingAttributeWidget();
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnComboBox(int value);
	private:
		QComboBox*				mComboBox;
		ColorMapper*			mColorMapper;
		ColorMappingWidget*		mColorMappingWidget;
};


// window function attribute widget
class QTBASE_API WindowFunctionAttributeWidget : public AttributeWidget
{
	Q_OBJECT
	public:
		WindowFunctionAttributeWidget(const Core::Array<Core::Attribute*> attributes, Core::AttributeSettings* attributeSettings, void* customData, bool readOnly, bool creationMode=false);
		~WindowFunctionAttributeWidget();
		void SetValue(Core::Attribute* attribute) override;
	protected slots:
		void OnComboBox(int value);
	private:
		QComboBox*				mComboBox;
		WindowFunction*			mWindowFunction;
		WindowFunctionWidget*	mWindowFunctionWidget;
};


#endif
