#include "sr_pch.h"

#include "property_system/sr_meta_prototype.h"
#include "property_system/sr_meta_property.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
InstanceUID_t CMetaPrototypeBase::generatePrototypeUID()
{
    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

    const InstanceUID_t uid = system->generateObjectUID();
    return uid;
}

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaPrototypeBase::CMetaPrototypeBase(
        const std::string& aPrototypeId)
    : CMetaObject(CMetaPrototypeBase::generatePrototypeUID(), aPrototypeId)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool mapProperties(
        const MetaPropertyMap_t& aPropertyReference,
        const MetaPropertyMap_t& aPropertyInput,
        MetaPropertyMap_t&       aResult)
{
    MetaPropertyMap_t intermediate{};

    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

    for(const auto& [key, variant] : aPropertyReference)
    {
        // Select the variant to copy from, either prototype or override.
        MetaPropertyVariant_t selectedVariant =
            (aPropertyInput.contains(key))
                ? aPropertyInput.at(key)
                : variant;

        // Overwrite the UID with a newly generated property UID.
        const PropertyUID_t uid = system->generatePropertyUID();
        SR_RESOLVE(SSetPropertyUIDResolver, selectedVariant, uid);

        intermediate[key] = selectedVariant;
    }

    aResult = (intermediate);
    return true;
}
//<-----------------------------------------------------------------------------
