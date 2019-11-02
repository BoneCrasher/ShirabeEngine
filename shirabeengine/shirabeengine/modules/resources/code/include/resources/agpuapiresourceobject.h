//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/patterns/observer.h>

#include "resources/igpuapiresourceobject.h"

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
            CEngineResult<> create(CGpiApiDependencyCollection const &aDependencies)  override;
            CEngineResult<> load()    override;
            CEngineResult<> unload()  override;
            CEngineResult<> destroy() override;

            // ILogicalResourceObject
            CEngineResult<> bind()     override;
            CEngineResult<> transfer() override;
            CEngineResult<> unbind()   override;

            Shared<ObservableState_t> observableState() final;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            {
                return mDescription;
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
        CEngineResult<> AGpuApiResourceObject<TDescription>::create(CGpiApiDependencyCollection const &aDependencies)
        {
            SHIRABE_UNUSED(aDependencies);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::load()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::unload()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::destroy()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::bind()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::transfer()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::unbind()
        {
            return { EEngineStatus::Ok };
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
