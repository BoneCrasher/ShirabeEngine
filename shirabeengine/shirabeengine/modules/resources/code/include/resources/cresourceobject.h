//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__

#include <platform/platform.h>

#include "resources/iresourceobjectprivate.h"
#include "resources/iresourceobject.h"

class CResourceManager; // Declared/defined in resources/cresourcemanager.h

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        class [[nodiscard]] SHIRABE_LIBRARY_EXPORT CResourceObject
            : public IResourceObject
        {
            friend class CResourceManager;

        public_constructors:
            explicit CResourceObject(TDescription const &aDescription);

        public_destructors:
            ~CResourceObject() override = default;

        public_api:
            // IResourceObject
            CEngineResult<> bind()     override;
            CEngineResult<> unbind()   override;
            CEngineResult<> transfer() override;

        private_methods:
            // IResourceObjectPrivate
            CEngineResult<> create()  override;
            CEngineResult<> load()    override;
            CEngineResult<> unload()  override;
            CEngineResult<> destroy() override;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            { return mDescription; }

        private_members:
            TDescription const mDescription;
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
        CEngineResult<> CResourceObject<TDescriptor>::create()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::load()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::unload()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::destroy()
        {
            return { EEngineStatus::Ok };
        }
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
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
