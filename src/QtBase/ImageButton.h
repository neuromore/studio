/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_IMAGEBUTTON_H
#define __QTBASE_IMAGEBUTTON_H

#include <Core/StandardHeaders.h>
#include "QtBaseConfig.h"
#include <QPushButton>


#define IMAGEBUTTONSIZE_BIG 64

class QTBASE_API ImageButton : public QPushButton
{
	Q_OBJECT
	public:
		enum Type
		{
			TYPE_STDICON = 0,
			TYPE_TILE = 1
		};

		// constructor & destructor
		ImageButton(QWidget* parent=NULL);
		ImageButton(const char* imageFilename, QWidget* parent=NULL);
		ImageButton(const char* imageFilename, int size, const char* tooltip, Type type=TYPE_STDICON, QWidget* parent=NULL);
		virtual ~ImageButton();

		void InitImages(QString imageFilename, Type type=TYPE_STDICON);

		void UpdateInterface();

		static QPixmap CreateHoverPixmap(const QPixmap& pixmap, Type type);
		static QPixmap CreateDisabledPixmap(const QPixmap& pixmap, Type type);

	private:
		enum State
		{
			STATE_UNDEFINED,
			STATE_ENABLED,
			STATE_ENABLED_HOVERED,
			STATE_DISABLED,
			STATE_DISABLED_HOVERED
		};

		void changeEvent(QEvent* event) override;
		void enterEvent(QEvent* event) override;
		void leaveEvent(QEvent* event) override;
		void resizeEvent(QResizeEvent* event) override;

		State	mState;
		QSize	mSize;
		bool	mIsHovered;
		QIcon	mNormal;
		QIcon	mHovered;
		QIcon	mDisabled;
};


#endif
