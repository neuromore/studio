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

#include <neuromoreEngine.h>

#include <jni.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#ifdef ANDROID
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "neuromoreEngine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "neuromoreEngine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "neuromoreEngine", __VA_ARGS__))
#else
#define LOGI(...) 
#define LOGW(...) 
#define LOGE(...) 
#endif

// a persistent reference to the JavaVM instance
// used to retrieve the JNIEnv* from anywhere
static JavaVM* javavm;

/********************************************************/
/*    Implementation of the neuromore Callback class    */
/*      Will invoke the callbacks on java interface     */
/********************************************************/
class JNICallback : public neuromoreEngine::Callback
{
protected:
   jclass jcallback;
   jmethodID methodOnStop;
   jmethodID methodOnPlayAudio;
   jmethodID methodOnStopAudio;
   jmethodID methodOnPauseAudio;
   jmethodID methodOnSetAudioVolume;
   jmethodID methodOnSeekAudio;
   jmethodID methodOnPlayVideo;
   jmethodID methodOnStopVideo;
   jmethodID methodOnPauseVideo;
   jmethodID methodOnSetVideoVolume;
   jmethodID methodOnSeekVideo;
   jmethodID methodOnShowImage;
   jmethodID methodOnHideImage;
   jmethodID methodOnShowText;
   jmethodID methodOnHideText;
   jmethodID methodOnShowButton;
   jmethodID methodOnClearButtons;
   jmethodID methodOnCommand;

private:
   void callVoid(jmethodID method, ...)
   {
      if (!method)
         return;

      va_list args;
      JNIEnv* env;

      if (JNI_OK == javavm->AttachCurrentThread(&env, NULL))
      {
         va_start(args, method);
         env->CallVoidMethod(jcallback, method, args);
         va_end(args);
      }
   }

public:
   JNICallback(JNIEnv* env, jobject callback)
   {
      // turn java callback instance into a global ref so we can cache it
      jcallback = (jclass)env->NewGlobalRef(callback);

      if (!jcallback)
      {
         LOGE("Failed to convert callback instance into global ref.");
         return;
      }

      env->DeleteLocalRef(callback);

      // get the java class implementing this ICallback java interface
      jclass clazz = env->GetObjectClass(jcallback);

      if (!clazz)
      {
         LOGE("Failed to retrieve class of callback instance.");
         return;
      }

      // resolve callback methods from java class implementing callback interface
      methodOnStop           = env->GetMethodID(clazz, "OnStop",           "(I)V");
      methodOnPlayAudio      = env->GetMethodID(clazz, "OnPlayAudio",      "(Ljava/lang/String;IDD)V");
      methodOnStopAudio      = env->GetMethodID(clazz, "OnStopAudio",      "(Ljava/lang/String;)V");
      methodOnPauseAudio     = env->GetMethodID(clazz, "OnPauseAudio",     "(Ljava/lang/String;Z)V");
      methodOnSetAudioVolume = env->GetMethodID(clazz, "OnSetAudioVolume", "(Ljava/lang/String;D)V");
      methodOnSeekAudio      = env->GetMethodID(clazz, "OnSeekAudio",      "(Ljava/lang/String;I)V");
      methodOnPlayVideo      = env->GetMethodID(clazz, "OnPlayVideo",      "(Ljava/lang/String;IDD)V");
      methodOnStopVideo      = env->GetMethodID(clazz, "OnStopVideo",      "()V");
      methodOnPauseVideo     = env->GetMethodID(clazz, "OnPauseVideo",     "(Ljava/lang/String;Z)V");
      methodOnSetVideoVolume = env->GetMethodID(clazz, "OnSetVideoVolume", "(Ljava/lang/String;D)V");
      methodOnSeekVideo      = env->GetMethodID(clazz, "OnSeekVideo",      "(Ljava/lang/String;I)V");
      methodOnShowImage      = env->GetMethodID(clazz, "OnShowImage",      "(Ljava/lang/String;)V");
      methodOnHideImage      = env->GetMethodID(clazz, "OnHideImage",      "()V");
      methodOnShowText       = env->GetMethodID(clazz, "OnShowText",       "(Ljava/lang/String;FFFF)V");
      methodOnHideText       = env->GetMethodID(clazz, "OnHideText",       "()V");
      methodOnShowButton     = env->GetMethodID(clazz, "OnShowButton",     "(Ljava/lang/String;I)V");
      methodOnClearButtons   = env->GetMethodID(clazz, "OnClearButtons",   "()V");
      methodOnCommand        = env->GetMethodID(clazz, "OnCommand",        "(Ljava/lang/String;)V");

      // log about those not found
      if (!methodOnStop)           LOGW("Failed to find method 'OnStop' in callback instance.");
      if (!methodOnPlayAudio)      LOGW("Failed to find method 'OnPlayAudio' in callback instance.");
      if (!methodOnStopAudio)      LOGW("Failed to find method 'OnStopAudio' in callback instance.");
      if (!methodOnPauseAudio)     LOGW("Failed to find method 'OnPauseAudio' in callback instance.");
      if (!methodOnSetAudioVolume) LOGW("Failed to find method 'OnSetAudioVolume' in callback instance.");
      if (!methodOnSeekAudio)      LOGW("Failed to find method 'OnSeekAudio' in callback instance.");
      if (!methodOnPlayVideo)      LOGW("Failed to find method 'OnPlayVideo' in callback instance.");
      if (!methodOnStopVideo)      LOGW("Failed to find method 'OnStopVideo' in callback instance.");
      if (!methodOnPauseVideo)     LOGW("Failed to find method 'OnPauseVideo' in callback instance.");
      if (!methodOnSetVideoVolume) LOGW("Failed to find method 'OnSetVideoVolume' in callback instance.");
      if (!methodOnSeekVideo)      LOGW("Failed to find method 'OnSeekVideo' in callback instance.");
      if (!methodOnShowImage)      LOGW("Failed to find method 'OnShowImage' in callback instance.");
      if (!methodOnHideImage)      LOGW("Failed to find method 'OnHideImage' in callback instance.");
      if (!methodOnShowText)       LOGW("Failed to find method 'OnShowText' in callback instance.");
      if (!methodOnHideText)       LOGW("Failed to find method 'OnHideText' in callback instance.");
      if (!methodOnShowButton)     LOGW("Failed to find method 'OnShowButton' in callback instance.");
      if (!methodOnClearButtons)   LOGW("Failed to find method 'OnClearButtons' in callback instance.");
      if (!methodOnCommand)        LOGW("Failed to find method 'OnCommand' in callback instance.");
   }

