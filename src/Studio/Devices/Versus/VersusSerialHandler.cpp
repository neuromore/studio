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

// include required files
#include "VersusSerialHandler.h"
#include <Devices/Versus/VersusDevice.h>

#ifdef INCLUDE_DEVICE_SENSELABS_VERSUS

using namespace Core;

// constructor
VersusSerialHandler::VersusSerialHandler(SerialPort* port, VersusDevice* headset, QObject* parent) : QObject(parent)
{
	mSerialPort = port;
	mHeadset = headset;

	mLastPacketIndex = 0;
	mSampleRate = mHeadset->GetSampleRate();

	mTimer = new QTimer();
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(ReadStream()));
	mTimer->setTimerType(Qt::PreciseTimer);
	mTimer->setInterval(2 * 1000 / mSampleRate);

	mIsConnected = false;
}


// destructor
VersusSerialHandler::~VersusSerialHandler()
{
	if (mSerialPort != NULL)
	{
		// FIXME serial port close() hangs 15 secs if bluetooth device is disconnected

		// if device has timed out, it means the bluetooth connection is gone
		if (mHeadset->IsTimeoutReached() == false)
			Disconnect();
		else
		{
			mSerialPort->Close();
			mSerialPort->Clear();
		}
	}
}


bool VersusSerialHandler::Connect()
{
	if (mSerialPort == NULL)
		return false;

	// initialize qt serial component
	mSerialPort->Init();

	LogDetailedInfo("Versus: configuring serial port");

	// 1) try to configure and open the port
	bool success = mSerialPort->SetBaudRate(115200) &&
		mSerialPort->SetParity(SerialPort::NO_PARITY) &&
		mSerialPort->SetNumDataBits(8) &&
		mSerialPort->SetNumStopBits(1);

	LogDetailedInfo("Versus: opening serial port");

	success = success && mSerialPort->Open();

	// configure/open failed
	if (success == false)
	{
		LogError("Versus: failed opening serial port");
		mSerialPort->PrintError();
		Disconnect();
		return false;
	}

	// wait a little before sending the first command (if the versus was connected via usb just
	// prior to the detection, it is booting up right now - sending the command will brick the device)
	Thread::Sleep(2000);

	// clear buffers before sending / receiving the very first command
	mSerialPort->Clear();

	LogDetailedInfo("Versus: starting raw stream");

	// send the start-raw-stream command
	if (SendCommand(STREAM_RAW_DATA, 1, 0) == false)
	{
		mSerialPort->PrintError();
		Disconnect();
		return false;
	}

	// start data receive thread
	mTimer->start();

	return true;
}


// disconnect from versus
void VersusSerialHandler::Disconnect()
{
	if (mSerialPort == NULL)
		return;

	if (mSerialPort->IsOpen() == true)
	{
		SendCommand(STOP_STREAM);
		mSerialPort->Close();
	}

	mSerialPort->deleteLater();
	mSerialPort = NULL;

	mIsConnected = false;
}


void VersusSerialHandler::ProcessStreamPacket(const VersusStreamPacket& packet)
{
	if (mHeadset->IsEnabled() == false)
		return;

	// this is first packet (we now know the versus has received our start stream command and answered)
	if (mIsConnected == false)
	{
		mIsConnected = true;
		LogInfo("Versus connected");
	}

	// get current packet index and check for lost packets
	const uint32 currentPacketIndex = (uint32)packet.mData.mIndex;
	const uint32 expectedPacketIndex = (mLastPacketIndex + 1) % 256;
	if (currentPacketIndex != expectedPacketIndex)
	{

		// we lost packets -> calculate how many (remember: index is modulo 256!)
		uint32 numLostPackets = 0;
		if (currentPacketIndex > expectedPacketIndex)
			numLostPackets = currentPacketIndex - expectedPacketIndex;
		else
			numLostPackets = (currentPacketIndex + 256) - expectedPacketIndex;

		LogDetailedInfo("VersusSerialHandler: missing %i packets (index is %i, should be %i)", numLostPackets, currentPacketIndex, expectedPacketIndex);

		// add zero values to compensate for lost samples
		for (uint32 i = 0; i<VersusStreamData::NUM_SENSORS; ++i)
			mHeadset->GetSensor(i)->HandleLostSamples(numLostPackets);

	}
	mLastPacketIndex = currentPacketIndex;

	// read out streampacket and feed raw samples into sensor channels
	const VersusStreamRawSensor* sensors = packet.mData.mSensors;
	for (uint32 i = 0; i<VersusStreamData::NUM_SENSORS; ++i)
	{
		double sampleValue = sensors[i].mRawData * 0.2;			// amplifier 'equation' from senselabs code
		mHeadset->GetSensor(i)->AddQueuedSample(sampleValue);
	}

	// update battery charge level
	mHeadset->SetBatteryChargeLevel(packet.mData.mBattLife / 100.0);
}


