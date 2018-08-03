#ifndef __SHIRABE_THREADING_LOOPER_IMPL_H__
#define __SHIRABE_THREADING_LOOPER_IMPL_H__

namespace engine {
  namespace Threading {

    template <typename TTaskResult>
    ILooper<TTaskResult>::Task
      ::Task()
      : m_priority(Priority::Normal)
      , m_task()
    { }

    template <typename TTaskResult>
    std::future<TTaskResult> ILooper<TTaskResult>::Task
      ::bind(std::function<TTaskResult()>& fn)
    {
      m_task = std::packaged_task<TTaskResult()>(fn);
      return std::move(m_task.get_future());
    }

    /**
     * \fn  template <typename TTaskResult> void Looper<TTaskResult>::Handler ::storeDelayedPostFuture(std::future<TTaskResult>& f)
     *
     * \brief Stores delayed post future
     *
     * \tparam  TTaskResult Type of the task result.
     * \param [in,out]  f A std::future&lt;bool&gt; to process.
     *
     * \return  A Looper&lt;TTaskResult&gt;::Handler.
     **************************************************************************************************/
    template <typename TTaskResult>
    void Looper<TTaskResult>::Handler
      ::storeDelayedPostFuture(std::future<TTaskResult>& f)
    {
      std::lock_guard<std::recursive_mutex> guard(m_delayedPostFuturesMutex);
      m_delayedPostFutures.push_back(std::move(f));
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult>::Handler ::is_ready(std::future<TTaskResult> const& f)
     *
     * \brief Query if 'f' is ready
     *
     * \tparam  TTaskResult Type of the task result.
     * \param f A const to process.
     *
     * \return  True if ready, false if not.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>::Handler
      ::is_ready(std::future<TTaskResult> const& f)
    {
      return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    /**
     * \fn  template <typename TTaskResult> void Looper<TTaskResult>::Handler ::checkDelayedPostFutures()
     *
     * \brief Check delayed post futures
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper&lt;TTaskResult&gt;::Handler.
     **************************************************************************************************/
    template <typename TTaskResult>
    void Looper<TTaskResult>::Handler
      ::checkDelayedPostFutures()
    {
      std::lock_guard<std::recursive_mutex> guard(m_delayedPostFuturesMutex);

      if(m_delayedPostFutures.empty())
        return;

      // Convenience
      using future_type = std::future<TTaskResult>;
      using future_list = std::vector<future_type>;
      using f_it        = future_list::const_iterator;
      using f_it_list   = std::vector<future_list::const_iterator>;

      f_it      it;
      f_it_list readyFutures;

      for(it  = m_delayedPostFutures.begin();
        it != m_delayedPostFutures.end();
        it++) {
        if(is_ready(*it))
          readyFutures.push_back(it);
      }

      for(f_it& readyIt : readyFutures)
        m_delayedPostFutures.erase(readyIt);
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult>::Handler ::post(TaskType&& runnable)
     *
     * \brief Post this message
     *
     * \tparam  TTaskResult Type of the task result.
     * \param [in,out]  runnable  The runnable.
     *
     * \return  A Looper::Handler.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>::Handler
      ::post(TaskType&& runnable)
    {
      checkDelayedPostFutures();
      return m_assignedLooper.post(std::move(runnable));
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult>::Handler ::postDelayed(TaskType&& runnable, uint64_t timeoutMilliseconds)
     *
     * \brief Posts a delayed
     *
     * \tparam  TTaskResult Type of the task result.
     * \param [in,out]  runnable            The runnable.
     * \param           timeoutMilliseconds The timeout in milliseconds.
     *
     * \return  A Looper::Handler.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>::Handler
      ::postDelayed(TaskType&& runnable, uint64_t timeoutMilliseconds)
    {
      std::function<bool()> fn = [=] () -> bool
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMilliseconds));
        return m_assignedLooper.post(std::move(runnable));
      };

      try {
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

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::post(TaskType&& runnable)
     *
     * \brief Post this message
     *
     * \tparam  TTaskResult Type of the task result.
     * \param [in,out]  runnable  The runnable.
     *
     * \return  A Looper&lt;TRunParameters...&gt;::Handler.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::post(TaskType&& runnable)
    {
      try {
        std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
        m_runnables.push_back(std::move(runnable));
      }
      catch(...) {
        return false;
      }

      return true;
    }

    /**
     * \fn  template <typename TTaskResult> Looper<TTaskResult> ::Looper() : ILooper<TTaskResult>() , m_running(false) , m_abortRequested(false) , m_handler(*this)
     *
     * \brief Gets the looper
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper&lt;TTaskResult&gt;
     **************************************************************************************************/
    template <typename TTaskResult>
    Looper<TTaskResult>
      ::Looper()
      : ILooper<TTaskResult>()
      , m_running(false)
      , m_abortRequested(false)
      , m_handler(*this)
    { }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::initialize()
     *
     * \brief Initializes this object
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::initialize()
    {
      return true;
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::deinitialize()
     *
     * \brief Gets the cleanup
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper.
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::deinitialize()
    {
      if(running()) {
        //Log::Error(logTag(), "Cannot cleanup while running. Abort looper first!");
        return false;
      }

      std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
      if(!m_runnables.empty())
        m_runnables.clear();

      return true;
    }

    /**
     * \fn  template <typename TTaskResult> typename Looper<TTaskResult>::TaskType&& Looper<TTaskResult> ::nextRunnable()
     *
     * \brief Next runnable
     *
     * \exception std::runtime_error  Raised when a runtime error condition occurs.
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper&lt;TRunParameters...&gt;
     **************************************************************************************************/
    template <typename TTaskResult>
    typename Looper<TTaskResult>::TaskType
      Looper<TTaskResult>
      ::nextRunnable()
    {
      std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
      if(m_runnables.empty()) {
        throw std::runtime_error("Empty queue...");
      }

      // Implement dequeueing by priority with proper
      // load balancing here.
      typename Looper<TTaskResult>::TaskType runnable = std::move(m_runnables.back());
      m_runnables.pop_back();

      return std::move(runnable);
    }

    /**
     * \fn  template <typename TTaskResult> void Looper<TTaskResult> ::runFunc()
     *
     * \brief Executes the function operation
     *
     * \exception std::runtime_error  Raised when a runtime error condition occurs.
     * \exception std::exception      Thrown when an exception error condition occurs.
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper.
     **************************************************************************************************/
    template <typename TTaskResult>
    void Looper<TTaskResult>
      ::runFunc()
    {
      // Setup
      m_running.store(true);

      // Run
      while(!m_abortRequested.load()) {
        try {
          TaskType task = nextRunnable();
          if(!loop(std::move(task))) {
            // Error in loop. Step into catch clause.
            throw std::runtime_error("Failed to execute loop function.");
          }
        }
        // TODO: Consider format and name of looper
        catch(std::runtime_error& e) {
          //Log::Error(logTag(), e.what());
        }
        catch(...) {
          //Log::Error(logTag(), "Unknown error.");
        }
      }

      // Shutdown
      m_running.store(false);
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::run()
     *
     * \brief Gets the run
     *
     * \exception std::exception  Thrown when an exception error condition occurs.
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper&lt;TRunParameters...&gt;
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::run()
    {
      try {
        m_thread = std::thread(&Looper::runFunc, this);
      }
      catch(...) {
        //Log::Error(logTag(), "Failed to start thread.");
        return false;
      }

      return true;
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::running()
     *
     * \brief Gets the running
     *
     * \tparam  TTaskResult Type of the task result.
     *
     * \return  A Looper&lt;TRunParameters...&gt;
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::running()
    {
      return m_running.load();
    }

    /**
     * \fn  template <typename TTaskResult> bool Looper<TTaskResult> ::abortAndJoin(uint64_t timeoutMilliseconds)
     *
     * \brief Abort and join
     *
     * \tparam  TTaskResult Type of the task result.
     * \param timeoutMilliseconds The timeout in milliseconds.
     *
     * \return  A Looper&lt;TRunParameters...&gt;
     **************************************************************************************************/
    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::abortAndJoin(uint64_t timeoutMilliseconds)
    {
      m_abortRequested.store(true);

      // If the thread was detached, it won't be joinable.
      // Consequently it should check for abortRequested().
      // Otherwise: Do join!
      if(m_thread.joinable())
        m_thread.join();

      // m_running.store(false);

      return true;
    }

    template <typename TTaskResult>
    bool Looper<TTaskResult>
      ::loop(typename ILooper<TTaskResult>::Task&& runnable)
    {
      //TTaskResult result = runnable.run();
      try {
        if(runnable)
          runnable.run();
        return true;
      }
      catch(std::exception e) {
        Log::Error(logTag(), std::string("Exception in Looper::loop(...): ") + e.what());
        return false;
      }
      catch(...) {
        Log::Error(logTag(), "Unknown error in Looper::loop(...)...");
        return false;
      }
    }
  }
}

#endif