#ifndef m_IOCProperty_H_
#define m_IOCProperty_H_

#include <functional>
#include <memory>
#include <vector>
#include <algorithm>

#include "compiler.hpp"

#include "ValueChangedArguments.hpp"
#include "FunctorPropertyChangeListener.hpp"

namespace DataBinding {

    // Dummy class to enable storage of arbitrary derived classes abusing polymorphism.
    class CallerBase {};


// Important: Variadic template arguments are not supported before MSVC2013 or GCC 4.3/4.4
#if ( (defined(__GNUG__ /* = __GNUC__ && __cplusplus */) && CHECK_GCC_VERSION_AT_LEAST(40300)) \
      || (defined(_MSC_VER) && (_MSC_VER >= 1800))                                             \
    )

    template<typename T, typename R, typename... Args>
    /**
     * @brief bind a member function of type T with a variable number of arguments and a specific instance of
     *        type T together as a callable std::function.
     * @param instance
     * @return
     */
    std::function<R(Args...)> bind(T& instance, R(T::*target)(Args...)) {
        return[&instance, target](Args... args) mutable -> R{ return (instance.*target)(args...);  };
    }

    template <typename R, typename... Args>
    /**
     * @brief The Caller class encapsulates a specific std::function object and makes it
     *        callable reusably with variable arguments provided to "call".
     */
    class Caller : public CallerBase {
            std::function<R(Args...)> function;
        public:
            Caller(std::function<R(Args...)> func)
                : CallerBase(),
                  function(func)
            {}

            /**
             * @brief call the function stored in the std::function with the provided params.
             * @param params
             * @return
             */
            R call(Args... params) {
                return function(std::forward<Args>(params)...);
            }
    };

#else

    // If no variadic templates are supported due to compiler limitations, use the same components as above for no and one parameter,
    // which is currently sufficient!
    // If more parameters must be supported for whatever reasons, copy the below bind-function and Caller-class and extend it by
    // additional parameters.

    template<typename T, typename R>
    /**
     * @brief bind a member function of type T with no arguments and a specific instance of
     *        type T together as a callable std::function.
     * @param instance
     * @return
     */
    std::function<R()> bind(T &instance, R(T::*target)()) {
        return[&instance, target]() mutable -> R{ return (instance.*target)(); };
    }

    template<typename T, typename R, typename TParam0>
    /**
     * @brief bind a member function of type T with one argument and a specific instance of
     *        type T together as a callable std::function.
     * @param instance
     * @return
     */
    std::function<R(TParam0)> bind(T& instance, R(T::*target)(TParam0)) {
        return[&instance, target](TParam0 param) mutable -> R{return (instance.*target)(param); };
    }

    template <typename R>
    /**
     * @brief The Caller class encapsulates a specific std::function object and makes it
     *        callable reusably with no arguments provided to "call".
     */
    class Caller : public CallerBase {
            std::function<R()> function;
        public:
            Caller(std::function<R()> func) : CallerBase(), function(func)
            {}

            R call() {
                return function();
            }
    };

    template <typename R, typename TParam0>
    /**
     * @brief The Caller class encapsulates a specific std::function object and makes it
     *        callable reusably with one argument provided to "call".
     */
    class CallerP0 : public CallerBase {
            std::function<R(TParam0)> function;
        public:
            CallerP0(std::function<R (TParam0)> func) : CallerBase(), function(func)
            {}

            R call(TParam0 param) {
                return function(param);
            }
    };

#endif

    // Helper makros to generate signatures for differently-scoped IOC-Properties
