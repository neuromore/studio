/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_LAYOUTCOMBOBOX_H
#define __QTBASE_LAYOUTCOMBOBOX_H

// include required headers
#include "QtBaseConfig.h"
#include <QComboBox>


class QTBASE_API LayoutComboBox : public QComboBox
{
	Q_OBJECT
	public:
		LayoutComboBox(QWidget* parent=NULL);
		virtual ~LayoutComboBox();

		void ReInit();

		void SilentChangeCurrentIndex(int newIndex);

	public slots:
		void OnCurrentIndexChanged(int index);

	private:
		QComboBox* mComboBox;
};


#endif
