#include "Threading/Looper.h"

namespace Engine {
	namespace Threading {

		/**********************************************************************************************//**
		 * \fn	void Looper::Handler::storeDelayedPostFuture(std::future<bool>& f)
		 *
		 * \brief	Stores delayed post future
		 *
		 * \param [in,out]	f	A std::future&lt;bool&gt; to process.
		 **************************************************************************************************/
		void Looper::Handler::storeDelayedPostFuture(std::future<bool>& f) {
			std::lock_guard<std::recursive_mutex> guard(m_delayedPostFuturesMutex);
			m_delayedPostFutures.push_back(std::move(f));
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper::Handler::is_ready(std::future<bool> const& f)
		 *
		 * \brief	Query if 'f' is ready
		 *
		 * \param	f	A const to process.
		 *
		 * \return	True if ready, false if not.
		 **************************************************************************************************/
		bool Looper::Handler::is_ready(std::future<bool> const& f) {
			return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		}

		/**********************************************************************************************//**
		 * \fn	void Looper::Handler::checkDelayedPostFutures()
		 *
		 * \brief	Check delayed post futures
		 **************************************************************************************************/
		void Looper::Handler::checkDelayedPostFutures() {
			std::lock_guard<std::recursive_mutex> guard(m_delayedPostFuturesMutex);

			if( m_delayedPostFutures.empty() )
				return;

			// Convenience
			using future_type = std::future<bool>;
			using future_list = std::vector<future_type>;
			using f_it        = future_list::const_iterator;
			using f_it_list   = std::vector<future_list::const_iterator>;

			f_it      it;
			f_it_list readyFutures;

			for( it  = m_delayedPostFutures.begin();
				it != m_delayedPostFutures.end();
				it++ ) {
				if( is_ready(*it) )
					readyFutures.push_back(it);
			}

			for( f_it& readyIt : readyFutures )
				m_delayedPostFutures.erase(readyIt);
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper::Handler ::post(RunnableTypePtr runnable)
		 *
		 * \brief	Post this message
		 *
		 * \param	runnable	The runnable.
		 *
		 * \return	A Looper::Handler.
		 **************************************************************************************************/
		bool Looper::Handler
			::post(RunnableTypePtr runnable)
		{
			checkDelayedPostFutures();
			return m_assignedLooper.post(runnable);
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper::Handler ::postDelayed(RunnableTypePtr runnable, uint64_t timeoutMilliseconds)
		 *
		 * \brief	Posts a delayed
		 *
		 * \param	runnable		   	The runnable.
		 * \param	timeoutMilliseconds	The timeout in milliseconds.
		 *
		 * \return	A Looper::Handler.
		 **************************************************************************************************/
		bool Looper::Handler
			::postDelayed(RunnableTypePtr runnable, uint64_t timeoutMilliseconds)
		{
			std::function<bool()> fn = [=] () -> bool
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMilliseconds));
				return m_assignedLooper.post(runnable);
			};

			try {
				std::future<bool> f = std::async(std::launch::async, fn);
				storeDelayedPostFuture(std::move(f));
				checkDelayedPostFutures();
			} catch( std::bad_alloc ba ) {
				return false;
			} catch( std::system_error se ) {
				return false;
			} catch( ... ) {
				return false;
			}

			return true;
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper ::post(RunnableTypePtr runnable)
		 *
		 * \brief	Post this message
		 *
		 * \param	runnable	The runnable.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;::Handler.
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 **************************************************************************************************/
		bool Looper
			::post(RunnableTypePtr runnable) 
		{
			try {
				std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
				m_runnables.push_back(runnable);
			}
			catch( ... ) {
				return false;
			}

			return true;
		}

		/**********************************************************************************************//**
		 * \fn	Looper ::Looper()
		 *
		 * \brief	Gets the looper
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 * ### return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		Looper
			::Looper()
			: ILooper()
			, m_running(false)
			, m_abortRequested(false)
			, m_handler(*this)
		{ }

		/**********************************************************************************************//**
		 * \fn	bool Looper ::initialize()
		 *
		 * \brief	Initializes this object
		 *
		 * \return	A Looper.
		 **************************************************************************************************/
		bool Looper
			::initialize()
		{
			return true;
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper ::cleanup()
		 *
		 * \brief	Gets the cleanup
		 *
		 * \return	A Looper.
		 **************************************************************************************************/
		bool Looper
			::cleanup()
		{
			if( running() ) {
				Log::Error(logTag(), "Cannot cleanup while running. Abort looper first!");
				return false;
			}

			std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
			if( !m_runnables.empty() )
				m_runnables.clear();

			return true;
		}

		/**********************************************************************************************//**
		 * \fn	typename Looper::RunnableTypePtr Looper ::nextRunnable()
		 *
		 * \brief	Next runnable
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 **************************************************************************************************/
		typename Looper::RunnableTypePtr 
			Looper
			::nextRunnable()
		{
			std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
			if( m_runnables.empty() )
				return nullptr;

			// Implement dequeueing by priority with proper
			// load balancing here.
			RunnableTypePtr runnable = m_runnables.back();
			m_runnables.pop_back();

			return runnable;
		}

		/**********************************************************************************************//**
		 * \fn	void Looper ::runFunc()
		 *
		 * \brief	Executes the function operation
		 *
		 * \exception	std::exception	Thrown when an exception error condition occurs.
		 *
		 * \return	A Looper.
		 **************************************************************************************************/
		void Looper
			::runFunc()
		{
		    // Setup
			m_running.store(true);

			// Run
			while( !m_abortRequested.load() ) {
				RunnableTypePtr runnable = nextRunnable();
				if( !runnable ) {
					// No runnable available. Pause.
					// sleep(...);
					continue;
				}

				try {
					if( !loop(runnable) ) {
						// Error in loop. Step into catch clause.
						throw std::exception("Failed to execute loop function.");
					}
				}
				// TODO: Consider format and name of looper
				catch( std::exception& e ) {
					Log::Error(logTag(), e.what());
				}
				catch( ... ) {
					Log::Error(logTag(), "Unknown error.");
				}
			}

			// Shutdown
			m_running.store(false);
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper ::run()
		 *
		 * \brief	Gets the run
		 *
		 * \exception	std::exception	Thrown when an exception error condition occurs.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 **************************************************************************************************/
		bool Looper
			::run()
		{
			try {
				m_thread = std::thread(&Looper::runFunc, this);
			}
			catch( ... ) {
				Log::Error(logTag(), "Failed to start thread.");
				return false;
			}

			return true;
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper ::running()
		 *
		 * \brief	Gets the running
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 **************************************************************************************************/
		bool Looper
			::running()
		{
			return m_running.load();
		}

		/**********************************************************************************************//**
		 * \fn	bool Looper ::abortAndJoin(uint64_t timeoutMilliseconds)
		 *
		 * \brief	Abort and join
		 *
		 * \param	timeoutMilliseconds	The timeout in milliseconds.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 *
		 * ### tparam	TRunParameters	Type of the run parameters.
		 **************************************************************************************************/
		bool Looper
			::abortAndJoin(uint64_t timeoutMilliseconds) 
		{
			m_abortRequested.store(true);

			// If the thread was detached, it won't be joinable.
			// Consequently it should check for abortRequested().
			// Otherwise: Do join!
			if( m_thread.joinable() )
				m_thread.join();

			return true;
		}
	}
}