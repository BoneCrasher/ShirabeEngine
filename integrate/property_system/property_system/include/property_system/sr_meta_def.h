/*!
 * @file      sr_meta_def.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      23/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <string>
#include <core/base/types/sr_map.h>

/*!
 * Forward declaration of CMetaObject for several dependent
 * components to avoid circular include dependencies.
 */
class CMetaObject;

/*!
 * Forward declaration of CMetaPrototype<T> for several dependent
 * components to avoid circular include dependencies.
 */
template <typename TClassType>
class CMetaPrototype;

/*!
 * Forward declaration of CMetaProperty<T> for several dependent
 * components to avoid circular include dependencies.
 *
 * @tparam TType       The underlying property value type.
 * @tparam TEnableFlag SFINAE-switch to select proper specialization.
 */
template <typename TType, typename TEnableFlag = void>
class CMetaProperty;

/*!
 * Generic UID type used by default for all UIDs in the system
 * unless another type is required.
 */
using GenericUID_t  = uint16_t;
/*!
 * UID-type for all CMetaObject instances and CMetaPrototype<T>-instances.
 */
using InstanceUID_t = GenericUID_t;

/*!
 * UID-type for all CMetaProperty<T> instances.
 */
using PropertyUID_t = GenericUID_t;

/*!
 * List of supported property types.
 * Declare any additional type here.
 */
#define SR_SUPPORTED_PROPERTY_TYPES               \
            int8_t, int16_t, int32_t, int64_t,    \
            uint8_t, uint16_t, uint32_t, uint64_t,\
            float, double,                        \
            std::string, std::wstring,            \
            CStdSharedPtr_t<CMetaObject>

/*!
 * Apply the function macro fn(param) for each supported property type.
 * If another property type will be supported, add it in the list below.
 */
#define SR_APPLY_FOREACH_PROPERTY_TYPE(_aFn)   \
            _aFn(int8_t)                       \
            _aFn(int16_t)                      \
            _aFn(int32_t)                      \
            _aFn(int64_t)                      \
            _aFn(uint8_t)                      \
            _aFn(uint16_t)                     \
            _aFn(uint32_t)                     \
            _aFn(uint64_t)                     \
            _aFn(float)                        \
            _aFn(double)                       \
            _aFn(std::string)                  \
            _aFn(std::wstring)                 \
            _aFn(CStdSharedPtr_t<CMetaObject>)

/*!
 * Expand a std::visit resolver resolver_id(param) for each supported property type
 * and invoke std::visit using the resolver on the variant provided.
 * If another property type will be supported, add it in the list below.
 */
#define SR_RESOLVE(_aResolverId, _aVariant, ...)                                         \
          {                                                                              \
              auto const __variantResolver =                                             \
                SVariantResolver                                                         \
              {                                                                          \
                  _aResolverId<int8_t>                      ::getResolver(__VA_ARGS__),  \
                  _aResolverId<int16_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<int32_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<int64_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<uint8_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<uint16_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<uint32_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<uint64_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<float>                       ::getResolver(__VA_ARGS__),  \
                  _aResolverId<double>                      ::getResolver(__VA_ARGS__),  \
                  _aResolverId<std::string>                 ::getResolver(__VA_ARGS__),  \
                  _aResolverId<std::wstring>                ::getResolver(__VA_ARGS__),  \
                  _aResolverId<CStdSharedPtr_t<CMetaObject>>::getResolver(__VA_ARGS__),  \
              };                                                                         \
              std::visit(__variantResolver, _aVariant);                                  \
          }

#define SR_RESOLVE_CONVERTER(_aResolverId, _aTargetType, _aVariant, ...)                               \
          {                                                                                            \
              auto const __variantResolver =                                                           \
                SVariantResolver                                                                       \
              {                                                                                        \
                  _aResolverId<_aTargetType, int8_t>                      ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, int16_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, int32_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, int64_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, uint8_t>                     ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, uint16_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, uint32_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, uint64_t>                    ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, float>                       ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, double>                      ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, std::string>                 ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, std::wstring>                ::getResolver(__VA_ARGS__),  \
                  _aResolverId<_aTargetType, CStdSharedPtr_t<CMetaObject>>::getResolver(__VA_ARGS__),  \
              };                                                                                       \
              std::visit(__variantResolver, _aVariant);                                                \
          }
