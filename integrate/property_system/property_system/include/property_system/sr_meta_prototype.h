/*!
 * @file      sr_meta_class.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      25/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <string>
#include <functional>

#include <core/base/defines/sr_defines.h>
#include <core/base/types/sr_map.h>

#include "property_system/sr_meta_object.h"

/*!
 * Helper-function to match a set of input properties against the property set of a prototype.
 * Important: This matching will perform a deep-copy of each property-value (if applicable).
 *
 * @param [in] aPropertyReference The prototypes reference property set to fallback, if no override is provided.
 * @param [in] aPropertyInput     The overrides to be preferred during match.
 * @param [in] aResult            The finally merged result.
 * @return                        True, if successful. False, otherwise.
 */
bool mapProperties(
        const MetaPropertyMap_t& aPropertyReference,
        const MetaPropertyMap_t& aPropertyInput,
        MetaPropertyMap_t&       aResult);


#define SR_DECLARE_PROTOTYPE(_aPrototypeName)                                 \
            public_static_functions:                                          \
                static const std::string& getPrototypeId()                    \
                {                                                             \
                    static const std::string sPrototypeId = #_aPrototypeName; \
                    return sPrototypeId;                                      \
                }

/*!
 * Type erasure base class for all specific prototype specializations
 */
class CMetaPrototypeBase
        : public CMetaObject
{
public_typedefs:
    using PrototypeStaticHandle_t = GenericUID_t;

public_api:
    /*!
     * Create an abstract instance of the prototypes' state as pointer to CMetaObject.
     *
     * @param [in] aInstanceUID            The instance UID of the new instance created.
     * @param [in] aInstanceName           The human readable name of the instance created.
     * @param [in] aInstancePropertyValues Override property values to diverge from the prototype default,
     *                                     where required, e.g. deserialization.
     * @return                             A valid pointer to a CMetaObject instance containing the merged
     *                                     set of prototype properties and overrides.
     * @return                             Nullpointer on error.
     */
    virtual CStdSharedPtr_t<CMetaObject> createAbstractInstance(
            const InstanceUID_t&     aInstanceUID,
            const std::string&       aInstanceName,
            const MetaPropertyMap_t& aInstancePropertyValues = MetaPropertyMap_t()) = 0;

protected_constructors:
    /*!
     * Default construct a CMetaPrototypeBase.
     */
    CMetaPrototypeBase() = default;

    /*!
     * Construct a CMetaPrototypeBase instance and initialize it to 'aClsId'.
     *
     * @param [in] aPrototypeId The unique prototypeId of the CMetaPrototype instance within the CMetaSystem.
     */
    CMetaPrototypeBase(
            const std::string& aPrototypeId);

private_static_functions:
    static InstanceUID_t generatePrototypeUID();
};

/*!
 * The CMetaPrototype class wraps a list of properties and it's parent CMetaPrototype and provides
 * functionality for CRUD-access.
 *
 * @tparam TClassType Class-type, for which a prototype should be defined.
 */
