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

#ifndef __NEUROMORE_BATTERYSTATUSWIDGET_H
#define __NEUROMORE_BATTERYSTATUSWIDGET_H

// include required headers
#include "../Config.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include <Core/Array.h>
#include <QWidget>
#include <QLabel>


class BatteryStatusWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		BatteryStatusWidget(QWidget* parent=NULL);
		virtual ~BatteryStatusWidget();

		// discrete levels
		enum EStatus
		{
			STATUS_UNKNOWN,			// display unknown icon
			STATUS_EMPTY,			// blinking empty
			STATUS_CRITICAL,		// critical state (single red bar)
			STATUS_OK				// 2-4 bars
		};

		
		// set status using a critical threshold
		void SetStatus(double normalizedLevel, double criticalLevel = 0.25);
	
		// set level and its interpretation
		void SetStatus(double normalizedLevel, EStatus status);
	

	private:

		// image indices for mPixmaps array
		enum EPixmaps
		{
			PIXMAP_UNKNOWN  = 0,
			PIXMAP_STROKE   = 1,
			PIXMAP_0		= 2,
			PIXMAP_25		= 3,
			PIXMAP_50		= 4,
			PIXMAP_75		= 5,
			PIXMAP_100		= 6,

			// blinking empty
			PIXMAP_EMPTY1   = PIXMAP_0,
			PIXMAP_EMPTY2   = PIXMAP_STROKE,

			// blinking critical
			PIXMAP_CRITICAL1 = PIXMAP_25,
			PIXMAP_CRITICAL2 = PIXMAP_0,

		};

		void AddPixmap(QString filename);

		EStatus					mBatteryStatus;
		double					mBatteryLevel;
		int32					mPixmapHeight;
		Core::String			mTempString;
		QLabel*					mTextLabel;			// displays battery level
		QLabel*					mPixmapLabel;		// displays image
		Core::Array<QPixmap>	mPixmaps;			// all battery images

		QTimer*					mAnimationTimer;
		bool					mAnimationState;	// flipflop state for blinking empty symbol

	private slots:
		void OnTimerEvent();
	
};


#endif
