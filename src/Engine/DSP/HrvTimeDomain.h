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

#ifndef __NEUROMORE_HRVTIMEDOMAIN_H
#define __NEUROMORE_HRVTIMEDOMAIN_H

// include required headers
#include "../Config.h"
#include "ChannelReader.h"
#include "Channel.h"


class HrvTimeDomain
{
	public:
		enum EMethod
		{
			METHOD_RMSSD,					// the root mean square of successive differences between adjacent RRs.
			METHOD_SDSD,					// the standard deviation of the successive differences between adjacent RRs.
			METHOD_EBC,						// estimated breath cycle: calc the range (max-min) in a sliding window
			METHOD_RR50,					// the number of pairs of successive RRs that differ by more than 50 ms
			METHOD_pRR50,					// the proportion of "the number of pairs of successive RRs that differ by more than 50 ms." divided by total number of RRs
			METHOD_pRR20,					// same as pRR50 but with 20ms
			NUM_TIME_DOMAIN_METHODS
		};

		// time domain functions
		static void CORE_CDECL RMSSD(ChannelReader* inputReader, Channel<double>* output);
		static void CORE_CDECL SDSD(ChannelReader* inputReader, Channel<double>* output);
		static void CORE_CDECL EBC(ChannelReader* inputReader, Channel<double>* output);
		static void CORE_CDECL RR50(ChannelReader* inputReader, Channel<double>* output);
		static void CORE_CDECL pRR50(ChannelReader* inputReader, Channel<double>* output);
		static void CORE_CDECL pRR20(ChannelReader* inputReader, Channel<double>* output);

		// hrv time domain function pointer
		typedef void (CORE_CDECL *Function)(ChannelReader* inputReader, Channel<double>* output);

		// helpers
		static void RRX(ChannelReader* inputReader, Channel<double>* output, double millisecs);
		static void pRRX(ChannelReader* inputReader, Channel<double>* output, double millisecs);

		static const char* GetName(EMethod method);
		static bool GetIntervalRequirement(EMethod method);
		static Function GetFunction(EMethod method);
};


#endif
