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
#ifndef __NEUROMORE_VIEWSPECTRUMPLUGIN_H
#define __NEUROMORE_VIEWSPECTRUMPLUGIN_H

// include required headers
#include "../../Config.h"
#include <Core/EventHandler.h>
#include <Core/String.h>
#include <PluginSystem/Plugin.h>
#include <Slider.h>
#include <Graph/ViewNode.h>
#include "ViewSpectrumWidget.h"
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QComboBox>

class ViewSpectrumPlugin : public Plugin, public Core::EventHandler
{
	Q_OBJECT
	public:

		enum
		{
			ATTRIB_MINFREQUENCY = NUM_BASEATTRIBUTES,
			ATTRIB_MAXFREQUENCY,
		};

		ViewSpectrumPlugin();
		virtual ~ViewSpectrumPlugin();

		// overloaded
		const char* GetName() const override								{ return "Spectrum View"; }
		static const char* GetStaticTypeUuid()								{ return "a4b0e290-3872-11e5-a151-feff819cdc9f"; }
		const char* GetRuleName() const override							{ return "PLUGIN_SpectrumView"; }
		Plugin* Clone() override											{ return new ViewSpectrumPlugin(); }

		// overloaded main init function
		bool Init() override;

		void RegisterAttributes() override;

		double GetMinFrequency()											{ return GetFloatAttribute(ATTRIB_MINFREQUENCY); }
		double GetMaxFrequency()											{ return GetFloatAttribute(ATTRIB_MAXFREQUENCY); }

		// access to the multi channels
		uint32				GetNumMultiChannels();
		const MultiChannel&	GetMultiChannel(uint32 index);		
        Core::Color GetChannelColor(uint32 multichannel, uint32 index);

	private:
		ViewSpectrumWidget*		mViewWidget;
};


#endif
