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
        template <typename TReturn, typename... TArgs>
        std::function<TReturn(TArgs...)> makeNoOp()
        {
            return [] (TArgs...) -> TReturn {};
        }

        template <typename TDescriptor, typename TDependencies>
        struct SLogicalOps
        {
            using InitializeFn_t =
                std::function<CEngineResult<>(TDependencies const &)>;
            using LoadFn_t =
                std::function<CEngineResult<>()>;
            using TransferFn_t =
                std::function<CEngineResult<>()>;
            using UnloadFn_t =
                std::function<CEngineResult<>()>;
            using DeinitializeFn_t =
                std::function<CEngineResult<>(TDependencies const &)>;

            SLogicalOps() = default;

            SHIRABE_INLINE
            SLogicalOps(SLogicalOps const &aOther)
                    : initialize  (aOther.initialize  )
                    , load        (aOther.load        )
                    , transfer    (aOther.transfer    )
                    , unload      (aOther.unload      )
                    , deinitialize(aOther.deinitialize)
            { }

            SHIRABE_INLINE
            SLogicalOps &operator=(SLogicalOps const &aOther)
            {
                if(&aOther != this)
                {
                    initialize   = aOther.initialize;
                    load         = aOther.load;
                    transfer     = aOther.transfer;
                    unload       = aOther.unload;
                    deinitialize = aOther.deinitialize;
                }

                return (*this);
            }

            static SLogicalOps DefaultOps()
            {
                SLogicalOps ops {};
                ops.initialize   = makeNoOp<CEngineResult<>, TDependencies const &>();
                ops.load         = makeNoOp<CEngineResult<>>();
                ops.transfer     = makeNoOp<CEngineResult<>>();
                ops.unload       = makeNoOp<CEngineResult<>>();
                ops.deinitialize = makeNoOp<CEngineResult<>, TDependencies const &>();
                return ops;
            }

            InitializeFn_t   initialize;
            LoadFn_t         load;
            TransferFn_t     transfer;
            UnloadFn_t       unload;
            DeinitializeFn_t deinitialize;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription, typename TDependencies>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceObject
            : public ILogicalResourceObject
            , public SLogicalOps<TDescription, TDependencies>
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

            [[nodiscard]]
            GpuApiHandle_t getGpuApiResourceHandle() const final;

            std::optional<TDependencies> const &getCurrentDependencies() const { return mDependencies; }

        private_api:
            void setGpuApiResourceHandle(GpuApiHandle_t const &aHandle) final;

            void setLogicalOps(SLogicalOps<TDescription, TDependencies> const &aOps)
            {
                SLogicalOps<TDescription, TDependencies>::operator=(aOps);
            }

            void setCurrentDependencies(TDependencies const &aDependencies)
            {
                mDependencies = aDependencies;
            }

        protected_methods:
        private_members:
            TDescription  const          mDescription;
            std::optional<TDependencies> mDependencies;

            GpuApiHandle_t               mGpuApiResourceHandle;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription, typename TDependencies>
        CResourceObject<TDescription, TDependencies>::CResourceObject(const TDescription &aDescription)
            : ILogicalResourceObject()
            , SLogicalOps<TDescription, TDependencies>(SLogicalOps<TDescription, TDependencies>::DefaultOps())
            , mDescription(aDescription)
            , mDependencies()
            , mGpuApiResourceHandle()
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
