#ifndef __SHIRABE_THREADING_LOOPER_H__
#define __SHIRABE_THREADING_LOOPER_H__

#include <type_traits>
#include <thread>
#include <future>

#include <base/declaration.h>
#include <log/log.h>
#include "core/enginetypehelper.h"

namespace engine
{
    namespace threading
    {
        /**
         * @brief The Priority enum
         */
        enum class ETaskPriority
        {
            Least     =    1,
            Less      =    2,
            Normal    =    4,
            Higher    =    8,
            Highest   =   16,
            TonyStark = 1337 // 04/10/2017 - 04:49: Now i want to watch Avengers...
        };

        /**
         * Interface and Task-Implementation for tasks returning a TTaskResult.
         *
         * @tparam Return type of tasks executed by this looper.
         */
        template <typename TTaskResult>
        class ILooper
        {
            SHIRABE_DECLARE_INTERFACE(ILooper);

        public_classes:
            /**
             * @brief The CTask class
             */
            class CTask
            {
            public_typedefs:
                friend class ILooper<TTaskResult>;

            public_constructors:
                /**
                 * Construct an empty, unbound task.
                 */
                CTask();

                /**
                 * Deny copy, since the task itself contains uncopyable objects.
                 *
                 * @param aOther
                 */
                CTask(CTask const &aOther) = delete;

                /**
                 * Move-Construct this task from another task instance.
                 *
                 * @param aOther
                 */
                CTask(CTask&& aOther)
                    : mPriority(aOther.mPriority)
                    , mTask(std::move(aOther.m_task))
                {}

            public_operators:
                /**
                 * Move-Assign another task instance.
                 *
                 * @param aOther
                 * @return
                 */
                CTask& operator=(CTask&& aOther)
                {
                    mPriority = aOther.mPriority;
                    mTask     = std::move(aOther.v);

                    return *this;
                }

                /**
                 * Copy-Assignment is denied, since uncopyable objects are contained.
                 *
                 * @param aOther
                 * @return
                 */
                CTask& operator=(CTask const &aOther) = delete;

                /**
                 * Implicit bool conversion to check for validity.
                 */
                operator bool()
                {
                    return mTask.valid();
                }

            public_methods:
                /**
                 * Return the current task priority attached.
                 *
                 * @return
                 */
                SHIRABE_INLINE ETaskPriority const &priority() const
                {
                    return mPriority;
                }

                /**
                 * Set the task priority of this task to 'aPriority'.
                 *
                 * @param aPriority
                 */
                SHIRABE_INLINE void setPriority(ETaskPriority const &aPriority)
                {
                    mPriority = aPriority;
                }

                /**
                 * Bind a function returning TTaskResult to this task.
                 *
                 * @param aFunction
                 * @return
                 */
                std::future<TTaskResult> bind(std::function<TTaskResult()> &aFunction);

                /**
                 * Execute the task.
                 */
                void run()
                {
                    mTask(); // Return value is stored in the shared state wrapped by the future returned on bind!
                }

            private:
                ETaskPriority                     mPriority;
                std::packaged_task<TTaskResult()> mTask;
            };

            /**
             * Initialize the looper and run it.
             *
             * @return
             */
            virtual bool initialize()   = 0;

            /**
             * Stop and shutdown the looper.
             *
             * @return
             */
            virtual bool deinitialize() = 0;

            /**
             * 	Implementation of the effective loop function to be invoked.
             * 	Will be provided with the next to be executed runnable.
             *
             * @param runnable
             * @return
             */
            virtual bool loop(ILooper<TTaskResult>::CTask&& aRunnable) = 0;

        };

