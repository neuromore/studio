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

#ifndef __NEUROMORE_PRESESSIONWIDGET_H
#define __NEUROMORE_PRESESSIONWIDGET_H

// include required headers
#include "../../Config.h"
#include "../../AppManager.h"
#include "../../MainWindow.h"
#include <Networking/NetworkServerClient.h>
#include <ImageButton.h>

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <Spinbox.h>


// forward declaration
class SessionControlPlugin;

class PreSessionWidget : public QWidget
{
	Q_OBJECT
	public:
		static constexpr const uint32_t NUMELECTRODESELECT = 4;

		PreSessionWidget(SessionControlPlugin* plugin, QWidget* parent, int buttonSize);
		virtual ~PreSessionWidget() {}

		void Init();
		void ReInit(); 

		QPushButton* GetStartButton()									{ return mStartButton; }
		QPushButton* GetShowReportButton()								{ return mShowReportButton; }
		QPushButton* GetSelectUserButton()								{ return mSelectUserButton; }

		void UpdateChannels(ChannelSelectorNode* chs = 0);
		Core::String GetSelectedChannels();
		inline QString GetSelectedChannel(uint32 idx) { return idx < NUMELECTRODESELECT ? mElectrodeSelections[idx]->currentText() : ""; }

	private slots:
		void OnTotalTimeChanged(double value);
		void OnSelectVisualizationClicked();
		void OnSelectUserClicked()										{ GetMainWindow()->SelectSessionUser(); }
		void OnChannelSelected(const QString& text);

	signals:
		void SelectedChannelsChanged();

	private:
		SessionControlPlugin*	mPlugin;
		ImageButton*			mStartButton;
		uint32					mStartButtonSize;

		QGridLayout*			gLayout;

		QPushButton*			mBackToSelectionButton;

		QLabel*				mElectrodeSelectionLabel;
		QComboBox*			mElectrodeSelections[NUMELECTRODESELECT];

		QLabel*					mVisSelectionLabel;
		QPushButton*			mVisSelectionButton;

		QLabel*					mSelectUserLabel;
		QPushButton*			mSelectUserButton;
		
		//QLabel*				mTotalTimeLabel;
		//QLabel*				mTotalTimeSecondsLabel;
		//IntSpinBox*			mTotalTimeSpinbox;

		QPushButton*			mShowReportButton;
};


#endif
