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

// include required files
#include "eemagineDevices.h"
#include "../../EngineManager.h"
#include "../../Core/LogManager.h"

#ifdef INCLUDE_DEVICE_EEMAGINE

using namespace Core;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// base class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineDevice::eemagineDevice(DeviceDriver* driver) : BciDevice(driver)
{
   // start in connected state
   mState = STATE_IDLE;
}

eemagineDevice::~eemagineDevice()
{
}

void eemagineDevice::CreateSensors()
{
   // create EEG sensors first
   BciDevice::CreateSensors();

   // iterate neuro sensors (electrodes)
   const uint32_t numSensors = GetNumNeuroSensors();
   for (uint32_t e = 0; e < numSensors; e++)
   {
      if (Sensor* sensor = GetNeuroSensor(e))
      {
         // assign hw channel index to sensor

         // cap with 8 electrodes
         // taken from cap: CA-411
         if (numSensors == 8)
         {
            if      (String::SafeCompare(sensor->GetName(), "Fz") == 0)  sensor->SetHardwareChannel(0);
            else if (String::SafeCompare(sensor->GetName(), "Cz") == 0)  sensor->SetHardwareChannel(1);
            else if (String::SafeCompare(sensor->GetName(), "Pz") == 0)  sensor->SetHardwareChannel(2);
            else if (String::SafeCompare(sensor->GetName(), "F3") == 0)  sensor->SetHardwareChannel(3);
            else if (String::SafeCompare(sensor->GetName(), "F4") == 0)  sensor->SetHardwareChannel(4);
            else if (String::SafeCompare(sensor->GetName(), "Fpz") == 0) sensor->SetHardwareChannel(5);
            else if (String::SafeCompare(sensor->GetName(), "C3") == 0)  sensor->SetHardwareChannel(6);
            else if (String::SafeCompare(sensor->GetName(), "C4") == 0)  sensor->SetHardwareChannel(7);
         }

         // cap with 16 electrodes
         // taken from: CA-204
         else if (numSensors == 16)
         {
            if      (String::SafeCompare(sensor->GetName(), "Fp1") == 0)   sensor->SetHardwareChannel(0);
            else if (String::SafeCompare(sensor->GetName(), "Fp2") == 0)   sensor->SetHardwareChannel(2);
            else if (String::SafeCompare(sensor->GetName(), "F3") == 0)    sensor->SetHardwareChannel(4);
            else if (String::SafeCompare(sensor->GetName(), "Fz") == 0)    sensor->SetHardwareChannel(5);
            else if (String::SafeCompare(sensor->GetName(), "F4") == 0)    sensor->SetHardwareChannel(6);
            else if (String::SafeCompare(sensor->GetName(), "T7") == 0)    sensor->SetHardwareChannel(13);
            else if (String::SafeCompare(sensor->GetName(), "C3") == 0)    sensor->SetHardwareChannel(14);
            else if (String::SafeCompare(sensor->GetName(), "Cz") == 0)    sensor->SetHardwareChannel(15);
            else if (String::SafeCompare(sensor->GetName(), "C4") == 0)    sensor->SetHardwareChannel(16);
            else if (String::SafeCompare(sensor->GetName(), "T8") == 0)    sensor->SetHardwareChannel(17);
            else if (String::SafeCompare(sensor->GetName(), "P3") == 0)    sensor->SetHardwareChannel(24);
            else if (String::SafeCompare(sensor->GetName(), "Pz") == 0)    sensor->SetHardwareChannel(25);
            else if (String::SafeCompare(sensor->GetName(), "P4") == 0)    sensor->SetHardwareChannel(26);
            else if (String::SafeCompare(sensor->GetName(), "O1") == 0)    sensor->SetHardwareChannel(29);
            else if (String::SafeCompare(sensor->GetName(), "Oz") == 0)    sensor->SetHardwareChannel(30);
            else if (String::SafeCompare(sensor->GetName(), "O2") == 0)    sensor->SetHardwareChannel(31);
         }

         // cap with 32 electrodes
         // taken from: CAP-32CU
         else if (numSensors == 32)
         {
            if      (String::SafeCompare(sensor->GetName(), "Fp1") == 0)    sensor->SetHardwareChannel(0);
            else if (String::SafeCompare(sensor->GetName(), "Fpz") == 0)    sensor->SetHardwareChannel(1);
            else if (String::SafeCompare(sensor->GetName(), "Fp2") == 0)    sensor->SetHardwareChannel(2);
            else if (String::SafeCompare(sensor->GetName(), "F7") == 0)     sensor->SetHardwareChannel(3);
            else if (String::SafeCompare(sensor->GetName(), "F3") == 0)     sensor->SetHardwareChannel(4);
            else if (String::SafeCompare(sensor->GetName(), "Fz") == 0)     sensor->SetHardwareChannel(5);
            else if (String::SafeCompare(sensor->GetName(), "F4") == 0)     sensor->SetHardwareChannel(6);
            else if (String::SafeCompare(sensor->GetName(), "F8") == 0)     sensor->SetHardwareChannel(7);
            else if (String::SafeCompare(sensor->GetName(), "FC5") == 0)    sensor->SetHardwareChannel(8);
            else if (String::SafeCompare(sensor->GetName(), "FC1") == 0)    sensor->SetHardwareChannel(9);
            else if (String::SafeCompare(sensor->GetName(), "FC2") == 0)    sensor->SetHardwareChannel(10);
            else if (String::SafeCompare(sensor->GetName(), "FC6") == 0)    sensor->SetHardwareChannel(11);
            else if (String::SafeCompare(sensor->GetName(), "A1") == 0)     sensor->SetHardwareChannel(12);
            else if (String::SafeCompare(sensor->GetName(), "T7") == 0)     sensor->SetHardwareChannel(13);
            else if (String::SafeCompare(sensor->GetName(), "C3") == 0)     sensor->SetHardwareChannel(14);
            else if (String::SafeCompare(sensor->GetName(), "Cz") == 0)     sensor->SetHardwareChannel(15);
            else if (String::SafeCompare(sensor->GetName(), "C4") == 0)     sensor->SetHardwareChannel(16);
            else if (String::SafeCompare(sensor->GetName(), "T8") == 0)     sensor->SetHardwareChannel(17);
            else if (String::SafeCompare(sensor->GetName(), "A2") == 0)     sensor->SetHardwareChannel(18);
            else if (String::SafeCompare(sensor->GetName(), "CP5") == 0)    sensor->SetHardwareChannel(19);
            else if (String::SafeCompare(sensor->GetName(), "CP1") == 0)    sensor->SetHardwareChannel(20);
            else if (String::SafeCompare(sensor->GetName(), "CP2") == 0)    sensor->SetHardwareChannel(21);
            else if (String::SafeCompare(sensor->GetName(), "CP6") == 0)    sensor->SetHardwareChannel(22);
            else if (String::SafeCompare(sensor->GetName(), "P7") == 0)     sensor->SetHardwareChannel(23);
            else if (String::SafeCompare(sensor->GetName(), "P3") == 0)     sensor->SetHardwareChannel(24);
            else if (String::SafeCompare(sensor->GetName(), "Pz") == 0)     sensor->SetHardwareChannel(25);
            else if (String::SafeCompare(sensor->GetName(), "P4") == 0)     sensor->SetHardwareChannel(26);
            else if (String::SafeCompare(sensor->GetName(), "P8") == 0)     sensor->SetHardwareChannel(27);
            else if (String::SafeCompare(sensor->GetName(), "POz") == 0)    sensor->SetHardwareChannel(28);
            else if (String::SafeCompare(sensor->GetName(), "O1") == 0)     sensor->SetHardwareChannel(29);
            else if (String::SafeCompare(sensor->GetName(), "Oz") == 0)     sensor->SetHardwareChannel(30);
            else if (String::SafeCompare(sensor->GetName(), "O2") == 0)     sensor->SetHardwareChannel(31);
         }

         // cap with 64 electrodes
         // taken from: CA-208.s1
         else if (numSensors == 64)
         {
            if      (String::SafeCompare(sensor->GetName(), "Fp1") == 0)    sensor->SetHardwareChannel(0);
            else if (String::SafeCompare(sensor->GetName(), "Fpz") == 0)    sensor->SetHardwareChannel(1);
            else if (String::SafeCompare(sensor->GetName(), "Fp2") == 0)    sensor->SetHardwareChannel(2);
            else if (String::SafeCompare(sensor->GetName(), "F7") == 0)     sensor->SetHardwareChannel(3);
            else if (String::SafeCompare(sensor->GetName(), "F3") == 0)     sensor->SetHardwareChannel(4);
            else if (String::SafeCompare(sensor->GetName(), "Fz") == 0)     sensor->SetHardwareChannel(5);
            else if (String::SafeCompare(sensor->GetName(), "F4") == 0)     sensor->SetHardwareChannel(6);
            else if (String::SafeCompare(sensor->GetName(), "F8") == 0)     sensor->SetHardwareChannel(7);
            else if (String::SafeCompare(sensor->GetName(), "FC5") == 0)    sensor->SetHardwareChannel(8);
            else if (String::SafeCompare(sensor->GetName(), "FC1") == 0)    sensor->SetHardwareChannel(9);
            else if (String::SafeCompare(sensor->GetName(), "FC2") == 0)    sensor->SetHardwareChannel(10);
            else if (String::SafeCompare(sensor->GetName(), "FC6") == 0)    sensor->SetHardwareChannel(11);
            else if (String::SafeCompare(sensor->GetName(), "A1") == 0)     sensor->SetHardwareChannel(12);
            else if (String::SafeCompare(sensor->GetName(), "T7") == 0)     sensor->SetHardwareChannel(13);
            else if (String::SafeCompare(sensor->GetName(), "C3") == 0)     sensor->SetHardwareChannel(14);
            else if (String::SafeCompare(sensor->GetName(), "Cz") == 0)     sensor->SetHardwareChannel(15);
            else if (String::SafeCompare(sensor->GetName(), "C4") == 0)     sensor->SetHardwareChannel(16);
            else if (String::SafeCompare(sensor->GetName(), "T8") == 0)     sensor->SetHardwareChannel(17);
            else if (String::SafeCompare(sensor->GetName(), "A2") == 0)     sensor->SetHardwareChannel(18);
            else if (String::SafeCompare(sensor->GetName(), "CP5") == 0)    sensor->SetHardwareChannel(19);
            else if (String::SafeCompare(sensor->GetName(), "CP1") == 0)    sensor->SetHardwareChannel(20);
            else if (String::SafeCompare(sensor->GetName(), "CP2") == 0)    sensor->SetHardwareChannel(21);
            else if (String::SafeCompare(sensor->GetName(), "CP6") == 0)    sensor->SetHardwareChannel(22);
            else if (String::SafeCompare(sensor->GetName(), "P7") == 0)     sensor->SetHardwareChannel(23);
            else if (String::SafeCompare(sensor->GetName(), "P3") == 0)     sensor->SetHardwareChannel(24);
            else if (String::SafeCompare(sensor->GetName(), "Pz") == 0)     sensor->SetHardwareChannel(25);
            else if (String::SafeCompare(sensor->GetName(), "P4") == 0)     sensor->SetHardwareChannel(26);
            else if (String::SafeCompare(sensor->GetName(), "P8") == 0)     sensor->SetHardwareChannel(27);
            else if (String::SafeCompare(sensor->GetName(), "POz") == 0)    sensor->SetHardwareChannel(28);
            else if (String::SafeCompare(sensor->GetName(), "O1") == 0)     sensor->SetHardwareChannel(29);
            else if (String::SafeCompare(sensor->GetName(), "O2") == 0)     sensor->SetHardwareChannel(30);
            else if (String::SafeCompare(sensor->GetName(), "EOG") == 0)    sensor->SetHardwareChannel(31);
            else if (String::SafeCompare(sensor->GetName(), "AF7") == 0)    sensor->SetHardwareChannel(32);
            else if (String::SafeCompare(sensor->GetName(), "AF3") == 0)    sensor->SetHardwareChannel(33);
            else if (String::SafeCompare(sensor->GetName(), "AF4") == 0)    sensor->SetHardwareChannel(34);
            else if (String::SafeCompare(sensor->GetName(), "AF8") == 0)    sensor->SetHardwareChannel(35);
            else if (String::SafeCompare(sensor->GetName(), "F5") == 0)     sensor->SetHardwareChannel(36);
            else if (String::SafeCompare(sensor->GetName(), "F1") == 0)     sensor->SetHardwareChannel(37);
            else if (String::SafeCompare(sensor->GetName(), "F2") == 0)     sensor->SetHardwareChannel(38);
            else if (String::SafeCompare(sensor->GetName(), "F6") == 0)     sensor->SetHardwareChannel(39);
            else if (String::SafeCompare(sensor->GetName(), "FC3") == 0)    sensor->SetHardwareChannel(40);
            else if (String::SafeCompare(sensor->GetName(), "FCz") == 0)    sensor->SetHardwareChannel(41);
            else if (String::SafeCompare(sensor->GetName(), "FC4") == 0)    sensor->SetHardwareChannel(42);
            else if (String::SafeCompare(sensor->GetName(), "C5") == 0)     sensor->SetHardwareChannel(43);
            else if (String::SafeCompare(sensor->GetName(), "C1") == 0)     sensor->SetHardwareChannel(44);
            else if (String::SafeCompare(sensor->GetName(), "C2") == 0)     sensor->SetHardwareChannel(45);
            else if (String::SafeCompare(sensor->GetName(), "C6") == 0)     sensor->SetHardwareChannel(46);
            else if (String::SafeCompare(sensor->GetName(), "CP3") == 0)    sensor->SetHardwareChannel(47);
            else if (String::SafeCompare(sensor->GetName(), "CP4") == 0)    sensor->SetHardwareChannel(48);
            else if (String::SafeCompare(sensor->GetName(), "P5") == 0)     sensor->SetHardwareChannel(49);
            else if (String::SafeCompare(sensor->GetName(), "P1") == 0)     sensor->SetHardwareChannel(50);
            else if (String::SafeCompare(sensor->GetName(), "P2") == 0)     sensor->SetHardwareChannel(51);
            else if (String::SafeCompare(sensor->GetName(), "P6") == 0)     sensor->SetHardwareChannel(52);
            else if (String::SafeCompare(sensor->GetName(), "PO5") == 0)    sensor->SetHardwareChannel(53);
            else if (String::SafeCompare(sensor->GetName(), "PO3") == 0)    sensor->SetHardwareChannel(54);
            else if (String::SafeCompare(sensor->GetName(), "PO4") == 0)    sensor->SetHardwareChannel(55);
            else if (String::SafeCompare(sensor->GetName(), "PO6") == 0)    sensor->SetHardwareChannel(56);
            else if (String::SafeCompare(sensor->GetName(), "FT7") == 0)    sensor->SetHardwareChannel(57);
            else if (String::SafeCompare(sensor->GetName(), "FT8") == 0)    sensor->SetHardwareChannel(58);
            else if (String::SafeCompare(sensor->GetName(), "TP7") == 0)    sensor->SetHardwareChannel(59);
            else if (String::SafeCompare(sensor->GetName(), "TP8") == 0)    sensor->SetHardwareChannel(60);
            else if (String::SafeCompare(sensor->GetName(), "PO7") == 0)    sensor->SetHardwareChannel(61);
            else if (String::SafeCompare(sensor->GetName(), "PO8") == 0)    sensor->SetHardwareChannel(62);
            else if (String::SafeCompare(sensor->GetName(), "Oz") == 0)     sensor->SetHardwareChannel(63);
         }

         // TODO: Implement this more generic as new
         // SetBufferSizeInSeconds() in class Channel
         sensor->GetInput()->SetBufferSize((uint32_t)(GetSampleRate() * 10.0));
         sensor->GetOutput()->SetBufferSize((uint32_t)(GetSampleRate() * 10.0));
      }
   }
}

