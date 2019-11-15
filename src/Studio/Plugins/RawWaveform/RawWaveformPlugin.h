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

#ifndef __NEUROMORE_RAWWAVEFORMPLUGIN_H
#define __NEUROMORE_RAWWAVEFORMPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include "../../Widgets/ChannelMultiSelectionWidget.h"
#include "RawWaveformWidget.h"


class RawWaveformPlugin : public Plugin
{
	Q_OBJECT
	public:
		// constructor & destructor
		RawWaveformPlugin();
		virtual ~RawWaveformPlugin();

		// general
		const char* GetName() const override								{ return "Raw EEG"; }
		static const char* GetStaticTypeUuid()								{ return "c38f75d7-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override							{ return "PLUGIN_RawWaveform"; }
		Plugin* Clone() override											{ return new RawWaveformPlugin(); }
		virtual bool AllowMultipleInstances() const override                { return true; }

		// initialization
		void RegisterAttributes() override;
		bool Init() override;

		void UpdateInterface() override final;
		bool NeedRegularUpdateInterface() override final					{ return true; }

		// widget helpers
		ChannelMultiSelectionWidget* GetChannelSelectionWidget() const		{ return mChannelSelectionWidget; }

		// settings
		inline double GetAmplitudeScale()									{ return GetFloatAttributeByName("uV/Div"); }
		inline double GetTimeRange()										{ return GetFloatAttributeByName("time"); }
		inline bool GetShowVoltages()										{ return GetBoolAttributeByName("showVoltages"); }
		inline bool GetShowTimes()											{ return GetBoolAttributeByName("showTimes"); }
		inline bool GetAutoScaleY()											{ return GetBoolAttributeByName("autoScale"); }

	private slots:
		void OnShowUsedCheckboxToggled(int state);

	private:
		ChannelMultiSelectionWidget*	mChannelSelectionWidget;
		RawWaveformWidget*				mWaveformWidget;
};


#endif