template <typename TClassType>
class CMetaPrototype
        : public CMetaPrototypeBase
{
public_typedefs:
    using CreatorFn_t   =
            std::function<
                TClassType*(
                    CStdSharedPtr_t<CMetaPrototype<TClassType>> /* aInstancePrototype      */,
                    const InstanceUID_t&                        /* aInstanceUID            */,
                    const std::string&                          /* aInstanceName           */,
                    const MetaPropertyMap_t&                    /* aInstancePropertyValues */)>;

public_static_functions:
    /*!
     * Helper function to create a Creator based on the prototype instance class, which will plainly
     * invoke TClass::Create.
     */
    static CreatorFn_t makeDefaultCreator();

public_constructors:
    /*!
     * Default construct a CMetaPrototype instance and initialize it to "InvalidClassId" as
     * a convenience method facilitating storage in Maps.
     */
    CMetaPrototype();

    /*!
     * Construct a CMetaPrototype instance and initialize it to 'aClsId'.
     *
     * @param [in] aPrototypeId The unique prototypeId of the CMetaPrototype instance within the CMetaSystem.
     * @param [in] aCreatorFn   The creator fn, which is responsible for creating a proper instance of TClass.
     *                          Although TClass is known, the effective means of creating and instance is deferred
     *                          to the caller, in case some special stuff has to be performed...
     */
    CMetaPrototype(
            const std::string& aPrototypeId,
            const CreatorFn_t& aCreatorFn);

public_destructors:
    /*!
     * Clear all property descriptors and say goodbye cleanly.
     */
    ~CMetaPrototype();

public_methods:
    void constructPrototype();

    /*!
     * Create an abstract instance of the prototypes' state as pointer to CMetaObject.
     *
     * @param [in] aInstanceUID            The instance UID of the new instance created.
     * @param [in] aInstanceName           The human readable name of the instance created.
     * @param [in] aInstancePropertyValues Override property values to diverge from the prototype default,
     *                                     where required, e.g. deserialization.
     * @return                             A valid pointer to a CMetaObject instance containing the merged
     *                                     set of prototype properties and overrides.
     * @return                             Nullpointer on error.
     */
    CStdSharedPtr_t<CMetaObject> createAbstractInstance(
            const InstanceUID_t&     aInstanceUID,
            const std::string&       aInstanceName,
            const MetaPropertyMap_t& aInstancePropertyValues = MetaPropertyMap_t());

    /*!
     * Create a new instance of type TClass, initialize it and store it's reference.
     *
     * @param [in] aInstanceUID            Unique ID of the instance.
     * @param [in] aInstanceName           Human readable name of the instance.
     * @param [in] aInstancePropertyValues List of properties to store in the instance in addition to the prototype default props.
     * @return                             True, if successful.
     * @remarks                            By default, the instance receives and exact copy of all properties stored in its
     *                                     prototype. Providing properties with propertyIds equal to one or more prototype
     *                                     properties will cause the explicit input to overwrite the prototype property copy
     *                                     in the instance.
     *                                     Property sets in the prototype and instances will currently match, so that no more
     *                                     and no less properties than in the prototype set are available.
     */
    CStdSharedPtr_t<TClassType> createTypedInstance(
            const InstanceUID_t&     aInstanceUID,
            const std::string&       aInstanceName,
            const MetaPropertyMap_t& aInstancePropertyValues = MetaPropertyMap_t());

protected_methods:
    /*!
     * Prototype construction implementation. Can be extended.
     */
    virtual void definePrototype();

private_typedefs:
    /*!
     * Type-Alias to define a map of instances based on this prototype.
     */
    using InstanceMap_t = CMap<InstanceUID_t, CStdSharedPtr_t<TClassType>>;

private_methods:
    /*!
     * Register a newly created instance internally.
     *
     * @param [in] aInstanceUID The UID of the instance to be registered.
     * @param [in] aInstance    The pointer to the instance to be registered.
     * @return
     */
    bool registerInstance(
            const InstanceUID_t&        aInstanceUID,
            CStdSharedPtr_t<TClassType> aInstance);

private_members:
    std::string   mPrototypeId;
    CreatorFn_t   mCreatorFn;
    InstanceMap_t mInstances;
};
//<-----------------------------------------------------------------------------

/*!
 * Create this prototype instance and register it in the MetaSystem.
 * Should be called immediately after prototype declaration.
 *
 * @return a static numeric handle identifying the prototype instance.
 */
