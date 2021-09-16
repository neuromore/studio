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

#ifndef __NEUROMORE_BRAINALIVECOMMANDS_H
#define __NEUROMORE_BRAINALIVECOMMANDS_H

#include <Core/String.h>


// stream packet data (one sensor)
struct BrainAliveStreamEEGData
{
	char mEEGChannel[3];

	// Convert 24 bit integer to 32 bit integer
	int32 GetValue() const
	{
		int32 newInt = (
			((0xFF & mEEGChannel[0]) << 16) |
			((0xFF & mEEGChannel[1]) << 8) |
			(0xFF & mEEGChannel[2])
			);

		if ((newInt & 0x00800000) > 0)
			newInt |= 0xFF000000;
		else
			newInt &= 0x00FFFFFF;

		return newInt;
	}
};

struct BrainAliveStreamPPGData
{
	char mPPGChannel[3];

	// Convert 24 bit integer to 32 bit integer
	int32 GetValue() const
	{
		int32 newInt = (
			((0xFF & mPPGChannel[0]) << 16) |
			((0xFF & mPPGChannel[1]) << 8) |
			(0xFF & mPPGChannel[2])
			);

		if ((newInt & 0x00800000) > 0)
			newInt |= 0xFF000000;
		else
			newInt &= 0x00FFFFFF;

		return newInt;
	}
};
// accelerometer stream packet data
struct BrainAliveStreamAccData
{
	char mAccelerationChannel[2];

	// Convert 16 bit integer to 32 bit integer
	int32 GetValue() const
	{
		int newInt = (
			((0xFF & mAccelerationChannel[0]) << 8) |
			(0xFF & mAccelerationChannel[1])
			);

		if ((newInt & 0x00008000) > 0)
			newInt |= 0xFFFF0000;
		else
			newInt &= 0x0000FFFF;

		return newInt;
	}
};

// the complete stream packet
struct BrainAliveStreamPacket
{
	static char GetProtocolHeader() { return (unsigned char)0x0A; }
	static char GetProtocolFooter() { return (unsigned char)0x0D; }

	// The packet data:

	char mHeader; // header byte
	char mSampleNumber; // increasing sample number
	char mStatusbyte_1; // status register 1
	char mStatusbyte_2; // status register 2
	char mStatusbyte_3; // status register 3
	char mError_status; // Error status byte

	// 8X EEG Data
	BrainAliveStreamEEGData mSensors[8]; // int24 data

	// Accelerometer Data
	BrainAliveStreamAccData mAcceleration[3]; // int16 data

	BrainAliveStreamPPGData mPpg[3]; // int24 data

	char mFooter; // Footer byte

	bool Verify() { return (mHeader == GetProtocolHeader() && mFooter == GetProtocolFooter()); }

	char GetSampleNumber() const { return mSampleNumber; }
};


#endif