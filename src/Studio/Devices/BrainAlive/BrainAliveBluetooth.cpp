
#include <QLowEnergyController>
#include "BrainAliveBluetooth.h"
#include <qt/QtCore/qeventloop.h> 

static bool mDevice_connected = false;

/* Local Buffer to save BLE data using #Communication_Buffer structure */
static uint8_t BLE_Data_Buffer[BLE_DATA_BUFFER_SIZE][BLE_PACKET_SIZE];

/* #Communication_Buffer type structure to synchronize the Buffer Read/Write operations */
BLEInterface::Communication_Buffer BLE_DATA_Buffer_s;

#ifdef INCLUDE_DEVICE_BRAINALIVE

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
	mDevice_connected = false;
	BLEInterface::app_ble_data_buffer_init();
}

BLEInterface::~BLEInterface()
{
	qDeleteAll(m_devices);
	m_devices.clear();
}
bool BLEInterface:: BLE_Satus()
{
	return mDevice_connected;
}
void BLEInterface::scanDevices()
{
	m_devicesNames.clear();
	qDeleteAll(m_devices);
	m_devices.clear();
	emit devicesNamesChanged(m_devicesNames);

	m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);
	// Start the discovery process
	m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
	emit printf("Scanning for devices...");
	m_deviceDiscoveryAgent->stop();
}

void BLEInterface::read()
{
	if (m_service && m_readCharacteristic.isValid())
		m_service->readCharacteristic(m_readCharacteristic);
}

void BLEInterface::waitForWrite()
{
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
		m_service->writeCharacteristic(m_writeCharacteristic, data, m_writeMode);
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
	//m_deviceDiscoveryAgent->stop();

}

void BLEInterface::onDeviceConnected()
{
	m_servicesUuid.clear();
	m_services.clear();

	emit servicesChanged(m_services);
	m_control->discoverServices();
	setCurrentService(3);
	mDevice_connected = true;

}

void BLEInterface::onDeviceDisconnected()
{
	update_connected(false);
	emit printf("Service disconnected");
	qWarning() << "Remote device disconnected";
	mDevice_connected = false;
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
	uint8_t mData_2[46];
	Q_UNUSED(c)
		int i = 0;
		for (uint8_t mdata : value)
			mData_2[i++] = mdata & 0xFF;
		BLEInterface::write_data_buffer(&BLE_DATA_Buffer_s,&mData_2[0]/*(uint8_t *)&mData_2[0]*/, BLE_PACKET_SIZE);
		
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
	const QString Device_UUID_EEG_Characteristic = "{0000fe41-8e22-4541-9d4c-21edae82ed19}";
	if (m_service)
	{
		foreach(QLowEnergyCharacteristic c, m_service->characteristics()) {
			if (c.isValid())
			{
				if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse ||
					c.properties() & QLowEnergyCharacteristic::Write)
				{
					m_writeCharacteristic = /*c;*/ m_service->characteristic(QBluetoothUuid(Device_UUID_EEG_Characteristic));
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
				if (m_service && m_writeCharacteristic.isValid())
				{
					m_service->writeCharacteristic(m_writeCharacteristic, QByteArray::fromHex("0a8100000d"), m_writeMode);
					qDebug() << "BLEInterface::write: " << QByteArray::fromHex("0a8100000d");
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
void BLEInterface::app_ble_data_buffer_init(void)
{

	BLE_DATA_Buffer_s.data_buffer = &BLE_Data_Buffer[0][0];
	BLE_DATA_Buffer_s.buffer_size = BLE_PACKET_SIZE * BLE_DATA_BUFFER_SIZE;
	BLE_DATA_Buffer_s.data_count = 0U;
	BLE_DATA_Buffer_s.read_pointer = 0U;
	BLE_DATA_Buffer_s.write_pointer = 0U;
}


/* Buffer Reader Function */
uint8_t BLEInterface::read_data_buffer(BLEInterface::Communication_Buffer* buffer, uint8_t* data,
	uint32_t count)
{
	/* Check if amount of requested data is available */
	if ((buffer->data_count < count) || (count == 0))
		return 1;

	/* Assign the amount of data requested to the new buffer */
	for (uint32_t i = 0U; i < count; i++)
	{
		data[i] = buffer->data_buffer[buffer->read_pointer++];
		buffer->data_count--;

		/* Check if buffer overflows */
		if (buffer->read_pointer == buffer->buffer_size)
		{
			/* Go to Start - Buffer is GOL */
			buffer->read_pointer = 0;
		}
	}

	/* Return Success */
	return 0;
}
/* Buffer Writer Function */
uint8_t BLEInterface::write_data_buffer(BLEInterface::Communication_Buffer* buffer, uint8_t* data,
	uint32_t count)
{
	/* Check if space is available for the amount of data that has been pushed & some other param */
	if ((data == NULL) || (count == 0))
		return 1;

	/* Push the data into buffer */
	for (uint32_t i = 0U; i < count; i++)
	{
		buffer->data_buffer[buffer->write_pointer++] = data[i];

		if (buffer->data_count == buffer->buffer_size)
		{
			buffer->read_pointer++;

			/* Check if buffer overflows */
			if (buffer->read_pointer == buffer->buffer_size)
			{
				/* Go to Start - Buffer is GOL */
				buffer->read_pointer = 0;
			}
		}
		else
		{
			buffer->data_count++;
		}
		/* Check if buffer overflows */
		if (buffer->write_pointer == buffer->buffer_size)
		{
			/* Go to Start - Buffer is GOL */
			buffer->write_pointer = 0;
		}
	}

	/* Return Success */
	return 0;
}
/* Buffer Writer Function */
uint32_t get_data_buffer_size(BLEInterface::Communication_Buffer* buffer)
{
	return buffer->data_count;
}
void BLEInterface::Get_BLE_Data( uint8_t *mData)
{
	//uint8_t mData[BLE_PACKET_SIZE];
	if (get_data_buffer_size(&BLE_DATA_Buffer_s) >= BLE_PACKET_SIZE)
		BLEInterface::read_data_buffer(&BLE_DATA_Buffer_s, mData, BLE_PACKET_SIZE);
	
}

#endif