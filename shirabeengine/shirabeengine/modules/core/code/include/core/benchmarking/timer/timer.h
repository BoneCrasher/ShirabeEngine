#ifndef __SHIRABE__TIMER_H__
#define __SHIRABE__TIMER_H__

#include <platform/platform.h>
#include <log/log.h>

#include "core/enginetypehelper.h"
#include "core/benchmarking/measurementdatastore.h"
#include "core/benchmarking/timer/timespanunit.h"


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
        SHIRABE_DECLARE_LOG_TAG(CTimer);

    public_constructors:
        /**
         * Default construct a timer instance.
         */
        CTimer();

    public_destructors:
        /**
         * Destroy and run...
         */
        virtual ~CTimer() final;

    public_methods:
        /**
         * Initialize this timer.
         *
         * @return EEngineStatus::Ok, if successful. An error flag otherwise.
         */
		EEngineStatus initialize();
        /**
         * Update this timer.
         *
         * @return
         */
		EEngineStatus update();
        /**
         * Shutdown and cleanup this timer.
         *
         * @return
         */
		EEngineStatus cleanup();

        /**
         * Return the elapsed time units since the last frame tick.
         *
         * @param unit
         * @return
         */
        double elapsed(ETimeUnit unit = ETimeUnit::Seconds);
        /**
         * Return the elapsed time units since timer initialization.
         *
         * @param unit
         * @return
         */
        double total_elapsed(ETimeUnit unit = ETimeUnit::Seconds);

        /**
         * Return the current frames per second, averaged over a fixed number
         * of timepoint chunks.
         *
         * @return
         */
		float FPS();

        SHIRABE_INLINE
        void setTickDeltaMilliseconds(double const &aTickDeltaMilliseconds)
        {
            mTickDelta = aTickDeltaMilliseconds;
        }

        SHIRABE_INLINE
        bool isTick()
        {
            return (mTickElapsed >= mTickDelta);
        }

        SHIRABE_INLINE
        void resetTick()
        {
            mTickElapsed -= mTickDelta;
        }
		
    private_typedefs:

#ifdef SHIRABE_PLATFORM_WINDOWS
        using InternalTimeType_t = CWindowsTime;
#elif defined(SHIRABE_PLATFORM_LINUX)
        using InternalTimeType_t = CLinuxTime;
#endif
        using InternalTimeValueType_t = InternalTimeType_t::InternalTimeValueType_t;

    private_methods:
        /**
         * Set the time backend, depending on the runtime platform.
         *
         * @param aTimeInterface
         */
        void setTimeInterface(CStdSharedPtr_t<InternalTimeType_t> aTimeInterface);

    private_members:
        CStdSharedPtr_t<InternalTimeType_t> mTimeInterface;

        InternalTimeValueType_t             mConversionConstant;
        uint64_t                            mInitial;
        uint64_t                            mCurrent;
        uint64_t                            mElapsed;

        FPSDataStore_t                      mDataStore;
        double                              mChunkCounter;
        int                                 mFrames;

        double                              mTickElapsed;
        double                              mTickDelta;
	};
}

#ifdef SHIRABE_PLATFORM_WINDOWS
#pragma warning(pop)
#endif

#endif
