#include "Discovery20.h"

// Windows API DEFINE
#ifndef INITGUID
#define INITGUID
#endif

// Windows API
#include <SetupAPI.h> // SetupAPI
#include <cfgmgr32.h> // for MAX_DEVICE_ID_LEN and CM_Get_Device_ID
#include <devpkey.h>  // for DEVPKEY_Device_FriendlyName

int Discovery20::findCOM()
{
   HDEVINFO        devinfo;
   SP_DEVINFO_DATA devdata;
   CONFIGRET       status;
   TCHAR           devid[MAX_DEVICE_ID_LEN];
   DEVPROPTYPE     proptype;
   DWORD           size;
   WCHAR           buffer[4096];

   // device friendly name prefix to scan for
   static constexpr WCHAR scan[] = 
      L"Discovery 24E Module (COM";

   // init devdata
   memset(&devdata, 0, sizeof(devdata));
   devdata.cbSize = sizeof(devdata);

   // query comports devinfo
   devinfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
   if (devinfo == INVALID_HANDLE_VALUE)
      return 0;

   // iterate them
   for (DWORD i = 0; ; i++)
   {
      // query devdata
      if (!SetupDiEnumDeviceInfo(devinfo, i, &devdata))
         break;

      // query devid
      status = CM_Get_Device_ID(devdata.DevInst, devid, MAX_DEVICE_ID_LEN, 0);
      if (status != CR_SUCCESS)
         continue;

      // query devprop
      if (!SetupDiGetDevicePropertyW(devinfo, &devdata, &DEVPKEY_Device_FriendlyName, &proptype, (BYTE*)buffer, sizeof(buffer), &size, 0))
         continue;

      // compare and extract com port num
      const int scanlen = lstrlenW(scan);
      if (wcsncmp(buffer, scan, scanlen) == 0)
      {
         wchar_t* start = &buffer[scanlen];
         wchar_t* end   = &buffer[size-2];
         long x = wcstol(start, &end, 10);
         if (((x != 0) & (x != LONG_MIN) & (x != LONG_MAX)) != 0)
            return x;
      }
   }

   return 0;
}

void Discovery20::processQueue()
{
   uint32_t nret;
   switch (mState)
   {
   case State::UNSYNCED:
   {
      nret = (uint32_t)mSDK.AtlReadData(mFrame.data, Frame::SIZE);
      assert(nret == Frame::SIZE);
      for (uint32_t i = 0; i < nret; i++)
      {
         if (mFrame.data[i] == 0x20)
         {
            if (i > 0) {
               mNumBytes = nret - i;
               std::memcpy(mFrame.data, &mFrame.data[i], mNumBytes);
            }
            else {
               mNumBytes = 0;
            }
            mState = State::SYNCING;
            mNumSyncs = 0;
            mNextSync = 0x20;
            mCallback.onSyncStart();
            break;
         }
      }
      break;
   }
   case State::SYNCING:
   {
      if (mFrame.sync != mNextSync)
      {
         mState = State::UNSYNCED;
         mNumSyncs = 0;
         mCallback.onSyncFail();
         break;
      }
      if (mNumSyncs >= MINSYNCS)
      {
         mState = State::SYNCED;
         stepSync();
         mCallback.onSyncSuccess();
         break;
      }
      nret = (uint32_t)mSDK.AtlReadData(&mFrame.data[mNumBytes], Frame::SIZE-mNumBytes);
      assert(nret == Frame::SIZE-mNumBytes);
      mNumBytes = 0;
      stepSync();
      break;
   }
   case State::SYNCED:
   {
      nret = (uint32_t)mSDK.AtlReadData(mFrame.data, Frame::SIZE);
      assert(nret == Frame::SIZE);
      if (mFrame.sync != mNextSync)
      {
         mState = State::UNSYNCED;
         mNumSyncs = 0;
         mCallback.onSyncLost();
         break;
      }
      processFrame();
      stepSync();
      break;
   }
   }
}
