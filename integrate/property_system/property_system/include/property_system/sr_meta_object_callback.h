/*!
 * @file      sr_meta_object_callback.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      11/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <string>
#include <core/base/defines/sr_defines.h>
#include "property_system/sr_property_system_error.h"

/*!
 * The IMetaObjectPropertyAdditionCallback<TData> class provides public signatures
 * to invoke the addition of a property in a CMetaObject-instance based on a
 * 'aPropertyId' and 'aValue'.
 *
 * @tparam TDataType The underlying data-type of the recepient and it's property.
 */
template <typename TDataType>
class IMetaObjectPropertyAdditionCallback
{
    SR_DECLARE_INTERFACE(IMetaObjectPropertyAdditionCallback)

public_api:
    /*!
     * onPropertyAdded Notify the recepient of property addition with id 'aPropertyId'
     * and an initial 'aValue'.
     *
     * @param  [in] aPropertyId The id of the property created.
     * @param  [in] aValue      The initial value of the property created.
     * @return                  True, if the notification was processed successfully.
     *                          False otherwise.
     */
    virtual bool onPropertyAdded(const std::string& aPropertyId, const TDataType& aValue) = 0;
};

/*!
 * The IMetaObjectPropertyRemovalCallback class provides public signatures
 * to invoke the removal of a property in a CMetaObject-instance based on a
 * 'aPropertyId'.
 */
class IMetaObjectPropertyRemovalCallback
{
    SR_DECLARE_INTERFACE(IMetaObjectPropertyRemovalCallback)

public_api:
    /*!
     * Notify the recepient of the removal of a property with 'aPropertyId'.
     *
     * @param [in] aPropertyId The id of the property created.
     * @return                 True, if the notification was processed successfully.
     *                         False otherwise.
     */
    virtual EPropertySystemError onPropertyRemoved(const std::string& aPropertyId) = 0;
};

/*!
 * The IMetaObjectPropertyCallback class implements the
 * IMetaObjectPropertyAdditionCallbacks for a packed set of types as well
 * as the single point of contact IMetaObjectPropertyRemovalCallback.
 *
 * @tparam TTypes... Param-Pack containing a list of property types to declare
 *                   IMetaObjectPropertyAdditionCallbacks for.
 */
template <typename... TTypes>
class IMetaObjectPropertyCallback
        : public IMetaObjectPropertyAdditionCallback<TTypes>...
        , public IMetaObjectPropertyRemovalCallback
{
    SR_DECLARE_INTERFACE(IMetaObjectPropertyCallback)
};