bool eemagineDevice::CreateDefaultElectrodes(const uint32_t count)
{
   // only 8, 16, 32 and 64er default electrode sets supported
   if (count != 8 && count != 16 && count != 32 && count != 64)
      return false;

   mElectrodes.Clear();
   mElectrodes.Reserve(count);

   // default cap with 8 electrodes
   // taken from cap: CA-411
   if (count == 8)
   {
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));   // 01
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));   // 02
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));   // 03
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));   // 04
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));   // 05
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));  // 06
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));   // 07
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));   // 08
   }

   // cap with 16 electrodes
   // taken from: CA-204
   else if (count == 16)
   {
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));  // 01
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));  // 02
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));   // 03
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));   // 04
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));   // 05
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T7"));   // 06
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));   // 07
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));   // 08
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));   // 09
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T8"));   // 10
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));   // 11
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));   // 12
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));   // 13
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));   // 14
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));   // 15
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));   // 16
   }

   // cap with 32 electrodes
   // taken from: CAP-32CU
   else if (count == 32)
   {
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));  // 01
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));  // 02
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));  // 03
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));   // 04
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));   // 05
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));   // 06
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));   // 07
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));   // 08
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC5"));  // 09
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC1"));  // 10
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC2"));  // 11
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC6"));  // 12
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1"));   // 13 (= M1 ?)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T7"));   // 14
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));   // 15
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));   // 16
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));   // 17
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T8"));   // 18
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2"));   // 19 (= M2 ?)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));  // 20
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP1"));  // 21
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP2"));  // 22
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));  // 23
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P7"));   // 24
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));   // 25
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));   // 26
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));   // 27
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P8"));   // 28
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("POz"));  // 29
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));   // 30
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));   // 31
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));   // 32
   }

   // cap with 64 electrodes
   // taken from: CA-208.s1
   else if (count == 64)
   {
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp1"));  // 01
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fpz"));  // 02
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fp2"));  // 03
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F7"));   // 04
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F3"));   // 05
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Fz"));   // 06
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F4"));   // 07
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F8"));   // 08
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC5"));  // 09
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC1"));  // 10
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC2"));  // 11
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC6"));  // 12
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A1"));   // 13 (= M1 ?)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T7"));   // 14
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C3"));   // 15
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Cz"));   // 16
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C4"));   // 17
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("T8"));   // 18
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("A2"));   // 19 (= M2 ?)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP5"));  // 20
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP1"));  // 21
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP2"));  // 22
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP6"));  // 23
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P7"));   // 24
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P3"));   // 25
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Pz"));   // 26
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P4"));   // 27
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P8"));   // 28
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("POz"));  // 29
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O1"));   // 30
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("O2"));   // 31 (different than in 32er)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("EOG"));  // 32 (different than in 32er)
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AF7"));  // 33
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AF3"));  // 34
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AF4"));  // 35
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("AF8"));  // 36
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F5"));   // 37
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F1"));   // 38
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F2"));   // 39
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("F6"));   // 40
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC3"));  // 41
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FCz"));  // 42
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FC4"));  // 43
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C5"));   // 44
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C1"));   // 45
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C2"));   // 46
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("C6"));   // 47
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP3"));  // 48
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("CP4"));  // 49
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P5"));   // 50
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P1"));   // 51
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P2"));   // 52
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("P6"));   // 53
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO5"));  // 54
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO3"));  // 55
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO4"));  // 56
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO6"));  // 57
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT7"));  // 58
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("FT8"));  // 59
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP7"));  // 60
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("TP8"));  // 61 
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO7"));  // 62
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("PO8"));  // 63
      mElectrodes.Add(GetEEGElectrodes()->GetElectrodeByID("Oz"));   // 64
   }

   return true;
}

