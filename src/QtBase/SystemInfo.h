/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __NEUROMORE_SYSTEMINFO_H
#define __NEUROMORE_SYSTEMINFO_H

// include required headers
#include "QtBaseConfig.h"
#include <Core/StandardHeaders.h>

// windows
#ifdef NEUROMORE_PLATFORM_WINDOWS
#include <windows.h>
#include <psapi.h>
#include <Pdh.h>
#endif

// macOS
#ifdef NEUROMORE_PLATFORM_OSX
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#endif

// linux
#ifdef NEUROMORE_PLATFORM_LINUX
#include <sys/sysinfo.h>
#endif

class QTBASE_API SystemInfo
{
	public:
		SystemInfo();
		~SystemInfo();

		// virtual memory
		uint64 GetTotalVirtualMemoryInBytes() const;
		uint64 GetCurrentVirtualMemoryInBytes() const;
		uint64 GetProcessVirtualMemoryInBytes() const;

		// physical memory
		uint64 GetTotalMemoryInBytes() const;
		uint64 GetCurrentMemoryInBytes() const;
		uint64 GetProcessMemoryInBytes() const;

		// CPU usage
		double GetCpuUsage() const;
		double GetProcessCpuUsage();

	private:
		// windows
		#ifdef NEUROMORE_PLATFORM_WINDOWS
			// total cpu usage
			PDH_HQUERY cpuQuery;
			PDH_HCOUNTER cpuTotal;

			ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
			int numProcessors;
			HANDLE self;
		#endif

		// macOS
		#ifdef NEUROMORE_PLATFORM_OSX

		#endif

		// linux
		#ifdef NEUROMORE_PLATFORM_LINUX
			struct sysinfo memInfo;
		#endif
};


#endif
