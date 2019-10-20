//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__

#include <platform/platform.h>

#include "resources/iresourceobjectprivate.h"

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
            SHIRABE_LIBRARY_EXPORT APrivateResourceObject
            : public IResourceObjectPrivate
        {
            friend class CResourceManager;

        public_constructors:
            explicit APrivateResourceObject(TDescription const &aDescription);

        public_destructors:
            ~APrivateResourceObject() override = default;

        public_api:
            // IResourceObject
            CEngineResult<> create()  override;
            CEngineResult<> load()    override;
            CEngineResult<> unload()  override;
            CEngineResult<> destroy() override;

        private_api:

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
        APrivateResourceObject<TDescriptor>::APrivateResourceObject(const TDescriptor &aDescription)
            : mDescription(aDescription)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> APrivateResourceObject<TDescriptor>::create()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> APrivateResourceObject<TDescriptor>::load()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> APrivateResourceObject<TDescriptor>::unload()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> APrivateResourceObject<TDescriptor>::destroy()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_APRIVATERESOURCEOBJECT_H__
