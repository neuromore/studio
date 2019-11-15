/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_COLORLABEL_H
#define __NEUROMORE_COLORLABEL_H

// include required headers
#include "QtBaseConfig.h"
#include <QColor>
#include <QLabel>
#include <QColorDialog>


class QTBASE_API ColorLabel : public QLabel
{
	Q_OBJECT
	public:
		ColorLabel(QWidget* parent, const QColor& defaultColor);
		~ColorLabel();

		QColor GetColor() const							{ return mColorDialog->currentColor(); }

	signals:
		void ColorChangeEvent(const QColor& color);
		void ColorChangeEvent();

	public slots:
		void OnColorChanged(const QColor& qColor);

	protected:
		void mousePressEvent(QMouseEvent* event)		{ mColorDialog->open(); }
		void UpdateInterface();

		QColorDialog*	mColorDialog;
};


#endif
