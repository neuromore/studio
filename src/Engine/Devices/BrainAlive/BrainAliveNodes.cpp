#include "BrainAliveNodes.h"

#ifdef INCLUDE_DEVICE_BRAINALIVE

#include "../../EngineManager.h"

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
    Core::AttributeSettings *attribute = RegisterAttribute(
        "Channel 1", "channel1", "Channel 1",
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
  GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
  GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
  GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);
}

void BrainAliveNodeBase::OnAttributesChanged() {
  if (GetBoolAttributeByName("apply") == true) {
    SetBoolAttribute("apply", false);
    SynchronizeParams();
    // check current device is synced with params
    if(auto *currentDevice = GetCurrentDevice()) {
    
 GetDeviceManager()->RemoveDeviceAsync(currentDevice);
   }
    CreateNewDevice();
  }
  if (GetBoolAttributeByName("scan") == true) {
    //if ((mScan_Widget->isVisible()) == false) {
    //  Connect = new QPushButton();
    //  Connect->setText(" Connect ");

    //  mScan_Widget->setFixedHeight(400);
    //  mScan_Widget->setFixedWidth(400);
    //  QVBoxLayout *vLayout = new QVBoxLayout();

    //  mListWidget->setFixedHeight(380);
    //  mListWidget->setFixedWidth(380);
    //  vLayout->setMargin(10);
    //  vLayout->setSpacing(10);

    //  vLayout->addWidget(mListWidget, 1, Qt::AlignTop);
    //  vLayout->addWidget(Connect, 1, Qt::AlignBottom);
    //  mScan_Widget->setLayout(vLayout);
    //  mScan_Widget->setVisible(true);
    //  m_bleInterface->scanDevices();
    //  connect(Connect, &QPushButton::clicked, this,
    //          &GraphAttributesWidget::On_connect);
    //} else {
    //  mScan_Widget->show();
    //  // mScan_Widget->showMaximized();
    //  mScan_Widget->activateWindow();
    //  mListWidget->show();
    //}
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

void BrainAliveNodeBase::SynchronizeParams() 
{
 GetAttributeValue(GetInt32AttributeByName("channel1"))->ConvertToString(mParams.channel_1);
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
}

void BrainAliveNodeBase::CreateNewDevice() 
{
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
