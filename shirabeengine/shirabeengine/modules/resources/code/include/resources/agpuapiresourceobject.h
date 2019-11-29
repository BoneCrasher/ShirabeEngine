//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/patterns/observer.h>

#include "resources/igpuapiresourceobject.h"
#include "cgpuapiresourcestorage.h"

class CResourceManager; // Declared/defined in resources/cresourcemanager.h

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT AGpuApiResourceObject
            : public IGpuApiResourceObject
        {
            friend class CResourceManager;

        public_constructors:
            explicit AGpuApiResourceObject();

        public_destructors:
            ~AGpuApiResourceObject() override = default;

        public_api:
            // IGpuApiResourceObject
            virtual CEngineResult<> create(  typename TResource::Descriptor_t   const &aDescription
                                           , typename TResource::Dependencies_t const &aDependencies
                                           , GpuApiResourceDependencies_t       const &aResolvedDependencies);
            virtual CEngineResult<> destroy();

            Shared<ObservableState_t> observableState() final;

        public_methods:
            [[nodiscard]]
            SHIRABE_INLINE EGpuApiResourceState getResourceState() const final
            {
                return mState;
            }

            SHIRABE_INLINE std::optional<typename TResource::Descriptor_t> const &getCurrentDescriptor() const
            {
                return mDescriptor;
            }

            SHIRABE_INLINE std::optional<typename TResource::Dependencies_t> const &getCurrentDependencies() const
            {
                return mDependencies;
            }

        protected_methods:
            SHIRABE_INLINE void setResourceState(EGpuApiResourceState const &aState)
            {
                mState = aState;
                // observableState()->notify(makeSharedFromThis(this), mState);
            };

            SHIRABE_INLINE void setCurrentDescriptor(typename TResource::Descriptor_t const &aDescriptor)
            {
                mDescriptor = aDescriptor;
            }

            SHIRABE_INLINE void setCurrentDependencies(typename TResource::Dependencies_t const &aDependencies)
            {
                mDependencies = aDependencies;
            }

        private_members:
            std::optional<typename TResource::Descriptor_t>   mDescriptor;
            std::optional<typename TResource::Dependencies_t> mDependencies;

            Shared<ObservableState_t>  mObservableState;
            EGpuApiResourceState       mState;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        AGpuApiResourceObject<TResource>::AGpuApiResourceObject()
            : mDescriptor()
            , mDependencies()
            , mObservableState(makeShared<CSubject<EGpuApiResourceState>>())
            , mState          (EGpuApiResourceState::Unknown)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> AGpuApiResourceObject<TResource>::create(  typename TResource::Descriptor_t   const &aDescription
                                                                 , typename TResource::Dependencies_t const &aDependencies
                                                                 , GpuApiResourceDependencies_t       const &aResolvedDependencies)
        {
            SHIRABE_UNUSED(aResolvedDependencies);

            mDescriptor   = aDescription;
            mDependencies = aDependencies;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CEngineResult<> AGpuApiResourceObject<TResource>::destroy()
        {
            mDependencies.reset();
            mDescriptor.reset();

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        Shared<IGpuApiResourceObject::ObservableState_t> AGpuApiResourceObject<TResource>::observableState()
        {
            return mObservableState;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
