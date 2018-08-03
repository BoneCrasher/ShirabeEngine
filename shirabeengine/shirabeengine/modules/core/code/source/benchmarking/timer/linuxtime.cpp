#include "core/benchmarking/timer/linuxtime.h"

#include <iostream>

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CLinuxTime::CLinuxTime()
        : CPlatformTime<timespec>()
	{
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CLinuxTime::~CLinuxTime()
    {
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getTimestamp(InternalTimeValueType_t &aBuffer) const
    {
        EEngineStatus const result = queryPerformanceCounter(aBuffer);

        return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getConversionConstant(InternalTimeValueType_t &aBuffer) const
    {
        EEngineStatus const result = queryPerformanceFrequency(aBuffer);

        return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLinuxTime::requestSetConversionFactors(ConversionFactorMap_t &aOutMap)
    {
        aOutMap.insert({ ETimeUnit::NanoSeconds,  1000000000 }); // might cause crash! be careful!
        aOutMap.insert({ ETimeUnit::MicroSeconds, 1000000 });
        aOutMap.insert({ ETimeUnit::MilliSeconds, 1000 });
        aOutMap.insert({ ETimeUnit::Seconds,      1 });
        aOutMap.insert({ ETimeUnit::Minutes,      0.0166666666666667 });
        aOutMap.insert({ ETimeUnit::Hours,        2.777777777777778e-4 });
        aOutMap.insert({ ETimeUnit::Day,          1.157407407407407e-5 });
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::queryPerformanceFrequency(InternalTimeValueType_t &aBuffer) const
	{
		EEngineStatus result = EEngineStatus::Ok;

        int32_t const success = clock_getres(CLOCK_REALTIME, &aBuffer);

		return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::queryPerformanceCounter(InternalTimeValueType_t &aBuffer) const
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
