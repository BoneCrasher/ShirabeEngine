#include "Benchmarking/Timer/CallbackTimer.h"

#include <stdio.h>
#include <time.h>

namespace Engine {

    CallbackTimer::CallbackTimer(
            std::function<void()> cbFn,
            unsigned int          tickInMs,
            bool                  blockWhileRunning,
            bool                  once)
        : _tickMs(tickInMs),
          _once(once),
          _blockWhileRunning(blockWhileRunning),
          _elapsed(0.0),
          _running(false),
          _pause(false),
          _interrupt(false),
          _timerCallbackFunction(cbFn) {
    }

    bool CallbackTimer::run()  {
        // Do not re-run a running timer.
        if(_running.load())
            return false;

        _running.store(true);
        _pause.store(false);
        _interrupt.store(false);

        if(!_timerThread) {
            _timerThread
                    = std::shared_ptr<std::thread>(
                          new std::thread(&CallbackTimer::exec, this),
                          [] (std::thread *p) { if(p) { if(p->joinable()) p->join(); delete p; } }
                      );

            if(_blockWhileRunning) {
                _timerThread->join();
                _timerThread = nullptr;
            }
            else
                _timerThread->detach();
        }

        return true;
    }

    bool CallbackTimer::pause() {
        // Do not allow multithreaded execution corruption by
        // having one thread unblock another.
        // Do not pause a not running timer.
        // Do not interfere if the timer is currently being interrupted.
        if(_blockWhileRunning
                || !_running.load()
                || _interrupt.load())
            return false;

        _pause.store(true); // Will halt next frame!
        return true;
    }

    bool CallbackTimer::resume() {
        if(!_running.load()) // Implicit run feature.
            run();
        else {
            // Do not allow multithreaded execution corruption by
            // having one thread unblock another.
            if(_blockWhileRunning)
                return false;

            _pause.store(false); // Will continue after Sleep!
        }

        return true;
    }

    bool CallbackTimer::stop() {
        // Do not pause a not running timer.
        if(!_running.load())
            return false;

        _interrupt.store(true); // Will stop next frame!

        // MB20161205 - ATTENTION:
        //    Most likely this is the cause of timer related crashes currently occurring.
        //    The thread instance could be destroyed BEFORE the internal function has returned.
        //    UhOh... Manage this in existing custom deleter?
        _timerThread = nullptr;

        return true;
    }


    void CallbackTimer::exec() {
        std::clock_t curr, prev;

        curr = std::clock();

        double ms = 0.0;
        while(true) {
            if(_interrupt.load())
                break;

            prev = curr;
            curr = std::clock();


            double step = ((float) (curr - prev) / CLOCKS_PER_SEC) * 1000.0;
            ms += step;

            _elapsed.store(_elapsed.load() + ms);

            if(ms >= _tickMs) {
                // TODO: Instead of subtracting, do modulo if the timer stalled for whatever reason?
                _elapsed.store((ms -= _tickMs)); // Store the previous cycle's overhead as the current elapsed.

                if(_timerCallbackFunction
                        && !_pause.load())
                    _timerCallbackFunction(); // Async management should be handled externally if necessary!

                if(_once)
                    break;
            }
        }

        _running.store(false);
        _interrupt.store(false); // Will stop next frame!
    }

}
