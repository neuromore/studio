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

#ifndef __NEUROMOREENGINE_H
#define __NEUROMOREENGINE_H

#if defined(NEUROMORE_ENGINE_SHARED)
#if defined(_WIN32)
#define NEUROMORE_EXPORT __declspec(dllexport)
#else
#define NEUROMORE_EXPORT __attribute__((visibility("default")))
#endif
#else
#define NEUROMORE_EXPORT
#endif

#define BOOL int

#ifdef __cplusplus
namespace neuromoreEngine
{
   extern "C" {
#endif

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Engine initialization
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Set the engine buffer length, in seconds.
   * Use this to adjust the buffer size of the engine. If you update the engine not often enough, or if you push too many samples in the devices at once,
   * the buffers will overflow and OnError() will be called and the engine will stop. Default value is 5 seconds.
   */
   NEUROMORE_EXPORT BOOL SetBufferLength(double seconds);

   enum EPowerLineFrequencyType
   {
      //POWERLINEFREQ_AUTO = 0,
      POWERLINEFREQ_50HZ = 1,
      POWERLINEFREQ_60HZ = 2
   };

   /**
   * Set the power line frequency.
   * @param[in] powerLineFrequencyType The power line frequency for the current location of the device.
   */
   NEUROMORE_EXPORT BOOL SetPowerLineFrequencyType(enum EPowerLineFrequencyType powerLineFrequencyType);

   NEUROMORE_EXPORT double GetPowerLineFrequency();

   /**
   * Initialize the neuromore Engine.
   * Call this before calling any other function from the engine.
   * @result True in case everything got initialized correctly, false in case something failed.
   */
   NEUROMORE_EXPORT BOOL Init();

   /*
   * Check if neuromore Engine is initialized and ready. You can call Start() only if this method returns true.
   * @return True in case the engine is ready for action, false in case an error happened or Init() wasn't called yet.
   */
   NEUROMORE_EXPORT BOOL IsInitialized();

   /**
   * Shutdown the neuromore Engine.
   * Call this after the last engine function call inside your application. This will destruct all used objects and cleanup memory.
   */
   NEUROMORE_EXPORT void Shutdown();

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Engine basics: Start, Stop, Cleanup, Update etc
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Check if the engine is ready to start
   * This requires: 1) classifier/statemachine is loaded 2) required devices were added
   */
   NEUROMORE_EXPORT BOOL IsReady();

   /**
   * Set the maximum duration a single session should be running.
   * It will stop execution if the time is reached, but can also be stopped by the Statemachine (if there is one)
   */
   NEUROMORE_EXPORT void SetSessionLength(double seconds);

   /**
   * Begin processing data.
   * The engine is not running by default. It has to configured first, a classifier must be loaded etc. As soon as IsInitialized() as well as IsReady() report true, everything is ready 
   * and you can start the engine by calling this function. As soon as its running you MUST call the Update() method regularly, with the correct time delta, and you 
   * MUST feed in the device samples with the required sample rate. If you call Start() everything is reset first and internal elapsed time is set to t=0. 
   * Make sure that you clear all device sample buffers before starting, samples must NOT be older than the time where Start() was called.
   * @return true if engine could be started, false if not (same return value as IsInitialized())
   */
   NEUROMORE_EXPORT BOOL Start();
   NEUROMORE_EXPORT BOOL StartThreaded();

   /**
   * Update the neuromore Engine.
   * The engine needs to be update regularily and close to real-time. Call this function inside your application real-time loop. Call this function as often as possible so that the time deltas are small.
   */
   NEUROMORE_EXPORT BOOL Update();

   /*
   **/
   NEUROMORE_EXPORT BOOL GetPerformanceStatistics(double* outFps, double* outTheoreticalFps, double* outAveragedTiming, double* outBestCaseTiming, double* outWorstCaseTiming);

   /**
   * Check if the engine is currently running. 
   * Note that you cannot modify the engine in any way during runtime.
   * @return true if the engine is currently processing data
   */
   NEUROMORE_EXPORT BOOL IsRunning();

   /**
   * Stops the engine if it is currently running. 
   * Also resets the classifier, statemachine and devices. If you call Start() it will run from the beginning again and not continue where you left of.
   */
   NEUROMORE_EXPORT BOOL Stop();
   NEUROMORE_EXPORT BOOL StopThreaded();

   /**
   * Resets the classifier, statemachine and clears devices inputs (but does not remove devices)
   */
   NEUROMORE_EXPORT void Reset();

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Helpers
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Enable debug logging.
   * Only do this for development builds and never call this for production. This might spawn your console.
   */
   NEUROMORE_EXPORT void EnableDebugLogging();

   /**
   * Asset streaming option.
   * Enable this in case the internet connection is stable enough for audio and video streaming.
   */
   NEUROMORE_EXPORT void SetAllowAssetStreaming(BOOL allow);

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Devices
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   enum EDevice
   {
      // Vital Signs Sensor Devices          // Win/OSX/Linux                             / iOS                                        /  Android
      DEVICE_GENERIC_HEARTRATE         = 0,  // org.bluetooth.service.heart_rate          / HKQuantityTypeIdentifierHeartRate          / android.hardware.Sensor TYPE_HEART_RATE  (OR com.google.heart_rate from google fit)
      //DEVICE_GENERIC_RESPIRATION     = 1,  // -                                         / HKQuantityTypeIdentifierRespiratoryRate
      //DEVICE_GENERIC_THERMOMETER     = 2,  // org.bluetooth.service.health_thermometer  / HKQuantityTypeIdentifierBodyTemperature

      // Platform devices                    // Win/OSX/Linux              /  iOS               /  Android
      DEVICE_GENERIC_ACCELEROMETER     = 3,  // platform specific driver   /  CMMotionManager   /  android.hardware.Sensor TYPE_ACCELEROMETER
      DEVICE_GENERIC_GYROSCOPE         = 4,  // platform specific driver   /  CMMotionManager   /  android.hardware.Sensor TYPE_GYROSCOPE
      //DEVICE_GENERIC_GEOPOSITION     = 5,  // platform specific driver   /  CLLocation        /  android.location.LocationManager
      //DEVICE_GENERIC_AMBIENT_LIGHT   = 6,  // -                          /  (n/a. use camera) /  android.hardware.Sensor TYPE_LIGHT
      //DEVICE_GENERIC_AUDIO_IN        = 7,  // QT Audio                   /  AudioUnits ?      /  android.media.AudioRecord
      //DEVICE_GENERIC_AUDIO_OUT       = 8,  // QT Audio                   /  AudioUnits ?      /  android.media.AudioTrack  
      //DEVICE_GENERIC_CAMERA          = 9,  // OpenCV                     /  ?                 /  android.hardware.camera2

      // EEG devices
      DEVICE_INTERAXON_MUSE         = 10,
      DEVICE_EMOTIV_EPOC            = 11,
      DEVICE_EMOTIV_INSIGHT         = 12,
      DEVICE_NEUROSKY_MINDWAVE      = 13,
      DEVICE_SENSELABS_VESUS        = 14,

      // other devices
      DEVICE_ESENSE_SKINRESPONSE    = 15
   };

   enum EMuseInputs
   {
      MUSE_INPUT_EEG_TP9            = 0,  // = 220 Hz
      MUSE_INPUT_EEG_FP1            = 1,  // = 220 Hz
      MUSE_INPUT_EEG_FP2            = 2,  // = 220 Hz
      MUSE_INPUT_EEG_TP10           = 3,  // = 220 Hz
      MUSE_INPUT_CONCENTRATION      = 4,  // = 10 Hz
      MUSE_INPUT_MELLOW             = 5,  // = 10 Hz
      MUSE_INPUT_DRL                = 6,  // = 220 Hz
      MUSE_INPUT_REF                = 7,  // = 220 Hz
      MUSE_INPUT_ACC_FORWARD        = 8,  // = 50 Hz
      MUSE_INPUT_ACC_UP             = 9,  // = 50 Hz
      MUSE_INPUT_ACC_LEFT           = 10, // = 50 Hz
      MUSE_INPUT_EYEBLINK           = 11, // = 10 Hz
      MUSE_INPUT_JAWCLENCH          = 12, // = 10 Hz
      MUSE_INPUT_EEG_TP9_OK         = 13, // < 10 Hz // Note: Mapping from HorseShoe Values to our scale:
      MUSE_INPUT_EEG_FP1_OK         = 14, // < 10 Hz //       1.0  =>  1.0
      MUSE_INPUT_EEG_FP2_OK         = 15, // < 10 Hz //       2.0  =>  0.5
      MUSE_INPUT_EEG_TP10_OK        = 16, // < 10 Hz //      >3.0  =>  0.0
      MUSE_INPUT_TOUCHING_FOREHEAD  = 17, // < 10 Hz // Note: 1=true, 0=false
   };

   enum EGenericDeviceInputs
   {
      HEARTRATE_INPUT_BPM     = 0,  // <  10 Hz
      HEARTRATE_INPUT_RR      = 1,  //  - 

      ACCELEROMETER_INPUT_X   = 0,  // < 50 Hz		// TODO I need to know if we can get aequidistant sampled accelerometer data on iOS, or if the sample time jitteres (needed for integration (the mathematical kind))
      ACCELEROMETER_INPUT_Y   = 1,  // < 50 Hz
      ACCELEROMETER_INPUT_Z   = 2,  // < 50 Hz

      GYROSCOPE_INPUT_X = 0,        // < 50 Hz
      GYROSCOPE_INPUT_Y = 1,        // < 50 Hz
      GYROSCOPE_INPUT_Z = 2,        // < 50 Hz
   };

   enum eSenseSkinResponseInputs
   {
   ESENSE_SKINRESPONSE_INPUT = 0,
   };


   /**
   * Add a device of a certain type.
   * @return The index of the device in the internal device manager. -1 in case something failed.
   */
   NEUROMORE_EXPORT int AddDevice(enum EDevice type);

   /**
   * Get the number of active devices.
   * @return the number of devices that were added to the engine
   */
   NEUROMORE_EXPORT int GetNumDevices();

   /**
   * Remove a device.
   * This is only possible if the device exists and the engine is not running.
   * @return true if the device was removed successfully
   */
   NEUROMORE_EXPORT BOOL RemoveDevice(int deviceIndex);

   /**
   * Get the type of the active device with the given index
   * @return the number of devices that were added to the engine
   */
   NEUROMORE_EXPORT enum EDevice GetDevice(int deviceIndex);

   /**
   * If as at least one device was added to the engine
   * @return true if the device was added successfully
   */
   NEUROMORE_EXPORT BOOL HasDevice(enum EDevice type);

   NEUROMORE_EXPORT BOOL ConnectDevice(int deviceIndex);
   NEUROMORE_EXPORT BOOL DisonnectDevice(int deviceIndex);

   /**
   * Get the number of inputs of a device.
   * @return the number of inputs of the device. -1 will be returned if the device does not exist.
   */
   NEUROMORE_EXPORT int GetNumInputs(int deviceIndex);

   /**
   * Push a value into a device input.
   * Use this method to forward samples from input devices to the engine. It can be called concurrent to the update loop. 
   * Best practice: forward the input data as soon as possible and keep the latency as low as possible (especially if the sensor has high sample rates)
   */
   NEUROMORE_EXPORT BOOL AddInputSample(int deviceIndex, int inputIndex, double value);

   /**
   * Set the battery charge level of a device.
   * Forward the battery charge so it can be monitored by the engine. The engine will not start if the battery charge is too low.
   */
   NEUROMORE_EXPORT BOOL SetBatteryChargeLevel(int deviceIndex, double normalizedCharge);

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Classifier
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Load a classifier from the given JSON and activate it.
   * @param[in] jsonContent A string containing valid JSON content of a classifier.
   * @param[in] uuid The UUID of the file from the backend or a manually chosen UUID.
   * @param[in] revision The classifier file revision as delivered from the back-end.
   * @return True in case the classifier got loaded correctly, false in case an error happened.
   */
   NEUROMORE_EXPORT BOOL LoadClassifier(const char* jsonContent, const char* uuid, int revision);

   /*
   * Check if a classifier is present.
   * @return True in case a classifier was loaded, false if an error happened during loading OR if the engine is not initialized OR in case no classifier is declared as active
   */
   NEUROMORE_EXPORT BOOL HasClassifier();

   /**
   * Check if the active classifier requires the given device.
   * A classifier requires a given device, in case the device node is used inside the classifier.
   * Do not allow to Start() in case a device is missing. Show and wait for all required devices on the sensor waiting screen before allowing the user to start a session.
   * @return true if the device was added successfully
   */
   NEUROMORE_EXPORT BOOL IsDeviceRequiredByClassifier(enum EDevice deviceType);

   /**
   * Get the number of feedback nodes from the currently active classifier.
   * @result The number of custom feedback nodes in the classifier. -1 will be returned in case there is no active classifier.
   */
   NEUROMORE_EXPORT int GetNumFeedbacks();

   /**
   * Get the name of a feedback node
   * @result The name of the node. It can be empty.
   */
   NEUROMORE_EXPORT const char* GetFeedbackName(int index);

   NEUROMORE_EXPORT int FindFeedbackIndexByName(const char* name);

   /**
   * Get the current value from the given feedback node.
   * @param[in] The index of the custom feedback node from which we want to extract data. The index has to be in range of [0, GetNumFeedbackss()].
   * @return The current feedback value. In case no classifier is active or the index is invalid 0.0 will be returned.
   */
   NEUROMORE_EXPORT double GetCurrentFeedbackValue(int index);

   /**
   * Get the value range of a feedback
   * @param[in]  index		  The index of the custom feedback node from which we want to know the value range. The index has to be in range of [0, GetNumFeedbacks()].
   * @param[out] outMinValue The minimum value set in the node. In case no classifier is active or the index is invalid 0.0 will be filled.
   * @param[out] outMaxValue The maximum value set in the node. In case no classifier is active or the index is invalid 0.0 will be filled.
   */
   NEUROMORE_EXPORT void GetFeedbackRange(int index, double* outMinValue, double* outMaxValue);


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Cloud Data Serialization
   // A data chunk is one session or experience. It ranges from Start() to Stop().
   // Each data chunk contains several channels. Each channel needs to get uploaded via HTTP POST POST /api/datachunks/upload
   // Workflow:
   // 1. GetCreateDataChunkJson()
   // 2. POST /api/datachunks/create
   // 3. int numChannels = GetNumDataChunkChannels();
   // 4. FOR_EACH( data_chunk_channel [0, numChannels-1] )
   // 5.1		GetDataChunkChannelJson()
   // 5.2		GenerateDataChunkChannelData()
   // 5.3		GetDataChunkChannelData()
   // 5.4		GetDataChunkChannelDataSize()
   // 5.5		Remember json and data in target application
   // 5.6		ClearDataChunkChannelData()
   // 5.7		POST /api/datachunks/upload()
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Get the create data chunk json for POST /api/datachunks/create
   * @param userId The currently logged in user UUID.
   * @param experienceUuid The UUID from the experience that just got stopped.
   * @param experienceRevision The revision number of the experience that just got stopped.
   * @result The ready json string for POST /api/datachunks/create.
   */
   NEUROMORE_EXPORT const char* GetCreateDataChunkJson(const char* userId, const char* experienceUuid, int experienceRevision);

   /**
   * Get the number of data chunk channels for all feedback nodes that have data upload enabled.
   * @result The number of data chunk channels.
   */
   NEUROMORE_EXPORT int GetNumDataChunkChannels();

   // get the data chunk channel json for POST /api/datachunks/upload
   NEUROMORE_EXPORT const char* GetDataChunkChannelJson(const char* userId, const char* dataChunkUuid, int channelIndex);

   NEUROMORE_EXPORT BOOL GenerateDataChunkChannelData(int channelIndex);
   NEUROMORE_EXPORT const char* GetDataChunkChannelData(int channelIndex);
   NEUROMORE_EXPORT int GetDataChunkChannelDataSize(int channelIndex);

   NEUROMORE_EXPORT void ClearDataChunkChannelData();

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Classifier Parameters
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Get the number of controllable parameters in the classifier.
   * @return The number of parameters that can be controlled.
   */
   //int GetNumParameters();

   /**
   * Get the configuration of a controllable parameter
   * @param[in]  index				The Index of the parameter.
   * @param[out] outName			Name of the parameter.
   * @param[out] outDefaultValue	The default value of this parameter (current value after loading the classifier)
   * @param[out] outMinValue		The minimum value the parameter can be set to. Use this for minimum value for the slider.
   * @param[out] outMaxValue		The maximum value the parameter can be set to. Use this for minimum value for the slider.
   */
   //void GetParameterConfig(int index, const char* outName, double outDefaultValue, double* outMinValue, double* outMaxValue);

   /**
   * Set (update) a parameter value.
   * @param[in]  index		The index of the parameter.
   * @param[out] value		The value to set the parameter to. 
   */
   //void SetParameterValue(int index, double value);


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Classifier Cloud Parameters
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // TODO implement this in case we want to interact with the cloud parameter manually; for now we keep using the json generator methods
   /*int GetNumCloudInputs();
   BOOL GetCloudInputConfig(int index, const char* outName, int* outType, double* outTimeRange);
   void AddCloudInputValue(int index, double value);

   int GetNumCloudOutputs();
   BOOL GetCloudOutputConfig(int index, const char* outName, int* outType, int* outNumValues);
   double GetCloudOutputValue(int outputIndex, int valueIndex);*/

   /**
   * Create the JSON request string for retrieving the parameters from the backend via (POST /api/users/<id>/parameters/find)
   * The JSON reply you get from the backend must be handed back to the engine via HandleJSONReplyFindParameters(..)
   * Classifier Parameters must be retreived before every Session. If there are no parameters, an empty string is returned and nothing has to be done.
   * @param[in]	userId	The user ID string.
   * @return the JSON string or an empty string. Data is valid until a CreateJSONRequest method is called again.
   */
   NEUROMORE_EXPORT const char* CreateJSONRequestFindParameters(const char* userId);

   /**
   * Handle the reply of a find parameters JSON request.
   * The JSON is parsed and all input parameters of the classifier are updated with the current values.
   * @param[in]	jsonString	The JSON reply from a find parameters request.
   * @return True if the JSON was parsed successfully.
   */
   NEUROMORE_EXPORT BOOL HandleJSONReplyFindParameters(const char* jsonString, const char* userId);

   /**
   * Create the JSON request string for updating the parameters in the backend via (POST /api/users/<id>/parameters/set)
   * Parameters must be send to the backend after a session has completed successfully.  If there are no parameters, an empty string is returned and nothing has to be done.
   * @return the JSON string (valid until a CreateJSONRequest method is called again)
   */
   NEUROMORE_EXPORT const char* CreateJSONRequestSetParameters();

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // State Machine
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Load a state machine from the given JSON and activate it.
   * @param[in] jsonContent A string containing valid JSON content of a state machine.
   * @param[in] uuid The UUID of the file from the backend or a manually chosen UUID.
   * @param[in] revision The state machine file revision as delivered from the back-end.
   * @return True in case the state machine got loaded correctly, false in case an error happened.
   */
   NEUROMORE_EXPORT BOOL LoadStateMachine(const char* jsonContent, const char* uuid, int revision);

   /**
   * Check if a state machine is present.
   * @return True in case a state machine was loaded, false in case an error happened during loading or in case no state machine is declared as active.
   */
   NEUROMORE_EXPORT BOOL HasStateMachine();

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Multimedia Assets
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   enum AssetType
   {
      ASSET_NONE     = 0,
      ASSET_AUDIO    = 1,
      ASSET_VIDEO    = 2,
      ASSET_IMAGE    = 3
   };

   NEUROMORE_EXPORT int GetNumAssetsOfType(enum AssetType type);
   NEUROMORE_EXPORT const char* GetAssetLocationOfType(enum AssetType type, int index);
   NEUROMORE_EXPORT BOOL GetAssetAllowStreamingOfType(enum AssetType type, int index);

   NEUROMORE_EXPORT int GetNumAssets();
   NEUROMORE_EXPORT const char* GetAssetLocation(int index);
   NEUROMORE_EXPORT BOOL GetAssetAllowStreaming(int index);

   /**
   * Signal the engine that a video asset has finished playback.
   * Call this as soon as the playing video asset has reached the end of the file.
   */
   //void OnImageAssetFinishedDisplay(int videoAssetIndex);

   /**
   * Signal the engine that a video asset has finished playback.
   * Call this as soon as the playing video asset has reached the end of the file.
   */
   //void OnTextAssetFinishedDisplay(int videoAssetIndex);


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Input Events
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /**
   * Call this when a button got clicked.
   * As the engine doesn't know about any user interface elements, we need to let it know about any external input events.
   * Buttons created via Callback::OnShowButton() deliver a buttonId with the callback. Please feed back the button id to this input event when the given button got clicked.
   * @param[in] buttonId The button id of the button that got clicked.
   */
   NEUROMORE_EXPORT BOOL ButtonClicked(int buttonId);

   /**
   * Call this when an audio file looped.
   * As the engine doesn't know anything about the current status of the played audio files, we need to forward the information manually.
   * This is important for all audio files played via Callback::OnPlayAudio(). Please feed back the same audio url that got delivered for the OnPlayAudio() event.
   * @param[in] url The audio location of the audio file that looped.
   */
   NEUROMORE_EXPORT BOOL AudioLooped(const char* url);

   /**
   * Call this when an video file looped.
   * As the engine doesn't know anything about the current status of the played video files, we need to forward the information manually.
   * This is important for all video files played via Callback::OnPlayVideo(). Please feed back the same audio url that got delivered for the OnPlayVideo() event.
   * @param[in] url The videolocation of the audio file that looped.
   */
   NEUROMORE_EXPORT BOOL VideoLooped(const char* url);

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Callback - TODO: This is still C++
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   enum EStatus
   {
      STATUS_OK = 0,		// session completed successfully
      STATUS_DEVICE_ERROR = 1,		// aborted due to device error (no data/battery died)			-> user or app/lib fault
      STATUS_SIGNAL_ERROR = 2,		// aborted due to very bad signal quality						-> user or hardware fault
      STATUS_ENGINE_ERROR = 3,		// aborted due to internal engine error (thats the worst case)	-> app/engine fault
   };

