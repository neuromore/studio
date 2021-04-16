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

// include required headers
#include "ChannelMultiSelectionWidget.h"
#include <Core/LogManager.h>

#include <QHBoxLayout>
#include <QApplication>
#include <QtCore/QObject>
#include	<ctime>
#include <fstream>

using namespace Core;
using namespace std;

uint mData_2[50];
bool device_connected = false;
//ofstream outdata;
int i = 0;

DeviceInfo::DeviceInfo(const QBluetoothDeviceInfo& info) :
	QObject(), m_device(info)
{

}

QBluetoothDeviceInfo DeviceInfo::getDevice() const
{
	return m_device;
}

QString DeviceInfo::getAddress() const
{
#ifdef Q_OS_MAC
	// workaround for Core Bluetooth:
	return m_device.deviceUuid().toString();
#else
	return m_device.address().toString();
#endif
}

void DeviceInfo::setDevice(const QBluetoothDeviceInfo& device)
{
	m_device = device;
	emit deviceChanged();
}

BLEInterface::BLEInterface(QObject* parent) : QObject(parent),
m_currentDevice(0),
m_control(0),
m_service(0),
m_readTimer(0),
m_connected(false),
m_currentService(0)
{
	m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

	connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
		this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
	connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
		this, SLOT(onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
	connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(onScanFinished()));
}

BLEInterface::~BLEInterface()
{
	qDeleteAll(m_devices);
	m_devices.clear();
}

void BLEInterface::scanDevices()
{
	m_devicesNames.clear();
	qDeleteAll(m_devices);
	m_devices.clear();
	emit devicesNamesChanged(m_devicesNames);
	m_deviceDiscoveryAgent->start();
	//m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(10000);
	emit printf("Scanning for devices...");
}

void BLEInterface::read() {
	if (m_service && m_readCharacteristic.isValid())
		m_service->readCharacteristic(m_readCharacteristic);
}

void BLEInterface::waitForWrite() {
	QEventLoop loop;
	connect(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic, QByteArray)),
		&loop, SLOT(quit()));
	loop.exec();
}

void BLEInterface::write(const QByteArray& data)
{
	qDebug() << "BLEInterface::write: " << data;
	if (m_service && m_writeCharacteristic.isValid())
	{
		if (data.length() > CHUNK_SIZE)
		{
			int sentBytes = 0;
			while (sentBytes < data.length())
			{
				m_service->writeCharacteristic(m_writeCharacteristic,
					data.mid(sentBytes, CHUNK_SIZE),
					m_writeMode);
				sentBytes += CHUNK_SIZE;
				if (m_writeMode == QLowEnergyService::WriteWithResponse)
				{
					waitForWrite();
					if (m_service->error() != QLowEnergyService::NoError)
						return;
				}
			}

		}
		else
			m_service->QLowEnergyService::writeCharacteristic(m_writeCharacteristic, data, m_writeMode);
	}
}

void BLEInterface::addDevice(const QBluetoothDeviceInfo& device)
{
	if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
	{
		qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
			<< device.address().toString();
		m_devicesNames.append(device.name());
		DeviceInfo* dev = new DeviceInfo(device);
		m_devices.append(dev);
		emit devicesNamesChanged(m_devicesNames);
		emit printf("Low Energy device found. Scanning for more...");
	}
	//...
}

void BLEInterface::onScanFinished()
{
	if (m_devicesNames.size() == 0)
		/*emit*/ printf("No Low Energy devices found");
}

void BLEInterface::onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
	if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
		/*emit*/ printf("The Bluetooth adaptor is powered off, power it on before doing discovery.");
	else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
		/*emit*/ printf("Writing or reading from the device resulted in an error.");
	else
		/*emit*/ printf("An unknown error has occurred.");
}



