//
// Created by dotti on 09.11.19.
//
#include "resources/cgpuapiresourcestorage.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
namespace engine::resources
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CGpuApiResourceStorage::add(GpuApiHandle_t const &aId, engine::Shared<IGpuApiResourceObject> aResourceReference)
    {
        mDependencies.erase(aId);
        mDependencies.insert({aId, std::move(aResourceReference)});

        return true;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CGpuApiResourceStorage::remove(GpuApiHandle_t const &aId)
    {
        mDependencies.erase(aId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<IGpuApiResourceObject> const CGpuApiResourceStorage::get(GpuApiHandle_t const &aId) const
    {
        if( mDependencies.end() == mDependencies.find(aId))
        {
            return nullptr;
        }

        return mDependencies.at(aId);
    }
    //<-----------------------------------------------------------------------------
}
