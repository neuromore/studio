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

#ifndef __NEUROMORE_HEARTRATEWIDGET_H
#define __NEUROMORE_HEARTRATEWIDGET_H

// include required headers
#include "../Config.h"
#include <Core/StandardHeaders.h>
#include <Core/String.h>
#include "SwitchingImage.h"
#include <QWidget>
#include <QCheckBox>


class HeartRateWidget : public QWidget
{
	Q_OBJECT
	public:
		// constructor & destructor
		HeartRateWidget(QWidget* parent=NULL);
		virtual ~HeartRateWidget();

		void SetHeartRate(int heartRateBPM, int HRVinMS);

	private:
		SwitchingImage* mHeart;
		Core::String	mHeartRateString;
		QLabel*			mHeartRateLabel;
		Core::String	mHRVString;
		QLabel*			mHRVLabel;
};


#endif