// send one command to the versus
bool VersusSerialHandler::SendCommand(EVersusCommand command, uint32 numArgs, uint32 arg1, uint32 arg2, char address)
{
	if (mSerialPort == NULL)
		return false;

	// construct command packet
	VersusCommandPacket packet;
	packet.Init(numArgs);
	packet.mAddress = address;
	packet.SetCommand(command);

	if (numArgs >= 1)
		packet.mData[0].Set(arg1);

	if (numArgs >= 2)
		packet.mData[1].Set(arg2);

	return WriteCommandPacket(&packet);
}


// write one command packet to the serial port
bool VersusSerialHandler::WriteCommandPacket(VersusCommandPacket* data)
{
	if (mSerialPort == NULL)
		return false;

	//OutputDebugStringA("Sending Command ");
	//(*data).Print();
	//OutputDebugStringA("\n");

	// send data
	const uint32 numBytes = (*data).GetSize();
	const uint32 result = mSerialPort->Write((const char*)data, numBytes);
	mSerialPort->Flush();

	if (result != numBytes)
		return false;

	return true;
}


// read one command packet from the serial port
bool VersusSerialHandler::ReadCommandPacket(VersusCommandPacket* data)
{
	if (mSerialPort == NULL)
		return false;

	// check if enough bytes are present
	const uint32 packetSize = data->GetSize();
	if (mSerialPort->GetNumBytesAvailable() < packetSize)
		return false;

	// try to read from serial port
	const uint32 result = mSerialPort->Read((char*)data, packetSize);

	if (result != packetSize)
		return false; // FAIL, should not happen

	const bool success = data->Verify();
	return success;
}


// read one stream packet
bool VersusSerialHandler::ReadStreamPacket(VersusStreamPacket* data)
{
	if (mSerialPort == NULL)
		return false;

	// no data -> return (probably does not happen?)
	if (mSerialPort->GetNumBytesAvailable() == 0)
		return false;

	// check if first byte is a stream packet header char
	char header;
	mSerialPort->GetChar(&header);
	if (header != '@')
	{
		// first byte is not the header of a stream packet 
		//  -> skip chars up to (and including) the next '\r', and check the header again
		char c = 'X';
		while (header != '@')
		{
			LogDetailedInfo("Warning: Serial stream contains unknown header '%c', skipping until end of line", header);

			// seek for next '\r'
			c = 'X';
			bool success = true;
			while (c != '\r')
			{
				const uint32 result = mSerialPort->Read(&c, 1);
				if (result != 1)
				{
					// no more data in buffer
					success = false;
					break;
				}
			}

			if (success == false)
				break;

			// try to read header
			const uint32 result = mSerialPort->Read(&header, 1);
			if (result != 1)
			{
				// no more data in buffer
				return false;
			}
		}

	}

	// put back the header char
	mSerialPort->UngetChar(header);

	// check if the buffer has enough bytes are present to read the rest
	const uint32 packetSize = data->GetSize();
	if (mSerialPort->GetNumBytesAvailable() < packetSize)
		return false;

	// try to read the stream packet
	const uint32 result = mSerialPort->Read((char*)data, packetSize);

	CORE_ASSERT(result == packetSize);

	// debug : print received data bytes as hex to console
	/*String tmp;
	unsigned char* bdata = reinterpret_cast<unsigned char*>(data);
	for (uint32 i=0; i<result; i++)
	{
	tmp.Format("%x ",(int)bdata[i]);
	OutputDebugStringA(tmp.AsChar());
	} */

	return true;
}

void VersusSerialHandler::ReadStream()
{
	//uint numPackets = 0;
	//try to read stream packets
	while (ReadStreamPacket(&mStreamPacket) == true)
	{
		if (mStreamPacket.Verify() == true)
		{
			ProcessStreamPacket(mStreamPacket);
			//numPackets++;
		}
		else
		{
			LogError("VersusSerialHandler: received invalid stream packet:");
			mStreamPacket.Print();
		}
	}

	//String tmp;
	//tmp.Format("Read %i stream packets", numPackets);
	//LogDetailedInfo(tmp.AsChar());
}

#endif
