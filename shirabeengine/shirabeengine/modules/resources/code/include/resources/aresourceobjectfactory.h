//
// Created by dottideveloper on 20.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>

#include "resources/iresourceobjectprivate.h"

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

        template <typename TDescriptor>
        class SHIRABE_LIBRARY_EXPORT CResourceObjectCreator
            : public IResourceObjectCreatorBase
        {
        public_typedefs:
            using Fn_t = std::function<Unique<IResourceObjectPrivate>(TDescriptor const &)>;

        public_constructors:
            explicit CResourceObjectCreator(Fn_t const &aFn)
                : mFn(aFn)
            {};

        public_methods:
            Unique<IResourceObjectPrivate> create(TDescriptor const &aDescriptor)
            {
                return (nullptr == mFn) ? nullptr : mFn(aDescriptor);
            }

        private_methods:
            Fn_t mFn;
        };

        class SHIRABE_LIBRARY_EXPORT CResourceObjectFactory
        {
            friend class CResourceManager;

        private_typedefs:

        public_constructors:
            CResourceObjectFactory() = default;

        public_destructors:
            virtual ~CResourceObjectFactory() = default;

        protected_methods:
            template <typename T>
            void setCreatorForType(Unique<IResourceObjectCreatorBase> aCreator)
            {
                std::type_info const &typeInfo = typeid(T);

                mCreators[typeInfo] = std::move(aCreator);
            };

        private_methods:
            template <typename T>
            Unique<IResourceObjectPrivate> create(typename T::SDescriptor const &aDescriptor)
            {
                std::type_info const &typeInfo = typeid(T);

                auto entry = std::find(mCreators.begin(), mCreators.end(), typeInfo);
                if(mCreators.end() == entry)
                {
                    return nullptr;
                }

                Unique<IResourceObjectPrivate>            result      = nullptr;
                Unique<IResourceObjectCreatorBase> const &creatorBase = mCreators[typeInfo];

                auto creator = static_cast<CResourceObjectCreator<T> const *const>(creatorBase.get());
                if(nullptr != creator)
                {
                    result = std::move(creator->create(aDescriptor));
                }
                return result;
            }

        private_members:
            std::unordered_map<std::type_info, Unique<IResourceObjectCreatorBase>> mCreators;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTFACTORY_H__
