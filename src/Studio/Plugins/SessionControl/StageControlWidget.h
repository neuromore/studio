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

#ifndef __NEUROMORE_STAGECONTROLWIDGET_H
#define __NEUROMORE_STAGECONTROLWIDGET_H

// include required headers
#include "../../Config.h"
#include <Core/String.h>
#include <Networking/NetworkServer.h>
#include <QCheckBox>
#include <QLabel>
#include <QTableWidget>


/**
 *
 *
 */
class StageControlWidget : public QWidget
{
	Q_OBJECT
	public:
		StageControlWidget(QWidget* parent);
		virtual ~StageControlWidget() {};

		bool Init();
		void ReInit();
		void UpdateInterface();
		
		// switch to another stage (updates table highlight)
		void SwitchStage( int index );

		// send SwitchStage events to visualization client
		void SendSwitchStageEvent( int index );
		//void SendSetManualStageModeEvent( bool value );

	private slots:
		void OnManualStageModeCheckBoxStateChanged( int state );
		void OnStageTableDoubleClick ( int row, int col );

	private:
		QCheckBox*				mManualStageModeCheckBox;
		QTableWidget*			mStageTable;

		bool					mManualStageMode;
		int						mCurrentStage;

		void HighlightRow( int index );
		void ResetTablePalette();
};


#endif