   ~JNICallback()
   {
   }

   virtual void OnStop(EStatus status) override
   {
      JNIEnv* env;

      if (!methodOnStop || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnStop, (int)status);
   }

   virtual void OnLogBackend(const char* message, const char* logLevel, const char* logTime) override
   {
      LOGI(message);
   }

   virtual void OnLog(const char* message) override
   {
      LOGI(message);
   }

   ////////////////////////////////////////////////
   // Multimedia Actions for State Machine
   ////////////////////////////////////////////////

   virtual void OnPlayAudio(const char* url, int numLoops, double beginAt = 0.0, double volume = 1.0) override
   {
      JNIEnv* env;

      if (!methodOnPlayAudio || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnPlayAudio,
      env->NewStringUTF(url), numLoops, beginAt, volume);
   }

   virtual void OnStopAudio(const char* url) override
   {
      JNIEnv* env;

      if (!methodOnStopAudio || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnStopAudio,
      env->NewStringUTF(url));
   }

   virtual void OnPauseAudio(const char* url, bool unPause) override
   {
      JNIEnv* env;

      if (!methodOnPauseAudio || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnPauseAudio,
      env->NewStringUTF(url), unPause);
   }

   virtual void OnSetAudioVolume(const char* url, double volume) override
   {
      JNIEnv* env;

      if (!methodOnSetAudioVolume || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnSetAudioVolume,
      env->NewStringUTF(url), volume);
   }

   virtual void OnSeekAudio(const char* url, int millisecs) override
   {
      JNIEnv* env;

      if (!methodOnSeekAudio || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnSeekAudio,
      env->NewStringUTF(url), millisecs);
   }

   virtual void OnPlayVideo(const char* url, int numLoops, double beginAt = 0.0, double volume = 1.0) override
   {
      JNIEnv* env;

      if (!methodOnPlayVideo || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnPlayVideo,
      env->NewStringUTF(url), numLoops, beginAt, volume);
   }

   virtual void OnStopVideo() override
   {
      JNIEnv* env;

      if (!methodOnStopVideo || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnStopVideo);
   }

