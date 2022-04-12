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
#include "StageControlWidget.h"
#include <Studio/MainWindow.h>

using namespace Core;

// constructor
StageControlWidget::StageControlWidget(QWidget* parent) : QWidget(parent)
{
	mManualStageModeCheckBox = NULL;
	mStageTable	= NULL;	
	mManualStageMode = true;
	Init();
}


// initialize the widget
bool StageControlWidget::Init()
{
	// create a grid layout
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(0);
	uint32 row = 0;

	// manual mode checkbox
	mManualStageModeCheckBox = new QCheckBox("Manual Stage Control");
	mManualStageModeCheckBox->setChecked( mManualStageMode );
#ifdef NEUROMORE_BRANDING_ANT
	mManualStageModeCheckBox->setHidden(true);
#endif
	connect( mManualStageModeCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( OnManualStageModeCheckBoxStateChanged( int ) ) );

	gridLayout->addWidget( mManualStageModeCheckBox, row, 0 );
	row++;

	// stages table
	mStageTable = new QTableWidget();
	mStageTable->setSelectionBehavior( QAbstractItemView::SelectRows );
	mStageTable->setSelectionMode( QAbstractItemView::NoSelection );
	//mStageTable->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

	connect( mStageTable, SIGNAL( cellDoubleClicked ( int, int ) ), this, SLOT( OnStageTableDoubleClick( int, int ) ) );

	gridLayout->addWidget(mStageTable, row, 0);
	row++;

	setLayout(gridLayout);
	ReInit();
	return true;
}


// reinit the stage table widget
void StageControlWidget::ReInit()
{
	// check box correctly
	mManualStageModeCheckBox->setChecked( mManualStageMode );

	// clear the table
	mStageTable->clear();

	// hide vertical headers
	mStageTable->verticalHeader()->hide();

	// horizontal headers
	mStageTable->setColumnCount(4);
	QStringList horizontalHeaderLabels;
	horizontalHeaderLabels.append( "Stage" );
	horizontalHeaderLabels.append( "Name" );
	horizontalHeaderLabels.append( "Entry Points" );
	horizontalHeaderLabels.append( "Entry Time" );
	mStageTable->setHorizontalHeaderLabels(horizontalHeaderLabels);

	// set item resize mode
	mStageTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	// FIXME: stretchLastSection does not work
	mStageTable->horizontalHeader()->stretchLastSection();
	
	NetworkServer*  networkServer = GetNetworkServer();

	// no visualization client present: empty table
	NetworkServerClient* visClient = networkServer->GetPrimaryVisualizationClient();
	if (visClient == NULL) {
		return;
	}

	
	// parse the levels from the client config and fill table
	Json* clientConfig = visClient->GetConfig();
	Json::Item stageListItem = clientConfig->Find("stages");
	
	// level has no stage item
	if (stageListItem.IsNull() == true)
		return;

	// level has empty stage item
	if (stageListItem.Size() == 0)
		return;
	
	// set numrows and resize table
	uint32 numStages = stageListItem.Size();
	mStageTable->setRowCount( numStages );
	//mStageTable->setMinimumHeight( 18 * numStages + mStageTable->horizontalHeader()->height() );

	// parse stages and add them to the table
	for (uint32 i=0; i<numStages; i++)
	{
		mStageTable->setRowHeight( i, 18 );

		Json::Item stageItem = stageListItem[i];
		if (stageItem.IsNull() == true)
			continue;

		uint32 column = 0;

		//------ stage number (1-indexed) ---------
		QTableWidgetItem* item = new QTableWidgetItem( String( i + 1 ).AsChar() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		mStageTable->setItem(i, column, item);

		//------ stage name -----------------------
		column++;
		Json::Item nameItem = stageItem.Find( "name" );
		String name = "Unknown";
		if (nameItem.IsString() == true)
			name = nameItem.GetString();

		item =  new QTableWidgetItem(name.AsChar());
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		mStageTable->setItem(i, column, item);
		
		//------ entry points ----------------------
		column++;
		Json::Item pointsItem = stageItem.Find("entryPoints");
		int entryPoints = 0;
		if (pointsItem.IsNumber() == true)
			entryPoints = pointsItem.GetInt();

		item = new QTableWidgetItem( String( entryPoints ).AsChar() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		mStageTable->setItem(i, column, item);

		//------ entry points ----------------------
		column++;
		Json::Item timeItem = stageItem.Find("entryTime");
		String entryTime = "";
		if (timeItem.IsString() == true)
			entryTime = timeItem.GetString();

		item = new QTableWidgetItem( entryTime.AsChar() );
		item->setFlags( item->flags() & ~Qt::ItemIsEditable );
		mStageTable->setItem( i, column, item );
	}
}


// checkbox state change slot
void StageControlWidget::OnManualStageModeCheckBoxStateChanged( int state )
{
	mManualStageMode = (state == Qt::Checked);
	//SendSetManualStageModeEvent( mManualStageMode );
}


void StageControlWidget::OnStageTableDoubleClick( int row, int col )
{
	// double click only allowed in manual stage mode
	if (mManualStageMode == false)
		return;

	// switch stage in the widget and send json event to client
	SendSwitchStageEvent(row);

	// highlight the new row
	mCurrentStage = row;
	ResetTablePalette();
	HighlightRow(row);
}


void StageControlWidget::SendSwitchStageEvent( int index )
{
	LogDebug("sending event: switching to stage %i", index);

	CORE_EVENTMANAGER.OnSwitchStage( index );
}

//
//// TODO deprecated
//void StageControlWidget::SendSetManualStageModeEvent( bool value )
//{
//	if (mClient == NULL) 
//		return;
//
//	// TODO no longer required, because in the future the vis won't be able to switch stages
//	//LogDebug( "sending event: %s manual stage control", (value ? "enable" : "disable" ) );
//
//	//Json command;
//	//Json::Item rootItem = command.GetRootItem();
//	//command.AddBool( "setManualStageMode", command.GetRootItem(), value );
//	//String jsonString;
//	//command.WriteToString( jsonString );
//
//	//NetworkMessageEvent* message = new NetworkMessageEvent( jsonString );
//	//mClient->WriteMessage( message );
//	//delete message;
//}


// highlight one row in the table
void StageControlWidget::HighlightRow( int index )
{
	int numCols = mStageTable->columnCount();
	QPalette palette = mStageTable->palette();

	// highlight the row
	for (int c = 0; c < numCols; c++)
	{
		QTableWidgetItem* item = mStageTable->item( index, c );
		item->setBackground( palette.highlight() );
		item->setForeground( palette.highlightedText() );
	}
}


// reset all table items do unhighlighted palette
void StageControlWidget::ResetTablePalette()
{
	int numRows = mStageTable->rowCount();
	int numCols = mStageTable->columnCount();
	QPalette palette = mStageTable->palette();

	// reset color of all items to unhighlighted
	for (int c = 0; c < numCols; c++)
	{
		for (int r = 0; r < numRows; r++)
		{
			QTableWidgetItem* item = mStageTable->item( r, c );
			item->setBackground( palette.background() );
			item->setForeground( palette.foreground() );
		}
	}
}


// update interface
void StageControlWidget::UpdateInterface()
{
}