#if defined(NEUROMORE_ENGINE_CPP_CALLBACK)
   /**
   * Async event callback interface for C++. DEPRECATED.
   * Derive from this class and customize your event responses.
   */
   class NEUROMORE_EXPORT Callback
   {
   public:
      Callback() {}
      virtual ~Callback() {}

      virtual void OnStop(EStatus status) = 0;
      virtual void OnLogBackend(const char* message, const char* logLevel, const char* logTime) = 0;
      virtual void OnLog(const char* message) = 0;

      virtual void OnError(int errorId, const char* message, const char* description) {}
      virtual void OnClearError(int errorId) {}
      virtual void OnWarning(int warningId, const char* message, const char* description) {}
      virtual void OnClearWarning(int warningId) {}
      virtual void OnInfo(int infoId, const char* message, const char* description) {}
      virtual void OnPlayAudio(const char* url, int numLoops, double beginAt=0.0, double volume=1.0) {}
      virtual void OnStopAudio(const char* url) {}
      virtual void OnPauseAudio(const char* url, bool unPause) {}
      virtual void OnSetAudioVolume(const char* url, double volume) {}
      virtual void OnSeekAudio(const char* url, int millisecs) {}
      virtual void OnPlayVideo(const char* url, int numLoops, double beginAt=0.0, double volume=1.0) {}
      virtual void OnStopVideo() {}
      virtual void OnPauseVideo(const char* url, bool unPause) {}
      virtual void OnSetVideoVolume(const char* url, double volume) {}
      virtual void OnSeekVideo(const char* url, int millisecs) {}
      virtual void OnShowImage(const char* url) {}
      virtual void OnHideImage() {}
      virtual void OnShowText(const char* text, float colorR, float colorG, float colorB, float colorA) {}
      virtual void OnHideText() {}
      virtual void OnShowButton(const char* text, int buttonId) {}
      virtual void OnClearButtons() {}
      virtual void OnSetBackgroundColor(float colorR, float colorG, float colorB, float colorA) {} 
      virtual void OnSetFourZoneAVEColors(const float* colorsR, const float* colorsG, const float* colorsB, const float* colorsA) {}
      virtual void OnHideFourZoneAVE() {}
      virtual void OnCommand(const char* command) {}
   };

   /**
   * Set the active C++ callback.
   */
   NEUROMORE_EXPORT void SetCallback(Callback* callback);

