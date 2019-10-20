//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABE_BUFFER_H__
#define __SHIRABE_BUFFER_H__

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include <platform/platform.h>
#include <core/bitfield.h>
#include <core/databuffer.h>
#include <resources/cresourceobject.h>

namespace engine
{
    namespace gfxapi
    {
        using namespace engine::resources;

        struct [[nodiscard]] SHIRABE_LIBRARY_EXPORT SBufferDescription
        {
            std::string                       name;
            VkBufferCreateInfo                createInfo;
            // std::vector<DataSourceAccessor_t> initialData;
        };

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CBufferResource
            : public CResourceObject<SBufferDescription>
        {
        public_typedefs:
            using SDescriptor = SBufferDescription;

        public_constructors:
            using CResourceObject<SBufferDescription>::CResourceObject;

        public_destructors:
            ~CBufferResource() final  = default;

        public_api:
            // IResourceObjectPrivate
            CEngineResult<> create()  final;
            CEngineResult<> load()    final;
            CEngineResult<> unload()  final;
            CEngineResult<> destroy() final;

            // IResourceObject
            CEngineResult<> bind()     final;
            CEngineResult<> unbind()   final;
            CEngineResult<> transfer() final;

        private_members:
        };

        // IResourceObjectPrivate
        CEngineResult<> CBufferResource::create()   { return EEngineStatus::Ok; }
        CEngineResult<> CBufferResource::load()     { return EEngineStatus::Ok; }
        CEngineResult<> CBufferResource::unload()   { return EEngineStatus::Ok; }
        CEngineResult<> CBufferResource::destroy()  { return EEngineStatus::Ok; }

        // IResourceObject
        CEngineResult<> CBufferResource::bind()     { return EEngineStatus::Ok; }
        CEngineResult<> CBufferResource::unbind()   { return EEngineStatus::Ok; }
        CEngineResult<> CBufferResource::transfer() { return EEngineStatus::Ok; }
    }
}

#endif //__SHIRABEDEVELOPMENT_BUFFER_H__
