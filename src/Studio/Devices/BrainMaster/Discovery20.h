#pragma once

#ifdef INCLUDE_DEVICE_BRAINMASTER

// WINDOWS X86 ONLY
#if defined(_WIN32) && (defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(__i686__))

// C++ Standards
#include <cstdint>
#include <iostream>
#include <cassert>
#include <chrono>

// Windows API
#include <windows.h> 

/// <summary>
/// Discovery 20 EEG Amplifier.
/// Requires installed device drivers (FTDI Serial over USB) and the bmrcm.dll.
/// </summary>
class Discovery20
{
public:
   /// <summary>
   /// Runtime DLL Wrapper
   /// </summary>
   class SDK
   {
   public:
      static constexpr TCHAR NAME[] = TEXT("bmrcm.dll");
   protected:
      typedef BOOL  (__cdecl* FuncAtlOpenPort)            (int32_t portno, int32_t baudrate, HANDLE* h);
      typedef BOOL  (__cdecl* FuncAtlClosePort)           (int32_t portno);
      typedef BOOL  (__cdecl* FuncAtlSetBaudRate)         (int32_t ratecode);
      typedef BOOL  (__cdecl* FuncAtlWriteSamplingRate)   (int32_t samplingrate);
      typedef BOOL  (__cdecl* FuncAtlClearSpecials)       ();
      typedef void  (__cdecl* FuncAtlFlush)               ();
      typedef int   (__cdecl* FuncBmrLoginDevice)         (char* codekey, char* serialnumber, char* passkey);
      typedef BOOL  (__cdecl* FuncDiscStartModule)        ();
      typedef BOOL  (__cdecl* FuncDiscStopModule)         ();
      typedef DWORD (__cdecl* FuncAtlGetBytesInQue)       ();
      typedef int   (__cdecl* FuncAtlReadData)            (uint8_t* buffer, int32_t count);
      typedef BOOL  (__cdecl* FuncAtlSelectImpedanceChans)(int selectcode);
      typedef BOOL  (__cdecl* FuncAtlSelectSpecial)       (int selectcode);
      typedef BOOL  (__cdecl* FuncAtlSetNotchFilters)     (int selectcode);
      typedef int   (__cdecl* FuncAtlPeek)                (uint16_t location);
      typedef int   (__cdecl* FuncAtlPoke)                (uint16_t location, uint16_t data);
      typedef int   (__cdecl* FuncAtlQueryFirmware)       (int32_t authorization);

   public:
      enum BRCodes : int32_t {
         BR460800 = 0x10,
         BR115200 = 0x20,
         BR9600   = 0x30
      };
      enum LoginCodes : int32_t {
         EARLY2E = 11,
         READY2E = 12,
         WIDEB2E = 13
      };
   public:
      HMODULE                     Handle;
      FuncAtlOpenPort             AtlOpenPort;
      FuncAtlClosePort            AtlClosePort;
      FuncAtlSetBaudRate          AtlSetBaudRate;
      FuncAtlWriteSamplingRate    AtlWriteSamplingRate;
      FuncAtlClearSpecials        AtlClearSpecials;
      FuncAtlFlush                AtlFlush;
      FuncBmrLoginDevice          BmrLoginDevice;
      FuncDiscStartModule         DiscStartModule;
      FuncDiscStopModule          DiscStopModule;
      FuncAtlGetBytesInQue        AtlGetBytesInQue;
      FuncAtlReadData             AtlReadData;
      FuncAtlSelectImpedanceChans AtlSelectImpedanceChans;
      FuncAtlSelectSpecial        AtlSelectSpecial;
      FuncAtlSetNotchFilters      AtlSetNotchFilters;
      FuncAtlPeek                 AtlPeek;
      FuncAtlPoke                 AtlPoke;
      FuncAtlQueryFirmware        AtlQueryFirmware;

