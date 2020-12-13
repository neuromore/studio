#include "BrainFlowNodes.h"

#include "../../EngineManager.h"

namespace
{
	constexpr int MinBoardID = static_cast<int>(BoardIds::FIRST);
	constexpr int MaxBoardID = static_cast<int>(BoardIds::LAST);
	constexpr int BoardIDSize = MaxBoardID - MinBoardID + 1;
	constexpr int BoardIDShift = MinBoardID < 0 ? -MinBoardID : 0; // shift until zero
	constexpr int DefaultBoardID = static_cast<int>(BoardIds::SYNTHETIC_BOARD);

	constexpr int MinIPPortValue = 1;
	constexpr int MaxIPPortValue = 65535;
	constexpr int DefaultIPPortValue = 50000;

	constexpr auto DefaultIPAddressValue = "127.0.0.1";


	unsigned boardIDToBoardIndex(int boardID)
	{
		return boardID + BoardIDShift;
	}

	int boardIndexToBoardID(unsigned boardIndex)
	{
		return static_cast<int>(boardIndex) - BoardIDShift;
	}


	std::string getDeviceNameSafely(int boardID)
	{
		try {
			return BoardShim::get_device_name(boardID);
		}
		catch (...)
		{
			return "";
		}
	}
}

void BrainFlowNode::Init()
{
	DeviceInputNode::Init();
	{

		Core::AttributeSettings* attribute = RegisterAttribute("Board ID", "boardID", "Identificator of the board", Core::ATTRIBUTE_INTERFACETYPE_COMBOBOX);
		attribute->ResizeComboValues(BoardIDSize);
		for (int boardID = MinBoardID; boardID <= MaxBoardID; ++boardID)
			attribute->SetComboValue(boardIDToBoardIndex(boardID), getDeviceNameSafely(boardID).c_str());
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
		Core::AttributeSettings* attribute = RegisterAttribute("Apply", "apply", "Apply", Core::ATTRIBUTE_INTERFACETYPE_CHECKBOX);
		attribute->SetDefaultValue(Core::AttributeBool::Create(false));
	}

	GetAttributeSettings(ATTRIB_RAWOUTPUT)->SetVisible(false);
	GetAttributeSettings(ATTRIB_UPLOAD)->SetVisible(false);
	GetAttributeSettings(ATTRIB_DEVICEINDEX)->SetVisible(false);
}

void BrainFlowNode::OnAttributesChanged()
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

Device* BrainFlowNode::FindDevice()
{
	for (unsigned i = 0; i < GetDeviceManager()->GetNumDevices(); ++i)
		if (auto* device = dynamic_cast<BrainFlowDevice*>(GetDeviceManager()->GetDevice(i)))
			if (device->GetParams() == GetParams() && device->GetBoardId() == GetBoardID())
				return device;

	return nullptr;
}

void BrainFlowNode::ReInit(const Core::Time& elapsed, const Core::Time& delta)
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

void BrainFlowNode::SynchronizeParams()
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

void BrainFlowNode::CreateNewDevice()
{
	// create new device
	if (auto* deviceManager = GetDeviceManager())
		if (auto* deviceDriver = deviceManager->FindDeviceDriverByDeviceType(BrainFlowDevice::TYPE_ID))
			if (auto* newDevice = new BrainFlowDevice(static_cast<BoardIds>(GetBoardID()), GetParams(),	deviceDriver))
				GetDeviceManager()->AddDeviceAsync(newDevice);
}