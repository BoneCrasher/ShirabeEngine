#ifndef __SHIRABE_THREADING_LOOPER_H__
#define __SHIRABE_THREADING_LOOPER_H__

#include <type_traits>
#include <thread>
#include <future>

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

namespace Engine {
	namespace Threading {

		template<typename... TRunParameters>
		struct param_pack { };
		
		/**********************************************************************************************//**
		 * \fn	DeclareInterface(ILooper);
		 *
		 * \brief	Constructor
		 *
		 * \param	parameter1	The first parameter.
		 **************************************************************************************************/
		template <typename... TRunParameters>
		DeclareInterface(ILooper);
		public:
			using params = param_pack<TRunParameters...>;

			/**********************************************************************************************//**
			 * \enum	Priority
			 *
			 * \brief	Values that represent priorities
			 **************************************************************************************************/
			enum class Priority {
				Least     =    1,
				Less      =    2,
				Normal    =    4,
				Higher    =    8,
				Highest   =   16,
				TonyStark = 1337 // 04/10/2017 - 04:49: Now i want to watch Avengers...
			};
			/**************************************************************************************************//**/
			/* 
			/**
		     * \fn	DeclareInterface(IRunnable);
			 *
			 * \brief	IRunnable instances can be dequeued and run by a Looper.
			 *
			 * \param	parameter1	The first parameter.
			 **************************************************************************************************/
			DeclareInterface(IRunnable);

			/**********************************************************************************************//**
			 * \fn	virtual Priority priority() = 0;
			 *
			 * \brief	Gets the priority
			 *
			 * \return	A Priority.
			 **************************************************************************************************/
			virtual Priority priority() = 0;

			/**********************************************************************************************//**
			 * \fn	virtual bool run(TRunParameters&&...) = 0;
			 *
			 * \brief	Runs the given ...
			 *
			 * \param [in,out]	...	The ...
			 *
			 * \return	True if it succeeds, false if it fails.
			 **************************************************************************************************/
			virtual bool run(TRunParameters...) = 0;

			DeclareInterfaceEnd(IRunnable);
			DeclareSharedPointerType(IRunnable);
			/**************************************************************************************************//**/
			
			virtual bool initialize() = 0;
			virtual bool cleanup()    = 0;

			/**********************************************************************************************//**
			 * \fn	virtual bool Looper::loop() = 0;
			 *
			 * \brief	Implementation of the effective loop function to be invoked.
			 * 			Will be provided with the next to be executed runnable.
			 *
			 * \return	True if it succeeds, false if it fails.
			 **************************************************************************************************/
			virtual bool loop(const typename ILooper<TRunParameters...>::IRunnablePtr& runnable) = 0;
	   
		DeclareInterfaceEnd(ILooper);

		template <typename... TRunParameters>
		DeclareTemplatedSharedPointerType(ILooper, Template(ILooper<TRunParameters...>));

		/**********************************************************************************************//**
		 * \class	Looper
		 *
		 * \brief	A looper.
		 *
		 * \tparam	TDerivedRunnableType	Type of the derived runnable type used for this specific looper.
		 **************************************************************************************************/
		template <typename... TRunParameters>
		class Looper
			: public ILooper<TRunParameters...>
		{
			DeclareLogTag(Looper<TRunParameters...>);

		public:
			using LooperType      = Looper<TRunParameters...>;
			using RunnableTypePtr = Ptr<typename ILooper<TRunParameters...>::IRunnable>;

			/**********************************************************************************************//**
			 * \class	Handler
			 *
			 * \brief	A handler.
			 **************************************************************************************************/
			class Handler {
				friend class Looper<TRunParameters...>; // Allow the looper to access the private constructor.

			public:
				bool post(RunnableTypePtr);
				bool postDelayed(RunnableTypePtr, uint64_t timeoutMilliseconds = 0);

			private:
				inline Handler(LooperType& l)
					: m_assignedLooper(l)
				{}

				inline void storeDelayedPostFuture(std::future<bool>& f) {
					std::lock_guard<std::recursive_mutex> guard(m_delayedPostFuturesMutex);
					m_delayedPostFutures.push_back(std::move(f));
				}

				inline bool is_ready(std::future<bool> const& f)
				{
					return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
				}

				inline void checkDelayedPostFutures() {
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
						 it ++) {
						if( is_ready(*it) )
							readyFutures.push_back(it);
					}

					for( f_it& readyIt : readyFutures )
						m_delayedPostFutures.erase(readyIt);
				}

				LooperType& m_assignedLooper;

				std::recursive_mutex           m_delayedPostFuturesMutex;
				std::vector<std::future<bool>> m_delayedPostFutures;
			};
			/**************************************************************************************************//**/

