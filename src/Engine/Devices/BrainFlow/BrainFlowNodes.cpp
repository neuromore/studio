#include "BrainFlowNodes.h"

#ifdef INCLUDE_DEVICE_BRAINFLOW

#include "../../EngineManager.h"
#include <brainflow/utils/brainflow_constants.h>
#include <brainflow/cpp-package/board_shim.h>

namespace
{
	int BrainFlowBoardIdsNeuromore[] = {
		(int)BoardIds::PLAYBACK_FILE_BOARD,
		(int)BoardIds::STREAMING_BOARD,
		(int)BoardIds::SYNTHETIC_BOARD,
		(int)BoardIds::CYTON_BOARD,
		(int)BoardIds::GANGLION_BOARD,
		(int)BoardIds::CYTON_DAISY_BOARD,
		(int)BoardIds::GANGLION_WIFI_BOARD,
		(int)BoardIds::CYTON_WIFI_BOARD,
		(int)BoardIds::CYTON_DAISY_WIFI_BOARD,
		(int)BoardIds::BRAINBIT_BOARD,
		(int)BoardIds::UNICORN_BOARD,
		(int)BoardIds::CALLIBRI_EEG_BOARD,
		(int)BoardIds::CALLIBRI_EMG_BOARD,
		(int)BoardIds::CALLIBRI_ECG_BOARD,
		(int)BoardIds::NOTION_1_BOARD,
		(int)BoardIds::NOTION_2_BOARD,
		(int)BoardIds::IRONBCI_BOARD,
		(int)BoardIds::GFORCE_PRO_BOARD,
		(int)BoardIds::FREEEEG32_BOARD,
		(int)BoardIds::BRAINBIT_BLED_BOARD,
		(int)BoardIds::GFORCE_DUAL_BOARD,
		(int)BoardIds::MUSE_S_BLED_BOARD,
		(int)BoardIds::MUSE_2_BLED_BOARD,
		(int)BoardIds::CROWN_BOARD,
		(int)BoardIds::ANT_NEURO_EE_410_BOARD,
		(int)BoardIds::ANT_NEURO_EE_411_BOARD,
		(int)BoardIds::ANT_NEURO_EE_430_BOARD,
		(int)BoardIds::ANT_NEURO_EE_211_BOARD,
		(int)BoardIds::ANT_NEURO_EE_212_BOARD,
		(int)BoardIds::ANT_NEURO_EE_213_BOARD,
		(int)BoardIds::ANT_NEURO_EE_214_BOARD,
		(int)BoardIds::ANT_NEURO_EE_215_BOARD,
		(int)BoardIds::ANT_NEURO_EE_221_BOARD,
		(int)BoardIds::ANT_NEURO_EE_222_BOARD,
		(int)BoardIds::ANT_NEURO_EE_223_BOARD,
		(int)BoardIds::ANT_NEURO_EE_224_BOARD,
		(int)BoardIds::ANT_NEURO_EE_225_BOARD,
		(int)BoardIds::ENOPHONE_BOARD,
		(int)BoardIds::MUSE_2_BOARD,
		(int)BoardIds::MUSE_S_BOARD,
		(int)BoardIds::BRAINALIVE_BOARD,
		(int)BoardIds::MUSE_2016_BOARD,
		(int)BoardIds::MUSE_2016_BLED_BOARD
	};

	constexpr int DefaultBoardID = static_cast<int>(BoardIds::SYNTHETIC_BOARD);

	constexpr int MinIPPortValue = 1;
	constexpr int MaxIPPortValue = 65535;
	constexpr int DefaultIPPortValue = 50000;

	constexpr auto DefaultIPAddressValue = "192.168.4.1";


	unsigned boardIDToBoardIndex(int boardID)
	{
		for (int i = 0; i < sizeof(BrainFlowBoardIdsNeuromore) / sizeof(int); i++)
			if (boardID == BrainFlowBoardIdsNeuromore[i])
				return i;
		return boardIDToBoardIndex(DefaultBoardID); // for safety, should not be executed
	}

