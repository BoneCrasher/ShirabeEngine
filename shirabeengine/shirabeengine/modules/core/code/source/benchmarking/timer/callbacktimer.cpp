#include "core/benchmarking/timer/callbacktimer.h"

#include <stdio.h>
#include <time.h>

namespace Engine
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CCallbackTimer::CCallbackTimer(
            std::function<void()> aCbFn,
            unsigned int          aTickInMs,
            bool                  aBlockWhileRunning,
            bool                  aOnce)
        : mTickMs(aTickInMs),
          mOnce(aOnce),
          mBlockWhileRunning(aBlockWhileRunning),
          mElapsed(0.0),
          mRunning(false),
          mPause(false),
          mInterrupt(false),
          mTimerCallbackFunction(aCbFn)
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CCallbackTimer::run()
    {
        // Do not re-run a running timer.
        if(mRunning.load())
            return false;

        mRunning.store(true);
        mPause.store(false);
        mInterrupt.store(false);

        if(!mTimerThread)
        {
            mTimerThread
                    = CStdSharedPtr_t<std::thread>(
                          new std::thread(&CCallbackTimer::exec, this),
                          [] (std::thread *p) { if(p) { if(p->joinable()) p->join(); delete p; } }
                      );

            if(mBlockWhileRunning)
            {
                mTimerThread->join();
                mTimerThread = nullptr;
            }
            else
                mTimerThread->detach();
        }

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CCallbackTimer::pause()
    {
        // Do not allow multithreaded execution corruption by
        // having one thread unblock another.
        // Do not pause a not running timer.
        // Do not interfere if the timer is currently being interrupted.
        if(mBlockWhileRunning
                || !mRunning.load()
                || mInterrupt.load())
            return false;

        mPause.store(true); // Will halt next frame!
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CCallbackTimer::resume()
    {
        if(!mRunning.load()) // Implicit run feature.
            run();
        else
        {
            // Do not allow multithreaded execution corruption by
            // having one thread unblock another.
            if(mBlockWhileRunning)
                return false;

            mPause.store(false); // Will continue after Sleep!
        }

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CCallbackTimer::stop()
    {
        // Do not pause a not running timer.
        if(!mRunning.load())
            return false;

        mInterrupt.store(true); // Will stop next frame!

        // MB20161205 - ATTENTION:
        //    Most likely this is the cause of timer related crashes currently occurring.
        //    The thread instance could be destroyed BEFORE the internal function has returned.
        //    UhOh... Manage this in existing custom deleter?
        mTimerThread = nullptr;

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CCallbackTimer::exec()
    {
        std::clock_t
                curr = 0,
                prev = 0;

        curr = std::clock();

        double ms = 0.0;

        while(true)
        {
            if(mInterrupt.load())
                break;

            prev = curr;
            curr = std::clock();

            double const step = ((float) (curr - prev) / CLOCKS_PER_SEC) * 1000.0;
            ms += step;

            mElapsed.store(mElapsed.load() + ms);

            if(ms >= mTickMs)
            {
                // TODO: Instead of subtracting, do modulo if the timer stalled for whatever reason?
                mElapsed.store((ms -= mTickMs)); // Store the previous cycle's overhead as the current elapsed.

                if(mTimerCallbackFunction && !mPause.load())
                    mTimerCallbackFunction(); // Async management should be handled externally if necessary!

                if(mOnce)
                    break;
            }
        }

        mRunning.store(false);
        mInterrupt.store(false); // Will stop next frame!
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    uint32_t CCallbackTimer::elapsedMs() const
    {
        return mElapsed.load();
    }
    //<-----------------------------------------------------------------------------
}
