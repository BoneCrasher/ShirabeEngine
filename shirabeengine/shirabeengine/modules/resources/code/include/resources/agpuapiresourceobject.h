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
        template <typename TDescription>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT AGpuApiResourceObject
            : public IGpuApiResourceObject
        {
            friend class CResourceManager;

        public_constructors:
            explicit AGpuApiResourceObject(TDescription const &aDescription);

        public_destructors:
            ~AGpuApiResourceObject() override = default;

        public_api:
            // IGpuApiResourceObject
            CEngineResult<> create(GpuApiResourceDependencies_t const &aDependencies) override;
            CEngineResult<> destroy() override;

            Shared<ObservableState_t> observableState() final;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            {
                return mDescription;
            }

            [[nodiscard]]
            SHIRABE_INLINE EGpuApiResourceState getResourceState() const final
            {
                return mState;
            }

        protected_methods:
            SHIRABE_INLINE void setResourceState(EGpuApiResourceState const &aState)
            {
                mState = aState;
                observableState()->notify(makeSharedFromThis(this), mState);
            };

        private_members:
            TDescription         const mDescription;
            Shared<ObservableState_t>  mObservableState;
            EGpuApiResourceState       mState;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        AGpuApiResourceObject<TDescription>::AGpuApiResourceObject(const TDescription &aDescription)
            : mDescription    (aDescription)
            , mObservableState(makeShared<CSubject<EGpuApiResourceState>>())
            , mState          (EGpuApiResourceState::Unknown)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::create(GpuApiResourceDependencies_t const &aDependencies)
        {
            SHIRABE_UNUSED(aDependencies);

            return { EEngineStatus::NotImplemented };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::destroy()
        {
            return { EEngineStatus::NotImplemented };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        Shared<IGpuApiResourceObject::ObservableState_t> AGpuApiResourceObject<TDescription>::observableState()
        {
            return mObservableState;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
