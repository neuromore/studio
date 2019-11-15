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

#ifndef __NEUROMORE_PARAMETERWIDGETS_H
#define __NEUROMORE_PARAMETERWIDGETS_H

// include required headers
#include "../../Config.h"
#include <Device.h>
#include <Slider.h>
#include <Graph/ParameterNode.h>
#include <QCheckBox>
#include <QComboBox>
#include <QBoxLayout>
#include <QPushButton>


class ParameterWidget : public QWidget
{
	Q_OBJECT
	public:

		enum EWidgetPosition
		{
			POSITION_BELOW,
			POSITION_LEFT,
			POSITION_RIGHT,
		};

		ParameterWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL);
		virtual ~ParameterWidget()										{}

		virtual uint32 GetType() const = 0;

		ParameterNode* GetParameterNode() const							{ return mParameterNode; }

		// update widget
		virtual void UpdateInterface();

		// set node parameter from control widget
		virtual void ApplyParameter()									{}

		// was the node modified?
		virtual bool HasNodeChanged() const;
		virtual void UpdateFromNode(ParameterNode* node = NULL);


	protected:
		virtual void Init(EWidgetPosition position) = 0;
		virtual void Init(QWidget* widget, EWidgetPosition = POSITION_BELOW);

		ParameterNode*	mParameterNode;

		QLabel*			mLabel;

		double			mDefaultValue;
};




class ParameterSeparatorWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x1337 }; 

		ParameterSeparatorWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterSeparatorWidget()							{}
		
		uint32 GetType() const override final				{ return TYPE_ID; }

	private slots:
		void OnCheckBoxValueChanged(int state)				{ ApplyParameter(); }

	private:
		void Init(EWidgetPosition position) override final;

		QWidget*    mLineWidget;
};




class ParameterSliderWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x001 }; 

		ParameterSliderWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterSliderWidget()							{}

		uint32 GetType() const override final				{ return TYPE_ID; }

		void UpdateInterface() override final;

		void ApplyParameter() override final				{ mParameterNode->SetValue(mSlider->GetValue()); }
		
		bool HasNodeChanged() const override final;
		void UpdateFromNode(ParameterNode* node) override final;
		
	private slots:
		void OnSliderValueChanged(double value)				{ ApplyParameter(); }

	private:
		void Init(EWidgetPosition position) override final;

		FloatSlider*    mSlider;

		double			mRangeMin;
		double			mRangeMax;
};



class ParameterCheckBoxWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x002 }; 

		ParameterCheckBoxWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterCheckBoxWidget()							{}
		
		uint32 GetType() const override final				{ return TYPE_ID; }

		void ApplyParameter() override final;
	
		bool HasNodeChanged() const override final;
		
		void UpdateFromNode(ParameterNode* node) override final;

	private slots:
		void OnCheckBoxValueChanged(int state);

	private:
		void Init(EWidgetPosition position) override final;

		QHBoxLayout*			mCheckboxLayout;
		Core::Array<QCheckBox*> mCheckboxes;
};



class ParameterComboBoxWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x003 }; 

		ParameterComboBoxWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterComboBoxWidget()							{}
		
		uint32 GetType() const override final				{ return TYPE_ID; }

		void ApplyParameter() override final				{ mParameterNode->SetValue( (double)mComboBox->currentIndex() ); }

		bool HasNodeChanged() const override final;

		void UpdateFromNode(ParameterNode* node) override final;
		
	private slots:
		void OnComboBoxSelectionChanged(int state)			{ ApplyParameter(); }

	private:
		void Init(EWidgetPosition position) override final;

		QComboBox*    mComboBox;

};



class ParameterSelectionButtonWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x004 }; 

		ParameterSelectionButtonWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterSelectionButtonWidget()					{}
		
		uint32 GetType() const override final				{ return TYPE_ID; }

		void ApplyParameter() override final;

		bool HasNodeChanged() const override final;

		void UpdateFromNode(ParameterNode* node) override final;
		
	private slots:
		void OnButtonClicked();

	private:
		void Init(EWidgetPosition position) override final;

		QHBoxLayout*			  mButtonLayout;
		Core::Array<QPushButton*> mButtons;
};



class ParameterEventButtonWidget : public ParameterWidget
{
	Q_OBJECT
	public:
		enum { TYPE_ID = 0x005 }; 

		ParameterEventButtonWidget(ParameterNode* node, EWidgetPosition position = POSITION_BELOW, QWidget* parent = NULL) : ParameterWidget(node, position, parent)		{ Init(position); }
		~ParameterEventButtonWidget()						{}
		
		uint32 GetType() const override final				{ return TYPE_ID; }

		bool HasNodeChanged() const override final;

		void UpdateFromNode(ParameterNode* node) override final;
		
	private slots:
		void OnButtonClicked();

	private:
		void Init(EWidgetPosition position) override final;

		QHBoxLayout*			  mButtonLayout;
		Core::Array<QPushButton*> mButtons;
};



#endif
