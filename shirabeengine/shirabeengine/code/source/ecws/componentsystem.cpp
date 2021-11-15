#include "ecws/componentsystem.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CComponentSystemManager::CComponentSystemManager()
        : mComponentSystemIdIssuer()
        , mComponentSystemRegistry()
        , mComponentSystemTypeRegistry()
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CComponentSystemManager::~CComponentSystemManager()
    {
        for(auto &[id, system] : mComponentSystemRegistry)
        {
            unregisterComponentSystem(id);
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CComponentSystemManager::initialize()
    {
        return EEngineStatus::NotImplemented;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CComponentSystemManager::deinitialize()
    {
        return EEngineStatus::NotImplemented;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<ComponentSystemId_t> CComponentSystemManager::registerComponentSystem(Shared<IComponentSystem> &&aComponentSystem)
    {
        if(aComponentSystem)
        {
            auto base = std::static_pointer_cast<IComponentSystemManagerBinding>(aComponentSystem);
            if(base->isRegisteredInManager())
            {
                return { EEngineStatus::Ok, aComponentSystem->getComponentSystemId() };
            }

            Vector<std::type_index> const &supportedComponentTypes = aComponentSystem->getSupportedComponentTypes();
            if(supportedComponentTypes.empty())
            {
                return {EEngineStatus::Error, 0};
            }

            ComponentSystemId_t const uid  = mComponentSystemIdIssuer.fetchUid();
            base->bindToManager(uid);

            mComponentSystemRegistry.insert({uid, aComponentSystem});
            for(auto const &componentTypeIndex : supportedComponentTypes)
            {
                mComponentSystemTypeRegistry.insert({componentTypeIndex, aComponentSystem});
            }

            return { EEngineStatus::Ok, uid };
        }

        return {EEngineStatus::Error, 0};
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<IComponentSystem>> CComponentSystemManager::unregisterComponentSystem(ComponentSystemId_t aComponentSystemId)
    {
        if(gInvalidComponentSystemId == aComponentSystemId)
        {
            return { EEngineStatus::Ok, nullptr };
        }

        auto system = findComponentSystem(aComponentSystemId);

        Vector<std::type_index> const &supportedComponentTypes = system->getSupportedComponentTypes();
        if(not supportedComponentTypes.empty())
        {
            for(auto const &componentTypeIndex : supportedComponentTypes)
            {
                mComponentSystemTypeRegistry.erase(componentTypeIndex);
            }
        }
        mComponentSystemRegistry.erase(aComponentSystemId);

        auto base = std::static_pointer_cast<IComponentSystemManagerBinding>(system);
        base->unbindFromManager();

        return { EEngineStatus::Ok, system };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<IComponentSystem> CComponentSystemManager::findComponentSystem(ComponentSystemId_t aComponentSystemId)
    {
        if(gInvalidComponentSystemId == aComponentSystemId)
        {
            return { nullptr };
        }

        auto iterator = mComponentSystemRegistry.find(aComponentSystemId);
        if(mComponentSystemRegistry.end() == iterator)
        {
            return { nullptr };
        }

        auto [id, system] = *iterator;
        return system;
    }
    //<-----------------------------------------------------------------------------
}
