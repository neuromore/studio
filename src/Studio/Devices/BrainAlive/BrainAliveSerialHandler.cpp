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

// include required files
#include "./BrainAliveSerialHandler.h"
#include "../Studio/Devices/BrainAlive/BrainAliveBluetooth.h"
#include <EngineManager.h>
#include <QCoreApplication>
#include <QTimer>
#include <chrono>

#ifdef INCLUDE_DEVICE_BRAINALIVE

using namespace Core;
using namespace std::chrono;

BLEInterface* mBLE_Interface;


// constructor
BrainAliveSerialHandler::BrainAliveSerialHandler(BrainAliveDeviceBase* headset, QObject* parent) : QObject(parent)
{

	mDevice = headset;

	// default gain values
	mElectrodeGainSettings.Resize(16);
	for (uint32 i = 0; i < 16; ++i)
		mElectrodeGainSettings[i] = 24; // BrainAlive default

	mLastPacketIndex = 0;
	mNumPackets = 0;
	mSampleRate = mDevice->GetSampleRate();

	mTimer = new QTimer();
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(ReadStream()));
	mTimer->setTimerType(Qt::PreciseTimer);
    mTimer->setInterval(1000 / mSampleRate);

	mIsConnected = false;

	mAccValueLeft = 0.0;
	mAccValueUp = 0.0;
	mAccValueForward = 0.0;
}


// destructor
BrainAliveSerialHandler::~BrainAliveSerialHandler()
{

}


bool BrainAliveSerialHandler::Connect()
{
	// start data receive thread
	mTimer->start();
	return true;
}


// disconnect from BrainAlive
void BrainAliveSerialHandler::Disconnect()
{
	mIsConnected = false;
}

void BrainAliveSerialHandler::ProcessStreamPacket(const BrainAliveStreamPacket& packet)
{
	if (mDevice->IsEnabled() == false)
		return;

	if (mIsConnected == false)
	{
		mIsConnected = true;
		LogInfo("BrainAlive connected");
	}

	// get current packet index and check for lost packets
	const uint32 currentPacketIndex = (uint32)packet.GetSampleNumber();
	const uint32 expectedPacketIndex = (mLastPacketIndex + 1) % 256;
	if (currentPacketIndex != expectedPacketIndex)
	{

		// we lost packets -> calculate how many (remember: index is modulo 256!)
		uint32 numLostPackets = 0;
		if (currentPacketIndex > expectedPacketIndex)
			numLostPackets = currentPacketIndex - expectedPacketIndex;
		else
			numLostPackets = (currentPacketIndex + 256) - expectedPacketIndex;
	
	}
	mLastPacketIndex = currentPacketIndex;


	// read out streampacket and feed raw samples into sensor channels
	const BrainAliveStreamEEGData* sensors = packet.mSensors;

	if (mDevice->GetType() == BrainAliveDevice::TYPE_ID)
	{
		const uint32 numElectrodes = mDevice->GetNumNeuroSensors();
		for (uint32 i = 0; i < numElectrodes; ++i)
		{
			const double gain = mElectrodeGainSettings[i];
			const uint32 base = Math::PowD(2, 23) - 1.0;
			const double scale = 4.5 / gain / base;

			const double sampleValue = sensors[i].GetValue() * scale;
			mDevice->GetSensor(i)->AddQueuedSample((double)sampleValue);
		}
	}

	// read out acceleration streampacket and feed them into the sensor channels
	const BrainAliveStreamAccData* accSensor = packet.mAcceleration;
	mPID = packet.mSampleNumber;
	double accValueLeftDelta = accSensor[0].GetValue();
	double accValueForwardDelta = accSensor[1].GetValue();
	double accValueUpDelta = accSensor[2].GetValue();
	const BrainAliveStreamPPGData* ppgSensor = packet.mPpg;
	double ppgValueIRDelta = ppgSensor[0].GetValue();
	double ppgValueRedDelta = ppgSensor[1].GetValue();
	double ppgValueGreenDelta = ppgSensor[2].GetValue();
	
	const uint32 scaleFactor = 8500;
	accValueLeftDelta /= scaleFactor;
	accValueUpDelta /= scaleFactor;
	accValueForwardDelta /= scaleFactor;

	if (accValueLeftDelta != 0)
		mAccValueLeft = accValueLeftDelta;

	if (accValueUpDelta != 0)
		mAccValueUp = accValueUpDelta;

	if (accValueForwardDelta != 0)
		mAccValueForward = accValueForwardDelta;

	if (ppgValueIRDelta != 0)
		mPPGValueIR = ppgValueIRDelta;

	if (ppgValueRedDelta != 0)
		mPPGValueRed = ppgValueRedDelta;

	if (ppgValueGreenDelta != 0)
		mPPGValueGreen = ppgValueGreenDelta;



	mDevice->GetAccLeftSensor()->AddQueuedSample((double)mAccValueLeft);
	mDevice->GetAccUpSensor()->AddQueuedSample((double)mAccValueUp);
	mDevice->GetAccForwardSensor()->AddQueuedSample((double)mAccValueForward);
	mDevice->GetPpgRedSensor()->AddQueuedSample((double)mPPGValueRed);
	mDevice->GetPpgIRSensor()->AddQueuedSample((double)mPPGValueIR);
	mDevice->GetPpgGreenSensor()->AddQueuedSample((double)mPPGValueGreen);
	mDevice->GetPIDSensor()->AddQueuedSample((double)mPID);
}


