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

// include precompiled header
#include <Studio/Precompiled.h>

// include required headers
#include "ClientInfoWidget.h"
#include <Core/LogManager.h>
#include <EngineManager.h>
#include <QtBaseManager.h>
#include "../../MainWindow.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QApplication>


using namespace Core;

// constructor
ClientInfoWidget::ClientInfoWidget(QWidget* parent) : QWidget(parent)
{
	mClientTable	= NULL;
	Init();
}


// initialize the widget
bool ClientInfoWidget::Init()
{
	// create a grid layout
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(0);
	uint32 row = 0;

	// clients table
	mClientTable = new QTableWidget();
	//mClientTable->setMaximumHeight(50);
	//gridLayout->addWidget(new QLabel("Clients"), row, 0);
	gridLayout->addWidget(mClientTable, row, 1);
	row++;

	setLayout(gridLayout);
	ReInit();
	return true;
}


// reinit the clients table widget
void ClientInfoWidget::ReInit()
{
	// clear the table
	mClientTable->clear();

	// hide vertical headers
	mClientTable->verticalHeader()->hide();

	// horizontal headers
	mClientTable->setColumnCount(4);
	QStringList horizontalHeaderLabels;
	horizontalHeaderLabels.append("Name");
	horizontalHeaderLabels.append("Type");
	horizontalHeaderLabels.append("IP Address");
	horizontalHeaderLabels.append("Protocol");
	mClientTable->setHorizontalHeaderLabels(horizontalHeaderLabels);

	// get the number of clients and iterate through them
	NetworkServer*  networkServer = GetNetworkServer();
	const uint32 numClients = networkServer->GetNumClients();
	mClientTable->setRowCount( numClients );
	for (uint32 i=0; i<numClients; ++i)
	{
		// setup row
		mClientTable->setRowHeight( i, 18 );

		// add items column by column
		uint32 column = 0;

		// get the host address
		NetworkServerClient* client = networkServer->GetClient(i);
		const QHostAddress& hostAddress = client->mHostAddress;

		// add the visualization name
		QTableWidgetItem* item = new QTableWidgetItem( client->mName.AsChar() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
		mClientTable->setItem( i, column, item );
		column++;
		
		//--------------

		// add client type
		switch (client->mClientType)
		{
			case NetworkClient::Visualization:		{ item = new QTableWidgetItem("V"); } break;
			case NetworkClient::Starter:			{ item = new QTableWidgetItem("S"); } break;
			default:								{ item = new QTableWidgetItem("?"); } break;
		}
		item->setFlags( item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable );
		mClientTable->setItem(i, column, item);
		column++;

		//----------------

		// add the ip address
		item = new QTableWidgetItem( hostAddress.toString() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable );
		mClientTable->setItem( i, column, item );
		column++;

		//----------------

		// add the protocol
		switch (hostAddress.protocol())
		{
			case QAbstractSocket::IPv4Protocol:		{ item = new QTableWidgetItem("IPv4"); } break;
			case QAbstractSocket::IPv6Protocol:		{ item = new QTableWidgetItem("IPv6"); } break;
			default:								{ item = new QTableWidgetItem("Unknown"); } break;
		}
		item->setFlags( item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable );
		mClientTable->setItem( i, column, item );
		column++;
	}

	// set item resize mode
	mClientTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
	mClientTable->horizontalHeader()->stretchLastSection();
}


// update interface
void ClientInfoWidget::UpdateInterface()
{
}