#define L(argT)   []    (const char* pPropertyName, const IOCProperty< ## argT ##>::value_changed_argument_type& arg) mutable -> void
#define L_R(argT) [=]   (const char* pPropertyName, const IOCProperty< ## argT ##>::value_changed_argument_type& arg) mutable -> void
#define L_T(argT) [this](const char* pPropertyName, const IOCProperty< ## argT ##>::value_changed_argument_type& arg) mutable -> void


    template<typename TCollection, typename TCollectionIterator>
    /**
     * @brief Helper function to apply a function on each element of a collection.
     * @param collection
     * @param itFunc
     */
    void iterate(
            const TCollection& collection,
            const std::function<void(TCollectionIterator& it)>& itFunc) {
        if(!itFunc)
            return;

        for(TCollectionIterator it = collection.begin(); it != collection.end(); ++it) {
            itFunc(it);
        }
    }

    template <typename T>
    /**
     * @brief A property implementation providing getters, setters and assignment operators.
     *        If the value changes, a propertyChanged-event is raised providing the old and new value to
     *        an arbitrary number of listeners.
     */
    class IOCPropertyBase {

        public:
            typedef IPropertyChangeListener<T>         listener_type;
            typedef listener_type*                     listener_type_pointer;
            typedef ValueChangedArguments<T>           value_changed_argument_type;
            typedef const value_changed_argument_type& const_value_changed_argument_type_ref;
            typedef std::vector<listener_type *>       vector_type;
            typedef std::function<void(const char *, const_value_changed_argument_type_ref)> function_type;
            typedef typename listener_type::callback_func_type  callback_func;
            typedef void(*func_ptr_type)(const char *, const_value_changed_argument_type_ref);

            IOCPropertyBase(const char *propertyName)
                : m_name(propertyName),
                  m_listeners(),
                  m_implicitlyCreatedListeners(),
                  m_filterFunc(nullptr),
                  m_postSetFunc(nullptr) {
            }

            IOCPropertyBase(const char *propertyName, const T& initialValue)
                : m_name(propertyName),
                  m_value(initialValue),
                  m_listeners(),
                  m_implicitlyCreatedListeners(),
                  m_filterFunc(nullptr),
                  m_postSetFunc(nullptr) {
            }

            // Destroy this IOCProperty instance
            // @Remarks:
            //			Since a property can create internal listeners, when a function callback
            //			or Lambda is provided as a listener-callback, they'll be deleted here finally.
            virtual ~IOCPropertyBase() {
                m_listeners.clear();

                iterate<vector_type, typename vector_type::const_iterator>(
                            m_implicitlyCreatedListeners,
                            [this] (typename vector_type::const_iterator it) -> void {
                    listener_type_pointer p = (*it);
                    delete p;
                });

                m_implicitlyCreatedListeners.clear();
            }

            inline void setFilterFunc(std::function<T(const T&)> filter) {
                m_filterFunc = filter;
            }

            inline void setPostSetFunc(std::function<void(const T&)> callback) {
                m_postSetFunc = callback;
            }

            void setNoNotify(const T value) { set(value, false); }

            // Assign a new value to the property and notify all listeners.
            void set(const T value,
                     bool    notify             = true,
                     bool    notifyOnChangeOnly = true) {

                T oldValue = m_value;
                T newValue = value;
                if(_filterFunc)
                    newValue = m_filterFunc(value);

                m_value = newValue;
                if(notify
                        && (!notifyOnChangeOnly
                            || (notifyOnChangeOnly && (oldValue != newValue))))
                    propertyChanged(_name, oldValue, m_value);

                if(_postSetFunc) {
                    m_postSetFunc(_value);
                }
            }

            T get() {
                return m_value;
            }

            const T get() const {
                return m_value;
            }

            T operator()() {
                return get();
            }

            const T operator()() const {
                return get();
            }

            T operator->() {
                return get();
            }

            const T operator->() const {
                return get();
            }



            //bool addListener(func_ptr_type funcPtr) {
            //	IPropertyChangeListener *listener = new FunctorPropertyChangeListener<func_ptr_type>(funcPtr);
            //	m_internallyCreatedListeners.push_back(listener);
            //	return addListener(listener);
            //};

            bool listen(function_type func) {
                listener_type_pointer listener = new FunctorPropertyChangeListener<T, function_type>(func);
                m_implicitlyCreatedListeners.push_back(listener);
                return listen(listener);
            }

            // Add a listener object derived from IPropertyChangeListener and register it for value change notifications.
            // @Param 'listener':
            //					The listener to be added.
            // @Returns
            //			True if the listener was successfully added.
            //			False if the listener is null or already added.
            bool listen(listener_type_pointer listener) {
                if (listener == nullptr)
                    // May not add nullptr as listener.
                    return false;

                if (std::find(_listeners.begin(), m_listeners.end(), listener) != m_listeners.end())
                    // Listener already added.
                    return false;

                m_listeners.push_back(listener);
                return true;
            }
            // Remove a specific listener object and unregister it from value change notification.
            // @Param 'listener':
            //					The listener to be removed.
            // @Returns
            //			True if the listener was found and successfully removed.
            //			False if no such listener was found.
            bool ignore(listener_type_pointer listener) {
                if (listener == nullptr)
                    // Nullptrs won't be stored anyway.
                    return false;

                typename vector_type::const_iterator it;
                if ((it = std::find(_listeners.begin(), m_listeners.end(), listener)) != m_listeners.end())
                    m_listeners.erase(it);

                return true;
            }

            // Once a property value has changed, this method should be called so that all listeners get notified.
            // @Param 'propertyName':
            //					The name of the underlying property.
            // @Param 'oldValue':
            //					The previous value of the property.
            // @Param 'newValue':
            //					The new value of the property.
            void propertyChanged(const char *propertyName, T oldValue, T newValue) const {
                typename vector_type::const_iterator it;
                for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
                    const value_changed_argument_type args(oldValue, newValue);

                    (*it)->onPropertyChanged(propertyName, args);
                }
            }

        private:
            const char                   *m_name;
            T                             m_value;
            vector_type                   m_listeners;
            vector_type                   m_implicitlyCreatedListeners;
            std::function<T(const T&)>    m_filterFunc;
            std::function<void(const T&)> m_postSetFunc;
    };

    template<typename T, class enable = void>
    /**
     * @brief Partially specialized IOCProperty-class for Non-arithmetic types.
     */
    class IOCProperty : public IOCPropertyBase < T >
    {
        public:
            typedef typename IOCPropertyBase<T>::listener_type   listener_type;
            typedef typename IOCPropertyBase<T>::listener_type_pointer listener_type_pointer;
            typedef typename IOCPropertyBase<T>::value_changed_argument_type value_changed_argument_type;
            typedef typename IOCPropertyBase<T>::const_value_changed_argument_type_ref const_value_changed_argument_type_ref;
            typedef typename IOCPropertyBase<T>::vector_type vector_type;
            typedef typename IOCPropertyBase<T>::function_type function_type;
            typedef typename IOCPropertyBase<T>::callback_func callback_func;

            IOCProperty(const char *propertyName)
                : IOCPropertyBase<T>(propertyName)
            {}

            IOCProperty(const char *propertyName, const T& value)
                : IOCPropertyBase<T>(propertyName, value)
            {}

            // Assign a new value to the property and notify all listeners.
            IOCProperty& operator=(const T pValue) {
                IOCPropertyBase<T>::set(pValue);
                return *this;
            }
    };

    template<typename T>
    /**
     * @brief Partially specialized IOCProperty-class for arithmetic types.
     *        Arithmetic assignment operators are provided for these kinds of model properties!
     */
    class IOCProperty<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
            : public IOCPropertyBase < T >
    {
        public:
            typedef typename IOCPropertyBase<T>::listener_type   listener_type;
            typedef typename IOCPropertyBase<T>::listener_type_pointer listener_type_pointer;
            typedef typename IOCPropertyBase<T>::value_changed_argument_type value_changed_argument_type;
            typedef typename IOCPropertyBase<T>::const_value_changed_argument_type_ref const_value_changed_argument_type_ref;
            typedef typename IOCPropertyBase<T>::vector_type vector_type;
            typedef typename IOCPropertyBase<T>::function_type function_type;
            typedef typename IOCPropertyBase<T>::callback_func callback_func;

            IOCProperty(const char *propertyName)
                : IOCPropertyBase<T>(propertyName) {}

            IOCProperty(const char *propertyName, T value)
                : IOCPropertyBase<T>(propertyName, value)
            {}

            // Assign a new value to the property and notify all listeners.
            IOCProperty& operator=(const T pValue) {
                IOCPropertyBase<T>::set(pValue, true);
                return *this;
            }

            using IOCPropertyBase<T>::operator->;
            using IOCPropertyBase<T>::operator ();

            IOCProperty& operator+=(const T val) {
                return this->operator=(this->get() + val);
            }

            IOCProperty& operator-=(const T val) {
                return this->operator=(this->get() - val);
            }

            IOCProperty& operator*=(const T val) {
                return this->operator=(this->get() * val);
            }

            IOCProperty& operator/=(const T val) {
                return this->operator=(this->get() / val);
            }
    };

}

#endif
