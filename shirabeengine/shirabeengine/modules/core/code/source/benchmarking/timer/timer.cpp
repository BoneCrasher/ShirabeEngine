#include <inttypes.h>

#include <base/declaration.h>
#include <log/log.h>
#include "core/benchmarking/timer/timer.h"
#include <base/string.h>

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CTimer::CTimer()
        : mTimeInterface(nullptr)
        , mConversionConstant(timespec())
        , mInitial(0)
        , mCurrent(0)
        , mElapsed(0)
        , mDataStore()
        , mChunkCounter(0.0)
        , mFrames(0)
    { };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTimer::~CTimer()
    {
        cleanup();
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CTimer::setTimeInterface(Shared<InternalTimeType_t> const &aTimeInterface)
    {
        SHIRABE_ASSERT_TEXT(nullptr != aTimeInterface, "The provided time interface is nullptr.");

        mTimeInterface = aTimeInterface;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CTimer::initialize()
    {
		EEngineStatus result = EEngineStatus::Ok;

        Shared<InternalTimeType_t> timeInterface =makeShared<InternalTimeType_t>();
        setTimeInterface(timeInterface);

        if (!mTimeInterface)
        {
            CLog::Error(logTag(), "FATAL_ERROR: Timer::initialize: Assigning by Time::getInterface() failed. Interface-pointer is NULL.");
			result = EEngineStatus::Timer_NoPlatformTimeInstance;
        }
        else
        {
            result = mTimeInterface->initialize();
            if (CheckEngineError(result))
            {
                CLog::Error(logTag(),
                            "FATAL_ERROR: TimeInterface::initialize:"
                            " Initializing the internal time interface failed.");

                result = EEngineStatus::Timer_PlatformTimeInstanceInitFailed;
			}

            result = mTimeInterface->getConversionConstant(mConversionConstant);
            if (CheckEngineError(result))
            {
                CLog::Error(logTag(),
                            "Couldn't fetch the timer conversion constant.");

                result = EEngineStatus::Timer_PlatformTimeInstanceInitFailed;
            }

            timespec initial = {};
            result = mTimeInterface->getTimestamp(initial);
            if (CheckEngineError(result))
            {
                CLog::Error(logTag(),
                            CString::format(
                                "ERROR: Timer::initialize: "
                                "An error occured on requesting the current timestamp from the time interface.\n"
                                "At: %0:%1", __FILE__, __LINE__));

                result = EEngineStatus::Timer_PlatformTimestampFetchFailed;
			} else
            {
                mCurrent = mInitial = static_cast<uint64_t>(initial.tv_sec * 1000000000) + initial.tv_nsec;
                }
		}

        mFrames       = 0;
        mChunkCounter = 0;

		return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CTimer::cleanup()
	{
		EEngineStatus result = EEngineStatus::Ok;

        mDataStore.clear();
        mTimeInterface = nullptr;

		return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CTimer::update() {
		EEngineStatus result = EEngineStatus::Ok;

        ++mFrames;

#ifdef _DEBUG
        if (mTimeInterface && (mConversionConstant.tv_sec || mConversionConstant.tv_nsec))
#else
        if (mTimeInterface)
#endif
		{
            mElapsed = mCurrent;

            timespec current = {};

            result = mTimeInterface->getTimestamp(current);
            if (CheckEngineError(result))
                CLog::Error(logTag(),
                            CString::format(
                                "ERROR: Timer::update(): "
                                "Failed at querying the current timestamp.\n%s: %" PRIu32, __FILE__, __LINE__));
            else {
                mCurrent = static_cast<uint64_t>(current.tv_sec * 1000000000) + static_cast<uint64_t>(current.tv_nsec);
                mElapsed = (mCurrent - mElapsed);

                mTickElapsed += elapsed(ETimeUnit::MilliSeconds);

                mChunkCounter += elapsed(ETimeUnit::Seconds);
                if (mChunkCounter >= 1.0)
                {
                    mDataStore.push(total_elapsed(ETimeUnit::Seconds), mFrames);

                    mFrames        = 0;
                    mChunkCounter -= 1.0;
				}
			}
		}

		return result;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    double CTimer::elapsed(ETimeUnit unit)
    {
		double factor = 1.0;

        if (mTimeInterface)
            factor = mTimeInterface->getConversionMask(unit);

        return (static_cast<double>(mElapsed) * factor);
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    double CTimer::total_elapsed(ETimeUnit unit)
    {
		double factor = 1.0;

        if (mTimeInterface)
            factor = mTimeInterface->getConversionMask(unit);

        return ((mCurrent - mInitial) * factor);
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    float CTimer::FPS()
    {
        return static_cast<float>(mDataStore.average(mDataStore.size() - 10, 10));
	};
    //<-----------------------------------------------------------------------------
}
