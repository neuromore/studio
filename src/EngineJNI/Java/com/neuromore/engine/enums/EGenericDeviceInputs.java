/*
 * neuromore Android App
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

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