   public:
      inline SDK() :
         Handle(LoadLibrary(NAME)),
         AtlOpenPort            (Handle ? (FuncAtlOpenPort)             GetProcAddress(Handle, "AtlOpenPort")             : 0),
         AtlClosePort           (Handle ? (FuncAtlClosePort)            GetProcAddress(Handle, "AtlClosePort")            : 0),
         AtlSetBaudRate         (Handle ? (FuncAtlSetBaudRate)          GetProcAddress(Handle, "AtlSetBaudRate")          : 0),
         AtlWriteSamplingRate   (Handle ? (FuncAtlWriteSamplingRate)    GetProcAddress(Handle, "AtlWriteSamplingRate")    : 0),
         AtlClearSpecials       (Handle ? (FuncAtlClearSpecials)        GetProcAddress(Handle, "AtlClearSpecials")        : 0),
         AtlFlush               (Handle ? (FuncAtlFlush)                GetProcAddress(Handle, "AtlFlush")                : 0),
         BmrLoginDevice         (Handle ? (FuncBmrLoginDevice)          GetProcAddress(Handle, "BmrLoginDevice")          : 0),
         DiscStartModule        (Handle ? (FuncDiscStartModule)         GetProcAddress(Handle, "DiscStartModule")         : 0),
         DiscStopModule         (Handle ? (FuncDiscStopModule)          GetProcAddress(Handle, "DiscStopModule")          : 0),
         AtlGetBytesInQue       (Handle ? (FuncAtlGetBytesInQue)        GetProcAddress(Handle, "AtlGetBytesInQue")        : 0),
         AtlReadData            (Handle ? (FuncAtlReadData)             GetProcAddress(Handle, "AtlReadData")             : 0),
         AtlSelectImpedanceChans(Handle ? (FuncAtlSelectImpedanceChans) GetProcAddress(Handle, "AtlSelectImpedanceChans") : 0),
         AtlSelectSpecial       (Handle ? (FuncAtlSelectSpecial)        GetProcAddress(Handle, "AtlSelectSpecial")        : 0),
         AtlSetNotchFilters     (Handle ? (FuncAtlSetNotchFilters)      GetProcAddress(Handle, "AtlSetNotchFilters")      : 0),
         AtlPeek                (Handle ? (FuncAtlPeek)                 GetProcAddress(Handle, "AtlPeek")                 : 0),
         AtlPoke                (Handle ? (FuncAtlPoke)                 GetProcAddress(Handle, "AtlPoke")                 : 0),
         AtlQueryFirmware       (Handle ? (FuncAtlQueryFirmware)        GetProcAddress(Handle, "AtlQueryFirmware")        : 0)
      {
      }
      inline ~SDK()
      {
         if (Handle)
         {
            FreeLibrary(Handle);
            Handle                  = 0;
            AtlOpenPort             = 0;
            AtlClosePort            = 0;
            AtlSetBaudRate          = 0;
            AtlWriteSamplingRate    = 0;
            AtlClearSpecials        = 0;
            AtlFlush                = 0;
            BmrLoginDevice          = 0;
            DiscStartModule         = 0;
            DiscStopModule          = 0;
            AtlGetBytesInQue        = 0;
            AtlReadData             = 0;
            AtlSelectImpedanceChans = 0;
            AtlSelectSpecial        = 0;
            AtlSetNotchFilters      = 0;
            AtlPeek                 = 0;
            AtlPoke                 = 0;
            AtlQueryFirmware        = 0;
         }
      }
   };

   /// <summary>
   /// 24 Floating Point Channel Values
   /// </summary>
   struct Channels
   {
      static constexpr size_t SIZE = 24;
      union {
         float data[SIZE];
         struct {
            float Fp1;   // ch01
            float F3;    // ch02
            float C3;    // ch03
            float P3;    // ch04
            float O1;    // ch05
            float F7;    // ch06
            float T3;    // ch07
            float T5;    // ch08
            float Fz;    // ch09
            float Fp2;   // ch10
            float F4;    // ch11
            float C4;    // ch12
            float P4;    // ch13
            float O2;    // ch14
            float F8;    // ch15
            float T4;    // ch16
            float T6;    // ch17
            float Cz;    // ch18
            float Pz;    // ch19
            float A2;    // ch20
            float Fpz;   // ch21
            float Oz;    // ch22
            float AUX23; // ch23
            float AUX24; // ch24
         };
      };
   };

