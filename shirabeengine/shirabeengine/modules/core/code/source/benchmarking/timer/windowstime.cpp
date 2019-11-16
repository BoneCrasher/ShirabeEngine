#include <platform/platform.h>

#ifdef SHIRABE_PLATFORM_WINDOWS
#include "core/benchmarking/timer/windowstime.h"

#include <iostream>

namespace engine {
	WindowsTime::WindowsTime() 
		: PlatformTime<LONGLONG>()
	{
	};

	WindowsTime::~WindowsTime() {
	};

	EEngineStatus WindowsTime::getTimestamp(internal_time_value_type& buffer) const {
		EEngineStatus res = queryPerformanceCounter(buffer); // get current timestamp

		return res;
	};

	EEngineStatus WindowsTime::getConversionConstant(internal_time_value_type& buffer) const {
		EEngineStatus res = queryPerformanceFrequency(buffer); // get current timestamp

		return res;
	};

	void WindowsTime::requestSetConversionFactors(ConversionFactorMap& map) {
		map.insert({ ETimeUnit::NanoSeconds,  1000000000 }); // might cause crash! be careful!
		map.insert({ ETimeUnit::MicroSeconds, 1000000 });
		map.insert({ ETimeUnit::MilliSeconds, 1000 });
		map.insert({ ETimeUnit::Seconds,      1 });
		map.insert({ ETimeUnit::Minutes,      0.0166666666666667 });
		map.insert({ ETimeUnit::Hours,        2.777777777777778e-4 });
		map.insert({ ETimeUnit::Day,          1.157407407407407e-5 });
	}

	EEngineStatus WindowsTime::queryPerformanceFrequency(internal_time_value_type &buffer) const
	{
		EEngineStatus result = EEngineStatus::Ok;

		if (!QueryPerformanceFrequency((LARGE_INTEGER *)&buffer)) {
#ifdef _DEBUG
			printf("FATAL_ERROR: Time_Win32::queryPerformanceFrequency: Cannot retrieve QueryPerformanceFrequency.");
#endif
			result = EEngineStatus::Time_Win32__QueryPerformanceFrequencyFailed;
		}
		else 
			result = EEngineStatus::Ok;

		return result;
	};

	EEngineStatus WindowsTime::queryPerformanceCounter(internal_time_value_type &buffer) const
	{
		EEngineStatus result = EEngineStatus::Ok;

		if (!QueryPerformanceCounter((LARGE_INTEGER *)&buffer)) {
#ifdef _DEBUG
			printf("FATAL_ERROR: Time_Win32::queryPerformanceCounter: Cannot retrieve QueryPerformanceCounter.");
#endif
			result = EEngineStatus::Time_Win32__QueryPerformanceCounterFailed;
		}
		else
			result = EEngineStatus::Ok;

		return result;
	};
}
#endif
