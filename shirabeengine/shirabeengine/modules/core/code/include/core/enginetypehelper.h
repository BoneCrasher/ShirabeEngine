/*!
 * @file      enginetypehelper.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_ENGINETYPEHELPER_H__
#define __SHIRABE_ENGINETYPEHELPER_H__

#include <any>
#include <optional>
#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>

#include <base/declaration.h>

#define SHIRABE_STR(m) #m
#define SHIRABE_LOG_FUNCTION(...) SHIRABE_STR( __VA_ARGS__)

namespace engine
{
    /**
     *
     */
    template <typename T>
    using Shared = std::shared_ptr<T>;

    /**
     *
     */
    template <typename T>
    using Unique = std::unique_ptr<T>;

    /**
     *
     */
    template <typename T>
    using Weak = std::weak_ptr<T>;

    /**
     * @brief makeShared
     * @param aArgs
     * @return
     */
    template <typename TUnderlyingType, typename ... TArgs>
    static Shared<TUnderlyingType> makeShared(TArgs&&... aArgs)
    {
        return std::make_shared<TUnderlyingType>(std::forward<TArgs>(aArgs)...);
    }

    template<typename T>
    struct no_delete
    {
        constexpr no_delete() noexcept = default;

        template<typename U, typename = std::_Require<std::is_convertible<U*, T*>>>
        explicit no_delete(no_delete<U> const&) noexcept
        { }

        void operator()(T* aInstance) const
        {
            SHIRABE_UNUSED(aInstance);
            /* No-op */
        }
    };

    template<typename T>
    struct no_delete<T[]>
    {
    public:
        constexpr no_delete() noexcept = default;

        template<typename U, typename = std::_Require<std::is_convertible<U(*)[], T(*)[]>>>
        explicit no_delete(no_delete<U[]> const&) noexcept
        { }

        template<typename U>
        typename std::enable_if<std::is_convertible<U(*)[], T(*)[]>::value>::type
        operator()(U* aArray) const
        {
            SHIRABE_UNUSED(aArray);
            /* No-Op */
        }
    };

    template <typename T>
    using DefaultDelete_t = std::default_delete<T>;

    template <typename T>
    using DefaultDeleteArray_t = std::default_delete<T[]>;

    template <typename T>
    using NoDelete_t = no_delete<T>;

    template <typename T>
    using NoDeleteArray_t = no_delete<T[]>;

    /**
     * @brief makeCStdSharedFromThis
     * @param instance
     * @return
     */
    template <typename T, typename TDeleter, typename TPtr = Shared<T>>
    static inline TPtr makeManagingPtrFromInstance(T* aInstance, TDeleter &&aDeleter = DefaultDelete_t<T>())
    {
        return TPtr(aInstance, aDeleter);
    }

    /**
     * @brief makeCStdSharedFromThis
     * @param instance
     * @return
     */
    template <typename T>
    static inline Shared<T> makeSharedFromInstance(T* aInstance)
    {
        return makeManagingPtrFromInstance<T, Shared<T>>(aInstance);
    }

    /**
     * @brief makeCStdSharedFromThis
     * @param instance
     * @return
     */
    template <typename T>
    static inline Shared<T> makeNonDeletingSharedFromInstance(T* aInstance)
    {
        return makeManagingPtrFromInstance<T, Shared<T>>(aInstance, NoDelete_t<T>());
    }

    /**
     * @brief makeUnique
     * @param aArgs
     * @return
     */
    template <typename TUnderlyingType, typename ... TArgs>
    static Unique<TUnderlyingType> makeUnique(TArgs&&... aArgs)
    {
        return std::make_unique<TUnderlyingType>(std::forward<TArgs>(aArgs)...);
    }

    template <typename T>
    static Unique<T> makeUniqueFromInstance(T *aInstance)
    {
        return makeManagingPtrFromInstance<T, Unique<T>>(aInstance);
    }

    template <typename T>
    static Unique<T> makeNonDeletingUniqueFromInstance(T *aInstance)
    {
        return makeManagingPtrFromInstance<T, Unique<T>>(aInstance, NoDelete_t<T>());
    }

    /**
     *
     */
    template <typename T>
    using Vector = std::vector<T>;

    /**
     *
     */
    template <typename TKey, typename TValue>
    using Map = std::unordered_map<TKey, TValue>;

    #define SHIRABE_DECLARE_LIST_OF_TYPE(type, prefix) \
        using prefix##List = std::vector<type>;

    #define SHIRABE_DECLARE_MAP_OF_TYPES(keytype, valuetype, prefix) \
        using prefix##Map = std::unordered_map<keytype, valuetype>;

    #define SHIRABE_DECLARE_INTERFACE(interface_name)                                \
            public_destructors:                                                      \
                /*!
                 * Declare virtual destructor for correct destruction behaviour
                 * during runtime.
                 */                                                                  \
                virtual ~interface_name() = default;                                 \
                                                                                     \
            private_constructors:                                                    \
                /*!
                 * Delete copy and move constructors to avoid copy/move-construction
                 *
                 * @param aOther The other interface instance to copy/move from.
                 */                                                                  \
                interface_name(interface_name const &aOther) = delete;               \
                /*!
                 * Delete copy and move constructors to avoid copy/move-construction
                 *
                 * @param aOther The other interface instance to copy/move from.
                 */                                                                  \
                interface_name(interface_name      &&aOther) = delete;               \
                                                                                     \
            protected_constructors:                                                  \
                /*!
                 * Default constructor protected, so that no instantiation is
                 * possible, but during runtinme the proper construction behaviour
                 * can be ensured.
                 */                                                                  \
                 interface_name() = default;                                         \
                                                                                     \
            private_operators:                                                       \
                                                                                     \
                /*!
                * Delete copy and move assignment to avoid copying/moving around
                * instances of this interface.
                *
                * @param   aOther
                * @returns Hopefully nothing. :P
                */                                                                   \
                interface_name& operator=(interface_name const &aOther) = delete;    \
                /*!
                * Delete copy and move assignment to avoid copying/moving around
                * instances of this interface.
                *
                * @param   aOther
                * @returns Hopefully nothing. :P
                */                                                                   \
                interface_name& operator=(interface_name      &&aOther) = delete;

    /**
     *
     */
    using Any_t = std::any;

    /**
     *
     */
    template <typename T>
    using Optional_t = std::optional<T>;

    /**
     *
     */
    template <typename T>
    using RefWrapper_t = std::reference_wrapper<T>;

    template <typename T>
    using OptionalRef_t = Optional_t<RefWrapper_t<T>>;

    /**
    * @brief bind a member function of type T with a variable number of arguments and a specific instance of
    *        type T together as a callable std::function.
    * @param instance
    * @return
    */
    template <typename    T,         // Instance type containing a member function
              typename    TFnReturn, // Member function return-type
              typename... TFnParams> // Member function parameters
    static std::function<TFnReturn(TFnParams...)> bindMethod(
        T &instance,
        TFnReturn(T::*fn)(TFnParams...))
    {
        auto const function = [&instance, fn](TFnParams... args) mutable -> TFnReturn
        {
            return (instance.*fn)(args...);
        };

        return function;
    }

}

#endif
