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

#ifndef __NEUROMORE_SPECTRUMANALYZERSETTINGSPLUGIN_H
#define __NEUROMORE_SPECTRUMANALYZERSETTINGSPLUGIN_H

// include required headers
#include "../../Config.h"
#include <PluginSystem/Plugin.h>
#include <EngineManager.h>
#include "SpectrumAnalyzerSettingsWidget.h"



class SpectrumAnalyzerSettingsPlugin : public Plugin
{
	Q_OBJECT

	public:
		SpectrumAnalyzerSettingsPlugin();
		virtual ~SpectrumAnalyzerSettingsPlugin();

		// overloaded
		const char* GetName() const override				{ return "Spectrum Analyzer Settings"; }
		static const char* GetStaticTypeUuid()				{ return "c38f75dd-7eca-11e4-b4a9-0800200c9a66"; }
		const char* GetRuleName() const override			{ return "PLUGIN_SpectrumAnalyzerSettings"; }

		// overloaded main init function
		bool Init() override;
		Plugin* Clone() override;
		void ReInit();

	private:
		SpectrumAnalyzerSettingsWidget*		mSettingsWidget;
};


#endif
