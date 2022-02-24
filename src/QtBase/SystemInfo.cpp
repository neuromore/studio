/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "SystemInfo.h"

//convert Bytes to Megabytes
#define MB 1048576


using namespace Core;

#ifdef NEUROMORE_PLATFORM_WINDOWS
	#pragma comment(lib, "pdh.lib")
#endif

// constructor
SystemInfo::SystemInfo()
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		// total cpu usage
		PdhOpenQueryA( (LPCSTR)NULL, (DWORD_PTR)NULL, &cpuQuery );
		PdhAddCounterA( cpuQuery, "\\Processor(_Total)\\% Processor Time", (DWORD_PTR)NULL, &cpuTotal );
		PdhCollectQueryData(cpuQuery);

		// process cpu usage
		SYSTEM_INFO sysInfo;
		FILETIME ftime, fsys, fuser;

		GetSystemInfo(&sysInfo);
		numProcessors = sysInfo.dwNumberOfProcessors;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&lastCPU, &ftime, sizeof(FILETIME));

		self = GetCurrentProcess();
		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
		memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
	#endif
}


// destructor
SystemInfo::~SystemInfo()
{
}


// total virtual memory
uint64 SystemInfo::GetTotalVirtualMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		return totalVirtualMem;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		struct statfs stats;
        return (0 == statfs("/", &stats)) ?
            (uint64_t)stats.f_bsize * stats.f_bfree : 0;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		sysinfo((struct sysinfo*)&memInfo);
		long long totalVirtualMem = memInfo.totalram;
		//Add other values in next statement to avoid int overflow on right hand side...
		totalVirtualMem += memInfo.totalswap;
		totalVirtualMem *= memInfo.mem_unit;
		return totalVirtualMem;
	#endif
}


// current virtual memory
uint64 SystemInfo::GetCurrentVirtualMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
		return virtualMemUsed;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		xsw_usage vmusage = {0};
		size_t size = sizeof(vmusage);
		if( sysctlbyname("vm.swapusage", &vmusage, &size, NULL, 0)!=0 )
		{
		   perror( "unable to get swap usage by calling sysctlbyname(\"vm.swapusage\",...)" );
		}
		return size;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
		//Add other values in next statement to avoid int overflow on right hand side...
		virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
		virtualMemUsed *= memInfo.mem_unit;
		return virtualMemUsed;
	#endif
}


// current's process virtual memory
uint64 SystemInfo::GetProcessVirtualMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		/*PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
		return virtualMemUsedByMe;*/
		return 0;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		struct task_basic_info t_info;
		mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

		if (KERN_SUCCESS != task_info(mach_task_self(),
									  TASK_BASIC_INFO, (task_info_t)&t_info, 
									  &t_info_count))
		{
			return -1;
		}
		// resident size is in t_info.resident_size;
		// virtual size is in t_info.virtual_size;
		return t_info.virtual_size;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		return 0;
		/*
		int parseLine(char* line){
		// This assumes that a digit will be found and the line ends in " Kb".
		int i = strlen(line);
		const char* p = line;
		while (*p <'0' || *p > '9') p++;
		line[i-3] = '\0';
		i = atoi(p);
		return i;
		}

		int getValue(){ //Note: this value is in KB!
		FILE* file = fopen("/proc/self/status", "r");
		int result = -1;
		char line[128];

		while (fgets(line, 128, file) != NULL){
		if (strncmp(line, "VmSize:", 7) == 0){
		result = parseLine(line);
		break;
		}
		}
		fclose(file);
		return result;
}
		*/
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// total physical memory
uint64 SystemInfo::GetTotalMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		return totalPhysMem;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		int mib[2];
		int64_t physical_memory;
		mib[0] = CTL_HW;
		mib[1] = HW_MEMSIZE;
		size_t length = sizeof(int64_t);
		sysctl(mib, 2, &physical_memory, &length, NULL, 0);
		return length;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		long long totalPhysMem = memInfo.totalram;
		//Multiply in next statement to avoid int overflow on right hand side...
		totalPhysMem *= memInfo.mem_unit;
		return totalPhysMem;
	#endif
}


