/*
 * neuromore Android App
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

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
