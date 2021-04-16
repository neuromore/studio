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

#ifndef __NEUROMORE_BRAINALIVESERIALHANDLER_H
#define __NEUROMORE_BRAINALIVESERIALHANDLER_H

// include required headers
#include <Config.h>
#include "BrainaliveCommands.h"
#include <System/SerialPort.h>
#include "../../../../build/vs/BrainaliveDevices.h"
#include <Device.h>
#include <QObject>
#include <QThread>
#include <QTimer>

#include <Core/Timer.h>

#ifdef INCLUDE_DEVICE_BRAINALIVE

class BrainAliveDevice;

// separate QObject class, just for handling the data (due to virtual function pointer table bullshit)
class BrainAliveSerialHandler : public QObject
{
	Q_OBJECT
public:
	BrainAliveSerialHandler(SerialPort* port, BrainAliveDeviceBase* headset, QObject* parent);
	virtual ~BrainAliveSerialHandler();

	bool Connect();

public slots:
	void ReadStream();
	void Disconnect();

private:
	// serial commands
	bool ConfigureChannels(const Device::DeviceConfig& config);
	bool ReadChannelConfig(Core::String& outConfig);
	bool SendChannelCommands(uint32 index, bool enabled, uint32 gain, uint32 inputType, bool bias, bool srb2, bool srb1);
	bool SendChannelPowerCommands(uint32 index, bool enabled);

	bool SendStartCommand();
	bool SendStopCommand();
	bool SendResetCommand();

	// process data from an incoming stream packet
	void ProcessStreamPacket(const BrainAliveStreamPacket& packet);
	bool ReadStreamPacket(BrainAliveStreamPacket* data);

private:
	BrainAliveDeviceBase* mDevice;
	SerialPort* mSerialPort;
	bool					mIsConnected;
	QTimer* mTimer;

	uint32					mLastPacketIndex;
	double					mSampleRate;

	BrainAliveStreamPacket		mStreamPacket;
	Core::Array<uint32>		mElectrodeGainSettings;
	double                  mAccValueLeft;
	double                  mAccValueUp;
	double                  mAccValueForward;

	uint32					mNumPackets;

};

#endif

#endif
