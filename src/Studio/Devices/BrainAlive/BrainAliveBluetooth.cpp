
#include <QLowEnergyController>
#include "BrainAliveBluetooth.h"
#include <qt/QtCore/qeventloop.h> 

static bool mDevice_connected = false;

/* BLE local Buffer */
BLEInterface::BLE_buffer_s BLE_buffer;

uint8_t mData[BLE_PACKET_SIZE];

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
	BLE_reset_buffer();
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

void BLEInterface::write_data(const QByteArray& data)
{

  qDebug() << "BLEInterface::write: " << QByteArray::fromHex(data);
  set_currentDevice(0);
  update_currentService(3);
  if (m_service && m_writeCharacteristic.isValid()) {
  
        m_service->writeCharacteristic(m_writeCharacteristic, QByteArray::fromHex(data),
                                       m_writeMode);
       
        if (m_writeMode == QLowEnergyService::WriteWithResponse) {
          waitForWrite();
          if (m_service->error() != QLowEnergyService::NoError)
            return;
        }
     
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
        m_deviceDiscoveryAgent->stop();
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
	Q_UNUSED(c)
	int i = 0;
	uint8_t mData_2[50][BLE_PACKET_SIZE];
	if ((value.size() % BLE_PACKET_SIZE) == 0)
	{
		uint8_t length = 0;
		for (uint8_t i = 0; i < value.size()/ BLE_PACKET_SIZE; i++)
		{
			for(uint8_t j =0; j< BLE_PACKET_SIZE;j++)
				mData_2[mWrite_pointer][j] = value[j + length];
			
			
			BLE_write_buffer(mData_2[mWrite_pointer], BLE_PACKET_SIZE);
			mWrite_pointer++;
			length += BLE_PACKET_SIZE;
			
		}
		mWrite_pointer = 0;
	}
	
		
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
	const QString Device_UUID_WRITE_Characteristic = "{0000fe41-8e22-4541-9d4c-21edae82ed19}";
    const QString Device_UUID_NOTIFY_Characteristic = "{0000fe42-8e22-4541-9d4c-21edae82ed19}";
	if (m_service)
	{
		foreach(QLowEnergyCharacteristic c, m_service->characteristics())
		{
            if (c.uuid() == QBluetoothUuid(Device_UUID_WRITE_Characteristic)) {
              if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse ||
                  c.properties() & QLowEnergyCharacteristic::Write) {
                m_writeCharacteristic = /*c;*/ m_service->characteristic(
                    QBluetoothUuid(Device_UUID_WRITE_Characteristic));
                update_connected(true);
                if (c.properties() & QLowEnergyCharacteristic::WriteNoResponse)
                  m_writeMode = QLowEnergyService::WriteWithoutResponse;
                else
                  m_writeMode = QLowEnergyService::WriteWithResponse;
              }
            }
            if (c.uuid() == QBluetoothUuid(Device_UUID_NOTIFY_Characteristic)) {
				m_notificationDesc = c.descriptor(
					QBluetoothUuid::ClientCharacteristicConfiguration);
				if (m_notificationDesc.isValid()) {
					m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
				}
				if (m_service && m_writeCharacteristic.isValid())
				{
					m_service->writeCharacteristic(m_writeCharacteristic, QByteArray::fromHex("0a8100000d"), m_writeMode);
                    qDebug() << "BLEInterface::write: "<< (" command send ");
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

uint16_t get_data_buffer_size(void)
{
	return BLE_buffer.size;
}

uint8_t BLEInterface::BLE_reset_buffer(void)
{

	BLE_buffer.read_pointer = 0;
	BLE_buffer.write_pointer = 0;
	BLE_buffer.size = 0;
	for (uint16_t i = 0; i < BLE_DATA_BUFFER_SIZE; i++)
		for (uint8_t j = 0; j < BLE_PACKET_SIZE; j++)
			BLE_buffer.value[i][j] = 0;
	return 0;
}

uint8_t BLEInterface::BLE_write_buffer(uint8_t* temp, uint16_t length)
{
	if (temp == NULL || length != BLE_PACKET_SIZE)
		return 1;

	for (uint16_t i = 0; i < length; i++)
		BLE_buffer.value[BLE_buffer.write_pointer][i] = temp[i];

	if (++(BLE_buffer.write_pointer) == BLE_DATA_BUFFER_SIZE)
		BLE_buffer.write_pointer = 0;

	if (BLE_buffer.size == BLE_DATA_BUFFER_SIZE)
	{
		if (++(BLE_buffer.read_pointer) == BLE_DATA_BUFFER_SIZE)
			BLE_buffer.read_pointer = 0;
	}
	else
		BLE_buffer.size++;

	return 0;
}

uint8_t BLEInterface::BLE_read_buffer(uint8_t* temp, uint16_t length)
{
	if (temp == NULL || length != BLE_PACKET_SIZE)
		return 1;

	if (BLE_buffer.size == 0)
		return 1;

	for (uint16_t i = 0; i < length; i++)
		temp[i] = BLE_buffer.value[BLE_buffer.read_pointer][i];

	if (++(BLE_buffer.read_pointer) == BLE_DATA_BUFFER_SIZE)
		BLE_buffer.read_pointer = 0;

	BLE_buffer.size--;

	return 0;
}
uint8_t* BLEInterface::Get_BLE_Data()
{
	
	if (get_data_buffer_size() >= BLE_PACKET_SIZE)
	{
		BLE_read_buffer(mData, BLE_PACKET_SIZE);
		return mData;
	}
	else
		return NULL;
}

#endif