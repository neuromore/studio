/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

 // include precompiled header
#include <QtBase/Precompiled.h>

// include the required headers
#include "ImageButton.h"
#include "ImageManipulation.h"
#include <Core/Timer.h>
#include <Core/LogManager.h>
#include <QPainter>
#include "QtBaseManager.h"
#include <QFile>


using namespace Core;

// constructor
ImageButton::ImageButton(QWidget* parent) : QPushButton(parent)
{
	mState = STATE_UNDEFINED;
	mIsHovered = false;
}


// constructor
ImageButton::ImageButton(const char* imageFilename, QWidget* parent) : QPushButton(parent)
{
	mState = STATE_UNDEFINED;
	mIsHovered = false;

	InitImages(imageFilename);
}


// constructor
ImageButton::ImageButton(const char* imageFilename, int size, const char* tooltip, Type type, QWidget* parent) : QPushButton(parent)
{
	mState = STATE_UNDEFINED;
	mIsHovered = false;

	setMinimumSize(size, size);
	setMaximumSize(size, size);

	setToolTip(tooltip);
	InitImages(imageFilename, type);
}


// destructor
ImageButton::~ImageButton()
{
}


void ImageButton::InitImages(QString imageFilename, Type type)
{
#ifndef PRODUCTION_BUILD
	Core::Timer timer;
#endif

	QString filename = imageFilename;

	// 1) load from resources
	QString resFilename = ":/" + QString(imageFilename);
	if (QFile::exists(resFilename) == true)
		filename = resFilename;

	// 2) load from application image directory
	else
	{
		QString diskFilename = GetQtBaseManager()->GetAppDir() + imageFilename;
		if (QFile::exists(diskFilename) == true)
			filename = diskFilename;
	}

	setObjectName("ImageButton");

	QPixmap normalPixmap(filename);

	mNormal		= normalPixmap;
	mHovered	= CreateHoverPixmap(normalPixmap, type);
	mDisabled	= CreateDisabledPixmap(normalPixmap, type);

	UpdateInterface();

#ifndef PRODUCTION_BUILD
	const double time = timer.GetTime().InMilliseconds();
	LogDebug( "ImageButton::SetImage('%s') took %.2f ms", imageFilename.toLatin1().data(), time );
#endif
}


QPixmap ImageButton::CreateHoverPixmap(const QPixmap& pixmap, Type type)
{
	switch (type)
	{
		case TYPE_STDICON:
		{
			QImage image = pixmap.toImage();
			QImage newImage = ChangeBrightness(image, 20);

			//LogInfo("pixmap.size()=%i, %i", pixmap.size().width(), pixmap.size().height());
			QPixmap result(pixmap.size());
			//result.fill(Qt::transparent);
			result.convertFromImage(newImage);
			return result;
		}

		default:
		{
			QPixmap result(pixmap.size());
			result.fill(Qt::transparent);

			QPainter painter(&result);
			painter.setOpacity(0.2);
			painter.drawPixmap(0, 0, pixmap);
			painter.end();

			return result;
		}
	}
}


QPixmap ImageButton::CreateDisabledPixmap(const QPixmap& pixmap, Type type)
{
	switch (type)
	{
		case TYPE_STDICON:
		{
			QImage image = pixmap.toImage();
			QImage newImage = ChangeBrightness(image, -40);

			//LogInfo("pixmap.size()=%i, %i", pixmap.size().width(), pixmap.size().height());
			QPixmap result(pixmap.size());
			//result.fill(Qt::transparent);
			result.convertFromImage(newImage);
			return result;
		}

		default:
		{
			return ChangeOpacity(pixmap, 0.1f);
		}
	}
}


// on mouse enter
void ImageButton::enterEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = true;
	UpdateInterface();
}


// on mouse leave
void ImageButton::leaveEvent(QEvent* event)
{
	QWidget::enterEvent( event );
	mIsHovered = false;
	UpdateInterface();
}


void ImageButton::resizeEvent(QResizeEvent* event)
{
	UpdateInterface();
}


void ImageButton::UpdateInterface()
{
	State oldState = mState;
	QSize oldSize = mSize;

	const bool isEnabled = QPushButton::isEnabled();
	if (isEnabled == true)
	{
		if (mIsHovered == true)
			mState = STATE_ENABLED_HOVERED;
		else
			mState = STATE_ENABLED;
	}
	else
	{
		if (mIsHovered == true)
			mState = STATE_DISABLED_HOVERED;
		else
			mState = STATE_DISABLED;
	}


	// update size
	mSize = QSize( width(), height() );
	if (mSize != oldSize)
	{
		setIconSize( mSize );
		//LogDebug( "Updating icon size to (%i, %i)", mSize.width(), mSize.height() );
	}


	// state changed?
	if (oldState != mState)
	{
		//LogDebug( "Updating icon from state %i to state %i", oldState, mState );

		switch (mState)
		{
			case STATE_ENABLED:				{ setIcon(mNormal); break;}
			case STATE_ENABLED_HOVERED:		{ setIcon(mHovered); break; }
			default:						{ setIcon(mDisabled); break; }
		}
	}
}


void ImageButton::changeEvent(QEvent* event)
{
	QPushButton::changeEvent(event);

	if (event->type() == QEvent::EnabledChange || event->type() == QEvent::Show)
		UpdateInterface();
}
