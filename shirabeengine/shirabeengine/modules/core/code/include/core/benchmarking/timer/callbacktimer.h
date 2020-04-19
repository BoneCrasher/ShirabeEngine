#ifndef __SHIRABE_ENGINE_CALLBACKTIMER_H__
#define __SHIRABE_ENGINE_CALLBACKTIMER_H__

#include <time.h>
#include <ctime>

#include <functional>
#include <memory>
#include <thread>
#include <atomic>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

namespace engine
{

    /**
     * The CallbackTimer class provides a multifunctional timing interface.
     * It runs in a separate thread and allows blocking execution for a given
     * time in milliseconds or until timing has finished/stopped.
     * It can fire a callback one a specific amount of time has passed, once
     * or repeatedly.
     */
    class CCallbackTimer
    {
    public:
        using CallbackFn = std::function<void()>;

        /**
         * Constructs a new timer.
         *
         * @param aCbFn              Callback to be executed on expiration of tickInMs.
         * @param aTickInMs          A generically usable time threshold for callbacks or blocking duration.
         * @param aBlockWhileRunning Should the execution block until the timer has
         *                           been stopped or finished?
         * @param aOnce              If a callback is to be fired, should it be fired only once?
         *                           The timer will stop after notification.
         */
        CCallbackTimer(
                std::function<void()> aCbFn,
                uint32_t              aTickInMs,
                bool                  aBlockWhileRunning = true,
                bool                  aOnce              = false);

        // Deny copy and move!
        CCallbackTimer(CCallbackTimer const &)             = delete;
        CCallbackTimer(CCallbackTimer &&)                  = delete;
        CCallbackTimer& operator =(CCallbackTimer const &) = delete;
        CCallbackTimer& operator =(CCallbackTimer &&)      = delete;

        /**
         * How many milliseconds have currently passed?
         *
         * @return  Returns the total amount of milliseconds passed from
         *          initial cycle or the start of subsequent cycles.
         * @remarks If no notifications are used and the timer should run
         *          infinitely or for a limited amount of time,
         *          this entire duration is considered "one cycle".
         */
        uint32_t elapsedMs() const;

        /**
         * Creates & initializes the timer runtime, if it was not created yet!
         *
         * @return True, if the timer is successfully set up and ready to be used.
         *         False, otherwise, e.g. the thread already runs.
         * @remarks This method will create a std::thread and detach the thread, if
         *          execution of the calling thread won't block. If it should block,
         *          the thread will be joined after the thread starts and the only ways
         *          out are:
         *              a) Calling stop to terminate the thread
         *              b) Having Callbacks be fired, on which the system can react.
         */
        CEngineResult<> run();

        /**
         * Pauses the thread execution without altering any of the
         * runtime states.
         *
         * @return True, if the thread was in a pausable state. False otherwise.
         */
        CEngineResult<> pause();

        /**
         * Resumes the thread, if created, running and paused.
         * Otherwise 'run()' will be invoked.
         *
         * @return True if the thread could be resumed or run. False, otherwise.
         */
        CEngineResult<> resume();

        /**
         * Stops & destroys the internal pointer instance.
         *
         * @return True, if the thread could be stopped. False, otherwise.
         */
        CEngineResult<> stop();

    private:
        /**
         * Internal execution function implementing the timing logic.
         */
        CEngineResult<> exec();

        uint32_t                     mTickMs;
        bool                         mOnce;
        bool                         mBlockWhileRunning;
        std::atomic<double>          mElapsed;
        std::atomic<bool>            mRunning;
        std::atomic<bool>            mPause;
        std::atomic<bool>            mInterrupt;
        Shared<std::thread> mTimerThread;
        std::function<void()>        mTimerCallbackFunction;
    };

    /**
     *
     */
    using CallbackTimerPtr = Shared<CCallbackTimer>;

}

#endif // __SHIRABE_ENGINE_CALLBACKTIMER_H__
