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

            virtual EEngineStatus load(Dependencies_t const &aDependencies) = 0;

            virtual EEngineStatus unload() = 0;

        private_api:
            void setGpuApiResourceHandle(GpuApiHandle_t const &aHandle) final;

            [[nodiscard]]
            GpuApiHandle_t getGpuApiResourceHandle() const final;

            SHIRABE_INLINE
            void setGpuApiResourceLoader  (std::function<EEngineStatus(std::vector<ResourceId_t> &&)> const &aLoader) final
            {
                mLoader = aLoader;
            }
            SHIRABE_INLINE
            void setGpuApiResourceUnloader(std::function<EEngineStatus(std::vector<ResourceId_t> &&)> const &aUnloader) final
            {
                mUnloader = aUnloader;
            }

        protected_methods:
            EEngineStatus loadGpuApiResource(std::vector<ResourceId_t> &&aDependencies)
            {
                return (mLoader) ? mLoader(std::move(aDependencies)) : EEngineStatus::Error;
            }
            EEngineStatus unloadGpuApiResource(std::vector<ResourceId_t> &&aDependencies)
            {
                return (mUnloader) ? mUnloader(std::move(aDependencies)) : EEngineStatus::Error;
            }

            std::optional<TDependencies> const &getCurrentDependencies() const { return mDependencies; }
            void resetCurrentDependencies() { mDependencies.reset(); }

        private_members:
            TDescription  const                                     mDescription;
            std::optional<TDependencies>                            mDependencies;

            GpuApiHandle_t                                          mGpuApiResourceHandle;
            IGpuApiResourceObject::ObservableState_t::ObserverPtr_t mStateObserver;

            std::function<EEngineStatus(std::vector<ResourceId_t> &&)> mLoader;
            std::function<EEngineStatus(std::vector<ResourceId_t> &&)> mUnloader;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription, typename TDependencies>
        CResourceObject<TDescription, TDependencies>::CResourceObject(const TDescription &aDescription)
            : mDescription(aDescription)
            , mDependencies()
            , mGpuApiResourceHandle()
            , mStateObserver()
            , mLoader()
            , mUnloader()
        {}
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription, typename TDependencies>
        void CResourceObject<TDescription, TDependencies>::setGpuApiResourceHandle(GpuApiHandle_t const &aHandle)
        {
            mGpuApiResourceHandle = aHandle;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription, typename TDependencies>
        GpuApiHandle_t CResourceObject<TDescription, TDependencies>::getGpuApiResourceHandle() const
        {
            return mGpuApiResourceHandle;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
