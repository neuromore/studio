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

#ifndef __NEUROMORE_FEEDBACKPLUGIN_H
#define __NEUROMORE_FEEDBACKPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <Core/String.h>
#include <PluginSystem/Plugin.h>
#include <Slider.h>
#include "FeedbackHistoryWidget.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QComboBox>


class FeedbackPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:
		FeedbackPlugin();
		virtual ~FeedbackPlugin();

		// overloaded
		const char* GetName() const override								{ return "Feedback"; }
		static const char* GetStaticTypeUuid()								{ return "c38f75d6-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override							{ return "PLUGIN_Feedback"; }
		Plugin* Clone() override											{ return new FeedbackPlugin(); }

		// overloaded main init function
		bool Init() override;
		//void ReInit();

		void RegisterAttributes() override;

		// settings
		inline double GetTimeRange()										{ return GetFloatAttributeByName("timeRange"); }
		inline uint32 GetSampleStyle()										{ return GetInt32AttributeByName("style"); }
		inline bool GetShowLatencyMarker()									{ return GetBoolAttributeByName("showLatencyMarker"); }

	private:
		FeedbackHistoryWidget*	mHistoryWidget;
};


#endif