void BLEInterface::connectCurrentDevice()
{
	if (m_devices.isEmpty())
		return;

	if (m_control)
	{
		m_control->disconnectFromDevice();
		delete m_control;
		m_control = 0;

	}
	m_control = new QLowEnergyController(m_devices[m_currentDevice]->getDevice(), this);
	connect(m_control, SIGNAL(connected()),
		this, SLOT(onDeviceConnected()));
	connect(m_control, SIGNAL(error(QLowEnergyController::Error)),
		this, SLOT(onControllerError(QLowEnergyController::Error)));
	connect(m_control, SIGNAL(disconnected()),
		this, SLOT(onDeviceDisconnected()));
	connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
		this, SLOT(onServiceDiscovered(QBluetoothUuid)));
	connect(m_control, SIGNAL(discoveryFinished()),
		this, SLOT(onServiceScanDone()));


	m_control->connectToDevice();
	m_deviceDiscoveryAgent->stop();

}

void BLEInterface::onDeviceConnected()
{
	m_servicesUuid.clear();
	m_services.clear();

	emit servicesChanged(m_services);
	m_control->discoverServices();
	setCurrentService(2);
	device_connected = true;

}

void BLEInterface::onDeviceDisconnected()
{
	update_connected(false);
	emit printf("Service disconnected");
	qWarning() << "Remote device disconnected";
	device_connected = false;
}

void BLEInterface::onServiceDiscovered(const QBluetoothUuid& gatt)
{
	Q_UNUSED(gatt)
		/* emit*/ printf("Service discovered. Waiting for service scan to be done...");
}

void BLEInterface::onServiceScanDone()
{
	m_servicesUuid = m_control->services();
	if (m_servicesUuid.isEmpty())
		/* emit*/ printf("Can't find any services.");
	else {
		m_services.clear();
		foreach(auto uuid, m_servicesUuid)
			m_services.append(uuid.toString());
		emit servicesChanged(m_services);
		m_currentService = -1;// to force call update_currentService(once)
		setCurrentService(0);
		/* emit*/ printf("All services discovered.");
	}
}


void BLEInterface::disconnectDevice()
{
	m_readTimer->deleteLater();
	m_readTimer = NULL;

	if (m_devices.isEmpty()) {
		return;
	}

	//disable notifications
	if (m_notificationDesc.isValid() && m_service) {
		m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
	}
	else {
		m_control->disconnectFromDevice();
		delete m_service;
		m_service = 0;
	}
}

void BLEInterface::onControllerError(QLowEnergyController::Error error)
{
	/* emit*/ printf("Cannot connect to remote device.");
	qWarning() << "Controller Error:" << error;
}

void BLEInterface::onCharacteristicChanged(const QLowEnergyCharacteristic& c,
	const QByteArray& value)
{
	//time_t timetoday;
	//time(&timetoday);
	Q_UNUSED(c)

		for each (uint mdata in value)
		{
			mData_2[i++] = mdata & 0xFF;
			//mData_2[i++] = mdata;
		}
	/*for (int j = 0; j < i; ++j)
	{
		outdata << mData_2[j];
		outdata << ",";
	}
	outdata << "\n";*/
	i = 0;

	qDebug() << value.toHex();
	//emit dataReceived(value);
}
void BLEInterface::onCharacteristicWrite(const QLowEnergyCharacteristic& c,
	const QByteArray& value)
{
	Q_UNUSED(c)
		qDebug() << "Characteristic Written: " << value;
}

void BLEInterface::update_currentService(int indx)
{
	delete m_service;
	m_service = 0;

	if (indx >= 0 && m_servicesUuid.count() > indx) {
		m_service = m_control->createServiceObject(
			m_servicesUuid.at(indx), this);
	}

	if (!m_service) {
		/* emit*/ printf("Service not found.");
		return;
	}

	connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
		this, SLOT(onServiceStateChanged(QLowEnergyService::ServiceState)));
	connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic, QByteArray)),
		this, SLOT(onCharacteristicChanged(QLowEnergyCharacteristic, QByteArray)));
	/*connect(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic, QByteArray)),
		this, SLOT(onCharacteristicRead(QLowEnergyCharacteristic, QByteArray)));*/
	connect(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic, QByteArray)),
		this, SLOT(onCharacteristicWrite(QLowEnergyCharacteristic, QByteArray)));
	connect(m_service, SIGNAL(error(QLowEnergyService::ServiceError)),
		this, SLOT(serviceError(QLowEnergyService::ServiceError)));

	if (m_service->state() == QLowEnergyService::DiscoveryRequired) {
		/* emit*/ printf("Connecting to service...");
		m_service->discoverDetails();
	}
	else
		searchCharacteristic();
}
void BLEInterface::onCharacteristicRead(const QLowEnergyCharacteristic& c,
	const QByteArray& value)
{
	Q_UNUSED(c)
		qDebug() << "Characteristic Read: " << value;
	/*mData_2 = value;*/
	emit dataReceived(value);
}

