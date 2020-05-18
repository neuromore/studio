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

package com.neuromore.engine.enums;

/**
 * Must match EMuseInputs in neuromoreEngine.h
 */
public final class EMuseInputs
{
    public static final int MUSE_INPUT_EEG_TP9           = 0;    // = 220 Hz
    public static final int MUSE_INPUT_EEG_FP1           = 1;    // = 220 Hz
    public static final int MUSE_INPUT_EEG_FP2           = 2;    // = 220 Hz
    public static final int MUSE_INPUT_EEG_TP10          = 3;    // = 220 Hz
    public static final int MUSE_INPUT_CONCENTRATION     = 4;    // = 10 Hz
    public static final int MUSE_INPUT_MELLOW            = 5;    // = 10 Hz
    public static final int MUSE_INPUT_DRL               = 6;    // = 220 Hz
    public static final int MUSE_INPUT_REF               = 7;    // = 220 Hz
    public static final int MUSE_INPUT_ACC_FORWARD       = 8;    // = 50 Hz
    public static final int MUSE_INPUT_ACC_UP            = 9;    // = 50 Hz
    public static final int MUSE_INPUT_ACC_LEFT          = 10;   // = 50 Hz
    public static final int MUSE_INPUT_EYEBLINK			 = 11;   // = 10 Hz
    public static final int MUSE_INPUT_JAWCLENCH		 = 12;   // = 10 Hz
    public static final int MUSE_INPUT_EEG_TP9_OK		 = 13;   // < 10 Hz // Note: Mapping from HorseShoe Values to our scale:
    public static final int MUSE_INPUT_EEG_FP1_OK		 = 14;   // < 10 Hz //       1.0  =>  1.0
    public static final int MUSE_INPUT_EEG_FP2_OK		 = 15;   // < 10 Hz //       2.0  =>  0.5
    public static final int MUSE_INPUT_EEG_TP10_OK		 = 16;   // < 10 Hz //      >3.0  =>  0.0
    public static final int MUSE_INPUT_TOUCHING_FOREHEAD = 17;   // < 10 Hz // Note: 1=true, 0=false
}