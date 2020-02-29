#ifndef m_IOCProperty_H_
#define m_IOCProperty_H_

#include <functional>
#include <memory>
#include <vector>
#include <algorithm>

#include <base/declaration.h>

#include "core/patterns/observer.h"

namespace engine
{
    /**
     * @brief A property implementation providing getters, setters and assignment operators.
     *        If the value changes, a propertyChanged-event is raised providing the old and new value to
     *        an arbitrary number of listeners.
     */
    template <typename T>
    class IOCPropertyBase
    {
    public_typedefs:
        using Subject_t     = CSubject<T const, T const>;
        using ObserverPtr_t = typename Subject_t::ObserverPtr_t;
        using FilterFn_t    = std::function<T(T const&)>;
        using PostSetFn_t   = std::function<void(T const&)>;

    public_constructors:
        /**
         * @brief IOCPropertyBase
         * @param aPropertyName
         */
        IOCPropertyBase(std::string const &aPropertyName)
            : mName(aPropertyName)
            , mFilterFn(nullptr)
            , mPostSetFn(nullptr)
        {}

        /**
         * @brief IOCPropertyBase
         * @param aPropertyName
         * @param aInitialValue
         */
        IOCPropertyBase(
                std::string const &aPropertyName,
                T           const &aInitialValue)
            : mName(aPropertyName)
            , mValue(aInitialValue)
            , mFilterFn(nullptr)
            , mPostSetFn(nullptr)
        {}

    public_destructors:
        /**
         * @brief ~IOCPropertyBase
         */
        virtual ~IOCPropertyBase() = default;

        /**
         * @brief setFilterFunc
         * @param aFilter
         */
        SHIRABE_INLINE void setFilterFunc(std::function<T(const T&)> aFilter)
        {
            mFilterFn = aFilter;
        }

        /**
         * @brief setPostSetFunc
         * @param aCallback
         */
        SHIRABE_INLINE void setPostSetFunc(std::function<void(const T&)> aCallback)
        {
            mPostSetFn = aCallback;
        }

        /**
         * @brief setNoNotify
         * @param aValue
         */
        void setNoNotify(T const &aValue)
        {
            set(aValue, false);
        }

        /**
         * @brief set
         * @param aValue
         * @param aNotify
         * @param aNotifyOnChangeOnly
         */
        void set(T    const &aValue,
                 bool        aNotify             = true,
                 bool        aNotifyOnChangeOnly = true)
        {
            T oldValue = mValue;
            T newValue = aValue;
            if(mFilterFn)
                newValue = mFilterFn(aValue);

            mValue = newValue;

            bool const valueChanged = (oldValue != newValue);
            bool const doNotify     = aNotify && (!aNotifyOnChangeOnly || valueChanged);

            if(doNotify)
                mSubject.notify(newValue, oldValue);

            if(mPostSetFn)
                mPostSetFn(mValue);
        }

        /**
         * @brief getResource
         * @return
         */
        T const &get()
        {
            return mValue;
        }

        /**
         * @brief getResource
         * @return
         */
        T const &get() const
        {
            return mValue;
        }

        /**
         * @brief operator ()
         * @return
         */
        T const &operator()()
        {
            return get();
        }

        /**
         * @brief operator ()
         * @return
         */
        T const &operator()() const
        {
            return get();
        }

        /**
         * @brief operator ->
         * @return
         */
        T &operator->()
        {
            return get();
        }

        /**
         * @brief operator ->
         * @return
         */
        T const &operator->() const
        {
            return get();
        }

        /**
         * @brief listen
         * @param aObserver
         * @return
         */
        bool listen(ObserverPtr_t aObserver)
        {
            if(!aObserver)
                return false;

            bool const observed = mSubject.observe(aObserver);
            return observed;
        }

        /**
         * @brief ignore
         * @param aObserver
         * @return
         */
        bool ignore(ObserverPtr_t aObserver)
        {
            if(!aObserver)
                return false;

            bool const ignored = mSubject.ignore(aObserver);
            return ignored;
        }

    private_members:
        std::string                   mName;
        T                             mValue;
        std::function<T(const T&)>    mFilterFn;
        std::function<void(const T&)> mPostSetFn;
        CSubject<T const, T const>    mSubject;
    };

    /**
     * @brief Partially specialized IOCProperty-class for Non-arithmetic types.
     */
    template<typename T, class enable = void>
    class IOCProperty: public IOCPropertyBase < T >
    {
    public_constructors:
        /**
         * @brief IOCProperty
         * @param propertyName
         */
        IOCProperty(std::string const &aPropertyName)
            : IOCPropertyBase<T>(aPropertyName)
        {}

        /**
         * @brief IOCProperty
         * @param propertyName
         * @param value
         */
        IOCProperty(
                std::string const &aPropertyName,
                T           const &aValue)
            : IOCPropertyBase<T>(aPropertyName, aValue)
        {}

    public_operators:
        /**
         * @brief operator =
         * @param pValue
         * @return
         */
        IOCProperty& operator=(T const &aValue)
        {
            IOCPropertyBase<T>::set(aValue);
            return *this;
        }
    };

    /**
     * @brief Partially specialized IOCProperty-class for arithmetic types.
     *        Arithmetic assignment operators are provided for these kinds of model properties!
     */
    template<typename T>
    class IOCProperty<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
            : public IOCPropertyBase < T >
    {
    public_constructors:
        /**
         * @brief IOCProperty
         * @param aPropertyName
         */
        IOCProperty(std::string const &aPropertyName)
            : IOCPropertyBase<T>(aPropertyName)
        {}

        /**
         * @brief IOCProperty
         * @param aPropertyName
         * @param value
         */
        IOCProperty(
                std::string const &aPropertyName,
                T           const &aValue)
            : IOCPropertyBase<T>(aPropertyName, aValue)
        {}

    public_operators:
        /**
         * @brief operator =
         * @param pValue
         * @return
         */
        IOCProperty &operator=(T const &aValue)
        {
            IOCPropertyBase<T>::set(aValue, true);
            return *this;
        }

        using IOCPropertyBase<T>::operator->;
        using IOCPropertyBase<T>::operator ();

        /**
         * @brief operator +=
         * @param val
         * @return
         */
        IOCProperty& operator+=(T const &aValue)
        {
            return operator=(this->get() + aValue);
        }

        /**
         * @brief operator -=
         * @param aValue
         * @return
         */
        IOCProperty& operator-=(T const &aValue)
        {
            return operator=(this->get() - aValue);
        }

        /**
         * @brief operator *=
         * @param aValue
         * @return
         */
        IOCProperty& operator*=(T const &aValue)
        {
            return operator=(this->get() * aValue);
        }

        /**
         * @brief operator /=
         * @param aValue
         * @return
         */
        IOCProperty& operator/=(T const &aValue)
        {
            return operator=(this->get() / aValue);
        }
    };
}

#endif