void BLEInterface::searchCharacteristic() {
	/*const QString Device_UUID_EEG_Characteristic = "{0000fe41-8e22-4541-9d4c-21edae82ed19}";*/
	if (m_service) {
		foreach(QLowEnergyCharacteristic c, m_service->characteristics()) {
			if (c.isValid())
			{
				if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse ||
					c.properties() & QLowEnergyCharacteristic::Write)
				{
					m_writeCharacteristic = c; /*m_service->characteristic(QBluetoothUuid(Device_UUID_EEG_Characteristic));*/
					update_connected(true);
					if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse)
						m_writeMode = QLowEnergyService::WriteWithoutResponse;
					else
						m_writeMode = QLowEnergyService::WriteWithResponse;

				}
				if (c.properties() & QLowEnergyCharacteristic::Read)
				{
					m_readCharacteristic = c;
					if (!m_readTimer)
					{
						m_readTimer = new QTimer(this);
						connect(m_readTimer, &QTimer::timeout, this, &BLEInterface::read);
						m_readTimer->start();
						m_readTimer->setInterval(4);
					}

				}
				m_notificationDesc = c.descriptor(
					QBluetoothUuid::ClientCharacteristicConfiguration);
				if (m_notificationDesc.isValid()) {
					m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
				}
			}
		}
	}
}


void BLEInterface::onServiceStateChanged(QLowEnergyService::ServiceState s)
{
	qDebug() << "serviceStateChanged, state: " << s;
	if (s == QLowEnergyService::ServiceDiscovered) {
		searchCharacteristic();
	}
}
void BLEInterface::serviceError(QLowEnergyService::ServiceError e)
{
	qWarning() << "Service error:" << e;
}


// constructor
ChannelMultiSelectionWidget::ChannelMultiSelectionWidget(QWidget* parent) : QWidget(parent)
{
	//outdata.open("C:\\Users\\Public\\Downloads\\data.txt");
	m_bleInterface = new BLEInterface(this);
	mShowUsedCheckbox = new QCheckBox();
	Scan = new QPushButton();
	Start = new  QPushButton();
	Start->setVisible(false);
	mShowUsedCheckbox->setText("Used");
	mShowUsedCheckbox->setToolTip("Show only channels used by classifier.");
	mShowUsedCheckbox->setChecked(false);
	connect(mShowUsedCheckbox, SIGNAL(stateChanged(int)), this, SLOT(OnShowUsedCheckboxToggled(int)));
	mChannelMultiCheckbox = new HMultiCheckboxWidget();
	connect(mChannelMultiCheckbox, SIGNAL(SelectionChanged()), this, SLOT(OnChannelSelectionChanged()));
	Scan->setText(" Scan ");
	Start->setText(" Start ");
	mDeviceSelectionWidget = new DeviceSelectionWidget();
	connect(mDeviceSelectionWidget, SIGNAL(DeviceSelectionChanged(Device*)), this, SLOT(OnDeviceSelectionChanged(Device*)));
	connect(Scan, &QPushButton::clicked, this, &ChannelMultiSelectionWidget::Scan_BLE);
	connect(Start, &QPushButton::clicked, this, &ChannelMultiSelectionWidget::On_Start);
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mDeviceSelectionWidget);
	hLayout->addWidget(mShowUsedCheckbox);
	hLayout->addWidget(mChannelMultiCheckbox);
	hLayout->addWidget(Scan);
	hLayout->addWidget(Start);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);


	connect(m_bleInterface, &BLEInterface::dataReceived,
		this, &ChannelMultiSelectionWidget::dataReceived);

	connect(m_bleInterface, &BLEInterface::devicesNamesChanged,
		[this](QStringList devices) {
			mListWidget->clear();
			mListWidget->addItems(devices);
		});
	connect(m_bleInterface, &BLEInterface::servicesChanged,
		[this](QStringList services) {
			mListWidget->clear();
			mListWidget->addItems(services);
		});
	setLayout(hLayout);
	mAutoSelectType = SELECT_NONE;
	mShowOnlyEEGChannels = false;

}