			Looper();
			~Looper() = default;

			// Loopers, like threads, may not be copied or moved.
			Looper(const LooperType&)                = delete;
			Looper(LooperType&&)                     = delete;
			LooperType& operator=(const LooperType&) = delete;
			LooperType& operator=(LooperType&&)      = delete;

			virtual bool initialize();
			virtual bool cleanup();

			bool run();
			bool running();
			bool abortAndJoin(uint64_t timeoutMilliseconds = 10000);

			inline Handler& getHandler() { return m_handler; }

		protected:
			inline void requestAbort()         { m_abortRequested.store(true);   }
			inline bool abortRequested() const { return m_abortRequested.load(); }

			RunnableTypePtr nextRunnable();

		private:
			void runFunc();

			bool post(RunnableTypePtr);

			std::thread      m_thread;
			std::atomic_bool m_running;
			std::atomic_bool m_abortRequested;

			Handler m_handler;

			std::recursive_mutex m_runnablesMutex;
			std::vector<typename ILooper<TRunParameters...>::IRunnablePtr> m_runnables;
		};

		/**********************************************************************************************//**
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...>::Handler ::post(RunnableTypePtr runnable)
		 *
		 * \brief	Post this message
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 * \param	runnable	The runnable.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;::Handler.
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>::Handler
			::post(RunnableTypePtr runnable) 
		{
			checkDelayedPostFutures();
			return m_assignedLooper.post(runnable);
		}

		/**********************************************************************************************//**
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...>::Handler ::postDelayed(RunnableTypePtr runnable, uint64_t timeoutMilliseconds)
		 *
		 * \brief	Posts a delayed
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 * \param	runnable		   	The runnable.
		 * \param	timeoutMilliseconds	The timeout in milliseconds.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;::Handler.
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>::Handler
			::postDelayed(RunnableTypePtr runnable, uint64_t timeoutMilliseconds)
		{
			std::function<bool()> fn = [=]() -> bool 
			{   
				std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMilliseconds));
			    return m_assignedLooper.post(runnable); 
			};

			try {
				std::future<bool> f = std::async(std::launch::async, fn);
				storeDelayedPostFuture(std::move(f));
				checkDelayedPostFutures();
			}
			catch( std::bad_alloc ba ) {
				return false;
			}
			catch( std::system_error se ) {
				return false;
			}
			catch( ... ) {
				return false;
			}

			return true;
		}

		
		/**********************************************************************************************//**
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...>::Handler ::post(RunnableTypePtr runnable)
		 *
		 * \brief	Post this message
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 * \param	runnable	The runnable.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;::Handler.
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
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
		 * \fn	template <typename... TRunParameters> Looper<TRunParameters...> ::Looper() : ILooper<TRunParameters...>() , m_running(false) , m_abortRequested(false)
		 *
		 * \brief	Gets the looper
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		template <typename... TRunParameters>
		Looper<TRunParameters...>
			::Looper()
			: ILooper<TRunParameters...>()
			, m_running(false)
			, m_abortRequested(false)
			, m_handler(*this)
		{ }

		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
			::initialize()
		{
			return true;
		}

		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
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
		 * \fn	template <typename... TRunParameters> typename Looper<TRunParameters...>::RunnableTypePtr Looper<TRunParameters...> ::nextRunnable()
		 *
		 * \brief	Next runnable
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		template <typename... TRunParameters>
		typename Looper<TRunParameters...>::RunnableTypePtr 
			Looper<TRunParameters...>
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

		template <typename... TRunParameters>
		void Looper<TRunParameters...>
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
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...> ::run()
		 *
		 * \brief	Gets the run
		 *
		 * \exception	std::exception	Thrown when an exception error condition occurs.
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
			::run()
		{
			try {
				m_thread = std::thread(&Looper<TRunParameters...>::runFunc, this);
			}
			catch( ... ) {
				Log::Error(logTag(), "Failed to start thread.");
				return false;
			}

			return true;
		}

		/**********************************************************************************************//**
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...> ::running()
		 *
		 * \brief	Gets the running
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
			::running()
		{
			return m_running.load();
		}

		/**********************************************************************************************//**
		 * \fn	template <typename... TRunParameters> bool Looper<TRunParameters...> ::abortAndJoin(uint64_t timeoutMilliseconds)
		 *
		 * \brief	Abort and join
		 *
		 * \tparam	TRunParameters	Type of the run parameters.
		 * \param	timeoutMilliseconds	The timeout in milliseconds.
		 *
		 * \return	A Looper&lt;TRunParameters...&gt;
		 **************************************************************************************************/
		template <typename... TRunParameters>
		bool Looper<TRunParameters...>
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

#endif