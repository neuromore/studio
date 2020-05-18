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

package com.neuromore.engine;

/**
 * This is a static, plain JNI wrapperclass for the native neuromore engine C-API calls.
 * It does nothing but provide callable native function signatures and
 * will load the necessary native library (*.so) before the first call is made.
 * For handling callbacks see ICallback, which gets set in init().
 */
public class Wrapper
{
    static
    {
        // load the neuromore library (libengine.so / engine.dll)
        System.loadLibrary("EngineJNI");
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Engine initialization
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native boolean SetPowerLineFrequencyType(int powerLineFrequencyType);
    public static native double GetPowerLineFrequency();
    public static native boolean SetBufferLength(double seconds);
    public static native void Init(ICallback callback);
    public static native boolean IsInitialized();
    public static native void Shutdown();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Engine basics: Start, Stop, Cleanup, Update etc
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native boolean IsReady();
    public static native void SetSessionLength(double seconds);
    public static native boolean Start();
    public static native boolean StartThreaded();
    public static native boolean Update();
    public static native boolean IsRunning();
    public static native boolean Stop();
    public static native boolean StopThreaded();
    public static native void Reset();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Helpers
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native void EnableDebugLogging();
    public static native void SetAllowAssetStreaming(boolean allow);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Devices
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native int AddDevice(int deviceType);  // use EDevice
    public static native int GetNumDevices();
    public static native boolean RemoveDevice(int deviceIndex);
    public static native int GetDevice(int deviceIndex);     // returns EDevice
    public static native boolean HasDevice(int deviceType);  // use EDevice
    public static native boolean ConnectDevice(int deviceIndex);
    public static native boolean DisconnectDevice(int deviceIndex);
    public static native int GetNumInputs(int deviceIndex);
    public static native boolean AddInputSample(int deviceIndex, int inputIndex, double value);
    public static native boolean SetBatteryChargeLevel(int deviceIndex, double normalizedCharge);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Classifier
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native boolean LoadClassifier(String jsonContent, String uuid, int revision);
    public static native boolean HasClassifier();
    public static native boolean IsDeviceRequiredByClassifier(int deviceType); // use EDevice
    public static native int GetNumFeedbacks();
    public static native String GetFeedbackName(int index);
    public static native double GetCurrentFeedbackValue(int index);
    //public static native void GetFeedbackRange();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Cloud Data Serialization
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native String GetCreateDataChunkJson(String userId, String experienceUuid, int experienceRevision);
    public static native int GetNumDataChunkChannels();
    public static native String GetDataChunkChannelJson(String userId, String dataChunkUuid, int channelIndex);
    public static native byte[] GetDataChunkChannelData(int channelIndex);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Classifier Cloud Parameters
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native String CreateJSONRequestFindParameters(String userId);
    public static native boolean HandleJSONReplyFindParameters(String jsonString, String userId);
    public static native String CreateJSONRequestSetParameters();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // State Machine
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native boolean LoadStateMachine(String jsonContent, String uuid, int revision);
    public static native boolean HasStateMachine();

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Multimedia Assets
    ///////////////////////////////////////////////////////////////////////////////////////////////

    public static native int GetNumAssetsOfType(int assetType); // use EAssetType
    public static native String GetAssetLocationOfType(int assetType, int index); // use EAssetType
    public static native boolean GetAssetAllowStreamingOfType(int assetType, int index); // use EAssetType

    public static native int GetNumAssets();
    public static native String GetAssetLocation(int index);
    public static native boolean GetAssetAllowStreaming(int index);

    public static native boolean ButtonClicked(int buttonId);
    public static native boolean AudioLooped(String url);
    public static native boolean VideoLooped(String url);

    ///////////////////////////////////////////////////////////////////////////////////////////////
}