   virtual void OnPauseVideo(const char* url, bool unPause) override
   {
      JNIEnv* env;

      if (!methodOnPauseVideo || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnPauseVideo,
      env->NewStringUTF(url), unPause);
   }

   virtual void OnSetVideoVolume(const char* url, double volume) override
   {
      JNIEnv* env;

      if (!methodOnSetVideoVolume || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnSetVideoVolume,
      env->NewStringUTF(url), volume);
   }

   virtual void OnSeekVideo(const char* url, int millisecs) override
   {
      JNIEnv* env;

      if (!methodOnSeekVideo || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnSeekVideo,
      env->NewStringUTF(url), millisecs);
   }

   virtual void OnShowImage(const char* url) override
   {
      JNIEnv* env;

      if (!methodOnShowImage || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnShowImage,
      env->NewStringUTF(url));
   }

   virtual void OnHideImage() override
   {
      JNIEnv* env;

      if (!methodOnHideImage || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnHideImage);
   }

   virtual void OnShowText(const char* text, float colorR, float colorG, float colorB, float colorA) override
   {
      JNIEnv* env;

      if (!methodOnShowText || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnShowText,
      env->NewStringUTF(text), colorR, colorG, colorB, colorA);
   }

   virtual void OnHideText() override
   {
      JNIEnv* env;

      if (!methodOnHideText || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnHideText);
   }

   virtual void OnShowButton(const char* text, int buttonId) override
   {
      JNIEnv* env;

      if (!methodOnShowButton || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnShowButton,
      env->NewStringUTF(text), buttonId);
   }

   virtual void OnClearButtons() override
   {
      JNIEnv* env;

      if (!methodOnClearButtons || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnClearButtons);
   }

   virtual void OnCommand(const char* command) override
   {
      JNIEnv* env;

      if (!methodOnCommand || (JNI_OK != javavm->AttachCurrentThread(&env, NULL)))
         return;

      env->CallVoidMethod(jcallback, methodOnCommand,
      env->NewStringUTF(command));
   }
};

// stores an instance of our callback implementation
// note: this is destroyed by the neuromore shutdown.
static JNICallback* callback;

