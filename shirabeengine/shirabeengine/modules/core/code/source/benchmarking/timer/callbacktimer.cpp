#include "Core/Benchmarking/Timer/CallbackTimer.h"

#include <stdio.h>
#include <time.h>

namespace Engine {

    CallbackTimer::CallbackTimer(
            std::function<void()> cbFn,
            unsigned int          tickInMs,
            bool                  blockWhileRunning,
            bool                  once)
        : m_tickMs(tickInMs),
          m_once(once),
          m_blockWhileRunning(blockWhileRunning),
          m_elapsed(0.0),
          m_running(false),
          m_pause(false),
          m_interrupt(false),
          m_timerCallbackFunction(cbFn) {
    }

    bool CallbackTimer::run()  {
        // Do not re-run a running timer.
        if(m_running.load())
            return false;

        m_running.store(true);
        m_pause.store(false);
        m_interrupt.store(false);

        if(!m_timerThread) {
            m_timerThread
                    = std::shared_ptr<std::thread>(
                          new std::thread(&CallbackTimer::exec, this),
                          [] (std::thread *p) { if(p) { if(p->joinable()) p->join(); delete p; } }
                      );

            if(m_blockWhileRunning) {
                m_timerThread->join();
                m_timerThread = nullptr;
            }
            else
                m_timerThread->detach();
        }

        return true;
    }

    bool CallbackTimer::pause() {
        // Do not allow multithreaded execution corruption by
        // having one thread unblock another.
        // Do not pause a not running timer.
        // Do not interfere if the timer is currently being interrupted.
        if(m_blockWhileRunning
                || !m_running.load()
                || m_interrupt.load())
            return false;

        m_pause.store(true); // Will halt next frame!
        return true;
    }

    bool CallbackTimer::resume() {
        if(!m_running.load()) // Implicit run feature.
            run();
        else {
            // Do not allow multithreaded execution corruption by
            // having one thread unblock another.
            if(m_blockWhileRunning)
                return false;

            m_pause.store(false); // Will continue after Sleep!
        }

        return true;
    }

    bool CallbackTimer::stop() {
        // Do not pause a not running timer.
        if(!m_running.load())
            return false;

        m_interrupt.store(true); // Will stop next frame!

        // MB20161205 - ATTENTION:
        //    Most likely this is the cause of timer related crashes currently occurring.
        //    The thread instance could be destroyed BEFORE the internal function has returned.
        //    UhOh... Manage this in existing custom deleter?
        m_timerThread = nullptr;

        return true;
    }


    void CallbackTimer::exec() {
        std::clock_t curr, prev;

        curr = std::clock();

        double ms = 0.0;
        while(true) {
            if(m_interrupt.load())
                break;

            prev = curr;
            curr = std::clock();


            double step = ((float) (curr - prev) / CLOCKS_PER_SEC) * 1000.0;
            ms += step;

            m_elapsed.store(m_elapsed.load() + ms);

            if(ms >= m_tickMs) {
                // TODO: Instead of subtracting, do modulo if the timer stalled for whatever reason?
                m_elapsed.store((ms -= m_tickMs)); // Store the previous cycle's overhead as the current elapsed.

                if(m_timerCallbackFunction
                        && !m_pause.load())
                    m_timerCallbackFunction(); // Async management should be handled externally if necessary!

                if(m_once)
                    break;
            }
        }

        m_running.store(false);
        m_interrupt.store(false); // Will stop next frame!
    }

}
