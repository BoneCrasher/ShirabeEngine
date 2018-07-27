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

#include "base/declaration.h"

#define SHIRABE_STR(m) #m
#define SHIRABE_LOG_FUNCTION(...) SHIRABE_STR( __VA_ARGS__)

namespace Engine
{
    /**
     *
     */
    template <typename T>
    using CStdSharedPtr_t = std::shared_ptr<T>;

    /**
     *
     */
    template <typename T>
    using CStdUniquePtr_t = std::unique_ptr<T>;

    /**
     * @brief makeCStdSharedPtr
     * @param aArgs
     * @return
     */
    template <typename TUnderlyingType, typename ... TArgs>
    static CStdSharedPtr_t<TUnderlyingType> makeCStdSharedPtr(TArgs&&... aArgs)
    {
        return std::make_shared<TUnderlyingType>(std::forward<TArgs>(aArgs)...);
    }

    /**
     *
     */
    template <typename T>
    using CStdSharedPtrDeleterFn_t = std::function<void(T*)>;

    /**
     * @brief makeCStdSharedPtrCustomDeleter
     * @param aInstance
     * @param aDeleter
     * @return
     */
    template <typename T, typename TDeleter>
    static CStdSharedPtr_t<T> makeCStdSharedPtrCustomDeleter(
            T          *aInstance,
            TDeleter    aDeleter)
    {
        return CStdSharedPtr_t<T>(aInstance, aDeleter);
    }

    template <typename T, typename TPtr = CStdSharedPtr_t<T>>
    static inline TPtr makeCStdSharedFromThis(T* instance) {
        return TPtr(instance, [](T*) -> void {; /* Do not delete */ });
    }

    /**
     * @brief makeCStdUniquePtr
     * @param aArgs
     * @return
     */
    template <typename TUnderlyingType, typename ... TArgs>
    static CStdUniquePtr_t<TUnderlyingType> makeCStdUniquePtr(TArgs&&... aArgs)
    {
        return std::make_unique<TUnderlyingType>(std::forward<TArgs>(aArgs)...);
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
    using Any = std::any;

    /**
     *
     */
    template <typename T>
    using Optional = std::optional<T>;

    /**
     *
     */
    template <typename T>
    using RefWrapper = std::reference_wrapper<T>;

}

#endif
