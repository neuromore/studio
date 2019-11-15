/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_COLORMAPPINGWIDGET_H
#define __NEUROMORE_COLORMAPPINGWIDGET_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/StandardHeaders.h>
#include <ColorMapper.h>
#include <QWidget>


class QTBASE_API ColorMappingWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		ColorMappingWidget(ColorMapper* colorMapper, QWidget* parent=NULL);
		virtual ~ColorMappingWidget();

		void paintEvent(QPaintEvent* event);
	private:
		ColorMapper*		mColorMapper;
};


#endif
