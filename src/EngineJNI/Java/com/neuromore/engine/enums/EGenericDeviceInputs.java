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

package com.neuromore.engine.enums;

/**
 * Must match EGenericDeviceInputs in neuromoreEngine.h
 */
public final class EGenericDeviceInputs
{
    public static final int HEARTRATE_INPUT_BPM = 0;	// <  10 Hz
    public static final int HEARTRATE_INPUT_RR	= 1;	//  -

    public static final int ACCELEROMETER_INPUT_X = 0;	// < 50 Hz
    public static final int ACCELEROMETER_INPUT_Y = 1;	// < 50 Hz
    public static final int ACCELEROMETER_INPUT_Z = 2;	// < 50 Hz

    public static final int GYROSCOPE_INPUT_X = 0;		// < 50 Hz
    public static final int GYROSCOPE_INPUT_Y = 1;		// < 50 Hz
    public static final int GYROSCOPE_INPUT_Z = 2;		// < 50 Hz
}
