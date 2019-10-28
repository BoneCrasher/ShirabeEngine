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
            // ILogicalResourceObject
            CEngineResult<> create()  override;
            CEngineResult<> load()    override;
            CEngineResult<> unload()  override;
            CEngineResult<> destroy() override;

            // ILogicalResourceObject
            CEngineResult<> bind()     override;
            CEngineResult<> transfer() override;
            CEngineResult<> unbind()   override;

            ObservableState_t& observableState() final;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            { return mDescription; }

        private_members:
            TDescription      const mDescription;
            ObservableState_t       mState;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        AGpuApiResourceObject<TDescription>::AGpuApiResourceObject(const TDescription &aDescription)
            : mDescription(aDescription)
            , mState      ()
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        CEngineResult<> AGpuApiResourceObject<TDescription>::create()
        {
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
        IGpuApiResourceObject::ObservableState_t& AGpuApiResourceObject<TDescription>::observableState()
        {
            return mState;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
