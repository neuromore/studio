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
 * Must match EStatus in neuromoreEngine.h
 */
public final class EStatus
{
    public static final int STATUS_OK				= 0;	// session completed successfully
    public static final int STATUS_DEVICE_ERROR		= 1;	// aborted due to device error (no data/battery died)			-> user or app/lib fault
    public static final int STATUS_SIGNAL_ERROR		= 2;	// aborted due to very bad signal quality						-> user or hardware fault
    public static final int STATUS_ENGINE_ERROR		= 3;	// aborted due to internal engine error (thats the worst case)	-> app/engine fault
};
