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
#include "OpenBCISerialHandler.h"
#include <Devices/OpenBCI/OpenBCIDevices.h>
#include <EngineManager.h>
#include <QCoreApplication>
#include <QTimer>

#ifdef INCLUDE_DEVICE_OPENBCI

using namespace Core;

// constructor
OpenBCISerialHandler::OpenBCISerialHandler(SerialPort* port, OpenBCIDeviceBase* headset, QObject* parent) : QObject(parent)
{
	mSerialPort = port;
	mDevice = headset;
	
	// default gain values
	mElectrodeGainSettings.Resize(16);
	for (uint32 i=0; i<16; ++i)
		mElectrodeGainSettings[i] = 24;		// openbci default

	mLastPacketIndex = 0;
	mNumPackets = 0;
	mSampleRate = mDevice->GetSampleRate();

	mTimer = new QTimer();
	QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(ReadStream()));
	mTimer->setTimerType( Qt::PreciseTimer );
	mTimer->setInterval(2*1000/mSampleRate);

	mIsConnected = false;

	mAccValueLeft    = 0.0;
	mAccValueUp      = 0.0;
	mAccValueForward = 0.0;
}


// destructor
OpenBCISerialHandler::~OpenBCISerialHandler()
{
	if (mSerialPort != NULL)
	{
		// if device has timed out, it means the bluetooth connection is gone
		if (mDevice->IsTimeoutReached() == false)
			Disconnect();
		else
		{
			mSerialPort->Clear();
			mSerialPort->Close();
		}
	}
}


bool OpenBCISerialHandler::Connect()
{
	if (mSerialPort == NULL)
		return false;

	// initialize qt serial component
	mSerialPort->Init();

	LogDetailedInfo("OpenBCI: configuring serial port");

	// 1) try to configure and open the port
	bool success =	mSerialPort->SetBaudRate(115200) &&
					mSerialPort->SetParity(SerialPort::NO_PARITY) &&
					mSerialPort->SetNumDataBits(8) &&
					mSerialPort->SetNumStopBits(1);

	LogDetailedInfo("OpenBCI: opening serial port");

	success	= success && mSerialPort->Open();

	// configure/open failed
	if (success == false)
	{
		LogError("OpenBCI: failed opening serial port");
		mSerialPort->PrintError();
		Disconnect();
		return false;
	}

	// wait a little before sending the first command
	Thread::Sleep(5000);

	// clear buffers before sending / receiving the very first command
	mSerialPort->Clear();

	// configure channels
	if (mDevice->IsConfigured() == true)
	{
		LogDetailedInfo("OpenBCI: configuring channels");

		if (ConfigureChannels(mDevice->GetConfig()) == false)
			LogError("OpenBCI: channels not configured correctly");
#ifdef CORE_DEBUG
		else
		{
			// read back configuration
			String report; 
			if (ReadChannelConfig(report) == false)
				LogError("OpenBCI: reading channel config failed");
			else
				LogDebug("OpenBCI Channel config reported as:\n%s", report.AsChar());

		}
#endif
	}

	// clear buffers (device answers channel config commands)
	if (mSerialPort->Clear() == false)
		LogWarning("OpenBCI: serialport clear() failed");

	LogDetailedInfo("OpenBCI: starting raw stream");

	// send the start-raw-stream command
	if (SendStartCommand() == false)
	{
		mSerialPort->PrintError();
		LogError("OpenBCI:Error sending start command");
		Disconnect();
		return false;
	}

	
	// start data receive thread
	mTimer->start();

	return true;
}


// disconnect from openbci
void OpenBCISerialHandler::Disconnect()
{
	if (mSerialPort == NULL)
		return;

	if (mSerialPort->IsOpen() == true)
	{
		if (SendStopCommand() == true)
		    mSerialPort->Close();
	}

	mIsConnected = false;
}