   #pragma pack (push, 1)
   /// <summary>
   /// 24-Bit Signed Integer
   /// </summary>
   struct int24_t
   {
      uint16_t l;
      uint8_t  h;
      static constexpr size_t SIZE = 3U;
      static constexpr uint32_t M1 = 1U << (24U - 1U);
      static constexpr uint32_t M2 = (1U << 24) - 1U;
      // sign extended to 32-bit
      inline int32_t get() const { 
         return ((*(uint32_t*)this & M2) ^ M1) - M1;
      }
      // just the plain 24-bits
      inline uint32_t getBits() const { 
         return *(uint32_t*)this & 0x00FFFFFFU;
      }
   };

   /// <summary>
   /// Discovery 20 Data Frame
   /// </summary>
   struct Frame
   {
      static constexpr size_t SIZE = 78;
      static constexpr float CONVERTUV = 0.01658f;
      union {
         uint8_t data[SIZE];
         struct {
            uint8_t  sync;
            uint8_t  unused1;
            uint8_t  unused2;
            uint8_t  steering;
            uint16_t specialdata;
            int24_t  channels[Channels::SIZE];
         };
      };
      inline void extract(Channels& ch)
      {
         for (size_t i = 0; i < Channels::SIZE; i++) {
            ch.data[i] = (float)channels[i].get() * CONVERTUV;
         }
      }
   };
   #pragma pack(pop)

   /// <summary>
   /// Validate int24_t Size
   /// </summary>
   static_assert(sizeof(int24_t) == int24_t::SIZE);

   /// <summary>
   /// Validate Frame Size
   /// </summary>
   static_assert(sizeof(Frame) == Frame::SIZE);

   /// <summary>
   /// Callback Interface
   /// </summary>
   class Callback
   {
   public:
      inline virtual void onLoadSDKSuccess(HMODULE h)                    { }
      inline virtual void onLoadSDKFail()                                { }
      inline virtual void onDeviceFound(int32_t port)                    { }
      inline virtual void onDeviceConnected()                            { }
      inline virtual void onDeviceDisconnected()                         { }
      inline virtual void onDeviceTimeout()                              { }
      inline virtual void onSyncStart(uint8_t c1, uint8_t c2)            { }
      inline virtual void onSyncSuccess()                                { }
      inline virtual void onSyncFail(uint8_t expected, uint8_t received) { }
      inline virtual void onSyncLost()                                   { }
      inline virtual void onFrame(const Frame& f, const Channels& c)     { }
   };

   /// <summary>
   /// States
   /// </summary>
   enum class State {
      DISCONNECTED = 0, // not connected
      CONNECTED    = 1, // connected
      UNSYNCED     = 2, // streaming: unsynced
      SYNCING      = 3, // streaming: syncing
      SYNCED       = 4, // streaming: synced
   };

   /// <summary>
   /// Required successful sync byte iterations before frames
   /// are assumed to be safe enough for propagation.
   /// </summary>
   static constexpr uint32_t MINSYNCS = 4U;

   /// <summary>
   /// Sample Rate used on the Device
   /// </summary>
   static constexpr uint32_t SAMPLERATE = 256U;

   /// <summary>
   /// Size of Serial Port Receive and Send Buffers in Bytes
   /// </summary>
   static constexpr uint32_t BUFFERSIZE = 1024U * 1024U;

   /// <summary>
   /// Timeout for receiving data in streaming state (in milliseconds).
   /// </summary>
   static constexpr uint64_t TIMEOUT = 3000;

protected:
   SDK       mSDK;
   State     mState;
   int32_t   mPort;
   HANDLE    mHandle;
   uint32_t  mNumSyncs;
   uint32_t  mNumBytes;
   uint8_t   mNextSync;
   uint64_t  mTickLastData;
   union {
     Frame   mFrame;
     uint8_t mBuffer[Frame::SIZE*2];
   };
   Channels  mChannels;
   Callback& mCallback;

