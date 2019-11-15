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

#ifndef __NEUROMORE_NEUROSKYSERIALHANDLER_H
#define __NEUROMORE_NEUROSKYSERIALHANDLER_H

// include required headers
#include <Config.h>
#include <Devices/NeuroSky/NeuroSkyDevice.h>
#include <QObject>
#include <QThread>
#include <QTimer>

#ifdef INCLUDE_DEVICE_NEUROSKY_MINDWAVE

#include <NeuroSky/thinkgear.h>


// Neurosky serial handler: receive serial data using the neurosky api
class NeuroSkySerialHandler : public QObject
{
	Q_OBJECT
	public:
		NeuroSkySerialHandler(NeuroSkyDevice* headset, int connectionID, QObject* parent);
		~NeuroSkySerialHandler() {}

	public slots:
		void ReadStream();

	private:
		NeuroSkyDevice*		mHeadset;
		int					mConnectionID;
		QTimer*				mTimer;
};

#endif

#endif
