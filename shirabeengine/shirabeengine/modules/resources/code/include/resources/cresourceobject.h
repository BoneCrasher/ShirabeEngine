//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/patterns/observer.h>

#include "resources/ilogicalresourceobject.h"
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
            SHIRABE_LIBRARY_EXPORT CResourceObject
            : public ILogicalResourceObject
        {
            friend class CResourceManager;

        public_typedefs:
            using Descriptor_t = TDescription;

        public_constructors:
            explicit CResourceObject(TDescription const &aDescription);

        public_destructors:
            ~CResourceObject() override = default;

        private_api:
            void bindGpuApiResourceInterface(Unique<IGpuApiResourceObject> aGpuApiInterface) final;

            Unique<IGpuApiResourceObject>& getGpuApiResourceInterface() final;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            { return mDescription; }

        private_members:
            TDescription const mDescription;
            Unique<IGpuApiResourceObject>                           mGpuApiInterface;
            IGpuApiResourceObject::ObservableState_t::ObserverPtr_t mStateObserver;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CResourceObject<TDescriptor>::CResourceObject(const TDescriptor &aDescription)
            : mDescription(aDescription)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::bind()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::unbind()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::transfer()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        void CResourceObject<TDescriptor>::bindGpuApiResourceInterface(
                engine::Unique<engine::resources::IGpuApiResourceObject> aGpuApiInterface)
        {
            if(mGpuApiInterface)
            {
                mGpuApiInterface->observableState()->ignore(mStateObserver);
                mGpuApiInterface = nullptr;
            }

            mGpuApiInterface = std::move(aGpuApiInterface);

            mStateObserver = makeShared<CObserver<Unique<IGpuApiResourceObject>, EGpuApiResourceState>>(
                    mGpuApiInterface, // Taking the unique pointer by reference is valid at this point, since the ownership won't change anymore...
                    [this] (Unique<IGpuApiResourceObject> const &aSubject, EGpuApiResourceState &&aState)
                    {
                        SHIRABE_UNUSED(aSubject);

                        // ... Do stuff.
                    });

            mGpuApiInterface->observableState()->observe(mStateObserver);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        Unique<IGpuApiResourceObject>& CResourceObject<TDescriptor>::getGpuApiResourceInterface()
        {
            return mGpuApiInterface;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
