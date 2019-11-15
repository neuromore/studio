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

#ifndef THERMOMETER_H
#define THERMOMETER_H

// include required headers
#include "../Config.h"
#include <Core/StandardHeaders.h>
#include <QWidget>


class ThermometerWidget : public QWidget
{
	Q_OBJECT
	public:
		ThermometerWidget(QWidget *parent=NULL);
		virtual ~ThermometerWidget();

		inline double GetMin() const									{ return mMin; }
		inline double GetMax() const									{ return mMax; }
		inline double GetValue() const									{ return mValue;}
		inline double GetDigitsOffset() const							{ return mDigitsOffset; }
		inline QFont GDigitsFont() const								{ return mDigitsFont; }

		void SetMin(double value);
		void SetMax(double value);
		void setDigitOffset(double offset)								{ mDigitsOffset = offset; updateWithBackground();}
		void setDigitFont(QFont font)									{ mDigitsFont = font; updateWithBackground();  }

	protected:
		void paintEvent(QPaintEvent *event);
		void paintBackground(QPainter & painter);
		void initCoordinateSystem(QPainter & painter);

	public:
		void drawBackground ();
		void updateWithBackground ();
		bool doRepaintBackground();
		void repaintBackground();

	public slots:
		void SetValue(double val); 
	signals:
		void ValueChanged(double val);

protected:
		bool calcMaxMin();

		double			mStartValue;		/** Starting value on meter  this value is less than mMin */			
		double			mEndValue;			/** Endgind value on meter this value is more than mMax*/
		double			mMin;				/** Minimum that has to be on scale */
		double			mMax;				/** Maximum that has to be on scale */
		double			mValue;				/** Current value */
		double			mDigitsOffset;		/** Used to place scale digits offset. On manometer distance from the center on thermometer distance form left */
		QFont			mDigitsFont;		/** Font used to display scale digit/numbers */
		QPixmap*		mPixmap;
		bool			mModified;

		QColor			mWidgetBackgroundColor;
		QColor			mQuicksilverColor;
		QColor			mBorderColor;
		QColor			mTextColor;
		QColor			mInnerColor;
};


#endif