// destructor
ChannelMultiSelectionWidget::~ChannelMultiSelectionWidget()
{

}
void ChannelMultiSelectionWidget::On_Start()
{
	QByteArray data;
	m_bleInterface->setCurrentService(2);
	data = QByteArray::fromHex("0a8000000d");
	m_bleInterface->write("0a8000000d");
}

void ChannelMultiSelectionWidget::Scan_BLE()
{
	QWidget* mwidget = new QWidget();
	mwidget->setFixedHeight(400);
	mwidget->setFixedWidth(300);
	mwidget->setWindowModality(Qt::ApplicationModal);
	mListWidget = new QListWidget();
	QVBoxLayout* vLayout_2 = new QVBoxLayout();
	Connect = new QPushButton();

	mListWidget->setFixedHeight(400);
	mListWidget->setFixedWidth(300);

	vLayout_2->setMargin(0);
	vLayout_2->setSpacing(0);
	vLayout_2->addWidget(mListWidget, 1, Qt::AlignTop);
	vLayout_2->addWidget(Connect);
	Connect->setText(" Connect ");
	mwidget->setLayout(vLayout_2);
	mwidget->setVisible(true);
	mListWidget->setVisible(true);
	Connect->setVisible(true);
	m_bleInterface->scanDevices();
	connect(Connect, &QPushButton::clicked, this, &ChannelMultiSelectionWidget::On_connect);

}

void  ChannelMultiSelectionWidget::On_connect()
{

	m_bleInterface->set_currentDevice(0/*mListWidget->currentRow()*/);
	m_bleInterface->connectCurrentDevice();
	if (device_connected == true)
	{
		Start->setVisible(true);
		Connect->setVisible(false);
	}
	/*QByteArray data;
	data = QByteArray::fromHex("0a8000000d");
	m_bleInterface->write(data);*/
	//Start->setVisible(true);

}
void ChannelMultiSelectionWidget::dataReceived(QByteArray data)
{
	//mListWidget->clear();
	//mListWidget->addItem(data.toHex());
	//printf(data.toHex());
}

void ChannelMultiSelectionWidget::Init()
{
	// init device selection widget
	mDeviceSelectionWidget->Init();

	if (mDeviceSelectionWidget->GetSelectedDevice() != NULL)
		ReInit(mDeviceSelectionWidget->GetSelectedDevice());
}


