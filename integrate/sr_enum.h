/*!
 * @file      sr_enum.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      15/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <utility>
#include "core/base/defines/sr_defines.h"

/*!
 * The CEnum class contains several helpers to deal with C++11 and up enum classes.
 */
class CEnum
{
public_static_functions:
    /*!
     * Convenience typedef around std::underlying_type_t.
     */
    template <typename TEnumType>
    using UnderlyingType_t = std::underlying_type_t<TEnumType>;

    /*!
     * Return the underlying value of a specific value of a provided TEnum enum class.
     *
     * @param  [in] aEnumValue A value of TEnum to be converted.
     * @return                 Returns the underlying value of the provided enum value.
     */
    template <typename TEnumType>
    static UnderlyingType_t<TEnumType> getValue(const TEnumType& aEnumValue)
    {
        const UnderlyingType_t<TEnumType> value = static_cast<UnderlyingType_t<TEnumType>>(aEnumValue);
        return value;
    }
};
