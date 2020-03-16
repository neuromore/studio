/*
 * neuromore Android App
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

package com.neuromore.engine.enums;

/**
 * Must match EDevice in neuromoreEngine.h
 */
public final class EDevice
{
    public static final int DEVICE_UNKNOWN                  = -1;   // does not exist in C so far

    // Vital Signs Sensor Devices						            //		 Win/OSX/Linux						/			iOS								/		Android
    public static final int DEVICE_GENERIC_HEARTRATE        = 0;	//	org.bluetooth.service.heart_rate		/ HKQuantityTypeIdentifierHeartRate			/ android.hardware.Sensor TYPE_HEART_RATE  (OR com.google.heart_rate from google fit)
//	public static final int DEVICE_GENERIC_RESPIRATION      = 1;    //				  -							/ HKQuantityTypeIdentifierRespiratoryRate
//	public static final int DEVICE_GENERIC_THERMOMETER      = 2;	// org.bluetooth.service.health_thermometer / HKQuantityTypeIdentifierBodyTemperature

    // Platform devices									            //		  Win/OSX/Linux			            /		iOS				                    /		Android
    public static final int DEVICE_GENERIC_ACCELEROMETER    = 3;	//	 platform specific driver	            /	CMMotionManager		                    /  android.hardware.Sensor TYPE_ACCELEROMETER
    public static final int DEVICE_GENERIC_GYROSCOPE        = 4;	//	 platform specific driver	            /	CMMotionManager		                    /  android.hardware.Sensor TYPE_GYROSCOPE
//	public static final int DEVICE_GENERIC_GEOPOSITION      = 5;    //	 platform specific driver	            /	CLLocation			                    /  android.location.LocationManager
//	public static final int DEVICE_GENERIC_AMBIENT_LIGHT    = 6;    //				-				            /  (n/a.. use camera)	                    /  android.hardware.Sensor TYPE_LIGHT
//	public static final int DEVICE_GENERIC_AUDIO_IN	        = 7;    //          QT Audio			            /	AudioUnits ?		                    /  android.media.AudioRecord
//	public static final int DEVICE_GENERIC_AUDIO_OUT        = 8;    //			QT Audio			            /	AudioUnits ?		                    /  android.media.AudioTrack
//	public static final int DEVICE_GENERIC_CAMERA           = 9;    //			  OpenCV			            /		?				                    /  android.hardware.camera2

    // EEG devices
    public static final int DEVICE_INTERAXON_MUSE           = 10;
    public static final int DEVICE_EMOTIV_EPOC              = 11;
    public static final int DEVICE_EMOTIV_INSIGHT           = 12;
    public static final int DEVICE_NEUROSKY_MINDWAVE        = 13;
    public static final int DEVICE_SENSELABS_VESUS          = 14;

    // other devices
    public static final int DEVICE_ESENSE_SKINRESPONSE		= 15;

    /**
     * Returns a default-name for a neuromore engine EDevice enum value
     * @param deviceType neuromore EDevice value
     * @return String containing default name
     */
    public static String getDefaultNameForType(int deviceType)
    {
        switch(deviceType)
        {
            case EDevice.DEVICE_GENERIC_HEARTRATE: return "Generic Heart Rate Sensor";
            case EDevice.DEVICE_GENERIC_ACCELEROMETER: return "Generic Accelerometer";
            case EDevice.DEVICE_GENERIC_GYROSCOPE: return "Generic Gyroscope";
            case EDevice.DEVICE_INTERAXON_MUSE: return "Interaxon MUSE";
            case EDevice.DEVICE_EMOTIV_EPOC: return "DEVICE_EMOTIV_EPOC";
            case EDevice.DEVICE_EMOTIV_INSIGHT: return "DEVICE_EMOTIV_INSIGHT";
            case EDevice.DEVICE_NEUROSKY_MINDWAVE: return "DEVICE_NEUROSKY_MINDWAVE";
            case EDevice.DEVICE_SENSELABS_VESUS: return "DEVICE_SENSELABS_VESUS";
            case EDevice.DEVICE_ESENSE_SKINRESPONSE: return "eSense Skin response";
            default: return "DEVICE_UNKNOWN";
        }
    }
}