// initialize for a new device : get sensors, fill checkbox widget
void ChannelMultiSelectionWidget::ReInit(Device* device)
{
	// if device not specified, automatically use the first one
	if (device == NULL)
		device = mDeviceSelectionWidget->GetSelectedDevice();

	// prepare the arrays for the reinitialization
	mAvailableChannels.Clear();
	Array<String> names;
	Array<String> tooltips;
	Array<Color> colors;

	// in case there is no device the pointer will be NULL and we don't add any checkboxes
	BciDevice* headset = NULL;
	if (device != NULL)
	{
		//TODO for now, we only allow neuro headset and EEG channels
		if (device->GetBaseType() == BciDevice::BASE_TYPE_ID)
		{
			headset = static_cast<BciDevice*>(device);

			// add all EEG channels
			const uint32 numRawSensors = headset->GetNumNeuroSensors();
			for (uint32 i = 0; i < numRawSensors; i++)
			{
				Sensor* sensor = headset->GetNeuroSensor(i);
				Channel<double>* channel = sensor->GetChannel();

				// skip channel, if it is not used
				if (IsShowUsedChecked() == true && mUsedChannels.Contains(channel) == false)
					continue;

				mAvailableChannels.Add(channel);
				names.Add(channel->GetName());
				tooltips.Add(channel->GetName());
				colors.Add(channel->GetColor());
			}
			// add all other non-EEG sensors
			if (mShowOnlyEEGChannels == false)
			{
				const uint32 numSensors = headset->GetNumSensors();
				for (uint32 i = 0; i < numSensors; ++i)
				{
					Sensor* sensor = headset->GetSensor(i);
					Channel<double>* channel = sensor->GetChannel();

					// skip channel, if it is not used
					if (IsShowUsedChecked() == true && mUsedChannels.Contains(channel) == false)
						continue;

					// check if this a neuro sensor (there is no other way to do it right now)
					bool isNeuroSensor = false;
					/*const uint32 numRawSensors = */headset->GetNumNeuroSensors();
					for (uint32 j = 0; j < numSensors && isNeuroSensor == false; ++j)
					{
						if (headset->GetSensor(i) == headset->GetNeuroSensor(j))
							isNeuroSensor = true;
					}

					// don't add this sensor if its a neurosensor
					if (isNeuroSensor == false)
					{

						mAvailableChannels.Add(channel);
						names.Add(channel->GetName());
						tooltips.Add(channel->GetName());
						colors.Add(channel->GetColor());
					}
				}
			}

		}



	}

	// hide "used" checkbox is no device is present
	mShowUsedCheckbox->setVisible(device != NULL);

	// reinit the multi checkbox widget
	mChannelMultiCheckbox->ReInit(names, tooltips, colors, "All");

	// auto-select channels
	const uint32 numCheckBoxes = mChannelMultiCheckbox->GetNumCheckboxes();
	uint32 numBoxesToSelect = 0;
	switch (mAutoSelectType)
	{
	case AutoSelectType::SELECT_NONE: numBoxesToSelect = 0; break;
	case AutoSelectType::SELECT_FIRST: numBoxesToSelect = 1; break;

	case AutoSelectType::SELECT_ALL:
	default:
		numBoxesToSelect = numCheckBoxes; break;
	}
	mChannelMultiCheckbox->CheckXCheckboxes(numBoxesToSelect);

	// disable REF and GND by default
	for (uint32 i = 0; i < names.Size(); i++)
	{
		if (i >= mChannelMultiCheckbox->GetNumCheckboxes())
			return;

		if (names[i] == "REF" || names[i] == "GND")
			mChannelMultiCheckbox->GetCheckbox(i)->setChecked(false);
	}
}


void ChannelMultiSelectionWidget::OnChannelSelectionChanged()
{
	const uint32 numSensors = mAvailableChannels.Size();

	mSelectedChannels.Clear();

	for (uint32 i = 0; i < numSensors; ++i)
	{
		if (mChannelMultiCheckbox->IsChecked(i))
			mSelectedChannels.Add(mAvailableChannels[i]);
	}

	emit ChannelSelectionChanged();
}

void ChannelMultiSelectionWidget::OnDeviceSelectionChanged(Device* device)
{
	ReInit(device);
}


void ChannelMultiSelectionWidget::OnShowUsedCheckboxToggled(int state)
{
	// reinit with current device
	ReInit();

	emit ShowUsedCheckboxToggled(state);
}


void ChannelMultiSelectionWidget::SetChannelAsUsed(Channel<double>* channel, bool used)
{
	// not in list, add it 
	const bool inList = mUsedChannels.Contains(channel);
	if (used == true && inList == false)
		mUsedChannels.Add(channel);

	// in list, remove it
	else if (used == false && inList == true)
		mUsedChannels.RemoveByValue(channel);
}

void ChannelMultiSelectionWidget::ClearUsedChannels()
{
	mUsedChannels.Clear();
	mShowUsedCheckbox->setVisible(false);
}


void ChannelMultiSelectionWidget::SetChecked(Channel<double>* channel, bool checked)
{
	const uint32 channelIndex = mSelectedChannels.Find(channel);
	if (channelIndex != CORE_INVALIDINDEX32)
		SetChecked(channelIndex, checked);
}


void ChannelMultiSelectionWidget::SetChecked(uint32 index, bool checked)
{
	mChannelMultiCheckbox->SetChecked(index, checked);
}


void ChannelMultiSelectionWidget::SetVisible(uint32 index, bool visible)
{
	if (index >= mChannelMultiCheckbox->GetNumCheckboxes())
		return;

	mChannelMultiCheckbox->GetCheckbox(index)->setVisible(visible);
}

