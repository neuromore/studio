 /*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "SerialPort.h"
#include <EngineManager.h>
#include <Core/LogManager.h>

// TODO replace with our own regex wrapper
#include <regex>

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

using namespace Core;

// constructor
SerialPort::SerialPort(const char* portName, QObject* parent) : QObject(parent)
{
	mPortName = portName;
	mQtPort = NULL;
}


// destructor
SerialPort::~SerialPort()
{
	// free serial port
	GetEngine()->GetSerialPortManager()->ReleaseSerialPort(mPortName.AsChar());

	// FIXME if we try to delete the serial port on destruction, and the BT device is gone, the main thread will hang 15 sec	
	//mQtPort->deleteLater();
}


// initialize QT serial port
bool SerialPort::Init(const char* portName)
{
	if (IsInitialized() == true)
		Reset();

	if (portName != NULL)
	{
		mPortName = portName;
	}
	else
	{
		// if no port was set in constructor, the init argument mus be used
		if (mPortName == "")
			return false;
	}

	// check if the port can be used - return false if port is currently in use
	if (GetEngine()->GetSerialPortManager()->AcquireSerialPort(mPortName.AsChar()) == false)
		return false;
	
	// create serial port
	mQtPort = new QSerialPort(mPortName.AsChar(), this); 

	// disable flow control
	mQtPort->setFlowControl(QSerialPort::NoFlowControl);

	// forward signal
	connect(mQtPort, SIGNAL(readyRead()), this, SIGNAL(DataAvailable()));
	
	return true;
}


// close/delete QT serial port
void SerialPort::Reset()
{
	if (IsInitialized() == false)
		return;

	Close();

	// free serial port
	GetEngine()->GetSerialPortManager()->ReleaseSerialPort(mPortName.AsChar());

	mQtPort->deleteLater();
	mQtPort = NULL;
}


// search for all valid port with the same description string
Array<String> SerialPort::FindPortsByDescription(const char* description)
{
	GetEngine()->GetSerialPortManager()->LockListing();

	Array<String> foundPorts;

	// iterate over available ports
	QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
	const uint32 numPorts = portInfos.length();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// check if description strings match
		if (portInfos[i].description().compare(description) == 0)
		{
			// add found serial port name
			String name = FromQtString(portInfos[i].portName());
			foundPorts.Add(name);
		}
	}

	GetEngine()->GetSerialPortManager()->UnlockListing();

	return foundPorts;
}


// search for all valid port with the same description string
Array<String> SerialPort::FindPortsByUsbID(uint16 productID, uint16 vendorID)
{
	GetEngine()->GetSerialPortManager()->LockListing();

	Array<String> foundPorts;

	// iterate over available ports
	QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
	const uint32 numPorts = portInfos.length();
	for (uint32 i=0; i<numPorts; ++i)
	{
		// check if ids match
		if (portInfos[i].productIdentifier() == productID &&
			portInfos[i].vendorIdentifier() == vendorID)
		{
			// add found serial port name
			String name = FromQtString(portInfos[i].portName());
			foundPorts.Add(name);
		}
	}
	
	GetEngine()->GetSerialPortManager()->UnlockListing();

	return foundPorts;
}


// create a list of all bluetooth serial ports
Array<String> SerialPort::FindBluetoothPorts()
{
	// TODO this probably depends on the OS! Check string on OSX!
	return FindPortsByDescription("Standard Serial over Bluetooth link");
}


// search for all valid port with the same description string
Array<String> SerialPort::FindAllPorts()
{
	GetEngine()->GetSerialPortManager()->LockListing();

	Array<String> foundPorts;

	// iterate over available ports
	for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
	{
		// add found serial port name
		String name = FromQtString(info.portName());
		foundPorts.Add(name);
	}

	GetEngine()->GetSerialPortManager()->UnlockListing();

	return foundPorts;
}


String& SerialPort::LogPortsToString(String& inOutString)
{
	GetEngine()->GetSerialPortManager()->LockListing();

	String tempStr;
	uint32 numPorts = 0;

	// iterate over available ports
	for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
	{
		tempStr.Format(" portName = %s\n", FromQtString(info.portName()).AsChar()); inOutString += tempStr.AsChar();
		tempStr.Format(" systemLocation = %s\n", FromQtString(info.systemLocation()).AsChar()); inOutString += tempStr.AsChar();
		tempStr.Format(" manufacturer = %s\n", FromQtString(info.manufacturer()).AsChar()); inOutString += tempStr.AsChar();
		tempStr.Format(" description = %s\n", FromQtString(info.description()).AsChar()); inOutString += tempStr.AsChar();
		tempStr.Format(" serialNumber = %s\n", FromQtString(info.serialNumber()).AsChar()); inOutString += tempStr.AsChar();
		if (info.hasProductIdentifier() == true)
		{
			tempStr.Format(" productID = %i\n", info.productIdentifier());
			inOutString += tempStr.AsChar();
		}
		if (info.hasVendorIdentifier() == true)
		{
			tempStr.Format(" vendorID = %i\n", info.vendorIdentifier());
			inOutString += tempStr.AsChar();
		}

		inOutString += "\n\n";

		numPorts++;
	}

	GetEngine()->GetSerialPortManager()->UnlockListing();

	if (numPorts == 0)
		inOutString += "No Serial Ports found.";

	return inOutString;
}



String SerialPort::CreateWindowsCOMPortName(uint32 portNumber, bool escape)
{
	String portName;
	
	portName.Format("COM%i", portNumber);
	
	// prepend "special" sequence
	if (escape == true && portNumber >= 10)	
		portName = "\\\\.\\" + portName; 			// NOTE escape chars: this is actually "\\.\COM" in bytes
	
	return portName;
}


// reformat windows com port name, e.g. to apply double digit escape when neccessary
String SerialPort::FormatWindowsCOMPortName(const char* portName, bool doubleDigitEscape)
{
	const uint32 portNumber = ExtractPortNumber(portName);
	return CreateWindowsCOMPortName(portNumber, doubleDigitEscape);
}



uint32 SerialPort::ExtractPortNumber(const char* portName)
{
	// Explained from left to right:
	//
	//  COM		the three chars COM
	//  (\d+)	the group we want to capture: has one or more digits
	std::regex expression("COM(\\d+)", std::regex_constants::icase);	
	std::string input = portName;
	std::smatch results;

	if (std::regex_match(input, results, expression) == false)
		return 0; // TODO how to handle failure case?? return -1 or add 'IsValidPortName(portName)' method?

	// get string of first captured group
	// Note: str(0) contains whole string, str(1) first group, str(2) the second etc
	std::string portNumberString = results.str(1);

	// return port number
	const uint32 portNumber = String(portNumberString.c_str()).ToInt();
	return portNumber;
}
