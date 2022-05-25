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

#pragma once
#include <string>
#include <tuple>

struct BrainAliveInputParams {
  Core::String channel_1;
  Core::String channel_2;
  Core::String channel_3;
  Core::String channel_4;
  Core::String channel_5;
  Core::String channel_6;
  Core::String channel_7;
  Core::String channel_8;

  BrainAliveInputParams() {
    channel_1 = "F7";
    channel_2 = "FT7";
    channel_3 = "T7";
    channel_4 = "TP7";
    channel_5 = "Cz";
    channel_6 = "C4";
    channel_7 = "FC4";
    channel_8 = "F4";
  }
};