// configure channels: send channel commands to openbci and set gain values of device
bool OpenBCISerialHandler::ConfigureChannels(const Device::DeviceConfig& config)
{
	bool hasError = false;
	
	Json::Item rootItem = config.mJson.GetRootItem();
	String itemName;

	const uint32 numElectrodes = mDevice->GetNumNeuroSensors();
	for (uint32 i=0; i<numElectrodes; ++i)
	{
		itemName.Format("eeg%i", i+1);
		Json::Item eegItem = rootItem.Find(itemName.AsChar());

		// must be object
		if (eegItem.IsObject() == false)
			continue;

		// search for electrode parameters
		bool hasConfig = false;

		// 1) POWER_DOWN
		bool enable = true; // default
		Json::Item enableItem = eegItem.Find("enable");
		if (enableItem.IsBool() == true)
		{
			hasConfig = true;
			enable = enableItem.GetBool();
		}

		// 2) GAIN_SET
		uint32 gainIndex = 6; // default = 24x
		Json::Item gainItem = eegItem.Find("gain");
		if (gainItem.IsNumber() == true)
		{
			hasConfig = true;
			uint32 gain = gainItem.GetInt();

			// remember gain for raw value scaling
			mElectrodeGainSettings[i];

			// convert to index used by openbci command
			switch (gain)
			{
				case 1:  gainIndex = 0; break;
				case 2:  gainIndex = 1; break;
				case 4:  gainIndex = 2; break;
				case 6:  gainIndex = 3; break;
				case 8:  gainIndex = 4; break;
				case 12: gainIndex = 5; break;
				default:
					LogError("OpenBCI: tried to set unknown gain value \'%i\', defaulting to \'24\'", gain); // no break, fall through
					hasError = true;
				case 24: gainIndex = 6; break;
			}
		}
			
		// 3) INPUT_TYPE_SET
		uint32 typeIndex = 0; // default
		Json::Item typeItem = eegItem.Find("type");
		if (typeItem.IsString() == true)
		{
			hasConfig = true;
			String type = typeItem.GetString();
			if (type.CompareNoCase("normal") == 0)
				typeIndex = 0;
			else if (type.CompareNoCase("shorted") == 0)
				typeIndex = 1;
			else if (type.CompareNoCase("bias_meas") == 0)
				typeIndex = 2;
			else if (type.CompareNoCase("mvdd") == 0)
				typeIndex = 3;
			else if (type.CompareNoCase("temp") == 0)
				typeIndex = 4;
			else if (type.CompareNoCase("testsig") == 0)
				typeIndex = 5;
			else if (type.CompareNoCase("bias_drp") == 0)
				typeIndex = 6;
			else if (type.CompareNoCase("bias_drn") == 0)
				typeIndex = 7;
			else
			{
				LogError ("OpenBCI: config contains unknown channel mode '%s' for channel %i, defaulting to 'normal'", type.AsChar(), i+1);
				hasError = true;
			}
		}

		// 4) BIAS_SET
		bool bias = true; // default
		Json::Item biasItem = eegItem.Find("bias");
		if (biasItem.IsBool() == true) // power-off channel disconnects it from bias
		{
			hasConfig = true;
			bias = biasItem.GetBool();
		}

		// 5) SRB2_SET
		bool srb2 = true; // default
		Json::Item srb2Item = eegItem.Find("srb2");
		if (srb2Item.IsBool() == true) 
		{
			hasConfig = true;
			srb2 = srb2Item.GetBool();
		}

		// 6) SRB1_SET
		bool srb1 = false; // default
		Json::Item srb1Item = eegItem.Find("srb1");
		if (srb1Item.IsBool() == true)
		{
			hasConfig = true;
			srb1 = srb1Item.GetBool();
		}

		// power-off also requires discocnect from bias and SRB2 (taken from openbci code)
		if (enable == false)
		{
			bias = false;
			srb2 = false;
		}

		// skip command if no value is set (use openbci default)
		if (hasConfig == true)
		{
			// also send power-off command, too
			if (enable == false)
			{
				if (SendChannelPowerCommands(i, enable) == false)
				{
					LogError("OpenBCI: Error sending channel power-off command for channel %i", i+1);
					hasError = true;
				}
			}

			if (SendChannelCommands(i, enable, gainIndex, typeIndex, bias, srb2, srb1) == false)
			{
				LogError("OpenBCI: Error sending channel config command for channel %i", i+1);
				hasError = true;
			}

			
		}
	}


	// flush read buffer
	mSerialPort->Clear();

	return !hasError;
}


