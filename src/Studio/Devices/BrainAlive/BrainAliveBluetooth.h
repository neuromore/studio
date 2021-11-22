#include "qbluetoothdevicediscoveryagent.h"
#include "qbluetoothdeviceinfo.h"
#include "qlowenergycontroller.h"
#include "qlowenergyservice.h"
#include "qbluetooth.h"
#include <qobject.h>
#include "qlistwidget.h"
#include "qtimer.h"
#include <qthread.h>
#include <Config.h>

#ifdef INCLUDE_DEVICE_BRAINALIVE

#define QML_WRITABLE_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_READONLY_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name NOTIFY name##Changed) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    protected: \
        bool update_##name (type name) { \
            bool ret = false; \
            if ((ret = m_##name != name)) { \
                m_##name = name; \
                emit name##Changed (m_##name); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_CONSTANT_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name CONSTANT) \
    private: \
        type m_##name; \
    public: \
        type get_##name () const { \
            return m_##name ; \
        } \
    private:

#define QML_LIST_PROPERTY(CLASS, NAME, TYPE) \
    public: \
        static int NAME##_count (QQmlListProperty<TYPE> * prop) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            return (instance != NULL ? instance->m_##NAME.count () : 0); \
        } \
        static void NAME##_clear (QQmlListProperty<TYPE> * prop) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            if (instance != NULL) { \
                instance->m_##NAME.clear (); \
            } \
        } \
        static void NAME##_append (QQmlListProperty<TYPE> * prop, TYPE * obj) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            if (instance != NULL && obj != NULL) { \
                instance->m_##NAME.append (obj); \
            } \
        } \
        static TYPE * NAME##_at (QQmlListProperty<TYPE> * prop, int idx) { \
            CLASS * instance = qobject_cast<CLASS *> (prop->object); \
            return (instance != NULL ? instance->m_##NAME.at (idx) : NULL); \
        } \
        QList<TYPE *> get_##NAME##s (void) const { \
            return m_##NAME; \
        } \
    private: \
        QList<TYPE *> m_##NAME;

#define QML_ENUM_CLASS(name, ...) \
    class name : public QObject { \
        Q_GADGET \
    public: \
        enum Type { __VA_ARGS__ }; \
        Q_ENUMS (Type) \
    };

// wrapper properties
#define QML_WRITABLE_PROPERTY_WRAPPER(type, name, data, baseType) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    public: \
        type get_##name () const { \
            return data ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = data != (baseType)name)) { \
                data = name; \
                emit name##Changed (data); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_WRITABLE_PROPERTY_W(type, name, data) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    public: \
        type get_##name () const { \
            return data ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            bool ret = false; \
            if ((ret = data != name)) { \
                data = name; \
                emit name##Changed (data); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_WRITABLE_ENUM_PROPERTY_W(type, name, datatype, data) \
    protected: \
        Q_ENUMS(type) \
        Q_PROPERTY (type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    public: \
        type get_##name () const { \
            return (type)data ; \
        } \
        datatype get_src_##name () const { \
            return data ; \
        } \
    public Q_SLOTS: \
        bool set_##name (type name) { \
            return set_##name((datatype) name); \
        } \
        bool set_##name (datatype name) { \
            bool ret = false; \
            if ((ret = data != name)) { \
                data = name; \
                emit name##Changed ((type)data); \
            } \
            return ret; \
        } \
    Q_SIGNALS: \
        void name##Changed (type name); \
    private:

#define QML_WRITEONLY_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ get_##name WRITE set_##name) \
    private: \
        type m_##name; \
        const type& get_##name() const {return m_##name;} \
    public Q_SLOTS: \
        void set_##name (type name) { \
                m_##name = name; \
        } \
    private:

class QmlProperty : public QObject { Q_OBJECT }; // NOTE : to avoid "no suitable class found" MOC note


class DeviceInfo : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
        Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)
public:
    DeviceInfo(const QBluetoothDeviceInfo& device);
    void setDevice(const QBluetoothDeviceInfo& device);
    QString getName() const { return m_device.name(); }
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;

signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo m_device;
};

class BLEInterface : public QObject
{
    Q_OBJECT

        Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
        Q_PROPERTY(int currentService READ currentService WRITE setCurrentService NOTIFY currentServiceChanged)
        QML_WRITABLE_PROPERTY(int, currentDevice)
        QML_READONLY_PROPERTY(QStringList, devicesNames)
        QML_READONLY_PROPERTY(QStringList, services)
#define BLE_DATA_BUFFER_SIZE 250
#define BLE_PACKET_SIZE 46

        /*Communication Buffer typeDef */

public:
    typedef struct
    {
        uint16_t read_pointer;	/**< Buffer location from where data should be read. */
        uint16_t write_pointer; /**< Buffer location where data should be written. */
        uint16_t size;
        uint8_t value[BLE_DATA_BUFFER_SIZE][BLE_PACKET_SIZE];
    } BLE_buffer_s;

    explicit BLEInterface(QObject* parent = 0);
    ~BLEInterface();
    
    QBluetoothDeviceDiscoveryAgent* m_deviceDiscoveryAgent;
    void connectCurrentDevice();
    void disconnectDevice();
    bool BLE_Satus();
    Q_INVOKABLE void scanDevices();
    void write_data(const QByteArray &data);
    uint8_t BLE_read_buffer(uint8_t* temp, uint16_t length);
    uint8_t BLE_write_buffer(uint8_t* temp, uint16_t length);
    uint8_t BLE_reset_buffer(void);
    uint8_t* Get_BLE_Data( );
    bool isConnected() const
    {
        return m_connected;
    }


    int currentService() const
    {
        return m_currentService;
    }

   inline void waitForWrite();
    void update_connected(bool connected) {
      if (connected != m_connected) {
        m_connected = connected;
        emit connectedChanged(connected);
      }
    }

    QLowEnergyDescriptor m_notificationDesc;
    QLowEnergyController *m_control;
    QList<QBluetoothUuid> m_servicesUuid;
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_readCharacteristic;
    QLowEnergyCharacteristic m_writeCharacteristic;
    QList<DeviceInfo *> m_devices;
    QTimer *m_readTimer;
    bool m_connected;
    void searchCharacteristic();
    int m_currentService;
    QLowEnergyService::WriteMode m_writeMode;
    uint mWrite_pointer = 0;

public slots:
    void setCurrentService(int currentService)
    {
        if (m_currentService == currentService)
            return;
        update_currentService(currentService);
        m_currentService = currentService;
        emit currentServiceChanged(currentService);
    }

signals:
    void statusInfoChanged(QString info, bool isGood);
    void dataReceived(const QByteArray& data);
    void connectedChanged(bool connected);

    void currentServiceChanged(int currentService);

private slots:
    //QBluetothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo&);
    void onScanFinished();
    void onDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    //QLowEnergyController
    void onServiceDiscovered(const QBluetoothUuid&);
    void onServiceScanDone();
    void onControllerError(QLowEnergyController::Error);
    void onDeviceConnected();
    void onDeviceDisconnected();

    //QLowEnergyService
    void onServiceStateChanged(QLowEnergyService::ServiceState s);
    void onCharacteristicChanged(const QLowEnergyCharacteristic& c,
        const QByteArray& value);
    void serviceError(QLowEnergyService::ServiceError e);

    void read();
    void onCharacteristicRead(const QLowEnergyCharacteristic& c, const QByteArray& value);
    void onCharacteristicWrite(const QLowEnergyCharacteristic& c, const QByteArray& value);
    void update_currentService(int currentSerice);

private:
    

};

#endif