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

#ifndef __NEUROMORE_VERSUSSERIALHANDLER_H
#define __NEUROMORE_VERSUSSERIALHANDLER_H


// include required headers
#include <Config.h>
#include "VersusCommands.h"

#include <System/SerialPort.h>
#include <QObject>
#include <QThread>
#include <QTimer>

#include <Core/Timer.h>

class VersusDevice;

// separate QObject class, just for handling the data (due to virtual function pointer table bullshit)
class VersusSerialHandler : public QObject
{
	Q_OBJECT
public:
	VersusSerialHandler(SerialPort* port, VersusDevice* headset, QObject* parent);
	virtual ~VersusSerialHandler();

	bool Connect();

	// construct and send a command, with one or two parameters
	bool SendCommand(EVersusCommand command, uint32 numArgs = 0, uint32 arg1 = 0, uint32 arg2 = 0, char address = '0');

	// process data from an incoming stream packet
	void ProcessStreamPacket(const VersusStreamPacket& packet);

	public slots:
	void ReadStream();
	void Disconnect();

private:
	VersusDevice*			mHeadset;
	SerialPort*				mSerialPort;
	bool					mIsConnected;
	QTimer*					mTimer;

	uint32					mLastPacketIndex;
	double					mSampleRate;

	VersusStreamPacket		mStreamPacket;

	// command and stream read/write functions
	bool WriteCommandPacket(VersusCommandPacket* packet);			// write one command data packet
	bool ReadCommandPacket(VersusCommandPacket* inOutPacket);		// read one command data packet
	bool ReadStreamPacket(VersusStreamPacket* inOutPacket);			// read one stream data packet

};

#endif