// send the read-config command and append the answer to the string
bool OpenBCISerialHandler::ReadChannelConfig(String& outConfig)
{
	outConfig.Clear();

	// clear serial port buffers first
	mSerialPort->Clear();

	// write the command
	if (mSerialPort->Write("?", 1) != 1)
		return false;

	// read very slowly...
	String buff;
	while (mSerialPort->WaitForRead(200) == true)
	{
	
		const uint32 numBytes = mSerialPort->GetNumBytesAvailable();
		buff.Resize(numBytes);
		if (mSerialPort->Read(buff.AsChar(), numBytes) != numBytes)
			continue; // ??
		
		outConfig += buff;

		Thread::Sleep(50);
	}

	return true;
}


// configure the amplifier channels
// gain = 0-6 (=1/2/4/6/8/12/24)
// inputType = 0-7
bool OpenBCISerialHandler::SendChannelCommands(uint32 index, bool power, uint32 gainIndex, uint32 inputType, bool bias, bool srb2, bool srb1)
{
	// construct command (see https://github.com/OpenBCI/Docs/blob/master/software/01-OpenBCI_SDK.md)

	// map channel index to ascii char
	char channel;
	if (index < 8) // 0-7 -> '1'-'8'
		channel = char('1'+index);
	else // 8-15 -> Q/W/E/R/T/Y/U/I
	{
		switch (index)
		{
			case 8:  channel = 'Q'; break;
			case 9:  channel = 'W'; break;
			case 10: channel = 'E'; break;
			case 11: channel = 'R'; break;
			case 12: channel = 'T'; break;
			case 13: channel = 'Y'; break;
			case 14: channel = 'U'; break;
			case 15: channel = 'I'; break;
			default: CORE_ASSERT(false);
		}
	}
	
	String command;
	command.Format("x%c%c%i%i%c%c%cX", channel, (power ? '0' : '1'), gainIndex, inputType, (bias ? '1' : '0'), (srb2 ? '1' : '0'), (srb1 ? '1' : '0'));
	const uint32 numBytes = command.GetLength();
	CORE_ASSERT(numBytes == 2+7);
	LogDebug("OpenBCI: Sending Command '%s'", command.AsChar());

	mSerialPort->Clear();

	// send bytes separately and wait for a line of text as reply. This is faster than waiting for a safe amount of time each char (which adds up..)
	const char* data = command.AsChar();
	String buff;

	for (uint32 i=0; i<numBytes; ++i)
	{
		LogDebug("OpenBCI: writing \'%c\'", *(data+i));
		if (mSerialPort->Write(data+i, 1) != 1)
		{
			LogDebug("!!!!! write failed");
			//return false; // abort, write failed
		}

		mSerialPort->WaitForWrite(50);
		mSerialPort->WaitForRead(50);

		// QT BUG QT BUG QT BUG
		// HACKHACKHACK
		uint32 numTries = 0; const uint32 maxNumTries = 10; // 10*5 ms = 50 ms max time for response
		buff = mSerialPort->ReadAll();
		while (mSerialPort->WaitForRead(5) || mSerialPort->GetNumBytesAvailable() > 0 || numTries < maxNumTries)
		{
			
			// unless we call bytesAvailable() on the serial port, readAll doesn't reply with the current data (sometimes old data, but most of the times no data at all)
		//CORE_ASSERT(mSerialPort->GetNumBytesAvailable() != -1);
			buff += mSerialPort->ReadAll();
			numTries++;
		}

		LogDebug("OpenBCI: answered with '%s'", buff.AsChar());
	}
	mSerialPort->Clear();

	return true;
}


