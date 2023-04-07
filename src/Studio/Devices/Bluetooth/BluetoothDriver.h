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

#ifndef __NEUROMORE_BLUETOOTHDRIVER_H
#define __NEUROMORE_BLUETOOTHDRIVER_H

#include <Config.h>
#include <DeviceDriver.h>
#include <Core/EventHandler.h>
#include <Core/Array.h>
#include <QObject>
#include <QTimer>
#include <QBluetoothDeviceDiscoveryAgent>
#include "BluetoothDevice.h"


// the Bluetooth driver class
class BluetoothDriver : public QObject, public DeviceDriver, public Core::EventHandler
{
	Q_OBJECT
	public:

		enum { TYPE_ID = DeviceTypeIDs::DRIVER_TYPEID_BLUETOOTH};

		// constructor & destructor
		BluetoothDriver();
		virtual ~BluetoothDriver();

		const char* GetName() const override                                        { return "Bluetooth Devices"; }

		uint32 GetType() const override												{ return TYPE_ID; }

		// initialization
		bool Init() override;

		// main update function
        void Update(const Core::Time& elapsed, const Core::Time& delta) override    {}

		bool HasAutoDetectionSupport() const override                               { return true; }
		void SetAutoDetectionEnabled(bool enable = true) override;
		bool IsDetectionRunning() const override final                              { return mIsSearching; }
		virtual void DetectDevices() override;
		virtual Device* CreateDevice(uint32 deviceTypeID) override;

		// event handler (removes serial threads)
		void OnRemoveDevice(Device* device) override;

		// helper functions
		QBluetoothDeviceInfo* FindDeviceInfo(QString address);
        BluetoothDevice* FindDevice(const QBluetoothDeviceInfo& deviceInfo);
        uint32 FindBluetoothDeviceIndex(Device* device);
        uint32 FindBluetoothDeviceIndex(const QBluetoothDeviceInfo& deviceInfo);

        bool IsDeviceConnecting() const;

	private slots:
		// autodetect timer
		void OnDetectDevices();
		void OnDeviceFinished(BluetoothDevice* device);

		// Bluetooth device discovery agent slots
		void OnDeviceScanFinished();
		void OnDeviceDiscovered(const QBluetoothDeviceInfo& deviceInfo);
		void OnDeviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);
		void OnDeviceScanCanceled();

        //void OnConnectNextDevice();
void Connect(const QBluetoothDeviceInfo& deviceInfo);

   protected:
      virtual void StartAutoDetection() override;
      virtual void StopAutoDetection() override;

	private:
		// device discovery
		QTimer*								mAutodetectTimer;
		bool								mIsBtleSupported;
		bool								mIsSearching;
		QBluetoothDeviceDiscoveryAgent*		mBluetoothDeviceDiscoveryAgent;
		Core::Array<QBluetoothDeviceInfo>	mDiscoveredDeviceInfos;

		// Bluetooth devices
		Core::Array<BluetoothDevice*>		mDevices;
};


#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class BluetoothDeviceSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    BluetoothDeviceSelectorDialog(Core::Array<QBluetoothDeviceInfo> deviceInfos, QWidget* parent) : QDialog(parent)
    {
        const uint32 numDeviceInfos = deviceInfos.Size();
        
        if (numDeviceInfos > 1)
            setWindowTitle("Multiple Bluetooth devices found");
        else
            setWindowTitle("Bluetooth device found");
        setMinimumWidth(450);
        
        mClickedIndex = CORE_INVALIDINDEX32;
        
        QVBoxLayout* vLayout = new QVBoxLayout();
        setLayout(vLayout);
        
        vLayout->addWidget( new QLabel("Please select your Bluetooth device.\nIt can still take several seconds after selection till the connection to the device got established.") );

        for (uint32 i=0; i<numDeviceInfos; ++i)
        {
            QPushButton* button = new QPushButton();
            button->setText(deviceInfos[i].name());
            button->setProperty("index", i);
            connect( button, SIGNAL(clicked()), this, SLOT(OnButtonClicked()) );
            
            vLayout->addWidget(button);
        }
    }
    
public slots:
    void OnButtonClicked()
    {
        QPushButton* button = qobject_cast<QPushButton*>( sender() );
        mClickedIndex = button->property("index").toInt();
        accept();
    }
    
public:
    uint32 GetIndex() const                 { return mClickedIndex; }
    
private:
    uint32 mClickedIndex;
};


#endif