template <typename TPrototypeType>
CMetaPrototypeBase::PrototypeStaticHandle_t spawnPrototype()
{
    static CMetaPrototypeBase::PrototypeStaticHandle_t sPrototypeStaticHandleState = 0;

    CStdSharedPtr_t<CMetaSystem> metaSystem = CMetaSystem::get();
    metaSystem->registerPrototype<TPrototypeType>();

    return sPrototypeStaticHandleState++;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
typename CMetaPrototype<TClassType>::CreatorFn_t CMetaPrototype<TClassType>::makeDefaultCreator()
{
    CreatorFn_t creator =
            [] (
                CStdSharedPtr_t<CMetaPrototype<TClassType>> aInstancePrototype,
                const InstanceUID_t&                        aInstanceUID,
                const std::string&                          aInstanceName,
                const MetaPropertyMap_t&                    aInstancePropertyValues)
            -> TClassType*
    {
        // Delegate the specific type instantiation and setup back to the attached class...
        return TClassType::create(aInstancePrototype, aInstanceUID, aInstanceName, aInstancePropertyValues);
    };

    return creator;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
CMetaPrototype<TClassType>::CMetaPrototype()
    : CMetaPrototypeBase("InvalidPrototypeId")
    , mPrototypeId("InvalidPrototypeId")
    , mCreatorFn(nullptr)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
CMetaPrototype<TClassType>::CMetaPrototype(
        const std::string& aPrototypeId,
        const CreatorFn_t& aCreatorFn)
    : CMetaPrototypeBase(aPrototypeId)
    , mPrototypeId(aPrototypeId)
    , mCreatorFn(aCreatorFn)
{
    SR_ASSERT(false   == aPrototypeId.empty());
    SR_ASSERT(nullptr != aCreatorFn);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
CMetaPrototype<TClassType>::~CMetaPrototype()
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
/*!
 * Function invoked to define the prototype instance.
 */
template <typename TClassType>
void CMetaPrototype<TClassType>::constructPrototype()
{
    static bool gConstructed = false;
    if(!gConstructed)
    {
        this->definePrototype();
        gConstructed = true;
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
void CMetaPrototype<TClassType>::definePrototype()
{
    return;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
bool CMetaPrototype<TClassType>::registerInstance(
        const InstanceUID_t&        aInstanceUID,
        CStdSharedPtr_t<TClassType> aInstance)
{
    SR_RETURN_IF(nullptr == aInstance, false);

    if(!mInstances.contains(aInstanceUID))
    {
        mInstances[aInstanceUID] = aInstance;
    }

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
CStdSharedPtr_t<CMetaObject> CMetaPrototype<TClassType>::createAbstractInstance(
        const InstanceUID_t&     aInstanceUID,
        const std::string&       aInstanceName,
        const MetaPropertyMap_t& aInstancePropertyValues)
{
    CStdSharedPtr_t<TClassType>  ptr      = createTypedInstance(aInstanceUID, aInstanceName, aInstancePropertyValues);
    CStdSharedPtr_t<CMetaObject> abstract = std::static_pointer_cast<CMetaObject>(ptr);
    return abstract;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TClassType>
CStdSharedPtr_t<TClassType> CMetaPrototype<TClassType>::createTypedInstance(
        const InstanceUID_t&     aInstanceUID,
        const std::string&       aInstanceName,
        const MetaPropertyMap_t& aInstancePropertyValues)
{
    MetaPropertyMap_t propertySet = {};
    const bool propertiesValid = mapProperties(mProperties, aInstancePropertyValues, propertySet);
    SR_ASSERT(propertiesValid);

    CStdSharedPtr_t<CMetaPrototype<TClassType>> prototype = CStdSharedPtr_t<CMetaPrototype<TClassType>>(this, [] (CMetaPrototype<TClassType>*) {});

    TClassType* ptr = mCreatorFn(prototype, aInstanceUID, aInstanceName, propertySet);
    SR_ASSERT(nullptr != ptr);

    CStdSharedPtr_t<TClassType> instance = CStdSharedPtr_t<TClassType>(ptr);
    SR_ASSERT(nullptr != instance);

    const bool registered = registerInstance(aInstanceUID, instance); // Register, so that we can update on structural change or delete explicitly on object destruction in CObject.
    SR_ASSERT(true == registered);

    return instance;
}
//<-----------------------------------------------------------------------------

/*!
 * Global instantiation of the CMetaPrototype instance.
 */
template <typename TClassType>
static CMetaPrototypeBase::PrototypeStaticHandle_t const gMetaPrototypeStaticHandle = spawnPrototype<CMetaPrototype<TClassType>>();
