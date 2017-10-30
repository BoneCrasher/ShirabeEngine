#ifndef __SHIRABE_THREADING_LOOPER_H__
#define __SHIRABE_THREADING_LOOPER_H__

#include <type_traits>
#include <thread>
#include <future>

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

namespace Engine {
	namespace Threading {
				
		/**********************************************************************************************//**
		 * \fn	DeclareInterface(ILooper);
		 *
		 * \brief	Constructor
		 *
		 * \param	parameter1	The first parameter.
		 **************************************************************************************************/
	    template <typename TTaskResult>
		DeclareInterface(ILooper);
		public:
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

			class Task {
			public:
			private:
				std::packaged_task<TTaskResult> m_task;
			};
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
			virtual bool loop(const typename ILooper::TaskPtr& runnable) = 0;
	   
		DeclareInterfaceEnd(ILooper);
		DeclareSharedPointerType(ILooper);

		/**********************************************************************************************//**
		 * \class	Looper
		 *
		 * \brief	A looper.
		 *
		 * \tparam	TDerivedRunnableType	Type of the derived runnable type used for this specific looper.
		 **************************************************************************************************/
		class Looper
			: public ILooper
		{
			DeclareLogTag(Looper);

		public:
			using LooperType      = Looper;
			using RunnableTypePtr = Ptr<ITask>;

			/**********************************************************************************************//**
			 * \class	Handler
			 *
			 * \brief	A handler.
			 **************************************************************************************************/
			class Handler {
				friend class Looper; // Allow the looper to access the private constructor.

			public:
				bool post(RunnableTypePtr);
				bool postDelayed(RunnableTypePtr, uint64_t timeoutMilliseconds = 0);

			private:
				inline Handler(LooperType& l)
					: m_assignedLooper(l)
				{}

				void storeDelayedPostFuture(std::future<bool>& f);

				bool is_ready(std::future<bool> const& f);

				void checkDelayedPostFutures();

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

			std::recursive_mutex               m_runnablesMutex;
			std::vector<ILooper::TaskPtr> m_runnables;
		};
	}
}

#endif