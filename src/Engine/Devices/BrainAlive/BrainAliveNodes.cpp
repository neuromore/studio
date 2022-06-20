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

// include precompiled header
#include <Engine/Precompiled.h>

#include "BrainAliveNodes.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

// TODO: Must not include Studio headers into Engine
// TODO: Must not use QT classes in Engine
#include "../../../Studio/Devices/BrainAlive/BrainAliveBluetooth.h"
#include <Engine/EngineManager.h>

BLEInterface *m_bleInterface;

void BrainAliveNodeBase::Init() {
  DeviceInputNode::Init();
  {
    Core::AttributeSettings *attribute = RegisterAttribute(
        "Channel Settings", "channel_settings", "Channel Settings",
        Core::ATTRIBUTE_INTERFACETYPE_STRING);
    attribute->SetDefaultValue(Core::AttributeString::Create(""));
    attribute->SetVisible(false);
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 1", "channel1", "Channel 1",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "F7");
    attribute->SetComboValue(1, "F5");
    attribute->SetComboValue(2, "F3");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 2", "channel2", "Channel 2",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "FT7");
    attribute->SetComboValue(1, "FC5");
    attribute->SetComboValue(2, "FC3");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 3", "channel3", "Channel 3",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "T7");
    attribute->SetComboValue(1, "C5");
    attribute->SetComboValue(2, "C3");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 4", "channel4", "Channel 4",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "TP7");
    attribute->SetComboValue(1, "CP5");
    attribute->SetComboValue(2, "CP3");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 5", "channel5", "Channel 5",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(1);
    attribute->SetComboValue(0, "Cz");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 6", "channel6", "Channel 6",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "C4");
    attribute->SetComboValue(1, "C6");
    attribute->SetComboValue(2, "T8");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 7", "channel7", "Channel 7",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "FC4");
    attribute->SetComboValue(1, "FC6");
    attribute->SetComboValue(2, "FT8");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Channel 8", "channel8", "Channel 8",
                          Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
    attribute->ResizeComboValues(3);
    attribute->SetComboValue(0, "F4");
    attribute->SetComboValue(1, "F6");
    attribute->SetComboValue(2, "F8");
    attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
  }
  {
    Core::AttributeSettings *attribute = RegisterAttribute(
        "Apply", "apply", "Apply", Core::ATTRIBUTE_INTERFACETYPE_BUTTON);
    attribute->SetDefaultValue(Core::AttributeBool::Create(false));
  }
  {
    Core::AttributeSettings *attribute =
        RegisterAttribute("Write command", "write_command", "Write command",
                          Core::ATTRIBUTE_INTERFACETYPE_STRING);
    attribute->SetDefaultValue(Core::AttributeString::Create(""));
  }
  {
    Core::AttributeSettings *attribute = RegisterAttribute(
        "Write", "write", "Write", Core::ATTRIBUTE_INTERFACETYPE_BUTTON);
    attribute->SetDefaultValue(Core::AttributeBool::Create(false));
  }

  GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
  GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
  GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);

  m_bleInterface = new BLEInterface();
  m_bleInterface->scanDevices();
}

void BrainAliveNodeBase::OnAttributesChanged() {
  if (GetBoolAttributeByName("apply") == true) {
    SetBoolAttribute("apply", false);
    SynchronizeParams();
    m_bleInterface->set_currentDevice(0);
    m_bleInterface->connectCurrentDevice();

    if (auto *deviceManager = GetDeviceManager())
      if (auto *deviceDriver = deviceManager->FindDeviceDriverByDeviceType(
              BrainAliveDevice::TYPE_ID))
        if (auto *newDevice = new BrainAliveDevice(deviceDriver)) {
          newDevice->set_channel(data);
          newDevice->CreateElectrodes();
        }
  }
  if (GetBoolAttributeByName("write") == true) {
    SetBoolAttribute("write", false);
    if (m_bleInterface->BLE_Satus()) {
      QByteArray data = GetStringAttributeByName("write_command");
      m_bleInterface->write_data(data);
    }
    SetStringAttribute("write_command", "");
  }
}