// current physical memory
uint64 SystemInfo::GetCurrentMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		return physMemUsed;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		/*
		int main(int argc, const char * argv[]) {
			vm_size_t page_size;
			mach_port_t mach_port;
			mach_msg_type_number_t count;
			vm_statistics64_data_t vm_stats;

			mach_port = mach_host_self();
			count = sizeof(vm_stats) / sizeof(natural_t);
			if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
				KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
												(host_info64_t)&vm_stats, &count))
			{
				long long free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;

				long long used_memory = ((int64_t)vm_stats.active_count +
										 (int64_t)vm_stats.inactive_count +
										 (int64_t)vm_stats.wire_count) *  (int64_t)page_size;
				printf("free memory: %lld\nused memory: %lld\n", free_memory, used_memory);
			}

			return 0;
		}
		*/
		return 0;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		long long physMemUsed = memInfo.totalram - memInfo.freeram;
		//Multiply in next statement to avoid int overflow on right hand side...
		physMemUsed *= memInfo.mem_unit;
		return physMemUsed;
	#endif
}


// current's process physical memory
uint64 SystemInfo::GetProcessMemoryInBytes() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		/*PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
		return physMemUsedByMe;*/
		return 0;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		return 0;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		return 0;
		/*
		int getValue(){ //Note: this value is in KB!
		FILE* file = fopen("/proc/self/status", "r");
		int result = -1;
		char line[128];

		while (fgets(line, 128, file) != NULL){
		if (strncmp(line, "VmRSS:", 6) == 0){
		result = parseLine(line);
		break;
		}
		}
		fclose(file);
		return result;
		}
		*/
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// get total CPU usage
double SystemInfo::GetCpuUsage() const
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		PDH_FMT_COUNTERVALUE counterVal;

		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		return 0.0;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		return 0.0;
		/*
		static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

		void init(){
			FILE* file = fopen("/proc/stat", "r");
			fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
				&lastTotalSys, &lastTotalIdle);
			fclose(file);
		}

		double getCurrentValue(){
			double percent;
			FILE* file;
			unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

			file = fopen("/proc/stat", "r");
			fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
				&totalSys, &totalIdle);
			fclose(file);

			if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
				totalSys < lastTotalSys || totalIdle < lastTotalIdle){
				//Overflow detection. Just skip this value.
				percent = -1.0;
			}
			else{
				total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
					(totalSys - lastTotalSys);
				percent = total;
				total += (totalIdle - lastTotalIdle);
				percent /= total;
				percent *= 100;
			}

			lastTotalUser = totalUser;
			lastTotalUserLow = totalUserLow;
			lastTotalSys = totalSys;
			lastTotalIdle = totalIdle;

			return percent;
		}
		*/
	#endif
}


// get this process' CPU usage
double SystemInfo::GetProcessCpuUsage()
{
	// windows
	#ifdef NEUROMORE_PLATFORM_WINDOWS
		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		double percent;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= numProcessors;
		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;

		return percent * 100;
	#endif

	// macOS
	#ifdef NEUROMORE_PLATFORM_OSX
		return 0.0;
	#endif

	// linux
	#ifdef NEUROMORE_PLATFORM_LINUX
		return 0.0;
		/*
		#include "stdlib.h"
		#include "stdio.h"
		#include "string.h"
		#include "sys/times.h"
		#include "sys/vtimes.h"

		static clock_t lastCPU, lastSysCPU, lastUserCPU;
		static int numProcessors;

		void init(){
			FILE* file;
			struct tms timeSample;
			char line[128];

			lastCPU = times(&timeSample);
			lastSysCPU = timeSample.tms_stime;
			lastUserCPU = timeSample.tms_utime;

			file = fopen("/proc/cpuinfo", "r");
			numProcessors = 0;
			while(fgets(line, 128, file) != NULL){
				if (strncmp(line, "processor", 9) == 0) numProcessors++;
			}
			fclose(file);
		}

		double getCurrentValue(){
			struct tms timeSample;
			clock_t now;
			double percent;

			now = times(&timeSample);
			if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
				timeSample.tms_utime < lastUserCPU){
				//Overflow detection. Just skip this value.
				percent = -1.0;
			}
			else{
				percent = (timeSample.tms_stime - lastSysCPU) +
					(timeSample.tms_utime - lastUserCPU);
				percent /= (now - lastCPU);
				percent /= numProcessors;
				percent *= 100;
			}
			lastCPU = now;
			lastSysCPU = timeSample.tms_stime;
			lastUserCPU = timeSample.tms_utime;

			return percent;
		}
		*/
	#endif
}
