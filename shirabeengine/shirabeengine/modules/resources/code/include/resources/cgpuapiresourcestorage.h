//
// Created by dotti on 09.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_CGPUAPIRESOURCESTORAGE_H__
#define __SHIRABEDEVELOPMENT_CGPUAPIRESOURCESTORAGE_H__

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include "resources/igpuapiresourceobject.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
namespace engine::resources
{
    class
        SHIRABE_LIBRARY_EXPORT CGpuApiResourceStorage
    {
    public_constructors:
        CGpuApiResourceStorage() = default;

    public_destructors:
        ~CGpuApiResourceStorage() = default;

    public_methods:
        bool add(GpuApiHandle_t const &aId, engine::Shared<IGpuApiResourceObject> aResourceReference);
        void remove(GpuApiHandle_t const &aId);

        Shared<IGpuApiResourceObject> const get(GpuApiHandle_t const &aId) const;

        template<typename T>
        T const *extract(GpuApiHandle_t const &aId) const;

    private_members:
        std::unordered_map<GpuApiHandle_t, Shared<IGpuApiResourceObject>> mDependencies;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template<typename T>
    T const *CGpuApiResourceStorage::extract(GpuApiHandle_t const &aId) const
    {
        T const *result = nullptr;

        engine::Shared<IGpuApiResourceObject> const &ref = get(aId);
        if( nullptr != ref )
        {
            result = dynamic_cast<T *>(ref.get());
        }

        return result;
    }
}

#endif //__SHIRABEDEVELOPMENT_CGPUAPIRESOURCESTORAGE_H__
