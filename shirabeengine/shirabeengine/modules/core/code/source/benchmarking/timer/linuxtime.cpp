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
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CLinuxTime::~CLinuxTime()
    {
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getTimestamp(InternalTimeValueType_t &aBuffer) const
    {
        EEngineStatus const result = getClockTimestamp(aBuffer);

        return result;
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getConversionConstant(InternalTimeValueType_t &aBuffer) const
    {
        EEngineStatus const result = getClockResolution(aBuffer);

        return result;
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CLinuxTime::requestSetConversionFactors(ConversionFactorMap_t &aOutMap)
    {
        aOutMap.insert({ ETimeUnit::NanoSeconds,  1 });
        aOutMap.insert({ ETimeUnit::MicroSeconds, 0.001 });
        aOutMap.insert({ ETimeUnit::MilliSeconds, 0.000001 });
        aOutMap.insert({ ETimeUnit::Seconds,      0.000000001 });
        aOutMap.insert({ ETimeUnit::Minutes,      1.66666666666667e-11 });
        aOutMap.insert({ ETimeUnit::Hours,        2.777777777777778e-13 });
        aOutMap.insert({ ETimeUnit::Day,          1.157407407407407e-14 });
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getClockResolution(InternalTimeValueType_t &aBuffer) const
	{
		EEngineStatus result = EEngineStatus::Ok;

        if(0 > clock_getres(CLOCK_REALTIME, &aBuffer))
        {
#ifdef _DEBUG
            CLog::Error(logTag(),
                        "FATAL_ERROR: getClockResolution: "
                        "Cannot retrieve clock resolution.");
#endif
            result = EEngineStatus::Time_Win32__QueryPerformanceCounterFailed;
        }

		return result;
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CLinuxTime::getClockTimestamp(InternalTimeValueType_t &aBuffer) const
	{
		EEngineStatus result = EEngineStatus::Ok;

        if (0 > clock_gettime(CLOCK_REALTIME, &aBuffer))
        {
#ifdef _DEBUG
            CLog::Error(logTag(),
                        "FATAL_ERROR: getClockTimestamp: "
                        "Cannot retrieve timestamp.");
#endif
			result = EEngineStatus::Time_Win32__QueryPerformanceCounterFailed;
		}

		return result;
	}
    //<-----------------------------------------------------------------------------
}
