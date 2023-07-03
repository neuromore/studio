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

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "SessionInfoWidget.h"

using namespace Core;

#define AUTOREMOVE_TIMER_INTERVAL 0.1
#define AUTOREMOVE_FADE_INTERVAL 1
#define AUTOREMOVE_DURATION 7.0


SessionInfoWidget::SessionInfoWidget(QWidget* parent) : QWidget(parent)
{
	setObjectName("TransparentWidget");

	mGridLayout = new QGridLayout();
	mGridLayout->setColumnStretch(1,100);
	mGridLayout->setAlignment(Qt::AlignTop);
	setLayout(mGridLayout);

	// load icons
	const uint32 iconSize = 15;
	mIcons.Resize(NUM_TYPES);
	mIcons[TYPE_INFO]	 = QPixmap(":/Images/Icons/Info.png").scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mIcons[TYPE_WARNING] = QPixmap(":/Images/Icons/Warning.png").scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	mIcons[TYPE_ERROR]	 = QPixmap(":/Images/Icons/Error.png").scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	// timer
	mTimer = new QTimer();
	mTimer->setInterval(1000.0 * AUTOREMOVE_TIMER_INTERVAL);
	mTimer->start();
	connect (mTimer, &QTimer::timeout, this, &SessionInfoWidget::OnTimerEvent);

	UpdateLayout();

	CORE_EVENTMANAGER.AddEventHandler(this);
}


SessionInfoWidget::~SessionInfoWidget()
{
	CORE_EVENTMANAGER.RemoveEventHandler(this);
}


void SessionInfoWidget::ShowInfo (EInfoType type, const char* message, const char* description, double duration)
{
	// allow every message only once
	if (HasInfo(message) == true)
		return;

	mInfos.AddEmpty();
	Info& info = mInfos.GetLast();
	info.mType = type;
	info.mMessage = message;
	info.mDescription = description;
	info.mIsFading = false;

	if (duration > 0)
	{
		info.mAutoRemove = true;
		info.mTimeRemaining = duration;
	}
	else
	{
		info.mAutoRemove = false;
		info.mTimeRemaining = -1.0;
	}

	info.mIcon = new QLabel();
	info.mIcon->setPixmap( GetIcon(type) );
		
	// info label
	info.mLabel = new QLabel();
	//tempString.Format("<p style=\"color:%s\">%s</p><br>%s", GetHtmlColor(type), info.mMessage.AsChar(), info.mDescription.AsChar());
	//label->setText(tempString.AsChar());
	info.mLabel->setText(info.mMessage.AsChar());
	info.mLabel->setToolTip( info.mDescription.AsChar() );

	UpdateLayout();
}


bool SessionInfoWidget::HasInfo(const char* message)
{
	// try to remove first entry
	const uint32 numInfos = mInfos.Size();
	for (uint32 i=0; i<numInfos; ++i)
		if (mInfos[i].mMessage.IsEqual(message) == true)
			return true;

	return false;
}


void SessionInfoWidget::RemoveInfo(const char* message)
{
	// try to remove first entry
	const uint32 numInfos = mInfos.Size();
	for (uint32 i=0; i<numInfos; ++i)
	{
		if (mInfos[i].mMessage.IsEqual(message) == true)
		{
			// start fadeout; item will be removed by the timer
			if (mInfos[i].mTimeRemaining <= 0)
			{
				mInfos[i].mTimeRemaining = AUTOREMOVE_FADE_INTERVAL;
				mInfos[i].mAutoRemove = true;
			}
		}
	}
}


void SessionInfoWidget::Clear()
{
	const uint32 numInfos = mInfos.Size();
	for (uint32 i=0; i<numInfos; ++i)
	{
		delete mInfos[i].mIcon;
		delete mInfos[i].mLabel;
	}

	mInfos.Clear();

	UpdateLayout();
}


