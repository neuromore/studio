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

#ifndef __NEUROMORE_GRAPHATTRIBUTESWIDGET_H
#define __NEUROMORE_GRAPHATTRIBUTESWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <Graph/Node.h>
#include <Graph/Connection.h>
#include <Graph/State.h>
#include <Graph/ActionState.h>
#include <Graph/StateTransition.h>
#include <Graph/StateTransitionCondition.h>
#include <AttributeWidgets/PropertyTreeWidget.h>
#include <Widgets/GraphObjectViewWidget.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include "qtimer.h"
#include <qthread.h>
#include "qbluetoothdevicediscoveryagent.h"
#include "qbluetoothdeviceinfo.h"
#include "qlowenergycontroller.h"
#include "qlowenergyservice.h"
#include "qbluetooth.h"
#include <qobject.h>
#include "qlistwidget.h"
#include "../../studio/src/QtBase/AttributeWidgets/PropertyManager.h"



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
public:
    explicit BLEInterface(QObject* parent = 0);
    ~BLEInterface();
    QBluetoothDeviceDiscoveryAgent* m_deviceDiscoveryAgent;
    void connectCurrentDevice();
    void disconnectDevice();
    Q_INVOKABLE void scanDevices();
    void write(const QByteArray& data);

    bool isConnected() const
    {
        return m_connected;
    }

    int currentService() const
    {
        return m_currentService;
    }

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
    inline void waitForWrite();
    void update_connected(bool connected) {
        if (connected != m_connected) {
            m_connected = connected;
            emit connectedChanged(connected);
        }
    }

   
    QLowEnergyDescriptor m_notificationDesc;
    QLowEnergyController* m_control;
    QList<QBluetoothUuid> m_servicesUuid;
    QLowEnergyService* m_service;
    QLowEnergyCharacteristic m_readCharacteristic;
    QLowEnergyCharacteristic m_writeCharacteristic;
    QList<DeviceInfo*> m_devices;
    //    bool m_foundService;
    QTimer* m_readTimer;
    bool m_connected;
    void searchCharacteristic();
    int m_currentService;
    QLowEnergyService::WriteMode m_writeMode;
};

class GraphAttributesWidget : public QScrollArea, public Core::EventHandler
{
	Q_OBJECT
	public:
		GraphAttributesWidget(QWidget* parent=NULL);
		virtual ~GraphAttributesWidget();

		void InitForGraphObject(GraphObject* object, bool force=false);
		GraphObject* GetGraphObject() const										{ return mGraphObject; }

		void UpdateInterface();

		// event handlers
		void OnStartSession() override											{ ForceReInit(); }
		void OnStopSession() override											{ ForceReInit(); }
		void OnAttributeUpdated(Graph* graph, GraphObject* object, Core::Attribute* attribute) override;
		void OnGraphModified(Graph* graph, GraphObject* object) override;

	public slots:
		void ReInit()															{ InitForGraphObject( mGraphObject ); }
		void ForceReInit()														{ InitForGraphObject( mGraphObject, true ); }
		void OnValueChanged(Property* property);

	private slots:
		// conditions
		void OnAddConditionButtonClicked();
		void OnAddCondition();
		void OnRemoveCondition();

		// actions
		void OnAddActionButtonClicked(uint32 actionTypeIndex);
		void OnAddEnterActionButtonClicked()									{ OnAddActionButtonClicked(0); }
		void OnAddExitActionButtonClicked()										{ OnAddActionButtonClicked(1); }

		void OnAddAction(uint32 actionTypeIndex);
		void OnAddEnterAction()													{ OnAddAction(0); }
		void OnAddExitAction()													{ OnAddAction(1); }

		void OnRemoveAction(uint32 actionTypeIndex);
		void OnRemoveEnterAction()												{ OnRemoveAction(0); }
		void OnRemoveExitAction()												{ OnRemoveAction(1); }

	private:
		struct AttributeLink
		{
			AttributeWidget*			mWidget;
			GraphObject*				mGraphObject;
			uint32						mAttributeIndex;
			Property*					mProperty;

			AttributeLink()
			{
				mWidget			= NULL;
				mProperty		= NULL;
				mGraphObject	= NULL;
				mAttributeIndex	= CORE_INVALIDINDEX32;
			}
		};

		struct ButtonLookup
		{
			QObject*	mButton;
			uint32		mIndex;
		};
		Core::Array<ButtonLookup>	mRemoveButtonTable;

		void InitForGraph(Graph* node, bool showName=true);
		void InitForNode(Node* node, bool showName=true);
		void InitForConnection(Connection* connection);

		void AddAttributes(GraphObject* object, const char* parentGroupName, bool disableAttributes);

		void AddConditions(GraphObject* object, bool readOnly);
		void AddActions(GraphObject* object, bool readOnly);
        void dataReceived(QByteArray data);
        void On_connect();
        void On_Ok();
		uint32 FindRemoveButtonIndex(QObject* button) const;

		GraphObject*					mGraphObject;
		GraphObjectViewWidget*			mGraphObjectView;
		PropertyTreeWidget*				mPropertyTreeWidget;
		Property*						mNameProperty;
		Property*						mNameProperty_1;
		Property*						mNameProperty_2;
		Property*						mNameProperty_3;
		Property*						mNameProperty_4;
		Property*						mNameProperty_5;
		Property*						mNameProperty_6;
		Property*						mNameProperty_7;
		Property*						mNameProperty_8;
        Property*                       mNameProperty_9;
        Property*                       mNameProperty_10;

		Core::Array<AttributeLink>		mAttributeLinks;
		Core::AttributeSet*				mAttributeSet;
		Core::String					mParentGroupName;
		Core::String					mTempString;
        BLEInterface*                   m_bleInterface;
        QPushButton*                    Connect;
        QListWidget*                    mListWidget;
        QWidget*                        mwidget;
        QPushButton*                    Ok_Button;
        QWidget*                        mwidget_2;
        QListWidget*                    mListWidget_2;
};


#endif
