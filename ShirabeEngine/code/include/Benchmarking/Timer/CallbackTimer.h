#ifndef CALLBACKTIMER_H
#define CALLBACKTIMER_H

#include <time.h>
#include <ctime>

#include <functional>
#include <memory>
#include <thread>
#include <atomic>

namespace Engine {

    /**
     * @brief The CallbackTimer class provides a multifunctional timing interface.
     *        It runs in a separate thread and allows blocking execution for a given
     *        time in milliseconds or until timing has finished/stopped.
     *        It can fire a callback one a specific amount of time has passed, once
     *        or repeatedly.
     */
    class CallbackTimer {
    public:
        using CallbackFn = std::function<void()>;

        /**
         * @brief CallbackTimer Constructs a new timer.
         * @param cbFn Callback to be executed on expiration of tickInMs.
         * @param tickInMs A generically usable time threshold for callbacks or blocking duration.
         * @param blockWhileRunning Should the execution block until the timer has
         *                          been stopped or finished?
         * @param once If a callback is to be fired, should it be fired only once?
         *             The timer will stop after notification.
         */
        CallbackTimer(
                std::function<void()> cbFn,
                unsigned int          tickInMs,
                bool                  blockWhileRunning = true,
                bool                  once              = false);

        // Deny copy and move!
        CallbackTimer(const CallbackTimer&)             = delete;
        CallbackTimer& operator =(const CallbackTimer&) = delete;
        CallbackTimer(CallbackTimer&&)                  = delete;
        CallbackTimer& operator =(CallbackTimer&&)      = delete;

        /**
         * @brief   elapsedMs How many milliseconds have currently passed?
         * @return  Returns the total amount of milliseconds passed from
         *          initial cycle or the start of subsequent cycles.
         * @remarks If no notifications are used and the timer should run
         *          infinitely or for a limited amount of time,
         *          this entire duration is considered "one cycle".
         */
        inline unsigned int elapsedMs() const { return _elapsed.load(); }

        /**
         * @brief run Creates & initializes the timer runtime, if it was not
         *            created yet!
         * @return True, if the timer is successfully set up and ready to be used.
         *         False, otherwise, e.g. the thread already runs.
         * @remarks This method will create a std::thread and detach the thread, if
         *          execution of the calling thread won't block. If it should block,
         *          the thread will be joined after the thread starts and the only ways
         *          out are:
         *              a) Calling stop to terminate the thread
         *              b) Having Callbacks be fired, on which the system can react.
         */
        bool run();

        /**
         * @brief pause Pauses the thread execution without altering any of the
         *        runtime states.
         * @return True, if the thread was in a pausable state. False otherwise.
         */
        bool pause();

        /**
         * @brief resume Resumes the thread, if created, running and paused.
         *               Otherwise 'run()' will be invoked.
         * @return True if the thread could be resumed or run. False, otherwise.
         */
        bool resume();

        /**
         * @brief stop Stops & destroys the internal pointer instance.
         * @return True, if the thread could be stopped. False, otherwise.
         */
        bool stop();

    private:
        /**
         * @brief exec Internal execution function implementing the timing logic.
         */
        void exec();

        unsigned int                 _tickMs;
        bool                         _once;
        bool                         _blockWhileRunning;
        std::atomic<unsigned int>    _elapsed;
        std::atomic<bool>            _running;
        std::atomic<bool>            _pause;
        std::atomic<bool>            _interrupt;
        std::shared_ptr<std::thread> _timerThread;
        std::function<void()>        _timerCallbackFunction;
    };
    using CallbackTimerPtr = std::shared_ptr<CallbackTimer>;

}

#endif // CALLBACKTIMER_H
