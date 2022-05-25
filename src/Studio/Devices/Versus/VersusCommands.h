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

#ifndef __NEUROMORE_VERSUSCOMMANDS_H
#define __NEUROMORE_VERSUSCOMMANDS_H

#include <Core/String.h>

// versus commands
enum EVersusCommand
{
	NO_CMD = 0,
	ALARM_CODE = 6,
	POWER_OFF = 19,		// power off device?
	STOP_STREAM = 21,		// stop streaming
	STREAM_DATA = 20,		// start streaming normal data
	NAK_CODE = 31,
	STREAM_RAW_DATA = 50,		// start streaming raw eeg data
};


// one command parameter (used in command packets)
struct VersusCommandParameter
{
	char mData[4];		// hex encoded 32 bit value
	char mSpace;		// space between data sections

	// set/get hex data from 32 bit unsigned integer
	void Set(uint32 value)
	{
		mData[0] = ToHex((value >> 0) & 0xF);
		mData[1] = ToHex((value >> 4) & 0xF);
		mData[2] = ToHex((value >> 8) & 0xF);
		mData[3] = ToHex((value >> 12) & 0xF);
	}

	uint32 Get()
	{
		uint32 sum = 0;
		for (uint32 i = 0; i<4; i++)
			sum += FromHex(mData[i]) << (4 * i);
		return sum;
	}

	void Print()
	{
		Core::String tmp;
		tmp.Format("%c%c%c%c%c", mData[3], mData[2], mData[1], mData[0], mSpace);

#ifdef NEUROMORE_PLATFORM_WINDOWS
		OutputDebugStringA(tmp.AsChar());
#endif
	}


	// helper: int to single hex char
	char ToHex(uint32 num)
	{
		if (num < 10)
			return (char)(num + (uint32)'0');
		else if (num <= 16)
			return (char)(num - 10 + (uint32)'A');
		else
			return '?'; // failed
	}

	// helper: hex char to int
	uint32 FromHex(char c)
	{
		// 0 .. 9
		if ((uint32)c >= (uint32)'0' && (uint32)c <= (uint32)'9')
			return (uint32)c - (uint32)'0';

		// A .. F
		if ((uint32)c >= (uint32)'A' && (uint32)c <= (uint32)'F')
			return (uint32)c - (uint32)'A';

		// fail
		return CORE_INVALIDINDEX32;
	}

};


// a whole command packet with at max 5 parameters (arbitrary decision)
struct VersusCommandPacket
{
	char mHeader;
	char mAddress;
	char mSpace1;
	char mCommand[2];
	char mSpace2;
	VersusCommandParameter mData[2];
	char mCR;

	void Init(uint32 numParams = 0)
	{
		mHeader = '#';
		mAddress = '0';
		mSpace1 = ' '; mSpace2 = ' '; mCR = '\r';

		mNumParams = numParams;
		for (uint32 i = 0; i<numParams; i++)
		{
			mData[i].Set(0);
			mData[i].mSpace = ' ';
		}
		if (numParams < 5)
			mData[numParams].mData[0] = '\r';
	}

	// set command
	void SetCommand(EVersusCommand cmd)
	{
		sprintf(mCommand, "%i", cmd);

		// fix the \0 char caused by sprintf ^.^
		mSpace2 = ' ';
	}

	EVersusCommand GetCommand()
	{
		// convert two digit ascii decimal number to integer and convert it to enum
		uint32 command = ((uint32)mCommand[0] - (uint32)'0') * 10 + (uint32)mCommand[1] - (uint32)'0';
		return (EVersusCommand)command;
	}

	// check number of params by locating the \r end delimiter
	uint32 FindNumParams()
	{
		if (mSpace2 == '\r')					// case 1 : no parameters, no space after the command
			return 0;
		else if (mSpace2 == ' ')
			if (mData[0].mData[0] == '\r')		// case 2: no parameters, space after the command
				return 0;

		for (uint32 i = 0; i<2; i++)
		{

			if (mData[i].mSpace == '\r')		// case 3: at least one parameter, no space after the last parameter
				return i + 1;						// convert to 1-indexed

			else if (mData[i].mSpace == ' ')	// case 4: at least one parameter, space after the last parameter
			{
				// check the first byte of the next parameter or the mCR char for \r
				if (i<mNumParams)
				{
					if (mData[i + 1].mData[0] == '\r')
						return i + 1;				// convert to 1-indexed
				}
				else
				{
					if (mCR == '\r')
						return i + 1;				// convert to 1-indexed
				}
				continue;
			}
			else
			{
				return 0;	// invalid packet
			}
		}

		// failed
		return 0;
	}

	// actual size in bytes (depends on the number of parameters)
	uint32 GetSize() { return 6 + mNumParams * sizeof(VersusCommandParameter) + 1; }

	// verify that the command is valid
	bool Verify()	{ return (mHeader == '#') && (mSpace1 == ' ') && (mSpace2 == ' ') && (FindNumParams() == mNumParams); }

	// print whole command to debug console
	void Print()
	{
		Core::String tmp;
		tmp.Format("%c%c%c%c%c%c", mHeader, mAddress, mSpace1, mCommand[0], mCommand[1], mSpace2);

#ifdef NEUROMORE_PLATFORM_WINDOWS
		OutputDebugStringA(tmp.AsChar());
#endif

		uint32 numParams = FindNumParams();
		if (numParams > 0)
			for (uint32 i = 0; i<numParams; i++)
				mData[i].Print();

#ifdef NEUROMORE_PLATFORM_WINDOWS
		OutputDebugStringA("\n");
#endif

	}

	uint32 mNumParams;
};


// stream packet data (one sensor)
struct VersusStreamRawSensor
{
	int16 mStatus;
	int16 mRawData;
};


// stream packet data : raw data (24 bytes)
struct VersusStreamData
{
	enum { NUM_SENSORS = 5 };

	uint8	mIndex;
	uint8	mBattLife;
	uint16  mStatus;
	VersusStreamRawSensor mSensors[NUM_SENSORS];
};


// the complete stream packet
struct VersusStreamPacket
{
	char  mHeader;
	uint8 mAddress;
	uint8 mCommand;
	uint8 mLength;
	VersusStreamData mData;
	char  mCR;
	uint8 mStructAlignmentPaddingByte;		// for struct alignment

	bool Verify()	 { return (mHeader == '@') && (mAddress == 0) && (mCommand == 46) && (mLength == 24) && (mCR == '\r'); }
	uint32 GetSize() { return sizeof *this - 1; }	// struct 29 bytes and is zero padded with one byte at the end due to alignment

	// print whole packet to debug console
	void Print()
	{
		Core::String tmp;
		tmp.Format("%c %i %i %i %i %i", mHeader, (int)mAddress, (int)mCommand, (int)mLength, (int)mData.mIndex, (int)mData.mBattLife, (int)mData.mStatus);

#ifdef NEUROMORE_PLATFORM_WINDOWS
		OutputDebugStringA(tmp.AsChar());
#endif

		for (uint32 i = 0; i<5; i++)
		{
			tmp.Format("%x %x ", mData.mSensors[i].mStatus, mData.mSensors[i].mRawData);
#ifdef NEUROMORE_PLATFORM_WINDOWS
			OutputDebugStringA(tmp.AsChar());
#endif
		}

#ifdef NEUROMORE_PLATFORM_WINDOWS
		OutputDebugStringA("\n");
#endif
	}
};


#endif