void SessionInfoWidget::RemoveAt(uint32 index)
{
	// delete widgets
	delete mInfos[index].mIcon;
	delete mInfos[index].mLabel;

	mInfos.Remove(index);

	UpdateLayout();
}


void SessionInfoWidget::OnTimerEvent()
{
	// collect entries we have to remove
	Array<uint32> infosToRemove;
	const uint32 numInfos = mInfos.Size();
	for (uint32 i=0; i<numInfos; ++i)
	{
		Info& info = mInfos[i];

		// skip the static items
		if (info.mAutoRemove == false)
			continue;

		if (info.mTimeRemaining > 0)
		{
			// decrease time or remove 
			info.mTimeRemaining -= (double)AUTOREMOVE_TIMER_INTERVAL;

			// start fade out effect
			if (info.mTimeRemaining <= (double)AUTOREMOVE_FADE_INTERVAL && info.mIsFading == false)
			{
				info.mIsFading = true;

				QGraphicsOpacityEffect *effIcon = new QGraphicsOpacityEffect(this);
				QGraphicsOpacityEffect *effLabel = new QGraphicsOpacityEffect(this);
				info.mIcon->setGraphicsEffect(effIcon);
				info.mLabel->setGraphicsEffect(effLabel);

				QPropertyAnimation * animIcon = new QPropertyAnimation(effIcon, "opacity");
				animIcon->setDuration(1000.0 * (double)AUTOREMOVE_FADE_INTERVAL);
				animIcon->setStartValue(1);
				animIcon->setEndValue(0);
				animIcon->setEasingCurve(QEasingCurve::OutBack);
				animIcon->start(QPropertyAnimation::DeleteWhenStopped);

				QPropertyAnimation * animLabel = new QPropertyAnimation(effLabel, "opacity");
				animLabel->setDuration(1000.0 * (double)AUTOREMOVE_FADE_INTERVAL);
				animLabel->setStartValue(1);
				animLabel->setEndValue(0);
				animLabel->setEasingCurve(QEasingCurve::OutBack);
				animLabel->start(QPropertyAnimation::DeleteWhenStopped);
			}
		}
	}

	// remove all timed out infos
	bool removed = true;
	while (removed == true)
	{
		removed = false;
		for (uint32 j=0; j<numInfos; ++j)
			if (mInfos[j].mAutoRemove == true && mInfos[j].mTimeRemaining <= 0)
				RemoveAt( j );
	}
}


// function for sorting mInfos array by infotype in ascending order (Error -> Warning -> info)
template <class T>
static int32 CORE_CDECL InfoTypeAscendingCompare(const T& itemA, const T& itemB)
{
	if (itemA.mType < itemB.mType) return 1;
	else if (itemA.mType == itemB.mType) return 0;
	else return -1;
}


void SessionInfoWidget::UpdateLayout()
{
	// sort the list so errors are on the top, followed by warnings
	mInfos.Sort(InfoTypeAscendingCompare);

	hide();

	// clear the layout
	QLayoutItem *layoutItem;
	while ((layoutItem = mGridLayout->takeAt(0)) != 0)
		delete layoutItem;
	
	String tempString;
	
	// create new layout
	const uint32 numInfos = mInfos.Size();
	for (uint32 i=0; i<numInfos; ++i)
	{
		const Info& info = mInfos[i];

		// add to row i
		mGridLayout->addWidget(info.mIcon, i, 0);
		mGridLayout->addWidget(info.mLabel, i, 1);
	}

	if (numInfos > 0)
		show();
}


const char* SessionInfoWidget::GetHtmlColor(EInfoType type)
{
	switch (type)
	{
		case TYPE_INFO:		return "blue";
		case TYPE_WARNING:	return "yellow";
		case TYPE_ERROR:	return "red";
		default:			return "white";
	}
}


const char* SessionInfoWidget::GetName(EInfoType type)
{
	switch (type)
	{
		case TYPE_INFO:		return "Info";
		case TYPE_WARNING:	return "Warning";
		case TYPE_ERROR:	return "Error";
		default:			return "";
	}
}
