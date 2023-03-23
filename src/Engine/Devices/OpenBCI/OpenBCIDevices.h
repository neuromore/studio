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

#ifndef __NEUROMORE_OPENBCIHEADSET_H
#define __NEUROMORE_OPENBCIHEADSET_H

// include required headers
#include "../../BciDevice.h"
#include "../../DeviceDriver.h"

#ifdef INCLUDE_DEVICE_OPENBCI

// the base class for all OpenBCI devices
class ENGINE_API OpenBCIDeviceBase : public BciDevice
{
   class OpenBCISerialThread;
   friend class OpenBCISerialThread;

public:
   static constexpr uint32_t NUMELECTRODESCYTON = 8;
   static constexpr uint32_t NUMELECTRODESDAISY = 16;
   static constexpr uint32_t SAMPLERATECYTON    = 250;
   static constexpr uint32_t SAMPLERATEDAISY    = 125;
   static constexpr double   series_resistor_ohms = 2200; // Ohms. There is a series resistor on the 32 bit board.
   static constexpr double   leadOffDrive_amps = 6.0e-9;  // 6 nA, set by its Arduino code

   template<uint32 SAMPLERATE>
   struct Imp
   {
      static constexpr const double MULT = 1.0 / (double)SAMPLERATE;
      double imp;
      double sum;
      double val[SAMPLERATE];
      uint32 idx;
      inline void step(double v)
      {
         // standard deviation of last second
         sum -= val[idx];
         sum += v;
         val[idx] = v;
         idx = (idx < SAMPLERATE-1) ? idx + 1 : 0;
         double avg = sum * MULT;
         double dev = 0.0;
         for (uint32 i = 0; i < SAMPLERATE; i++)
            dev += ::std::abs(val[i] - avg);
         dev *= MULT;
         // impedance from standard deviation
         imp = (::sqrt(2.0) * dev * 1.0e-6) / OpenBCIDeviceBase::leadOffDrive_amps;
         imp -= OpenBCIDeviceBase::series_resistor_ohms;
         imp *= 0.001; // Ohm->kOhm
         if (imp < 0.0) {
            imp = 0.0;
         }
      }
   };

   using ImpCyton = Imp<SAMPLERATECYTON>;
   using ImpDaisy = Imp<SAMPLERATEDAISY>;

public:
   // constructors & destructor
   OpenBCIDeviceBase(DeviceDriver* driver = NULL);
   virtual ~OpenBCIDeviceBase() {}

   // information
   double GetLatency() const override        { return 0.1; }
   double GetExpectedJitter() const override { return 0.1; }
   bool IsWireless() const override          { return true; }
   bool HasTestMode() const override         { return true; }

   void CreateSensors() override;

   void StartTest() override;
   void StopTest() override;
   inline bool IsTestRunning() override { return mTesting; }
   inline bool HasEegContactQualityIndicator() override { return mTesting; }

   virtual void AddSample(uint32 idx, double v) = 0;

   // custom sensor
   inline Sensor* GetAccForwardSensor() const { return mAccForwardSensor; }
   inline Sensor* GetAccUpSensor()      const { return mAccUpSensor; }
   inline Sensor* GetAccLeftSensor()    const { return mAccLeftSensor; }

private:
   bool     mTesting;
   Sensor*  mAccForwardSensor;
   Sensor*  mAccUpSensor;
   Sensor*  mAccLeftSensor;
};



// the default OpenBCI device class
class ENGINE_API OpenBCIDevice : public OpenBCIDeviceBase
{
protected:
   Imp<SAMPLERATECYTON> mImpedances[NUMELECTRODESCYTON];

public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_OPENBCI };

   // constructors & destructor
   OpenBCIDevice(DeviceDriver* driver = NULL);
   virtual ~OpenBCIDevice();

   Device* Clone() override										{ return new OpenBCIDevice(); }

   // information
   uint32 GetType() const override									{ return TYPE_ID; }
   const char* GetTypeName() const override						{ return "openbci"; }
   const char* GetHardwareName() const override					{ return "OpenBCI"; }
   const char* GetUuid() const override							{ return "5108993a-fe1b-11e4-a322-1697f925ec7b"; }
   static const char* GetRuleName()								{ return "DEVICE_OpenBCI"; }
   double GetSampleRate() const override							{ return SAMPLERATECYTON; }

   double GetTimeoutLimit() const override							{ return 60; } // Long timeout limit because channel config takes so long

   void CreateElectrodes() override;

   inline double GetImpedance(uint32 idx) override { return mImpedances[idx].imp; }
   inline void AddSample(uint32 idx, double v) override
   {
      mSensors[idx]->AddQueuedSample(v);
      mImpedances[idx].step(v);
   }
};



class ENGINE_API OpenBCIDaisyDevice : public OpenBCIDeviceBase
{
protected:
   Imp<SAMPLERATEDAISY> mImpedances[NUMELECTRODESDAISY];

public:
   enum { TYPE_ID = DeviceTypeIDs::DEVICE_TYPEID_OPENBCI_DAISY };

   // constructors & destructor
   OpenBCIDaisyDevice(DeviceDriver* driver = NULL);
   virtual ~OpenBCIDaisyDevice();

   Device* Clone() override									{ return new OpenBCIDaisyDevice(); }

   // information
   uint32 GetType() const override								{ return TYPE_ID; }
   const char* GetTypeName() const override					{ return "openbci16"; }
   const char* GetHardwareName() const override				{ return "OpenBCI + Daisy"; }
   const char* GetUuid() const override						{ return "23893c38-8ecd-11e5-8994-feff819cdc9f"; }
   static const char* GetRuleName()							{ return "DEVICE_OpenBCIDaisy"; }
   double GetSampleRate() const override						{ return SAMPLERATEDAISY; }

   void CreateElectrodes() override;

   inline double GetImpedance(uint32 idx) override { return mImpedances[idx].imp; }
   inline void AddSample(uint32 idx, double v) override
   {
      mSensors[idx]->AddQueuedSample(v);
      mImpedances[idx].step(v);
   }
};


#endif

#endif