	int boardIndexToBoardID(unsigned boardIndex)
	{
		if (boardIndex <= sizeof(BrainFlowBoardIdsNeuromore) / sizeof(int))
			return BrainFlowBoardIdsNeuromore[boardIndex];
		return DefaultBoardID; // for safety
	}

	std::string getDeviceNameSafely(int boardID)
	{
		try {
			return BoardShim::get_device_name(boardID);
		}
		catch (...)
		{
			return "Unknown";
		}
	}
}

void BrainFlowNodeBase::Init()
{
	DeviceInputNode::Init();
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Board ID", "boardID", "Identificator of the board", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
		int NumBrainFlowBoards = sizeof(BrainFlowBoardIdsNeuromore) / sizeof(int);
		attribute->ResizeComboValues(NumBrainFlowBoards);
		for (int i = 0; i < NumBrainFlowBoards; ++i)
			attribute->SetComboValue(i, getDeviceNameSafely(boardIndexToBoardID(i)).c_str());
		attribute->SetDefaultValue(Core::AttributeInt32::Create(boardIDToBoardIndex(DefaultBoardID)));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Serial port", "serialPort", "Serial port", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("MAC address", "macAddress", "MAC address", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP address", "ipAddress", "IP address", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(DefaultIPAddressValue));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP port", "ipPort", "IP port", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
		attribute->SetDefaultValue(Core::AttributeInt32::Create(DefaultIPPortValue));
		attribute->SetMinValue(Core::AttributeInt32::Create(MinIPPortValue));
		attribute->SetMaxValue(Core::AttributeInt32::Create(MaxIPPortValue));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP protocol", "ipProtocol", "IP protocol", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
		attribute->ResizeComboValues(3);
		attribute->SetComboValue(0, "None");
		attribute->SetComboValue(1, "UDP");
		attribute->SetComboValue(2, "TCP");
		attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Other info", "otherInfo", "Other info", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Timeout", "timeout", "Timeout", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
		attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
		attribute->SetMinValue(Core::AttributeInt32::Create(0));
		attribute->SetMaxValue(Core::AttributeInt32::Create(CORE_INT32_MAX));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Serial number", "serialNumber", "Serial number", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("File", "file", "File", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Apply", "apply", "Apply", Core::ATTRIBUTE_INTERFACETYPE_BUTTON);
		attribute->SetDefaultValue(Core::AttributeBool::Create(false));
	}

	GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
	GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);
}

void BrainFlowNodeBase::OnAttributesChanged()
{
	if (!GetBoolAttributeByName("apply"))
		return;
	SetBoolAttribute("apply", false);
	SynchronizeParams();
	// check current device is synced with params
	if (auto* currentDevice = GetCurrentDevice())
	{
		if (GetBoardID() == currentDevice->GetBoardId() && GetParams() == currentDevice->GetParams())
			return; // no changes => no need to sync

		// remove current device if there are any changes
		GetDeviceManager()->RemoveDeviceAsync(currentDevice);
	}
	CreateNewDevice();
}

Device* BrainFlowNodeBase::FindDevice()
{
	for (unsigned i = 0; i < GetDeviceManager()->GetNumDevices(); ++i)
		if (auto* device = dynamic_cast<BrainFlowDevice*>(GetDeviceManager()->GetDevice(i)))
			if (device->GetParams() == GetParams() && device->GetBoardId() == GetBoardID())
				return device;

	return nullptr;
}

void BrainFlowNodeBase::ReInit(const Core::Time& elapsed, const Core::Time& delta)
{
	if (auto* device = FindDevice())
	{
		if (device != GetCurrentDevice())
		{
			// setup the output ports
			RegisterDeviceSensorsAsPorts(device);
			UseChannelColoring();
		}
	}
	else if (auto* currentDevice = GetCurrentDevice())
	{
		GetDeviceManager()->RemoveDeviceAsync(currentDevice);
	}
	DeviceInputNode::ReInit(elapsed, delta);
}

