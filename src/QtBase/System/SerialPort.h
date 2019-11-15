/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_SERIALPORT_H
#define __NEUROMORE_SERIALPORT_H

// include required headers
#include "../QtBaseConfig.h"
#include <Core/Array.h>
#include <Core/LogManager.h>

#include <QObject>
#include <QtSerialPort/QSerialPort>


// represents a single serial port
class QTBASE_API SerialPort : public QObject
{
	Q_OBJECT
	public:
		SerialPort(const char* portName = NULL, QObject* parent = NULL);
		virtual ~SerialPort();

		const char* GetPortName()						{ return mPortName.AsChar(); }
		
		// init / reset serial port (do this before / after using it)
		bool Init(const char* portName = NULL);
		bool IsInitialized() const						{ return (mQtPort != NULL); }
		void Reset();

		//open/close/states
		bool Open()										{ if (IsInitialized()) return mQtPort->open(QIODevice::ReadWrite);	return false; }
		void Close()									{ if (IsInitialized()) mQtPort->close();							 			  }
		bool IsOpen() const								{ if (IsInitialized()) return mQtPort->isOpen();					return false; }
		bool IsWriteable() const						{ if (IsInitialized()) return mQtPort->isWritable();				return false; }
		bool IsReadable() const							{ if (IsInitialized()) return mQtPort->isReadable();				return false; }
		uint32 GetNumBytesAvailable()					{ if (IsInitialized()) return mQtPort->bytesAvailable();			return 0;     }
		bool Flush()									{ if (IsInitialized()) return mQtPort->flush();						return false; }
		bool Clear()									{ if (IsInitialized()) return mQtPort->clear();						return false; }
														  
		// configure port								  
		enum EParity
		{
			NO_PARITY = 0,
			EVEN_PARITY = 2,
			ODD_PARITY = 3
		};

		bool SetBaudRate(uint32 rate)					{ if (IsInitialized()) return mQtPort->setBaudRate(rate);							return false; }
		bool SetParity(EParity parity)					{ if (IsInitialized()) return mQtPort->setParity((QSerialPort::Parity)parity);		return false; }
		bool SetNumDataBits(uint32 numBits)				{ if (IsInitialized()) return mQtPort->setDataBits((QSerialPort::DataBits)numBits); return false; }
		bool SetNumStopBits(uint32 numBits)				{ if (IsInitialized()) return mQtPort->setStopBits((QSerialPort::StopBits)numBits); return false; }

		// nonblocking read/write
		int Write(const char* data, uint32 numBytes)	{ if (IsInitialized()) return mQtPort->write(data, numBytes); return 0; }
		int Read(char* data, uint32 numBytes)			{ if (IsInitialized()) return mQtPort->read(data, numBytes);  return 0; }
		bool GetChar(char* character)					{ if (IsInitialized()) return mQtPort->getChar(character);  return false; }
		void UngetChar(char character)					{ if (IsInitialized()) mQtPort->ungetChar(character);  }

		// read/write line
		bool CanReadLine()								{ if (IsInitialized()) return mQtPort->canReadLine();  return false; }
		int ReadLine(Core::String& outLine)				{ if (!IsInitialized()) return 0; int size = mQtPort->readLine(outLine.AsChar(), outLine.GetLength()); outLine.Resize(size); return size; }
		int WriteLine(const Core::String& line)			{ if (IsInitialized()) return mQtPort->write(line.AsChar(), line.GetLength()) + mQtPort->write("\n", 1); return 0; }
		
		Core::String ReadAll()							{ if (IsInitialized()) return FromQtString(QString::fromLatin1(mQtPort->readAll()));  return ""; }

		// wait max. msec milliseconds for data, return false if timeout was reached
		bool WaitForRead(uint32 msec)				    { if (!IsInitialized()) return false; /*if (GetNumBytesAvailable() > 0) return true; */return mQtPort->waitForReadyRead(msec); }
		bool WaitForWrite(uint32 msec)				    { if (IsInitialized()) return mQtPort->waitForBytesWritten(msec); return false; }

		void PrintError() { Core::LogError("SerialPort: Error %i (%s)", mQtPort->error(), FromQtString(mQtPort->errorString()).AsChar()); }

		//
		// helpers
		//

		static Core::Array<Core::String> FindPortsByDescription(const char* description);
		static Core::Array<Core::String> FindPortsByUsbID(uint16 productID, uint16 vendorID);

		static Core::Array<Core::String> FindBluetoothPorts();
		static Core::Array<Core::String> FindAllPorts();

		static Core::String& LogPortsToString(Core::String& inOutString);

		static Core::String CreateWindowsCOMPortName(uint32 portNumber, bool doubleDigitEscape = true);
		static Core::String FormatWindowsCOMPortName(const char* portName, bool doubleDigitEscape = true);
		static uint32 ExtractPortNumber(const char* portName);

	signals:
		void DataAvailable();				

	private:

		Core::String				mPortName;

		// qt port wrapped by this class
		QSerialPort*				mQtPort;
};


#endif
