//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

#include <platform/platform.h>
#include <core/enginestatus.h>

#include "resources/resourcedescriptions.h"
#include "resources/ilogicalresourceobject.h"

namespace engine
{
    template <typename... TArgs>
    class CSubject;

    namespace resources
    {
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class CResourceManager;
        class CGpiApiDependencyCollection;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using GpuApiHandle_t = uint64_t;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        enum class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT EGpuApiResourceState
        {
            Unknown = 0
            , Unavailable
            , Loading
            , Loaded
            , Discarded
            , Error
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT IGpuApiResourceObject
            : public ILogicalResourceObject // Inherits signature of bind, unbind and transfer
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(IGpuApiResourceObject);

        public_typedefs:
            using ObservableState_t = CSubject<EGpuApiResourceState>;

        public_api:
            virtual resources::GpuApiHandle_t const getHandle() = 0;

            virtual CEngineResult<> create(CGpiApiDependencyCollection const &aDependencies) = 0;
            virtual CEngineResult<> load()                                                   = 0;
            virtual CEngineResult<> unload()                                                 = 0;
            virtual CEngineResult<> destroy()                                                = 0;

            virtual Shared<ObservableState_t> observableState() = 0;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class
            SHIRABE_LIBRARY_EXPORT CGpiApiDependencyCollection
        {
        public_constructors:
            CGpiApiDependencyCollection() = default;

        public_destructors:
            ~CGpiApiDependencyCollection() = default;

        public_methods:
            SHIRABE_INLINE
            bool add(ResourceId_t const &aId, Unique<IGpuApiResourceObject> &aResourceReference)
            {
                mDependencies.erase(aId);
                mDependencies.insert({ aId, aResourceReference });

                return true;
            };

            SHIRABE_INLINE
            Unique<IGpuApiResourceObject> const& get(ResourceId_t const &aId) const
            {
                static Unique<IGpuApiResourceObject> sNullRef = nullptr;

                if(mDependencies.end() == mDependencies.find(aId))
                {
                    return sNullRef;
                }

                return mDependencies.at(aId);
            }

            template <typename T>
            T const *const extract(ResourceId_t const &aId) const
            {
                T const *result = nullptr;

                Unique<IGpuApiResourceObject> const &ref = get(aId);
                if(nullptr != ref)
                {
                    result = dynamic_cast<T*>(ref.get());
                }

                return result;
            }

        private_members:
            std::unordered_map<ResourceId_t, Unique<IGpuApiResourceObject>&> mDependencies;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__
