/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_SPINBOX_H
#define __QTBASE_SPINBOX_H

#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "QtBaseConfig.h"
#include <QLineEdit>
#include <QToolButton>
#include <QValidator>


// forward declaration
class SpinBox;

class QTBASE_API DoubleValidator : public QValidator
{
	Q_OBJECT
	public:
		DoubleValidator(QObject* parent);
		virtual ~DoubleValidator();

		QValidator::State validate(QString& input, int& pos) const;

	private:
		QDoubleValidator* mValidator;
};


class QTBASE_API SpinboxButton : public QToolButton
{
	Q_OBJECT
	public:
		SpinboxButton(QWidget* parent, SpinBox* spinbox, bool leftButton);
		virtual ~SpinboxButton();

	public slots:
		void ClickButton();

	private:
		void OnUpButtonClicked();
		void OnDownButtonClicked();

		SpinBox*	mSpinbox;
		bool		mLeftButton;
};


class QTBASE_API SpinboxLineEdit : public QLineEdit
{
	Q_OBJECT
	public:
		SpinboxLineEdit(QWidget* parent, SpinBox* spinbox);
		virtual ~SpinboxLineEdit();

		void focusOutEvent(QFocusEvent* event);
		void mouseDoubleClickEvent(QMouseEvent* event);

	private:
		SpinBox*		mSpinbox;
		Core::String	mTemp;
};


class QTBASE_API SpinBox : public QWidget
{
	Q_OBJECT
	public:
		enum Type
		{
			DOUBLE = 0,
			INTEGER= 1
		};

		SpinBox(QWidget* parent=NULL, Type type=DOUBLE);
		virtual ~SpinBox();

		void setDecimals(int numDecimals)							{ mNumDecimals = numDecimals; Update(); }
		void setSingleStep(double deltaValue)						{ mSingleStep = deltaValue; }
		void setValue(double value, bool rangeCheck=true, bool update=true);
		void setRange(double min, double max)						{ mMinimum = min; mMaximum = max; Update(); }
		void setSuffix(const char* suffix)							{}

		double value() const										{ return mValue; }
		double minimum() const										{ return mMinimum; }
		double maximum() const										{ return mMaximum; }
		double singleStep() const									{ return mSingleStep; }

		Type GetType() const										{ return mType; }

		QLineEdit* GetLineEdit() const								{ return mLineEdit; }

		void Update();

		void EmitValueChangedSignal()								{ emit valueChanged(mValue); }

		void resizeEvent(QResizeEvent* event);

	signals:
		void valueChanged(double d);
		void valueChanged(const QString & text);

	private slots:
		void OnEditingFinished();
		void OnTextEdited(const QString& newText);

	protected:
		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);

		Type			mType;
		double			mValue;
		double			mMinimum;
		double			mMaximum;
		double			mSingleStep;
		int				mNumDecimals;
		Core::String	mText;
		Core::String	mTemp;

		QLineEdit*		mLineEdit;
		SpinboxButton*	mUpButton;
		SpinboxButton*	mDownButton;
};


class QTBASE_API IntSpinBox : public SpinBox
{
	public:
		IntSpinBox(QWidget* parent=NULL) : SpinBox(parent, SpinBox::INTEGER) {}
		~IntSpinBox() {}
};


#endif
