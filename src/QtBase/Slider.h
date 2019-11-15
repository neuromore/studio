/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_INTSLIDER_H
#define __QTBASE_INTSLIDER_H

#include <Core/StandardHeaders.h>
#include "QtBaseConfig.h"
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include "Spinbox.h"


class QTBASE_API IntSlider : public QWidget
{
	Q_OBJECT
	public:
		enum Mode
		{
			SPINNER = 0,
			LABEL	= 1
		};

		// constructor & destructor
		IntSlider(QWidget* parent=NULL, Mode mode=SPINNER);
		virtual ~IntSlider();

		void SetMode(Mode mode);

		void SetEnabled(bool isEnabled);
		void SetValue(int value);
		void SetRange(int min, int max);
		void SetSingleStep(int delta)							{ mSpinBox->setSingleStep(delta); }
		void BlockSignals(bool flag);

		inline int GetValue() const								{ return mSlider->value(); }
		inline QSlider* GetSlider() const						{ return mSlider; }
		inline IntSpinBox* GetSpinBox() const					{ return mSpinBox; }

	signals:
		void ValueChanged(int value);
		void ValueChanged();
		void FinishedValueChange(int value);

	private slots:
		void OnSpinBoxChanged(double value);
		void OnSliderChanged(int value);
		void OnSliderReleased();

	private:
		QSlider*		mSlider;

		Mode			mMode;
		QLabel*			mLabel;
		IntSpinBox*		mSpinBox;
};



class QTBASE_API FloatSlider : public QWidget
{
	Q_OBJECT
	public:
		enum Mode
		{
			SPINNER = 0,
			LABEL	= 1
		};

		// constructor & destructor
		FloatSlider(QWidget* parent=NULL, Mode mode=SPINNER);
		virtual ~FloatSlider();

		void SetMode(Mode mode);

		void SetEnabled(bool isEnabled);
		void SetValue(double value);
		void SetRange(double min, double max);
		void SetSingleStep(double delta)							{ mSpinBox->setSingleStep(delta); }
		void BlockSignals(bool flag);

		inline double GetValue() const							{ return mSpinBox->value(); }
		inline QSlider* GetSlider() const						{ return mSlider; }
		inline SpinBox* GetSpinBox() const						{ return mSpinBox; }
		inline bool IsPressed()									{ return mSlider->isSliderDown(); }

	signals:
		void ValueChanged(double value);
		void FinishedValueChange(double value);

	private slots:
		void OnSpinBoxChanged(double value);
		void OnSliderChanged(int value);
		void OnSliderReleased();

	private:
		void SetLabelValue(double value);
		QSlider*			mSlider;

		double Round(double x);

		Mode				mMode;
		QLabel*				mLabel;
		SpinBox*			mSpinBox;
};


#endif
