/*!
 * @file      sr_observer.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      11/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_PATTERNS_OBSERVER_H__
#define __SHIRABE_PATTERNS_OBSERVER_H__

#include <list>
#include <functional>
#include <base/declaration.h>
#include "core/enginetypehelper.h"

namespace engine
{

    /*!
     * The IObserver<T> interface provides the single callback signature to be notified of value change
     * observed subject.
     *
     * @tparam T Type of the data value whose change is being observed.
     */
    template <typename... TArgs>
    class IObserver
    {
        SHIRABE_DECLARE_INTERFACE(IObserver)

    public_api:
        virtual void onValueChanged(TArgs &&... aArgs) = 0;
    };

    /*!
     * The CSubject<T> class provides functionality to "observe" a specific value.
     * This class does not contain the value itself, but must be instantiated and invoked
     * by the instance containing the value.
     *
     * @tparam T Type of the subject/value to be indirectly observed.
     */
    template <typename... TArgs>
    class CSubject
    {
    public_typedefs:
        /*!
         * Type-Alias to declare a valid observer instance as a shared pointer to IObserver<T>.
         */
        using ObserverPtr_t = CStdSharedPtr_t<IObserver<TArgs...>>;

    public_methods:
        bool observe(ObserverPtr_t aObserver);
        bool ignore(ObserverPtr_t aObserver);
        bool notify(TArgs &&... aArgs) const;

    private_members:
        std::list<ObserverPtr_t> mObservers;
    };

    /*!
     * Register a new observer instance inside the internal collection, making it available for notify()-calls.
     *
     * @param aObserver The shared pointer to an observer instance to be registered.
     * @returns         True, if successfully added. False if already registered or nullptr.
     */
    template <typename... TArgs>
    bool CSubject<TArgs...>::observe(ObserverPtr_t aObserver)
    {
        bool const isNull = (nullptr == aObserver);
        SHIRABE_RETURN_IF(isNull, false);

        bool const contained = std::find(mObservers.begin(), mObservers.end(), aObserver) != mObservers.end();
        SHIRABE_RETURN_IF(contained, false);

        mObservers.push_back(aObserver);
        return true;
    }

    /*!
     * Register a new observer instance inside the internal collection, making it available for notify()-calls.
     *
     * @param aObserver The shared pointer to an observer instance to be registered.
     * @returns         True, if successfully added. False if not registered or nullptr.
     */
    template <typename... TArgs>
    bool CSubject<TArgs...>::ignore(ObserverPtr_t aObserver)
    {
        bool const isNull = (nullptr == aObserver);
        SHIRABE_RETURN_IF(isNull, false);

        typename std::list<ObserverPtr_t>::iterator it = std::find(mObservers.begin(), mObservers.end(), aObserver);
        bool const contained = (it != mObservers.end());
        SHIRABE_RETURN_IF(!contained, false);

        mObservers.erase(it);
        return true;
    }

    /*!
     * Loop through the current collection of observers and invoke 'onValueChanged(T const&)' on each of them.
     *
     * @param aNewValue The new value to be notified about.
     * @returns         True, if successfully notified. False otherwise.
     */
    template <typename... TArgs>
    bool CSubject<TArgs...>::notify(TArgs &&... aArgs) const
    {
        for(ObserverPtr_t observer : mObservers)
        {
            observer->onValueChanged(std::forward<TArgs>(aArgs)...);
        }

        return true;
    }

    /*!
     * The CObserver<T>-class provides reusable functionality to observe a value implementing IObserver<T> and
     * allowing to register a handler function to be invoked on value change.
     * The handler can be a function pointer, pointer to method (std::bind), an std::function or labda.
     *
     * @tparam T The datatype of the value to be observed indirectly.
     */
    template <typename TRefObject, typename... TArgs>
    class CObserver
        : public IObserver<TArgs...>
    {
    public_typedefs:
        /*!
         * Type-Alias to defined the handler-function signature.
         */
        using HandlerFunction_t = std::function<void(TRefObject const &, TArgs &&...)>;

    public_constructors:
        SHIRABE_INLINE CObserver(TRefObject const &aRefObject)
            : mRefObject(aRefObject)
        {}

    public_destructors:
        virtual ~CObserver() = default;

    public_methods:
        /*!
         * Register a handler function in the observer instance to be invoked on value change.
         *
         * @param aHandlerFn Handler-Function with signature void(T const&).
         *                   May be nullptr, to deactivate change-handling.
         */
        SHIRABE_INLINE void setHandlerFn(HandlerFunction_t const&aHandlerFn)
        {
            mHandlerFn = aHandlerFn;
        }

        /*!
         * Implementation of IObserver<T>. Invoked, when the observed value changes.
         *
         * @param aNewValue The new value of the observed storage.
         */
        SHIRABE_INLINE void onValueChanged(TArgs &&... aArgs) final
        {
            if(mHandlerFn)
            {
                mHandlerFn(mRefObject, std::forward<TArgs>(aArgs)...);
            }
        }

    private_members:
        TRefObject        const &mRefObject;
        HandlerFunction_t        mHandlerFn;
    };

}

#endif
