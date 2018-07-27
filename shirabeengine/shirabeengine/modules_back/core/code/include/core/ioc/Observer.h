#ifndef __SHIRABE_OBSERVER_H__
#define __SHIRABE_OBSERVER_H__

#include <memory>
#include <list>
#include <algorithm>
#include <functional>

#include "Core/EngineTypeHelper.h"

namespace Engine {
	namespace IOC {

		/**********************************************************************************************//**
		 * \class	Observer
		 *
		 * \brief	An observer.
		 *
		 * \tparam	TCallbackInterface	Type of the callback interface.
		 **************************************************************************************************/
		template <typename TCallbackInterface>
		class Observer 
			: public TCallbackInterface
		{
			typedef TCallbackInterface my_interface_type;
		public:
			inline Observer()
				: my_interface_type()
			{}

			// Anything?
		};

		/**********************************************************************************************//**
		 * \fn	template <typename TCallbackInterface> DeclareTemplatedSharedPointerType(Observer, Template(Observer<TCallbackInterface>));
		 *
		 * \brief	Constructor
		 *
		 * \tparam	TCallbackInterface	Type of the callback interface.
		 * \param	parameter1	The first parameter.
		 * \param	parameter2	The second parameter.
		 **************************************************************************************************/
		template <typename TCallbackInterface>
		DeclareTemplatedSharedPointerType(Observer, Template(Observer<TCallbackInterface>));

		/**********************************************************************************************//**
		 * \class	Subject
		 *
		 * \brief	A subject.
		 *
		 * \tparam	TCallbackInterface	Type of the callback interface.
		 **************************************************************************************************/
		template <typename TCallbackInterface>
		class Subject
		{
		public:
			using observer_type_ptr = ObserverCStdSharedPtr_t<TCallbackInterface>;
			using observer_list     = std::vector<observer_type_ptr>;

			template <typename... Args>
			using ObserverFwdFn = std::function<void(observer_type_ptr, Args&&...)>;

			inline Subject() 
				: m_observers() {}

			/**********************************************************************************************//**
			 * \fn	bool Subject::observe(TPtr observer)
			 *
			 * \brief	Register an TObserver-instance for notifications.
			 *
			 * \param	observer	The observer.
			 *
			 * \return	True if it succeeds, false if it fails.
			 **************************************************************************************************/
			bool observe(observer_type_ptr observer) {
				typename observer_list::const_iterator it = std::find(_observers.begin(), m_observers.end(), observer);
				if (it != m_observers.end())
					return false;
				else {
					_observers.push_back(observer);
					return true;
				}
			}

			/**
			 * @brief Unregister an TObserver-instance for notifications.
			 * @param observer
			 * @return
			 */
			bool ignore(observer_type_ptr observer) {
				typename observer_list::const_iterator it = std::find(_observers.begin(), m_observers.end(), observer);
				if (it == m_observers.end())
					return false;
				else {
					_observers.remove(observer);
					return true;
				}
			}

			/**
			 * @brief Iterates over all observers of that type and forwards it to a function that calls the appropriate method.
			 * @param fn
			 */
			template <typename... Args>
			void notify(ObserverFwdFn<Args...> fn, Args... args) {
				if (!fn) return;

				for (observer_type_ptr observer : m_observers)
					fn(observer, std::forward<Args>(args)...);
			}

		private:
			observer_list m_observers;
		};

		/**********************************************************************************************//**
		 * \fn	template <typename TCallbackInterface> DeclareTemplatedSharedPointerType(Subject, Template(Subject<TCallbackInterface>));
		 *
		 * \brief	Constructor
		 *
		 * \tparam	TCallbackInterface	Type of the callback interface.
		 * \param	parameter1	The first parameter.
		 * \param	parameter2	The second parameter.
		 **************************************************************************************************/
		template <typename TCallbackInterface>
		DeclareTemplatedSharedPointerType(Subject, Template(Subject<TCallbackInterface>));

	}
}

#endif // OBSERVER_HPP