#else

   /**
   * Async event callback interface for C
   */
   struct NEUROMORE_EXPORT Callback
   {
      // called as soon as the session must stop (either session has reached the end or an unrecoverable error has happened either in the experience or the engine)
      void (*OnStop)(enum EStatus status);

      // backend logging: log this to backend. if it can't be sent immediately during a session: copy the values (or the final json) into a queue and push them after it finished
      void (*OnLogBackend)(const char* message, const char* logLevel, const char* logTime);

      // engine log output (info/debug/warning/error), collect this only locally
      void (*OnLog)(const char* message);

      // errors / warnings / infos
      void (*OnError)(int errorId, const char* message, const char* description);
      void (*OnClearError)(int errorId);
      void (*OnWarning)(int warningId, const char* message, const char* description);
      void (*OnClearWarning)(int warningId);
      void (*OnInfo)(int infoId, const char* message, const char* description);

      ////////////////////////////////////////////////
      // Multimedia Actions for State Machine
      ////////////////////////////////////////////////

      // Note: This is the most basic interface I can think that is still powerfull enough for our next goals.
      //  It allows us to play audio clips as well as background soundtracks, it can play fullscreen videos and overlay it with an image or text.
      //  The required render order from bottom to top is: Background Color -> Video -> Image -> Text. 
      //  If its not possible overlay text or image over a video, then hide the video in this case and only show it if no image or text is displayed.

      // Audio
      // note: multiple audio files can be played simulatneously, and each has an individual volume setting

      // play the audio starting at the specified time at specified volume (normalized 0..1). 
      // Note that it must be possible to play back multiple assets at the same time, but not the same one (Use one track for each audio asset).
      void (*OnPlayAudio)(const char* url, int numLoops, double beginAt, double volume);
      // stop audio playback of this asset
      void (*OnStopAudio)(const char* url);
      // pause or unpause the audio
      void (*OnPauseAudio)(const char* url, BOOL unPause);
      void (*OnSetAudioVolume)(const char* url, double volume);
      // seek audio file to specified position (in milliseconds)
      void (*OnSeekAudio)(const char* url, int millisecs);

      // Video
      // note: we play only one video at a time

      // play a video fullscreen starting at the specified time at specified volume (normalized 0..1). Ignore this event if another video is currently playing.
      void (*OnPlayVideo)(const char* url, int numLoops, double beginAt, double volume);
      // stop the playing video (there can be only one)
      void (*OnStopVideo)();
      // pause or unpause the video playback
      void (*OnPauseVideo)(const char* url, BOOL unPause);
      void (*OnSetVideoVolume)(const char* url, double volume);
      // seek video to specified position (in milliseconds)
      void (*OnSeekVideo)(const char* url, int millisecs);

      // Image
      void (*OnShowImage)(const char* url);// show the image fullscreen
      void (*OnHideImage)();// hide the image again

      // Text
      // display some text over everything. Should be aesthetically pleasing with a font that fits the text length (it can be everything from a single word to multiple sentences).
      void (*OnShowText)(const char* text, float colorR, float colorG, float colorB, float colorA);
      // hide the text again
      void (*OnHideText)();

      // buttons
      void (*OnShowButton)(const char* text, int buttonId);
      void (*OnClearButtons)();

      // background Color
      // sets the background color (first layer, draw beneath everything)
      void (*OnSetBackgroundColor)(float colorR, float colorG, float colorB, float colorA);

      // 4 zone AVE

      // update the eight colors of the eight-zone AVE display. Start rendering AVE display if not already.
      void (*OnSetFourZoneAVEColors)(const float* colorsR, const float* colorsG, const float* colorsB, const float* colorsA);
      // stop rendering AVE display
      void (*OnHideFourZoneAVE)();

      // command
      void (*OnCommand)(const char* command);
   };

   /**
   * Set the active callback.
   */
   NEUROMORE_EXPORT void SetCallback(struct Callback* callback);

#endif
#ifdef __cplusplus
   }
};
#endif
#endif