Device *BrainAliveNodeBase::FindDevice() {
  if (auto *device =
          dynamic_cast<BrainAliveDevice *>(GetDeviceManager()->GetDevice(0)))
    return device;

  return nullptr;
}

void BrainAliveNodeBase::ReInit(const Core::Time &elapsed,
                                const Core::Time &delta) {
  if (auto *device = FindDevice()) {
    if (device != GetCurrentDevice()) {
      // setup the output ports
      RegisterDeviceSensorsAsPorts(device);
      UseChannelColoring();
    }
  } else if (auto *currentDevice = GetCurrentDevice()) {
    GetDeviceManager()->RemoveDeviceAsync(currentDevice);
  }
  DeviceInputNode::ReInit(elapsed, delta);
}

void BrainAliveNodeBase::SynchronizeParams() {
  GetAttributeValue(GetInt32AttributeByName("channel1"))
      ->ConvertToString(mParams.channel_1);
  GetAttributeValue(GetInt32AttributeByName("channel2"))
      ->ConvertToString(mParams.channel_2);
  GetAttributeValue(GetInt32AttributeByName("channel3"))
      ->ConvertToString(mParams.channel_3);
  GetAttributeValue(GetInt32AttributeByName("channel4"))
      ->ConvertToString(mParams.channel_4);
  GetAttributeValue(GetInt32AttributeByName("channel5"))
      ->ConvertToString(mParams.channel_5);
  GetAttributeValue(GetInt32AttributeByName("channel6"))
      ->ConvertToString(mParams.channel_6);
  GetAttributeValue(GetInt32AttributeByName("channel7"))
      ->ConvertToString(mParams.channel_7);
  GetAttributeValue(GetInt32AttributeByName("channel8"))
      ->ConvertToString(mParams.channel_8);

  if (mParams.channel_1 == "0")
    data[0] = "F7";
  else if (mParams.channel_1 == "1")
    data[0] = "F5";
  else if (mParams.channel_1 == "2")
    data[0] = "F3";
  if (mParams.channel_2 == "0")
    data[1] = "FT7";
  else if (mParams.channel_2 == "1")
    data[1] = "FC5";
  else if (mParams.channel_2 == "2")
    data[1] = "FC3";
  if (mParams.channel_3 == "0")
    data[2] = "T7";
  else if (mParams.channel_3 == "1")
    data[2] = "C5";
  else if (mParams.channel_3 == "2")
    data[2] = "C3";
  if (mParams.channel_4 == "0")
    data[3] = "TP7";
  else if (mParams.channel_4 == "1")
    data[3] = "CP5";
  else if (mParams.channel_4 == "2")
    data[3] = "CP3";
  if (mParams.channel_5 == "0")
    data[4] = "Cz";
  if (mParams.channel_6 == "0")
    data[5] = "C4";
  else if (mParams.channel_6 == "1")
    data[5] = "C6";
  else if (mParams.channel_6 == "2")
    data[5] = "T8";
  if (mParams.channel_7 == "0")
    data[6] = "FC4";
  else if (mParams.channel_7 == "1")
    data[6] = "FC6";
  else if (mParams.channel_7 == "2")
    data[6] = "FT8";
  if (mParams.channel_8 == "0")
    data[7] = "F4";
  else if (mParams.channel_8 == "1")
    data[7] = "F6";
  else if (mParams.channel_8 == "2")
    data[7] = "F8";
}

void BrainAliveNodeBase::CreateNewDevice() {
  // create new device
  if (auto *deviceManager = GetDeviceManager())
    if (auto *deviceDriver = deviceManager->FindDeviceDriverByDeviceType(
            BrainAliveDevice::TYPE_ID))
      if (auto *newDevice = new BrainAliveDevice(deviceDriver)) {
        GetDeviceManager()->AddDeviceAsync(newDevice);
        newDevice->CreateElectrodes();
      }
}

#endif
