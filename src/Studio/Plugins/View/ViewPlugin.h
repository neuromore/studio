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

#ifndef __NEUROMORE_VIEWPLUGIN_H
#define __NEUROMORE_VIEWPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <Core/String.h>
#include <PluginSystem/Plugin.h>
#include <AttributeWidgets/Property.h>
#include <Slider.h>
#include "ViewWidget.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QComboBox>


class ViewPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT

	public:

		enum
		{
			ATTRIB_TIMERANGE = NUM_BASEATTRIBUTES,
			ATTRIB_STYLE,
			ATTRIB_SHOWLATENCY,
		};

		ViewPlugin();
		virtual ~ViewPlugin();

		// overloaded
		const char* GetName() const override								{ return "Signal View"; }
		static const char* GetStaticTypeUuid()								{ return "30ca8a46-321b-11e5-a151-feff819cdc9f"; }
		const char* GetRuleName() const override							{ return "PLUGIN_SignalView"; }
		Plugin* Clone() override											{ return new ViewPlugin(); }

		// overloaded main init function
		bool Init() override;
		//void ReInit();

		void RegisterAttributes() override;
		
		// get settings
		double		 GetTimeRange()											{ return GetFloatAttribute(ATTRIB_TIMERANGE); }
		uint32		 GetSampleStyle()										{ return GetInt32Attribute(ATTRIB_STYLE); }
		bool	 	 GetShowLatencyMarker()									{ return GetBoolAttribute(ATTRIB_SHOWLATENCY); }

		// access to the multi channels
		uint32				GetNumMultiChannels();
		const MultiChannel&	GetMultiChannel(uint32 index);				
        Core::Color	GetChannelColor(uint32 multichannel, uint32 index);		

		void SetViewDuration(double seconds);

	private slots:
		void OnAttributeChanged(Property* property);

	private:

		ViewWidget*	mViewWidget;
};


#endif
