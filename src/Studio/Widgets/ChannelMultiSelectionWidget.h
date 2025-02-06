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

#ifndef __NEUROMORE_CHANNELMULTISELECTIONWIDGET_H
#define __NEUROMORE_CHANNELMULTISELECTIONWIDGET_H

// include required headers
#include "../Config.h"
#include <BciDevice.h>
#include <EngineManager.h>
#include "DeviceSelectionWidget.h"
#include "HMultiCheckboxWidget.h"


class ChannelMultiSelectionWidget : public QWidget
{
	Q_OBJECT
	public:
		// device-change behaviour (values used in Branding.h!)
		enum AutoSelectType { 
			SELECT_ALL         = 0,
			SELECT_NONE        = 1,
			SELECT_FIRST       = 2,
			SELECT_FIRST_TWO   = 3,
			SELECT_FIRST_EIGHT = 4,
		};

		// constructor & destructor
		ChannelMultiSelectionWidget(QWidget* parent=NULL);
		virtual ~ChannelMultiSelectionWidget();

		// initialize
		void Init();
		void ReInit(Device* device = NULL);

		// TODO: replace with transparent accessors (GetNumAvailableChannels(), GetAvailableChannel(uint32 index) etc.)
		const Core::Array<Channel<double>*>& GetAvailableChannels() const	{ return mAvailableChannels; }
		const Core::Array<Channel<double>*>& GetSelectedChannels() const	{ return mSelectedChannels; }

		uint32 GetNumSelectedChannels() const								{ return mSelectedChannels.Size(); }
		bool IsChannelSelected(Channel<double>* channel) const				{ return mSelectedChannels.Contains(channel); }
		void SetChannelAsUsed(Channel<double>* channel, bool used);
		void ClearUsedChannels();
		bool IsChannelUsed(Channel<double>* channel) const					{ return mUsedChannels.Contains(channel); }

		Device* GetSelectedDevice() const									{ return mDeviceSelectionWidget->GetSelectedDevice(); }

		// what channels to select when switching to a new device
		void SetAutoSelectType(AutoSelectType type)							{ mAutoSelectType = type; }

		uint32 GetHighlightedIndex() const									{ return mChannelMultiCheckbox->GetHighlightedIndex(); }

		// show only neuro channels 
		void SetShowNeuroChannelsOnly(bool enabled)							{ mShowOnlyEEGChannels = enabled; }

		void SetChecked(Channel<double>* channel, bool checked);
		void SetChecked(uint32 index, bool checked);
		void SetVisible(uint32 index, bool checked);

		bool IsShowUsedChecked()											{ return mShowUsedCheckbox->isChecked(); }
		void SetShowUsedChecked(bool checked)								{ mShowUsedCheckbox->setChecked(checked); }

		bool IsShowUsedVisible()											{ return mShowUsedCheckbox->isVisible(); }
		void SetShowUsedVisible(bool visible)								{ mShowUsedCheckbox->setVisible(visible); }

	private slots:
		void OnDeviceSelectionChanged(Device* device);
		void OnChannelSelectionChanged();
		void OnShowUsedCheckboxToggled(int state);

	signals:
		void ChannelSelectionChanged();
		void ShowUsedCheckboxToggled(int state);

	private:
		DeviceSelectionWidget*				mDeviceSelectionWidget;
		QCheckBox*							mShowUsedCheckbox;
		HMultiCheckboxWidget*				mChannelMultiCheckbox;

		Core::Array<Channel<double>*>		mAvailableChannels;
		Core::Array<Channel<double>*>		mSelectedChannels;
		Core::Array<Channel<double>*>		mUsedChannels;

		AutoSelectType						mAutoSelectType;
		bool								mShowOnlyEEGChannels;
};


#endif
