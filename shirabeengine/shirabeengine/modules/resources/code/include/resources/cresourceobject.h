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
        template <typename TDescription, typename TDependencies>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceObject
            : public ILogicalResourceObject
        {
            friend class CResourceManager;

        public_typedefs:
            using Descriptor_t   = TDescription;
            using Dependencies_t = TDependencies;

        public_constructors:
            explicit CResourceObject(TDescription const &aDescription);

        public_destructors:
            ~CResourceObject() override = default;

        public_api:
            SHIRABE_INLINE
            TDescription const &getDescription() const
            { return mDescription; }

            EEngineStatus load();

            EEngineStatus unload();

        private_api:
            void setGpuApiResourceHandle(GpuApiHandle_t const &aHandle) final;

            [[nodiscard]]
            GpuApiHandle_t getGpuApiResourceHandle() const final;

        public_methods:
            SHIRABE_INLINE
            void setDependencies(Dependencies_t const &aDependencies)
            {
                mDependencies = aDependencies;
            };

            [[nodiscard]]
            SHIRABE_INLINE
            Dependencies_t const &getDependencies() const
            {
                return mDependencies;
            }

        private_members:
            TDescription  const                                     mDescription;
            TDependencies                                           mDependencies;

            GpuApiHandle_t                                          mGpuApiResourceHandle;
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
        void CResourceObject<TDescriptor>::setGpuApiResourceHandle(GpuApiHandle_t const &aHandle)
        {
            mGpuApiResourceHandle = aHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        GpuApiHandle_t CResourceObject<TDescriptor>::getGpuApiResourceHandle() const
        {
            return mGpuApiResourceHandle;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
