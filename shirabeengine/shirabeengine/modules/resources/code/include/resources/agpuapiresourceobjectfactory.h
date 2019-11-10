//
// Created by dottideveloper on 20.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include "resources/cgpuapiresourcestorage.h"
#include "resources/igpuapiresourceobject.h"

namespace engine
{
    namespace resources
    {
        class CResourceManager;

        class SHIRABE_LIBRARY_EXPORT IResourceObjectCreatorBase
        {
            SHIRABE_DECLARE_INTERFACE(IResourceObjectCreatorBase);

        public_api:
        };

        template <typename TResource>
        class SHIRABE_LIBRARY_EXPORT CResourceObjectCreator
            : public IResourceObjectCreatorBase
        {
        public_typedefs:
            using Fn_t = std::function<Unique<IGpuApiResourceObject>(typename TResource::Descriptor_t const &)>;

        public_constructors:
            explicit CResourceObjectCreator(Fn_t const &aFn)
                : mFn(aFn)
            {};

        public_methods:
            Unique<IGpuApiResourceObject> create(typename TResource::Descriptor_t const &aDescriptor)
            {
                return (nullptr == mFn)
                            ? nullptr
                            : mFn(aDescriptor);
            }

        private_methods:
            Fn_t mFn;
        };

        class SHIRABE_LIBRARY_EXPORT CGpuApiResourceObjectFactory
        {
            friend class CResourceManager;

        private_typedefs:

        public_constructors:
            CGpuApiResourceObjectFactory() = default;

        public_destructors:
            virtual ~CGpuApiResourceObjectFactory() = default;

        protected_methods:
            template <typename T>
            void setCreatorForType(Unique<IResourceObjectCreatorBase> aCreator)
            {
                std::type_info const &typeInfo = typeid(T);

                mCreators[typeInfo.name()] = std::move(aCreator);
            };

            void removeAllCreators()
            {
                mCreators.clear();
            }

        private_methods:
            template <typename T>
            GpuApiHandle_t create(typename T::Descriptor_t const &aDescriptor)
            {
                std::type_info const &typeInfo = typeid(T);

                auto entry = mCreators.find(typeInfo.name());
                if(mCreators.end() == entry)
                {
                    return GpuApiHandle_t {};
                }

                Unique<IGpuApiResourceObject>             gpuApiObject = nullptr;
                Unique<IResourceObjectCreatorBase> const &creatorBase  = mCreators[typeInfo.name()];

                auto creator = static_cast<CResourceObjectCreator<T> *const>(creatorBase.get());
                if(nullptr != creator)
                {
                    gpuApiObject = std::move(creator->create(aDescriptor));

                    GpuApiHandle_t handle = gpuApiObject->getHandle();
                    mResourceStorage->add(handle, std::move(gpuApiObject));

                    return handle;
                }

                return GpuApiHandle_t{};
            }

            SHIRABE_INLINE
            Unique<IGpuApiResourceObject> const &get(GpuApiHandle_t const &aHandle)
            {
                return mResourceStorage->get(aHandle);
            }

            SHIRABE_INLINE
            void destroy(GpuApiHandle_t const &aHandle)
            {
                mResourceStorage->get(aHandle)->destroy();
                mResourceStorage->remove(aHandle);
            }

        private_members:
            Shared<CGpuApiResourceStorage>                                      mResourceStorage;
            std::unordered_map<char const*, Unique<IResourceObjectCreatorBase>> mCreators;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__