double eemagineDevice::GetImpedance(uint32 neuroSensorIndex)
{
   if (neuroSensorIndex < GetNumNeuroSensors())
      return GetNeuroSensor(neuroSensorIndex)->GetImpedance();

   return 0.0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-211
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE211Device::eemagineEE211Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-211 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE211Device::~eemagineEE211Device()
{
   LogDetailedInfo("Destructing eemagine EE-211 ...");
}

void eemagineEE211Device::CreateElectrodes()
{
   CreateDefaultElectrodes(64);
}

void eemagineEE211Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-212
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE212Device::eemagineEE212Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-212 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE212Device::~eemagineEE212Device()
{
   LogDetailedInfo("Destructing eemagine EE-212 ...");
}

void eemagineEE212Device::CreateElectrodes()
{
   CreateDefaultElectrodes(32);
}

void eemagineEE212Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-213
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE213Device::eemagineEE213Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-213 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE213Device::~eemagineEE213Device()
{
   LogDetailedInfo("Destructing eemagine EE-213 ...");
}

void eemagineEE213Device::CreateElectrodes()
{
   CreateDefaultElectrodes(16);
}

void eemagineEE213Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-214
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE214Device::eemagineEE214Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-214 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE214Device::~eemagineEE214Device()
{
   LogDetailedInfo("Destructing eemagine EE-214 ...");
}

void eemagineEE214Device::CreateElectrodes()
{
   CreateDefaultElectrodes(32);
}

void eemagineEE214Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-215
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE215Device::eemagineEE215Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-215 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE215Device::~eemagineEE215Device()
{
   LogDetailedInfo("Destructing eemagine EE-215 ...");
}

void eemagineEE215Device::CreateElectrodes()
{
   CreateDefaultElectrodes(64);
}

void eemagineEE215Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-221
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE221Device::eemagineEE221Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-221 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE221Device::~eemagineEE221Device()
{
   LogDetailedInfo("Destructing eemagine EE-221 ...");
}

void eemagineEE221Device::CreateElectrodes()
{
   CreateDefaultElectrodes(16);
}

void eemagineEE221Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-222
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE222Device::eemagineEE222Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-222 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE222Device::~eemagineEE222Device()
{
   LogDetailedInfo("Destructing eemagine EE-222 ...");
}

void eemagineEE222Device::CreateElectrodes()
{
   CreateDefaultElectrodes(32);
}

void eemagineEE222Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-223
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE223Device::eemagineEE223Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-223 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE223Device::~eemagineEE223Device()
{
   LogDetailedInfo("Destructing eemagine EE-223 ...");
}

void eemagineEE223Device::CreateElectrodes()
{
   CreateDefaultElectrodes(32);
}

void eemagineEE223Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-224
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE224Device::eemagineEE224Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-224 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE224Device::~eemagineEE224Device()
{
   LogDetailedInfo("Destructing eemagine EE-224 ...");
}

void eemagineEE224Device::CreateElectrodes()
{
   CreateDefaultElectrodes(64);
}

void eemagineEE224Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-225
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE225Device::eemagineEE225Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-225 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE225Device::~eemagineEE225Device()
{
   LogDetailedInfo("Destructing eemagine EE-225 ...");
}

void eemagineEE225Device::CreateElectrodes()
{
   CreateDefaultElectrodes(64);
}

void eemagineEE225Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-410
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE410Device::eemagineEE410Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-410 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE410Device::~eemagineEE410Device()
{
   LogDetailedInfo("Destructing eemagine EE-410 ...");
}

void eemagineEE410Device::CreateElectrodes()
{
   CreateDefaultElectrodes(8);
}

void eemagineEE410Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-411
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE411Device::eemagineEE411Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-411 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE411Device::~eemagineEE411Device()
{
   LogDetailedInfo("Destructing eemagine EE-411 ...");
}

void eemagineEE411Device::CreateElectrodes()
{
   CreateDefaultElectrodes(8);
}

void eemagineEE411Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EE-430
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

eemagineEE430Device::eemagineEE430Device(DeviceDriver* driver) : eemagineDevice(driver)
{
   LogDetailedInfo("Constructing eemagine EE-430 ...");

   // create all sensors
   CreateSensors();
}

eemagineEE430Device::~eemagineEE430Device()
{
   LogDetailedInfo("Destructing eemagine EE-430 ...");
}

void eemagineEE430Device::CreateElectrodes()
{
   CreateDefaultElectrodes(8);
}

void eemagineEE430Device::CreateSensors()
{
   eemagineDevice::CreateSensors();
}

#endif
