#ifndef __SHIRABE__TIMER_H__
#define __SHIRABE__TIMER_H__

#include "core/enginetypehelper.h"
#include "core/benchmarking/measurementdatastore.h"
#include "core/benchmarking/timer/timespanunit.h"

#include <platform/platform.h>

#ifdef SHIRABE_PLATFORM_WINDOWS
#  pragma warning(push)
#  pragma warning(disable:4820)
#  include "core/benchmarking/timer/windowstime.h"
#else
#  include "core/benchmarking/timer/linuxtime.h"
#endif

using namespace engine::benchmarking;

using FPSDataStore_t = CMeasurementDataStore<CMeasurementChunk<double, double>>;

namespace engine
{

#ifdef SHIRABE_PLATFORM_WINDOWS
    typedef class engine::CWindowsTime Time, Time, *LPTime;
#else
    typedef class engine::CLinuxTime Time, Time, *LPTime;
#endif

    /**
     * The timer class will use a specific PlatformTime specialization to hook into platform time
     * counters and evaluate total elapsed, elapsed since last frame and frames per second data.
     */
    class CTimer
    {
    public_constructors:
        CTimer();

    public_destructors:
        virtual ~CTimer() final;

    public_methods:
		EEngineStatus initialize();
		EEngineStatus update();
		EEngineStatus cleanup();

        double elapsed(ETimeUnit unit = ETimeUnit::Seconds);
        double total_elapsed(ETimeUnit unit = ETimeUnit::Seconds());

		float FPS();
		
    private_typedefs:

#ifdef SHIRABE_PLATFORM_WINDOWS
        using InternalTimeType_t = CWindowsTime;
#elif defined(SHIRABE_PLATFORM_LINUX)
        using InternalTimeType_t = CLinuxTime;
#endif
        using InternalTimeValueType_t = InternalTimeType_t::InternalTimeValueType_t;

    private_methods:
        void setTimeInterface(CStdSharedPtr_t<InternalTimeType_t> aTimeInterface);

    private_members:
        CStdSharedPtr_t<InternalTimeType_t> m_timeInterface;

        InternalTimeValueType_t mConversionConstant;
        InternalTimeValueType_t m_initial;
        InternalTimeValueType_t m_current;
        InternalTimeValueType_t m_elapsed;

        FPSDataStore_t mDataStore;
        double         mChunkCounter;
        int            mFrames;
	};
}

#ifdef SHIRABE_PLATFORM_WINDOWS
#pragma warning(pop)
#endif

#endif