   /// <summary>
   /// Returns next sync byte for sync byte s
   /// </summary>
   static inline uint8_t nextSync(const uint8_t s)
   {
      return  (s == 0xe0) ? 0x20 : s + 0x20;
   }

   /// <summary>
   /// True if b is next valid sync byte of a
   /// </summary>
   static inline bool isSyncPair(const uint8_t a, const uint8_t b)
   {
      return
         (a == 0x20 && b == 0x40) ||
         (a == 0x40 && b == 0x60) ||
         (a == 0x60 && b == 0x80) ||
         (a == 0x80 && b == 0xa0) ||
         (a == 0xa0 && b == 0xc0) ||
         (a == 0xc0 && b == 0xe0) ||
         (a == 0xe0 && b == 0x20);
   }

   /// <summary>
   /// Increments the sync byte to next expected sync byte
   /// and the sync counter by one.
   /// </summary>
   inline void stepSync() 
   {
      mNumSyncs++;
      mNextSync = nextSync(mNextSync);
   }

   /// <summary>
   /// Tries to find the COM port of the Discovery 20.
   /// Returns 0 if not found.
   /// </summary>
   int findCOM();

   /// <summary>
   /// Processes bytes (up to one frame) from the device queue.
   /// </summary>
   void processQueue();

   /// <summary>
   /// Gets a high precision clock tick.
   /// </summary>
   inline static uint64_t gettick()
   {
      return std::chrono::high_resolution_clock::now().time_since_epoch().count();
   }

public:
   /// <summary>
   /// Constructor
   /// </summary>
   inline Discovery20(Callback& cb) :
      mSDK(),
      mState(State::DISCONNECTED),
      mPort(0),
      mHandle(0),
      mNumSyncs(0),
      mNumBytes(0),
      mNextSync(0),
      mTickLastData(0),
      mBuffer(),
      mChannels(),
      mCallback(cb) 
   {
      if (mSDK.Handle) mCallback.onLoadSDKSuccess(mSDK.Handle);
      else mCallback.onLoadSDKFail();
   }

   /// <summary>
   /// Destructor
   /// </summary>
   inline ~Discovery20()
   {
      disconnect();
   }

   /// <summary>
   /// Returns COM port number of the device if successfully discoverd, else 0.
   /// </summary>
   inline const int32_t getPort() const { return mPort; }

   /// <summary>
   /// Get current state
   /// </summary>
   inline const State& getState() const { return mState; }

   /// <summary>
   /// Get last received frame.
   /// Meaningless if state is not SYNCED.
   /// </summary>
   inline const Frame& getFrame() const { return mFrame; }

   /// <summary>
   /// Get current channel values.
   /// Meaningless if state is not SYNCED.
   /// </summary>
   inline const Channels& getChannels() const { return mChannels; }

   /// <summary>
   /// True after successful execution of start() for states
   /// UNSYNCED, SYNCING and SYNCED.
   /// </summary>
   inline bool isStreaming() const { return mState > State::CONNECTED; }

   /// <summary>
   /// Tries to find a connected Discovery 20 device.
   /// Returns true on success.
   /// </summary>
   bool find();

   /// <summary>
   /// Try connect to Discovery 20 device.
   /// Returns true on success or if already connected.
   /// </summary>
   bool connect(char* codekey, char* serialnumber, char* passkey);

   /// <summary>
   /// Starts data streaming on a connected Discovery 20.
   /// You must regularly call update() if this succeeds.
   /// </summary>
   bool start();

   /// <summary>
   /// Stops streaming on a currently streaming Discovery 20.
   /// Returns true on success or if not streaming at all.
   /// </summary>
   bool stop();

   /// <summary>
   /// Disconnects a currently connected or streaming Discovery 20.
   /// Returns true on success or if not connected at all.
   /// </summary>
   bool disconnect();

   /// <summary>
   /// Processes pending data from the device queue.
   /// Must be called regularly if start() succeeded.
   /// </summary>
   void update();
};
#endif
#endif