bool BrainAliveSerialHandler::ReadStreamPacket(BrainAliveStreamPacket* data)
{
	if (mBLE_Interface->BLE_Satus() == false)
		return false;

	uint8_t* mData_2 = mBLE_Interface->Get_BLE_Data();
	
	// no data -> return (probably does not happen?)
        if (mData_2 == 0)
          return false;
        if ((mData_2[0] != data->GetProtocolHeader()) && (mData_2[45] != data->GetProtocolFooter()))
           return false;

		data->mHeader = (uint)mData_2[0];
		data->mStatusbyte_1 = (uint)mData_2[1];
		data->mStatusbyte_2 = (uint)mData_2[2];
		data->mStatusbyte_3 = (uint)mData_2[3];
		data->mSensors[0].mEEGChannel[0] = (uint)mData_2[4];
		data->mSensors[0].mEEGChannel[1] = (uint)mData_2[5];
		data->mSensors[0].mEEGChannel[2] = (uint)mData_2[6];
		data->mSensors[1].mEEGChannel[0] = (uint)mData_2[7];
		data->mSensors[1].mEEGChannel[1] = (uint)mData_2[8];
		data->mSensors[1].mEEGChannel[2] = (uint)mData_2[9];
		data->mSensors[2].mEEGChannel[0] = (uint)mData_2[10];
		data->mSensors[2].mEEGChannel[1] = (uint)mData_2[11];
		data->mSensors[2].mEEGChannel[2] = (uint)mData_2[12];
		data->mSensors[3].mEEGChannel[0] = (uint)mData_2[13];
		data->mSensors[3].mEEGChannel[1] = (uint)mData_2[14];
		data->mSensors[3].mEEGChannel[2] = (uint)mData_2[15];
		data->mSensors[4].mEEGChannel[0] = (uint)mData_2[16];
		data->mSensors[4].mEEGChannel[1] = (uint)mData_2[17];
		data->mSensors[4].mEEGChannel[2] = (uint)mData_2[18];
		data->mSensors[5].mEEGChannel[0] = (uint)mData_2[19];
		data->mSensors[5].mEEGChannel[1] = (uint)mData_2[20];
		data->mSensors[5].mEEGChannel[2] = (uint)mData_2[21];
		data->mSensors[6].mEEGChannel[0] = (uint)mData_2[22];
		data->mSensors[6].mEEGChannel[1] = (uint)mData_2[23];
		data->mSensors[6].mEEGChannel[2] = (uint)mData_2[24];
		data->mSensors[7].mEEGChannel[0] = (uint)mData_2[25];
		data->mSensors[7].mEEGChannel[1] = (uint)mData_2[26];
		data->mSensors[7].mEEGChannel[2] = (uint)mData_2[27];
		data->mPpg[0].mPPGChannel[0] = (uint)mData_2[28];
		data->mPpg[0].mPPGChannel[1] = (uint)mData_2[29];
		data->mPpg[0].mPPGChannel[2] = (uint)mData_2[30];
		data->mPpg[1].mPPGChannel[0] = (uint)mData_2[31];
		data->mPpg[1].mPPGChannel[1] = (uint)mData_2[32];
		data->mPpg[1].mPPGChannel[2] = (uint)mData_2[33];
		data->mPpg[2].mPPGChannel[0] = (uint)mData_2[34];
		data->mPpg[2].mPPGChannel[1] = (uint)mData_2[35];
		data->mPpg[2].mPPGChannel[2] = (uint)mData_2[36];
		data->mAcceleration[0].mAccelerationChannel[0] = (uint)mData_2[37];
		data->mAcceleration[0].mAccelerationChannel[1] = (uint)mData_2[38];
		data->mAcceleration[1].mAccelerationChannel[0] = (uint)mData_2[39];
		data->mAcceleration[1].mAccelerationChannel[1] = (uint)mData_2[40];
		data->mAcceleration[2].mAccelerationChannel[0] = (uint)mData_2[41];
		data->mAcceleration[2].mAccelerationChannel[1] = (uint)mData_2[42];
		//data->mDeviceStatus = (uint)mData_2[43];
		data->mSampleNumber = (uint)mData_2[43];
        data->mDeviceStatus = (uint)mData_2[44];
		data->mFooter = (uint)mData_2[45];
	
	return true;
		
}

void BrainAliveSerialHandler::ReadStream()
{
	//try to read stream packets
	if((ReadStreamPacket(&mStreamPacket) == true))
	{

			if (mStreamPacket.Verify() == true)
			{
				ProcessStreamPacket(mStreamPacket);
				mNumPackets++;
			}
			else
			{
				LogWarning("BrainAliveSerialHandler: received invalid stream packet:");
			}
	}
}

#endif