void BrainFlowNodeBase::SynchronizeParams()
{
	mBoardID = boardIndexToBoardID(GetInt32AttributeByName("boardID"));
	mParams.serial_port = GetStringAttributeByName("serialPort");
	mParams.mac_address = GetStringAttributeByName("macAddress");
	mParams.ip_address = GetStringAttributeByName("ipAddress");
	mParams.ip_port = GetInt32AttributeByName("ipPort");
	mParams.ip_protocol = GetInt32AttributeByName("ipProtocol");
	mParams.other_info = GetStringAttributeByName("otherInfo");
	mParams.timeout = GetInt32AttributeByName("timeout");
	mParams.serial_number = GetStringAttributeByName("serialNumber");
	mParams.file = GetStringAttributeByName("file");
}

void BrainFlowNodeBase::CreateNewDevice()
{
	// create new device
	if (auto* deviceManager = GetDeviceManager())
		if (auto* deviceDriver = deviceManager->FindDeviceDriverByDeviceType(BrainFlowDevice::TYPE_ID))
			if (auto* newDevice = new BrainFlowDevice(static_cast<BoardIds>(GetBoardID()), GetParams(),	deviceDriver))
				GetDeviceManager()->AddDeviceAsync(newDevice);
}

void BrainFlowCytonNode::OnAttributesChanged()
{
	SynchronizeParams();
	// check current device is synced with params
	if (auto* currentDevice = GetCurrentDevice())
	{
		if (GetBoardID() == currentDevice->GetBoardId() && GetParams() == currentDevice->GetParams())
			return; // no changes => no need to sync

		// remove current device if there are any changes
		GetDeviceManager()->RemoveDeviceAsync(currentDevice);
	}
	CreateNewDevice();
}

void BrainFlowCytonNode::Init()
{
	DeviceInputNode::Init();
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Board ID", "boardID", "Identificator of the board", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
		int NumBrainFlowBoards = sizeof(BrainFlowBoardIdsNeuromore) / sizeof(int);
		attribute->ResizeComboValues(NumBrainFlowBoards);
		for (int i = 0; i < NumBrainFlowBoards; ++i)
			attribute->SetComboValue(i, getDeviceNameSafely(boardIndexToBoardID(i)).c_str());
		attribute->SetDefaultValue(Core::AttributeInt32::Create(boardIDToBoardIndex(static_cast<int>(BoardIds::CYTON_BOARD))));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Serial port", "serialPort", "Serial port", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(mParams.serial_port.c_str()));
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("MAC address", "macAddress", "MAC address", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP address", "ipAddress", "IP address", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(DefaultIPAddressValue));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP port", "ipPort", "IP port", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
		attribute->SetDefaultValue(Core::AttributeInt32::Create(DefaultIPPortValue));
		attribute->SetMinValue(Core::AttributeInt32::Create(MinIPPortValue));
		attribute->SetMaxValue(Core::AttributeInt32::Create(MaxIPPortValue));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("IP protocol", "ipProtocol", "IP protocol", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
		attribute->ResizeComboValues(3);
		attribute->SetComboValue(0, "None");
		attribute->SetComboValue(1, "UDP");
		attribute->SetComboValue(2, "TCP");
		attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Other info", "otherInfo", "Other info", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Timeout", "timeout", "Timeout", Core::ATTRIBUTE_INTERFACETYPE_INTSPINNER);
		attribute->SetDefaultValue(Core::AttributeInt32::Create(0));
		attribute->SetMinValue(Core::AttributeInt32::Create(0));
		attribute->SetMaxValue(Core::AttributeInt32::Create(CORE_INT32_MAX));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Serial number", "serialNumber", "Serial number", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("File", "file", "File", Core::ATTRIBUTE_INTERFACETYPE_STRING);
		attribute->SetDefaultValue(Core::AttributeString::Create(""));
		attribute->SetVisible(false);
	}
	{
		Core::AttributeSettings* attribute = RegisterAttribute("Apply", "apply", "Apply", Core::ATTRIBUTE_INTERFACETYPE_BUTTON);
		attribute->SetDefaultValue(Core::AttributeBool::Create(false));
		attribute->SetVisible(false);
	}

	GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
	GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);

	CreateNewDevice();
}

#endif