// turn channels on or off
bool OpenBCISerialHandler::SendChannelPowerCommands(uint32 index, bool enable)
{
	// get command char using two 16-level switch condition
	char c = 0;

	// power on channel
	if (enable == true)
	{
		switch (index)
		{
			case 0: c = '!'; break;
			case 1: c = '@'; break;
			case 2: c = '#'; break;
			case 3: c = '$'; break;
			case 4: c = '%'; break;
			case 5: c = '^'; break;
			case 6: c = '&'; break;
			case 7: c = '*'; break;//_______________
			case  8: c = 'Q'; break;
			case  9: c = 'W'; break;
			case 10: c = 'E'; break;
			case 11: c = 'R'; break;
			case 12: c = 'T'; break;
			case 13: c = 'Y'; break;
			case 14: c = 'U'; break;
			case 15: c = 'I'; break;
			default: return false;
		}
	}
	else // power off channels
	{
		switch (index)
		{
			case 0: c = '1'; break;
			case 1: c = '2'; break;
			case 2: c = '3'; break;
			case 3: c = '4'; break;
			case 4: c = '5'; break;
			case 5: c = '6'; break;
			case 6: c = '7'; break;
			case 7: c = '8'; break;//_______________
			case  8: c = 'q'; break;
			case  9: c = 'w'; break;
			case 10: c = 'e'; break;
			case 11: c = 'r'; break;
			case 12: c = 't'; break;
			case 13: c = 'y'; break;
			case 14: c = 'u'; break;
			case 15: c = 'i'; break;
			default: return false;
		}
	}

	// send command
	const bool success = (mSerialPort->Write(&c, 1) == 1);
	return success;
}


// start raw data stream
bool OpenBCISerialHandler::SendStartCommand()
{
	if (mSerialPort->Write("b", 1) == 1)
		return true;

	return false;
}


// stop raw data stream
bool OpenBCISerialHandler::SendStopCommand()
{
	if (mSerialPort->Write("s", 1) == 1)
		return true;

	return false;
}


// reset board
bool OpenBCISerialHandler::SendResetCommand()
{
	if (mSerialPort->Write("v", 1) == 1)
		return true;

	return false;
}


void OpenBCISerialHandler::ProcessStreamPacket(const OpenBCIStreamPacket& packet)
{
	if (mDevice->IsEnabled() == false)
		return;

	// this is first packet (we now know the device has received our start stream command and answered)
	if (mIsConnected == false)
	{
		mIsConnected = true;
		LogInfo("OpenBCI connected");
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
		
		//LogDetailedInfo("OpenBCISerialHandler: missing %i packets (index is %i, should be %i)", numLostPackets, currentPacketIndex, expectedPacketIndex);
		// TODO: First two packets contains obsolete data. This causes an exception at this point. Has to be handled
		// add zero values to compensate for lost samples
		/*for (uint32 i=0; i<mDevice->GetNumElectrodes(); ++i)
			mDevice->GetSensor(i)->HandleLostSamples(numLostPackets);*/

	}
	mLastPacketIndex = currentPacketIndex;

	
	// read out streampacket and feed raw samples into sensor channels
	const OpenBCIStreamEEGData* sensors = packet.mSensors;

	// if this is a openbci + daisy device the 16 channels come in two successive packets (effective samplerate is halfed)
	if (mDevice->GetType() == OpenBCIDaisyDevice::TYPE_ID)
	{ 
		const uint32 numElectrodes = 8;
		// first or second half? (offset is either 0 (main board packets) or 7 (daisy packets)
		const bool isFromDaisy = (currentPacketIndex % 2 == 0) ? true : false;
		const uint32 sensorIndexOffset = (isFromDaisy ? numElectrodes : 0);
		for (uint32 i = 0; i < numElectrodes; ++i)
		{
			const double gain = mElectrodeGainSettings[i];
			const uint32 base = Math::PowD(2,23) - 1.0;
			const double scale = 4.5 / gain / base;

			const double sampleValue = sensors[i].GetValue() * scale;
			mDevice->GetSensor(i + sensorIndexOffset)->AddQueuedSample((double)sampleValue);
		}
	}
	else
	{
		// 8 channel device (and maybe ganglion)
		const uint32 numElectrodes = mDevice->GetNumNeuroSensors();
		for (uint32 i = 0; i < numElectrodes; ++i)
		{
			const double gain = mElectrodeGainSettings[i];
			const uint32 base = Math::PowD(2,23) - 1.0;
			const double scale = 4.5 / gain / base;

			const double sampleValue = sensors[i].GetValue() * scale;
			mDevice->GetSensor(i)->AddQueuedSample((double)sampleValue);
		}
	}

	// read out acceleration streampacket and feed them into the sensor channels
	const OpenBCIStreamAccData* accSensor = packet.mAcceleration;
	double accValueLeftDelta    = accSensor[0].GetValue();
	double accValueForwardDelta = accSensor[1].GetValue();
	double accValueUpDelta = accSensor[2].GetValue();

	const uint32 scaleFactor = 8500;
	accValueLeftDelta    /= scaleFactor;
	accValueUpDelta      /= scaleFactor;
	accValueForwardDelta /= scaleFactor;

	if (accValueLeftDelta != 0)
	    mAccValueLeft    = accValueLeftDelta;

	if (accValueUpDelta != 0)
	    mAccValueUp      = accValueUpDelta;

	if (accValueForwardDelta != 0)
	    mAccValueForward = accValueForwardDelta;

	// set values for acceleration
    mDevice->GetAccLeftSensor()->AddQueuedSample((double)mAccValueLeft);
    mDevice->GetAccUpSensor()->AddQueuedSample((double)mAccValueUp);
    mDevice->GetAccForwardSensor()->AddQueuedSample((double)mAccValueForward);
}