        /**
         * Defualt implementation of ILooper.
         */
        template <typename TTaskResult>
        class CLooper
                : public ILooper<TTaskResult>
        {
            SHIRABE_DECLARE_LOG_TAG(Looper<TTaskResult>)

        public_typedefs:
            using LooperType = CLooper<TTaskResult>;
            using TaskType   = typename ILooper<TTaskResult>::CTask;

        public_classes:
            /**
             * The Handler is the public interface to the looper and provides various
             * means of pushing a task over into the looper queue.
             */
            class CHandler
            {
            public_typedefs:
                friend class CLooper; // Allow the looper to access the private constructor.

            public_methods:
                /**
                 * Immediately append the task provided to the looper queue.
                 *
                 * @return True, if successful. False otherwise.
                 */
                bool post(TaskType &&aOther);

                /**
                 * Delay insert the task provided into the looper queue.
                 *
                 * @param aOther
                 * @param aTimeoutMilliseconds
                 * @return                     True, if successful. False otherwise.
                 */
                bool postDelayed(
                        TaskType      &&aOther,
                        uint64_t const &aTimeoutMilliseconds = 0);

            private_constructors:
                /**
                 * Create a handler based on the provided looper.
                 *
                 * @param aLooper
                 */
                SHIRABE_INLINE CHandler(LooperType &aLooper)
                    : mAssignedLooper(aLooper)
                {}

            private_methods:
                /**
                 * Check, if the task was executed and finished, indicated by
                 * the future having received any kind of result.
                 *
                 * @param f
                 * @return
                 */
                bool is_ready(std::future<TTaskResult> const& f);

                /**
                 * In case of a delayed post, store the future in a list.
                 * @param f
                 */
                void storeDelayedPostFuture(std::future<TTaskResult>& f);

                /**
                 * Review the list of delayed post handles and check if results
                 * have been returned already.
                 */
                void checkDelayedPostFutures();

            private_methods:
                LooperType                            &mAssignedLooper;
                std::recursive_mutex                   mDelayedPostFuturesMutex;
                std::vector<std::future<TTaskResult>>  mDelayedPostFutures;
            };

        public_constructors:
            /**
             * Default create and empty looper.
             */
            CLooper();

            // Deny copy and move...
            CLooper(const LooperType&)                = delete;
            CLooper(LooperType&&)                     = delete;
            LooperType& operator=(const LooperType&) = delete;
            LooperType& operator=(LooperType&&)      = delete;

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CLooper() = default;

        public_methods:
            /**
             * Initialize this looper and make it ready to run.
             *
             * @return
             */
            virtual bool initialize();

            /**
             * Stop operation and deinitialize this looper.
             *
             * @return
             */
            virtual bool deinitialize();

            /**
             * Run the looper's loop.
             *
             * @return
             */
            bool run();

            /**
             * Check, whether the looper internal thread is running.
             *
             * @return
             */
            bool running();

            /**
             * Stop the thread and returned.
             *
             * @param timeoutMilliseconds
             * @return
             */
            bool abortAndJoin(uint64_t const &timeoutMilliseconds = 10000);

            /**
             * Return the handler attached to the looper.
             * @return
             */
            SHIRABE_INLINE CHandler &getHandler()
            {
                return mHandler;
            }

         private_methods:
            /**
             * Signal the worker thread, that shutdown is required.
             */
            SHIRABE_INLINE void requestAbort()
            {
                mAbortRequested.store(true);
            }

            /**
             * Check, whether shutdown was requested.
             *
             * @return
             */
            SHIRABE_INLINE bool abortRequested() const
            {
                return mAbortRequested.load();
            }

            /**
             * Fetch the next task from the queue.
             *
             * @return
             */
            TaskType nextRunnable();

            /**
             * Thread func to be executed on the worker thread.
             */
            void runFunc();

            /**
             * Loop over one task and execute it.
             *
             * @param runnable
             * @return
             */
            bool loop(typename threading::ILooper<TTaskResult>::CTask&& aRunnable);

            /**
             * Post a task to the queue. Invoked exclusively by the attached handler.
             *
             * @return
             */
            bool post(TaskType &&aTask);

        private_members:
            std::thread           mThread;
            std::atomic_bool      mRunning;
            std::atomic_bool      mAbortRequested;

            CHandler               mHandler;

            std::recursive_mutex  mRunnablesMutex;
            std::vector<TaskType> mRunnables;
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        ILooper<TTaskResult>::CTask::CTask()
            : mPriority(ETaskPriority::Normal)
            , mTask()
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        std::future<TTaskResult>
            ILooper<TTaskResult>::CTask::bind(std::function<TTaskResult()> &aFunction)
        {
            mTask = std::packaged_task<TTaskResult()>(aFunction);

            return std::move(mTask.get_future());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        void CLooper<TTaskResult>::CHandler::storeDelayedPostFuture(std::future<TTaskResult> &aFunction)
        {
            std::lock_guard<std::recursive_mutex> guard(mDelayedPostFuturesMutex);

            mDelayedPostFutures.push_back(std::move(aFunction));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::CHandler::is_ready(std::future<TTaskResult> const &aFunction)
        {
            std::future_status const status = aFunction.wait_for(std::chrono::seconds(0));
            bool               const ready  = (status == std::future_status::ready);

            return ready;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        void CLooper<TTaskResult>::CHandler::checkDelayedPostFutures()
        {
            std::lock_guard<std::recursive_mutex> guard(mDelayedPostFuturesMutex);

            if(mDelayedPostFutures.empty())
            {
                return;
            }

            // Convenience
            using future_type = std::future<TTaskResult>;
            using future_list = std::vector<future_type>;
            using f_it        = typename future_list::const_iterator;
            using f_it_list   = std::vector<typename future_list::const_iterator>;

            f_it      it;
            f_it_list readyFutures;

            for(it  = mDelayedPostFutures.begin();
                it != mDelayedPostFutures.end();
                ++ it)
            {
                if(is_ready(*it))
                    readyFutures.push_back(it);
            }

            for(f_it& readyIt : readyFutures)
            {
                mDelayedPostFutures.erase(readyIt);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::CHandler::post(TaskType &&aRunnable)
        {
            checkDelayedPostFutures();

            return mAssignedLooper.post(std::move(aRunnable));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::CHandler
        ::postDelayed(
                TaskType      &&aRunnable,
                uint64_t const &aTimeoutMilliseconds)
        {
            std::function<bool()> fn = [=] () -> bool
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(aTimeoutMilliseconds));
                return mAssignedLooper.post(std::move(aRunnable));
            };

            try
            {
                std::future<bool> f = std::async(std::launch::async, fn);
                storeDelayedPostFuture(std::move(f));
                checkDelayedPostFutures();
            }
            catch(std::bad_alloc ba) {
                return false;
            }
            catch(std::system_error se) {
                return false;
            }
            catch(...) {
                return false;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::post(TaskType &&aRunnable)
        {
            try
            {
                std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);

                mRunnables.push_back(std::move(aRunnable));
            }
            catch(...) {
                return false;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        CLooper<TTaskResult>::CLooper()
            : ILooper<TTaskResult>()
            , mRunning(false)
            , mAbortRequested(false)
            , mHandler(*this)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::initialize()
        {
            return true;
        }

        template <typename TTaskResult>
        bool CLooper<TTaskResult>::deinitialize()
        {
            if(running())
            {
                return false;
            }

            std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);
            if(!mRunnables.empty())
                mRunnables.clear();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        typename CLooper<TTaskResult>::CTaskType CLooper<TTaskResult>::nextRunnable()
        {
            std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);

            if(mRunnables.empty())
            {
                throw std::runtime_error("Empty queue...");
            }

            // Implement dequeueing by priority with proper
            // load balancing here.
            typename CLooper<TTaskResult>::CTaskType runnable = std::move(mRunnables.back());
            mRunnables.pop_back();

            return std::move(runnable);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        void CLooper<TTaskResult>::runFunc()
        {
            mRunning.store(true);

            bool const shouldAbort = mAbortRequested.load();
            while(!shouldAbort)
            {
                try {
                    TaskType task = nextRunnable();
                    if(!loop(std::move(task)))
                    {
                        throw std::runtime_error("Failed to execute loop function.");
                    }
                }
                catch(std::runtime_error& e) {
                    CLog::Error(logTag(), e.what());
                }
                catch(...) {
                    CLog::Error(logTag(), "Unknown error.");
                }
            }

            mRunning.store(false);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::run()
        {
            try {
                mThread = std::thread(&CLooper::runFunc, this);
            }
            catch(...) {
                CLog::Error(logTag(), "Failed to start thread.");
                return false;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>
        ::running()
        {
            return mRunning.load();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::abortAndJoin(uint64_t const &aTimeoutMilliseconds)
        {
            mAbortRequested.store(true);

            // If the thread was detached, it won't be joinable.
            // Consequently it should check for abortRequested().
            // Otherwise: Do join!
            if(mThread.joinable())
                mThread.join();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TTaskResult>
        bool CLooper<TTaskResult>::loop(typename ILooper<TTaskResult>::CTask &&aRunnable)
        {
            //TTaskResult result = runnable.run();
            try
            {
                if(aRunnable)
                    aRunnable.run();

                return true;
            }
            catch(std::exception aException)
            {
                CLog::Error(logTag(), std::string("Exception in Looper::loop(...): ") + aException.what());
                return false;
            }
            catch(...)
            {
                CLog::Error(logTag(), "Unknown error in Looper::loop(...)...");
                return false;
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
#endif
