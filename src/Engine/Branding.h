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

#ifndef __NEUROMORE_BRANDING_H
#define __NEUROMORE_BRANDING_H

// Branding (private for b2b)
#ifdef NEUROMORE_BRANDING
#include <Engine/BrandingPrivate.h>
#else
class Branding
{
private:
   inline Branding() { }
public:
   static constexpr const char* CompanyName                 = "neuromore";      // do not put Inc. behind this as this is also used as folder name
   static constexpr const char* DeveloperName               = "neuromore co";
   static constexpr const char* Website                     = "https://www.neuromore.com";
   static constexpr const char* DocumentationUrl            = "https://doc.neuromore.com";
   static constexpr const char* AccountUrl                  = "https://account.neuromore.com";
   static constexpr const char* StoreUrl                    = "https://www.neuromore.com";
   static constexpr const char* ForgotPasswordUrl           = "https://account.neuromore.com/#/resetrequest";
   static constexpr const char* SupportEMail                = "support@neuromore.com";
   static constexpr const char* AppName                     = "neuromore Studio";
   static constexpr const char* AppShortName                = "NMStudio";
   static constexpr const char* MenuStudioName              = "NMStudio";
   static constexpr const char* LicenseUrl                  = "https://raw.githubusercontent.com/neuromore/studio/master/neuromore-licensing-info.md";
   static constexpr const char* CloudTermsUrl               = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-general-terms.md";
   static constexpr const char* PrivacyPolicyUrl            = "https://raw.githubusercontent.com/neuromore/legal/master/neuromore-privacy.md";
   static constexpr const char* LoginImageName              = ":/Images/Login-neuromore.png";
   static constexpr const char* SplashImageName             = ":/Images/SplashScreen-neuromore.png";
   static constexpr const char* AboutImageName              = ":/Images/About-neuromore.png";
   static constexpr const bool  LoginRemberMePrechecked     = true;
   static constexpr const bool  DefaultAutoDetectionEnabled = true;
   static constexpr const bool  DefaultTestDeviceEnabled    = true;
   static constexpr const bool  DefaultEemagineEnabled      = true;
   static constexpr const bool  DefaultBrainMasterEnabled   = false;
   static constexpr const int   DefaultServerPresetIdx      = 0;
   static constexpr const int   DefaultAutoSelectType       = 4; // = SELECT_FIRST_EIGHT (see ChannelMultiSelectionWidget.h)
};
#endif
#endif