// read one stream packet from serial port into data struct
bool OpenBCISerialHandler::ReadStreamPacket(OpenBCIStreamPacket* data)
{
	if (mSerialPort == NULL)
		return false;

	// no data -> return (probably does not happen?)
	if (mSerialPort->GetNumBytesAvailable() == 0)
		return false;

	// check if first byte is a stream packet header char
	char header;
	mSerialPort->GetChar(&header);
	if (header != OpenBCIStreamPacket::GetProtocolHeader())
	{
		// first byte is not the header of a stream packet 
		
		// if we received an 's', the openbci was disconnected
		if (header == 's' && mDevice->IsStreaming() == true)
		{
			Disconnect();
			return false;
		}	
		else
		{
			// forward until header char is found
			while (mSerialPort->GetNumBytesAvailable() > 0 && header != OpenBCIStreamPacket::GetProtocolHeader())
			{
				mSerialPort->GetChar(&header);
#ifdef CORE_DEBUG
				LogDebug("OpenBCI: dropped rx byte %c", header);
#endif
			}

			// put back header char (if any)
			if (header == OpenBCIStreamPacket::GetProtocolHeader())
				mSerialPort->UngetChar(header);
			else
				return false;
		}
	}
	else
	{
		mSerialPort->UngetChar(header);
	}

	// check if the buffer has enough bytes are present to read the rest
	//const uint32 packetSize = sizeof(data);
	// FIXME: Why is sizeof(data) = 4 bytes? This doesn't make any sense
	const uint32 packetSize = sizeof(data->mHeader) + sizeof(data->mSampleNumber) +
		                      sizeof(data->mSensors) + sizeof(data->mAcceleration) +
						      sizeof(data->mFooter);

	if (mSerialPort->GetNumBytesAvailable() < packetSize)
		return false;

	// try to read the stream packet
	const uint32 result = mSerialPort->Read((char*)data, packetSize);

	CORE_ASSERT (result == packetSize);

	// debug : print received data bytes as hex to console
	//String tmp;
	//unsigned char* bdata = reinterpret_cast<unsigned char*>(data);
	//for (uint32 i=0; i<result; i++)
	//{
	//	tmp.Format("%x ",(int)bdata[i]);
	//	OutputDebugStringA(tmp.AsChar());
	//}

	return true;
}

void OpenBCISerialHandler::ReadStream()
{
	//// DEBUG: flush serial instream do debug log
	//const uint32 numBytes = mSerialPort->GetNumBytesAvailable();
	//if (mSerialPort != NULL && numBytes > 0)
	//{
	//	String str; str.Resize(numBytes);
	//	const uint32 numBytesRead = mSerialPort->Read(str.AsChar(), numBytes);
	//	str.Resize(numBytesRead);
	//	LogDebug(str.AsChar());
	//}


	 //try to read stream packets
	while(ReadStreamPacket(&mStreamPacket) == true)
	{
		if (mStreamPacket.Verify() == true)
		{
			ProcessStreamPacket(mStreamPacket);
			mNumPackets++;

			//String tmp;
			//tmp.Format("Read %i stream packets", mNumPackets);
			//LogDetailedInfo(tmp.AsChar());
		}
		else
		{
			LogWarning("OpenBCISerialHandler: received invalid stream packet:");
		}
	}

	
}

#endif