/***********************************************************************************************************************************************************************/
/********************************************************************    JNI API     ***********************************************************************************/
/***********************************************************************************************************************************************************************/
extern "C" 
{
   jint JNI_OnLoad(JavaVM* jvm, void* reserved)
   {
      LOGI("JNI starting up..");

      // keep a global reference to the jvm
      // so we can get a JNIEnv* from anywhere (can't cache JNIEnv*)
      javavm = jvm;

      // define jni interface version
      return JNI_VERSION_1_6;
   }

   void JNI_OnUnload(JavaVM* jvm, void* reserved)
   {
      // cleanup if necessary
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Engine initialization
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jboolean Java_com_neuromore_engine_Wrapper_SetPowerLineFrequencyType(JNIEnv* env, jobject thiz, jint powerLineFrequencyType)
   {
      return neuromoreEngine::SetPowerLineFrequencyType((neuromoreEngine::EPowerLineFrequencyType)powerLineFrequencyType);
   }

   jdouble Java_com_neuromore_engine_Wrapper_GetPowerLineFrequency(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::GetPowerLineFrequency();
   }

   jboolean Java_com_neuromore_engine_Wrapper_SetBufferLength(JNIEnv* env, jobject thiz, jdouble seconds)
   {
      return neuromoreEngine::SetBufferLength(seconds);
   }

   void Java_com_neuromore_engine_Wrapper_Init(JNIEnv* env, jobject thiz, jobject jcallback)
   {
      // no need to init twice
      if (neuromoreEngine::IsInitialized())
         return;

      neuromoreEngine::Init();

      // init didn't work :(
      if (!neuromoreEngine::IsInitialized())
         return;

      // create a callback handler and set it
      callback = new JNICallback(env, jcallback);
      neuromoreEngine::SetCallback(callback);

      // log
      callback->OnLog("Initialization successful");
   }

   jboolean Java_com_neuromore_engine_Wrapper_IsInitialized(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::IsInitialized();
   }

   void Java_com_neuromore_engine_Wrapper_Shutdown(JNIEnv* env, jobject thiz)
   {
      if (!neuromoreEngine::IsInitialized())
         return;

      // this is going to be destroyed by shutdown,
      // so make sure we forget it
      callback = NULL;

      neuromoreEngine::Shutdown();
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Engine basics: Start, Stop, Cleanup, Update etc
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jboolean Java_com_neuromore_engine_Wrapper_IsReady(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::IsReady();
   }

   void Java_com_neuromore_engine_Wrapper_SetSessionLength(JNIEnv* env, jobject thiz, jdouble seconds)
   {
      neuromoreEngine::SetSessionLength(seconds);
   }

   jboolean Java_com_neuromore_engine_Wrapper_Start(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::Start();
   }

   jboolean Java_com_neuromore_engine_Wrapper_StartThreaded(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::StartThreaded();
   }

   jboolean Java_com_neuromore_engine_Wrapper_Update(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::Update();
   }

   jboolean Java_com_neuromore_engine_Wrapper_IsRunning(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::IsRunning();
   }

   jboolean Java_com_neuromore_engine_Wrapper_Stop(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::Stop();
   }

   jboolean Java_com_neuromore_engine_Wrapper_StopThreaded(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::StopThreaded();
   }

   void Java_com_neuromore_engine_Wrapper_Reset(JNIEnv* env, jobject thiz)
   {
      neuromoreEngine::Reset();
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Helpers
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   void Java_com_neuromore_engine_Wrapper_EnableDebugLogging(JNIEnv* env, jobject thiz)
   {
      neuromoreEngine::EnableDebugLogging();
   }

   void Java_com_neuromore_engine_Wrapper_SetAllowAssetStreaming(JNIEnv* env, jobject thiz, jboolean allow)
   {
      neuromoreEngine::SetAllowAssetStreaming(allow);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Devices
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jint Java_com_neuromore_engine_Wrapper_AddDevice(JNIEnv* env, jobject thiz, jint deviceType)
   {
      return neuromoreEngine::AddDevice((neuromoreEngine::EDevice)deviceType);
   }

   jint Java_com_neuromore_engine_Wrapper_GetNumDevices(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::GetNumDevices();
   }

   jboolean Java_com_neuromore_engine_Wrapper_RemoveDevice(JNIEnv* env, jobject thiz, jint deviceIndex)
   {
      return neuromoreEngine::RemoveDevice(deviceIndex);
   }

   jint Java_com_neuromore_engine_Wrapper_GetDevice(JNIEnv* env, jobject thiz, jint deviceIndex)
   {
      return neuromoreEngine::GetDevice(deviceIndex);
   }

   jboolean Java_com_neuromore_engine_Wrapper_HasDevice(JNIEnv* env, jobject thiz, jint deviceType)
   {
      return neuromoreEngine::HasDevice((neuromoreEngine::EDevice)deviceType);
   }

   jboolean Java_com_neuromore_engine_Wrapper_ConnectDevice(JNIEnv* env, jobject thiz, jint deviceIndex)
   {
      return neuromoreEngine::ConnectDevice(deviceIndex);
   }

   jboolean Java_com_neuromore_engine_Wrapper_DisconnectDevice(JNIEnv* env, jobject thiz, jint deviceIndex)
   {
      return neuromoreEngine::DisonnectDevice(deviceIndex);
   }

   jint Java_com_neuromore_engine_Wrapper_GetNumInputs(JNIEnv* env, jobject thiz, jint deviceIndex)
   {
      return neuromoreEngine::GetNumInputs(deviceIndex);
   }

   jboolean Java_com_neuromore_engine_Wrapper_AddInputSample(JNIEnv* env, jobject thiz, jint deviceIndex, jint inputIndex, jdouble value)
   {
      return neuromoreEngine::AddInputSample(deviceIndex, inputIndex, value);
   }

   jboolean Java_com_neuromore_engine_Wrapper_SetBatteryChargeLevel(JNIEnv* env, jobject thiz, jint deviceIndex, jdouble normalizedCharge)
   {
      return neuromoreEngine::SetBatteryChargeLevel(deviceIndex, normalizedCharge);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Classifier
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jboolean Java_com_neuromore_engine_Wrapper_LoadClassifier(JNIEnv* env, jobject thiz, jstring jsonContent, jstring uuid, jint revision)
   {
      const char* cstr_jsonContent = env->GetStringUTFChars(jsonContent, NULL);
      const char* cstr_uuid = env->GetStringUTFChars(uuid, NULL);

      bool ok = neuromoreEngine::LoadClassifier(cstr_jsonContent, cstr_uuid, revision);

      env->ReleaseStringUTFChars(jsonContent, cstr_jsonContent);
      env->ReleaseStringUTFChars(uuid, cstr_uuid);

      return ok;
   }

   jboolean Java_com_neuromore_engine_Wrapper_HasClassifier(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::HasClassifier();
   }

   jboolean Java_com_neuromore_engine_Wrapper_IsDeviceRequiredByClassifier(JNIEnv* env, jobject thiz, jint deviceType)
   {
      return neuromoreEngine::IsDeviceRequiredByClassifier((neuromoreEngine::EDevice)deviceType);
   }

   jint Java_com_neuromore_engine_Wrapper_GetNumFeedbacks(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::GetNumFeedbacks();
   }

   jstring Java_com_neuromore_engine_Wrapper_GetFeedbackName(JNIEnv* env, jobject thiz, jint index)
   {
      return env->NewStringUTF(neuromoreEngine::GetFeedbackName(index));
   }

   jdouble Java_com_neuromore_engine_Wrapper_GetCurrentFeedbackValue(JNIEnv* env, jobject thiz, jint index)
   {
      return neuromoreEngine::GetCurrentFeedbackValue(index);
   }

   /*void Java_com_neuromore_engine_Wrapper_GetFeedbackRange(JNIEnv* env, jobject thiz, jint index, jdoubleArray values)
   {
      env->doublearray
      //neuromoreEngine::GetFeedbackRange(index, values, value);
   }*/

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Cloud Data Serialization
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jstring Java_com_neuromore_engine_Wrapper_GetCreateDataChunkJson(JNIEnv* env, jobject thiz, jstring userId, jstring experienceUuid, jint experienceRevision)
   {
      const char* cstr_userId = env->GetStringUTFChars(userId, NULL);
      const char* cstr_experienceUuid = env->GetStringUTFChars(experienceUuid, NULL);

      const char* cstr_req = neuromoreEngine::GetCreateDataChunkJson(cstr_userId, cstr_experienceUuid, experienceRevision);

      env->ReleaseStringUTFChars(userId, cstr_userId);
      env->ReleaseStringUTFChars(experienceUuid, cstr_experienceUuid);

      return env->NewStringUTF(cstr_req);
   }

   jint Java_com_neuromore_engine_Wrapper_GetNumDataChunkChannels(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::GetNumDataChunkChannels();
   }

   jstring Java_com_neuromore_engine_Wrapper_GetDataChunkChannelJson(JNIEnv* env, jobject thiz, jstring userId, jstring dataChunkUuid, jint channelIndex)
   {
      const char* cstr_userId = env->GetStringUTFChars(userId, NULL);
      const char* cstr_dataChunkUuid = env->GetStringUTFChars(dataChunkUuid, NULL);

      const char* cstr_req = neuromoreEngine::GetDataChunkChannelJson(cstr_userId, cstr_dataChunkUuid, channelIndex);

      env->ReleaseStringUTFChars(userId, cstr_userId);
      env->ReleaseStringUTFChars(dataChunkUuid, cstr_dataChunkUuid);

      return env->NewStringUTF(cstr_req);
   }

   jbyteArray Java_com_neuromore_engine_Wrapper_GetDataChunkChannelData(JNIEnv* env, jobject thiz, jint channelIndex)
   {
      // try generate datachunk for this channel
      if (!neuromoreEngine::GenerateDataChunkChannelData(channelIndex))
         return env->NewByteArray(0);

      // try get size and native data
      const int   size = neuromoreEngine::GetDataChunkChannelDataSize(channelIndex);
      const char* data = neuromoreEngine::GetDataChunkChannelData(channelIndex);

      // invalid size or data
      if (size <= 0 || !data)
         return env->NewByteArray(0);

      // allocate java byte array
      jbyteArray jdata = env->NewByteArray(size);

      // this will copy data from native ptr to java byte array
      env->SetByteArrayRegion(jdata, 0, size, (jbyte*)data);

      // clear native data
      neuromoreEngine::ClearDataChunkChannelData();

      return jdata;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Classifier Cloud Parameters
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jstring Java_com_neuromore_engine_Wrapper_CreateJSONRequestFindParameters(JNIEnv* env, jobject thiz, jstring userId)
   {
      const char* cstr_userId = env->GetStringUTFChars(userId, NULL);	
      const char* cstr_req = neuromoreEngine::CreateJSONRequestFindParameters(cstr_userId);
      env->ReleaseStringUTFChars(userId, cstr_userId);

      return env->NewStringUTF(cstr_req);
   }

   jboolean Java_com_neuromore_engine_Wrapper_HandleJSONReplyFindParameters(JNIEnv* env, jobject thiz, jstring jsonString, jstring userId)
   {
      const char* cstr_jsonString = env->GetStringUTFChars(jsonString, NULL);
      const char* cstr_userId = env->GetStringUTFChars(userId, NULL);

      bool ok = neuromoreEngine::HandleJSONReplyFindParameters(cstr_jsonString, cstr_userId);

      env->ReleaseStringUTFChars(jsonString, cstr_jsonString);
      env->ReleaseStringUTFChars(userId, cstr_userId);

      return ok;
   }

   jstring Java_com_neuromore_engine_Wrapper_CreateJSONRequestSetParameters(JNIEnv* env, jobject thiz)
   {
      return env->NewStringUTF(neuromoreEngine::CreateJSONRequestSetParameters());
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // State Machine
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jboolean Java_com_neuromore_engine_Wrapper_LoadStateMachine(JNIEnv* env, jobject thiz, jstring jsonContent, jstring uuid, jint revision)
   {
      const char* cstr_jsonContent = env->GetStringUTFChars(jsonContent, NULL);
      const char* cstr_uuid = env->GetStringUTFChars(uuid, NULL);

      bool ok = neuromoreEngine::LoadStateMachine(cstr_jsonContent, cstr_uuid, revision);

      env->ReleaseStringUTFChars(jsonContent, cstr_jsonContent);
      env->ReleaseStringUTFChars(uuid, cstr_uuid);

      return ok;
   }

   jboolean Java_com_neuromore_engine_Wrapper_HasStateMachine(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::HasStateMachine();
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Multimedia Assets
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   jint Java_com_neuromore_engine_Wrapper_GetNumAssetsOfType(JNIEnv* env, jobject thiz, jint type)
   {
      return neuromoreEngine::GetNumAssetsOfType((neuromoreEngine::AssetType)type);
   }

   jstring Java_com_neuromore_engine_Wrapper_GetAssetLocationOfType(JNIEnv* env, jobject thiz, jint type, jint index)
   {
      return env->NewStringUTF(neuromoreEngine::GetAssetLocationOfType((neuromoreEngine::AssetType)type, index));
   }

   jboolean Java_com_neuromore_engine_Wrapper_GetAssetAllowStreamingOfType(JNIEnv* env, jobject thiz, jint type, jint index)
   {
      return neuromoreEngine::GetAssetAllowStreamingOfType((neuromoreEngine::AssetType)type, index);
   }

   jint Java_com_neuromore_engine_Wrapper_GetNumAssets(JNIEnv* env, jobject thiz)
   {
      return neuromoreEngine::GetNumAssets();
   }

   jstring Java_com_neuromore_engine_Wrapper_GetAssetLocation(JNIEnv* env, jobject thiz, jint index)
   {
      return env->NewStringUTF(neuromoreEngine::GetAssetLocation(index));
   }

   jboolean Java_com_neuromore_engine_Wrapper_GetAssetAllowStreaming(JNIEnv* env, jobject thiz, jint index)
   {
      return neuromoreEngine::GetAssetAllowStreaming(index);
   }

   jboolean Java_com_neuromore_engine_Wrapper_ButtonClicked(JNIEnv* env, jobject thiz, jint buttonId)
   {
      return neuromoreEngine::ButtonClicked(buttonId);
   }

   jboolean Java_com_neuromore_engine_Wrapper_AudioLooped(JNIEnv* env, jobject thiz, jstring url)
   {
      const char* cstr_url = env->GetStringUTFChars(url, NULL);

      bool ok = neuromoreEngine::AudioLooped(cstr_url);

      env->ReleaseStringUTFChars(url, cstr_url);

      return ok;
   }

   jboolean Java_com_neuromore_engine_Wrapper_VideoLooped(JNIEnv* env, jobject thiz, jstring url)
   {
      const char* cstr_url = env->GetStringUTFChars(url, NULL);

      bool ok = neuromoreEngine::VideoLooped(cstr_url);

      env->ReleaseStringUTFChars(url, cstr_url);

      return ok;
   }
}
